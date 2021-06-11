#ifndef TETRISGAME_H
#define TETRISGAME_H

#include <stdint.h>
#include <Arduino.h>

#include "LedMatrix.h"

#define SPRITESIZE 4

#define bitset(byte,nbit)   ((byte) |=  (1<<(nbit)))
#define bitclear(byte,nbit) ((byte) &= ~(1<<(nbit)))
#define bitflip(byte,nbit)  ((byte) ^=  (1<<(nbit)))
#define bitcheck(byte,nbit) ((byte) &   (1<<(nbit)))

class Tetris {
  
  public:
    uint8_t sprites[7][4][SPRITESIZE] = {
      { { 0xC, 0x8, 0x8, 0x0 }, { 0x8, 0xE, 0x0, 0x0 }, { 0x2, 0x2, 0x6, 0x0 }, { 0xE, 0x2, 0x0, 0x0 } },
      { { 0x6, 0x2, 0x2, 0x0 }, { 0xE, 0x8, 0x0, 0x0 }, { 0x8, 0x8, 0xC, 0x0 }, { 0x2, 0xE, 0x0, 0x0 } },
      { { 0x2, 0x2, 0x2, 0x2 }, { 0xF, 0x0, 0x0, 0x0 }, { 0x4, 0x4, 0x4, 0x4 }, { 0xF, 0x0, 0x0, 0x0 } },
      { { 0xE, 0x4, 0x0, 0x0 }, { 0x8, 0xC, 0x8, 0x0 }, { 0x4, 0xE, 0x0, 0x0 }, { 0x2, 0x6, 0x2, 0x0 } },
      { { 0x6, 0x6, 0x0, 0x0 }, { 0x6, 0x6, 0x0, 0x0 }, { 0x6, 0x6, 0x0, 0x0 }, { 0x6, 0x6, 0x0, 0x0 } },
      { { 0x8, 0xC, 0x4, 0x0 }, { 0x6, 0xC, 0x0, 0x0 }, { 0x4, 0x6, 0x2, 0x0 }, { 0x6, 0xC, 0x0, 0x0 } },
      { { 0x4, 0xC, 0x8, 0x0 }, { 0xC, 0x6, 0x0, 0x0 }, { 0x2, 0x6, 0x4, 0x0 }, { 0xC, 0x6, 0x0, 0x0 } }
    };

    unsigned int SPEED = 1500; // ms
    unsigned long ts;

    uint8_t rows;
    uint8_t cols;

    int curCol;
    uint8_t curRow;
    uint8_t curRot;
    
    Tetris() {};
    ~Tetris() {
      delete [] pixels;
    };
    Tetris(uint8_t, uint8_t);


    uint8_t * getPixels();
    void init(LedMatrix *,  void (*points)(), void (*end)());
    void clear();
    void sync();
    bool fall();
    void newGame();
    void newRow();

    void left();
    void right();
    void rotate(int);
    void down();
  
    bool playable();
    void addPoints(uint8_t);
    uint32_t getPoints();
    uint32_t getHighScore();
    void setHighScore(uint32_t);

    static int lowestBitPosition(uint8_t * _sprite) {
      int pos = -1;
      for (uint8_t r = 0; r < SPRITESIZE; r++) {
        for (uint8_t p = 0; p < 4; p++) {
          if (bitcheck(_sprite[r], p) && (p < pos || pos == -1)) pos = p; 
        }
      }
      return pos;
    }

    static int uppestBitPosition(uint8_t * _sprite) {
      int pos = -1;
      for (uint8_t r = 0; r < SPRITESIZE; r++) {
        for (uint8_t p = 0; p < 4; p++) {
          if (bitcheck(_sprite[r], p) && (p > pos || pos == -1)) pos = p; 
        }
      }
      return pos;
    }

  private:
    uint8_t *pixels;
    LedMatrix * matrix;
    
    bool isPlaying = false;
    uint32_t points;
    uint32_t highscore;
    uint8_t curElementIdx;
    uint8_t loop;

    bool forceFall = false;

    bool decreaseRow();
    void drawPixels(uint8_t *);
    bool hit(int, int, uint8_t);
    static uint8_t spriteHeight(uint8_t *);
    void freeze();
    uint8_t * getSprite();
    void removeRow(uint8_t *, uint8_t);
    void accelerate();
};

#endif