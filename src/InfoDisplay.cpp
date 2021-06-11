#include <Arduino.h>
#include "Constants.h"
#include "InfoDisplay.h"

InfoDisplay::InfoDisplay(uint8_t _addr, uint8_t _rowChars, uint8_t _rows): LiquidCrystal_I2C(_addr, _rowChars, _rows) {
  this->address = _addr;
  this->rowChars = _rowChars;
  this->nbRows = _rows;
  this->points = 0;
  this->highscore = 0;
  this->opencount = 0;
}

void InfoDisplay::start() {
  this->init();
  this->backlight();
}

void InfoDisplay::setMode(MODES _mode) {
  this->currentMode = _mode;

  unsigned long ts = millis();
  for (volatile uint8_t row = 0; row < this->nbRows; row++) {
    this->rowsPOS[this->currentMode][row] = 0;
    this->rowsTS[this->currentMode][row] = ts;
    this->display(row);
  }
}

void InfoDisplay::sync() {
  volatile unsigned long now = millis();
  for (volatile uint8_t row = 0; row < this->nbRows; row++) {
    if (now - this->rowsTS[this->currentMode][row] > this->rowsTO[this->currentMode][row]) {
      this->rowsTS[this->currentMode][row] = now;
      if (this->swipe(row)) {
        this->display(row);
      }
    }
  }
}

void InfoDisplay::setPoints(uint32_t _points) {
  this->points = _points;
  this->display(1); // refresh points row
}

void InfoDisplay::setHighScore(uint32_t _points) {
  this->highscore = _points;
  this->display(1); // refresh highscore row
}

void InfoDisplay::setOpenCount(uint32_t _count) {
  this->opencount = _count;
}

void InfoDisplay::display(uint8_t _row) {
  switch(this->currentMode) {
    case DEBUG:
      InfoDisplay::showDebug(_row);
      break;
    case START:
      InfoDisplay::showStart(_row);
      break;
    case ENDGAME:
      InfoDisplay::showEnd(_row);
      break;
    case GAME:
      InfoDisplay::showGame(_row);
      break;
  }
}

void InfoDisplay::showDebug(uint8_t _row) {
  char buffer[17];
  switch (_row) {
    case 0:
      this->setCursor(0, 0);
      this->print("* Maintenance * ");
      break;
    case 1:
      this->setCursor(0, 1);
      switch(this->rowsPOS[this->currentMode][_row]) {
        case 0:
          sprintf(buffer, "Open count %5d", (int)this->opencount);
          break;
        case 1:
          sprintf(buffer, "High score %5d", (int)this->highscore);
          break;
      }
      this->print(buffer);
      break;
  }
}

void InfoDisplay::showStart(uint8_t _row) {
  char buffer[17];
  switch (_row) {
    case 0:
      this->setCursor(0, 0);
      switch(this->rowsPOS[this->currentMode][_row]) {
        case 0: // Geocache name
          sprintf(buffer, "%16s", GC_CODE);
          break;
        case 1:
          sprintf(buffer, "%s", GC_NAME);
          break;
      }
      this->print(buffer);
      break;
    case 1:
      this->setCursor(0, 1);
      switch(this->rowsPOS[this->currentMode][_row]) {
        case 0:
          sprintf(buffer, " Press a button ");
          break;
        case 1:
          sprintf(buffer, "    to start    ");
          break;
        case 2:
          sprintf(buffer, " Open the cache ");
          break;
        case 3:
          sprintf(buffer, "with %4d points", POINTS_TO_OPEN);
          break;
      }
      this->print(buffer);
      break;
  }
}

void InfoDisplay::showEnd(uint8_t _row) {
  switch (_row) {
    case 0:
      this->setCursor(0, 0);
      this->print("* End of game * ");
      break;
    case 1:
      this->setCursor(0, 1);
      this->print("Happy Geocaching");
      break;
  }  
}

void InfoDisplay::showGame(uint8_t _row) {
  char buffer[17];
  switch (_row) {
    case 0:
      this->setCursor(0, 0);
      switch(this->rowsPOS[this->currentMode][_row]) {
        case 0: // Geocache name
          sprintf(buffer, "%16s", GC_CODE);
          break;
        case 1:
          sprintf(buffer, "%s", GC_NAME);
          break;
      }
      this->print(buffer);
      break;
    case 1:
      this->setCursor(0, 1);
      switch(this->rowsPOS[this->currentMode][_row]) {
        case 0:
          sprintf(buffer, "Points: %8d", (uint32_t)this->points);
          break;
        case 1:
          sprintf(buffer, "High score: %4d", (uint32_t)this->highscore);
          break;
      }
      this->print(buffer);
      break;
  }
}

bool InfoDisplay::swipe(uint8_t _row) {
  if (this->rowsMaxModes[this->currentMode][_row] > 1) {
    if (this->rowsPOS[this->currentMode][_row] >= this->rowsMaxModes[this->currentMode][_row] - 1) this->rowsPOS[this->currentMode][_row] = 0;
    else this->rowsPOS[this->currentMode][_row]++;
    // Serial.print("swipe row ");
    // Serial.print(_row);
    // Serial.print(" -> ");
    // Serial.println(this->rowsPOS[this->currentMode][_row]);
    return true;
  } else {
    return false;
  }
}