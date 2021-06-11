#include "LedMatrix.h"

LedMatrix::LedMatrix(int _pinCLK, int _pinCS, int _pinDIN, int _nbRows, int _nbCols, int _nbMAX7219) {
  Serial.begin(9600);

  nbRows = _nbRows;
  nbColumns = _nbCols;
  nbMAX7219 = _nbMAX7219;
  //
  pinCLK = _pinCLK;
  pinCS = _pinCS;
  pinDIN = _pinDIN;

  // init pixel array
  // display = new uint8_t[nbRows*nbMAX7219];

  // init LED controller
  lc = LedControl_SW_SPI();
}

/**
* LedMatrix initialization
*/
void LedMatrix::init() {
  lc.begin(pinDIN, pinCLK,pinCS, nbMAX7219);

  for (int m = 0; m < nbMAX7219; m++) {
    lc.setIntensity(m, 1);
    lc.shutdown(m, false);
  }
}

/**
 * LedMatrix sync (refresh)
*/
int LedMatrix::sync(uint8_t * pixels) {
  // uint8_t LedMatrix[nbRows*nbMAX7219];
  // memcpy(LedMatrix, display, nbRows*nbMAX7219);
  
  // if (!moved) {
  //   memcpy(display, LedMatrix, nbRows*nbMAX7219);
  //   if (tetris.playable()) {
  //     tetris.newRow();
  //   } else {
  //     tetris.newGame();
  //     Serial.println("Game ended");
  //   }
  // }

  this->draw(pixels);
}

/**
 * set all max7219 led status
 */
void LedMatrix::draw(uint8_t *_matrix) {
  for (int r = 0; r < nbRows; r++) {
    for(int c = 0; c < nbColumns; c++) {
      for(int m = 0; m < nbMAX7219; m++) {
        uint8_t rr = nbRows * m + r;
        bool state = 0 != (_matrix[rr] & (1 << c));
        lc.setLed(m,r,c, state);
      }
    }
  }
}