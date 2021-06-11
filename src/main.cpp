#include <Arduino.h>
#include <Wire.h> 
#include <Servo.h>
#include <EEPROM.h>

#include "Constants.h"
#include "InfoDisplay.h"
#include "Keyboard.h"
#include "Tetris.h"
#include "LedMatrix.h"

using namespace std;

// i2C LCD cnx
#define I2CLCD_ADDR 0x27
#define I2CLCD_ROWCHARS 16
#define I2CLCD_ROWS 2

// MAX7219 hardware cnx
#define NB_ROWS     8
#define NB_COLS     8
#define NB_MAX7219  2
#define PIN_CLK     12 // D12
#define PIN_CS      11 // D11
#define PIN_DIN     10 // D10

// Buttons controller cnx
// Nano accept interupts only on pins 2, 3
#define PIN_LEFT    5 // D5
#define PIN_RIGHT   6 // D6
#define PIN_ROTATE  7 // D7
#define PIN_DOWN    8 // D8

// Servo for lock
#define PIN_SERVO   9
#define LOCK_OPEN  0
#define LOCK_CLOSE 50

#define BOUNCE_TIMEOUT 200

InfoDisplay display = InfoDisplay(I2CLCD_ADDR, I2CLCD_ROWCHARS, I2CLCD_ROWS);
LedMatrix matrix = LedMatrix(PIN_CLK, PIN_CS, PIN_DIN, NB_ROWS, NB_COLS, NB_MAX7219);
Keyboard kb = Keyboard(PIN_LEFT, PIN_RIGHT, PIN_ROTATE, PIN_DOWN);

Tetris game = Tetris(NB_ROWS * NB_MAX7219, NB_COLS);

Servo lock;

static MODES gameMode = MODES::ENDGAME;
static bool boxinit = false;
static bool boxdebug = false;
static bool opened = false;
static uint32_t openCount = 0;

void EEPROMWritelong(int address, unsigned long value) {
  Serial.print("EEPROM write ");
  Serial.print(address);
  Serial.print(" ");
  Serial.println(value);
  //Decomposition from a long to 4 bytes by using bitshift.
  //One = Most significant -> Four = Least significant byte
  byte four = (value & 0xFF);
  byte three = ((value >> 8) & 0xFF);
  byte two = ((value >> 16) & 0xFF);
  byte one = ((value >> 24) & 0xFF);

  //Write the 4 bytes into the eeprom memory.
  EEPROM.write(address, four);
  EEPROM.write(address + 1, three);
  EEPROM.write(address + 2, two);
  EEPROM.write(address + 3, one);
}

unsigned long EEPROMReadlong(uint8_t address) {
  //Read the 4 bytes from the eeprom memory.
  unsigned long four = EEPROM.read(address);
  unsigned long three = EEPROM.read(address + 1);
  unsigned long two = EEPROM.read(address + 2);
  unsigned long one = EEPROM.read(address + 3);

  //Return the recomposed long by using bitshift.
  unsigned long value = ((four << 0x00) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
  return value;
}

void openLock() {
  if (boxinit || boxdebug) {
    lock.write((int)LOCK_OPEN);
    delay(600);
    lock.write((int)LOCK_CLOSE);
    //
    if (!boxdebug) {
      openCount++;
      EEPROMWritelong(ADDR_OPENCOUNT, openCount);
      display.setOpenCount(openCount);
    }
  }
}

void ISR_KEYPRESSED() {
  if (gameMode == MODES::START) {
    boxinit = true;
    opened = false;
    game.newGame();
    display.setMode(gameMode = MODES::GAME);
    display.setPoints(game.getPoints());
  } else if (gameMode == MODES::ENDGAME) {
    game.clear();
    display.setMode(gameMode = MODES::START);
  }
  
}

void ISR_LEFT() {
  ISR_KEYPRESSED();
  if (boxinit) game.left();
}

void ISR_RIGHT() {
  ISR_KEYPRESSED();
  if (boxinit) game.right();
}

void ISR_ROTATE() {
  ISR_KEYPRESSED();
  if (boxinit) game.rotate((uint8_t)1); // clockwise
}

void ISR_DOWN() {
  ISR_KEYPRESSED();
  if (boxinit) game.down();
}

void ISR_POINTS() {
  uint32_t points = game.getPoints();
  if (points >= POINTS_TO_OPEN && !opened) {
    opened = true;
    openLock();
  }

  display.setPoints(points);
  display.setHighScore(game.getHighScore());

  EEPROMWritelong(ADDR_HIGHSCORE, game.getHighScore());
}

void ISR_END() {
  boxinit = false;
  display.setMode(gameMode = MODES::ENDGAME);
  
}

void setup()
{
  randomSeed(analogRead(0));
  //
  Serial.begin(9600);
  Serial.println("Init geocache");
  //
  game.setHighScore(EEPROMReadlong(ADDR_HIGHSCORE));
  display.setHighScore(game.getHighScore());
  openCount = EEPROMReadlong(ADDR_OPENCOUNT);
  display.setOpenCount(openCount);
  // init servo lock
  lock.attach(PIN_SERVO);
  lock.write(LOCK_CLOSE);
  // init Led matrix
  matrix.init();
  // init random function
  // init keyboard
  kb.setup(ISR_LEFT, ISR_RIGHT, ISR_ROTATE, ISR_DOWN);
  // init LCD display
  display.start();
  // check maintenance mode
  if (!digitalRead(PIN_ROTATE) & !digitalRead(PIN_DOWN)) {
    display.setMode(gameMode = MODES::DEBUG);
    delay(2000);
    if ((!digitalRead(PIN_DOWN)) & digitalRead(PIN_ROTATE)) {
      boxdebug = true;
      openLock();
    }
  }
  if (!boxdebug) {
    display.setMode(gameMode = MODES::START);
  }
  // init game
  game.init(&matrix, ISR_POINTS, ISR_END);
}

void loop()
{
  kb.sync(); // keyboard read loop
  display.sync();
  game.sync();
}
