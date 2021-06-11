#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <LiquidCrystal_I2C.h>

#include "Constants.h"

#define GC_CODE "GC12345"
#define GC_NAME "  >> Tetris <<  "

class InfoDisplay: public LiquidCrystal_I2C {
  public:
    InfoDisplay(uint8_t, uint8_t, uint8_t);
    ~InfoDisplay() {
    };

    enum ROWTYPES {
      TITRE,
      POINTS,
      HIGHSCORE
    };

    void start();
    void sync();

    void setMode(MODES);
    void setPoints(uint32_t);
    void setHighScore(uint32_t);
    void setOpenCount(uint32_t);
  private:
    uint8_t address;
    uint8_t rowChars;
    uint8_t nbRows;

    MODES currentMode;

    uint32_t points;
    uint32_t highscore;
    uint32_t opencount;

    unsigned long rowsTS[MODES::MAX_MODES][2] = { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } };
    unsigned long rowsTO[MODES::MAX_MODES][2] = { { 0, 2000 }, { 6000, 2000 }, { 10000, 3000 }, { 6000, 3000 } }; // Time out per mode / row
    unsigned long rowsMaxModes[MODES::MAX_MODES][2] = { { 1, 2 }, { 2, 4 }, { 2, 2 }, { 1, 1 } }; // number of info per row
    unsigned long rowsPOS[MODES::MAX_MODES][2] = { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } };

    bool swipe(uint8_t);
    void display(uint8_t);

    void showDebug(uint8_t);
    void showStart(uint8_t);
    void showEnd(uint8_t);
    void showGame(uint8_t);
};

#endif
