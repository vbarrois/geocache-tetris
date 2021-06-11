#ifndef LEDMATRIX_H
#define LEDMATRIX_H

#include <LedControl_SW_SPI.h>

#define bitset(byte,nbit)   ((byte) |=  (1<<(nbit)))
#define bitclear(byte,nbit) ((byte) &= ~(1<<(nbit)))
#define bitflip(byte,nbit)  ((byte) ^=  (1<<(nbit)))
#define bitcheck(byte,nbit) ((byte) &   (1<<(nbit)))

class LedMatrix {
  public:
    LedMatrix();
    LedMatrix(int _pinCLK, int _pinCS, int _pinDIN, int _nbRows, int _nbCols, int _nbmax7219);
    ~LedMatrix() {
    }

    void init();
    int sync(uint8_t *);
  private:
    // max7219 configuration
    int nbRows;
    int nbColumns;
    int nbMAX7219;
    // Hardware connections
    int pinCLK;
    int pinCS;
    int pinDIN;
    // led controller object
    LedControl_SW_SPI lc;

    void draw(uint8_t *);
    void end();
};

#endif