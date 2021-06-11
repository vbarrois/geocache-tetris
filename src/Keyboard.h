#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

class Keyboard {
  public:
    Keyboard();
    Keyboard(uint8_t, uint8_t, uint8_t, uint8_t);
    ~Keyboard() {
    }
    void setup(void (*left)(), void (*right)(), void (*rotate)(), void (*down)());
    void sync();
  private:
    uint8_t pinLeft;
    uint8_t pinRight;
    uint8_t pinRotate;
    uint8_t pinDown;

    void onPressed(uint8_t);
};

#endif