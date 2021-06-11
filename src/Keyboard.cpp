#include <Arduino.h>
#include "Keyboard.h"

#define BOUNCE_TIMEOUT 100
#define REGISTER_CALLBACK(e,p) register_callback(e, (callback_void_t)p)
#define GET_CALLBACK(e,T) ((T)callback_func_ptrs[e])
#define SECRET_SEQ = {LEFT, LEFT, RIGHT, RIGHT, DOWN, ROTATE }

enum DIRECTIONS {
  LEFT,
  RIGHT,
  DOWN,
  ROTATE,
  MAX_DIRECTIONS
};

typedef void (*callback_void_t)(void);

static int buttons_flag[10] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
static uint32_t buttons_bounce_ts[10];
static callback_void_t callback_func_ptrs[10];

void register_callback(uint8_t _pin, callback_void_t _ptr) {
    callback_func_ptrs[_pin] = _ptr;
}

Keyboard::Keyboard(uint8_t _pinLeft, uint8_t _pinRight, uint8_t _pinRotate, uint8_t _pinDown) {
  this->pinLeft = _pinLeft;
  pinMode(this->pinLeft, INPUT);
  buttons_bounce_ts[this->pinLeft] = millis();
  this->pinRight = _pinRight;
  pinMode(this->pinRight, INPUT);
  buttons_bounce_ts[this->pinRight] = millis();
  this->pinRotate = _pinRotate;
  pinMode(this->pinRotate, INPUT);
  buttons_bounce_ts[this->pinRotate] = millis();
  this->pinDown = _pinDown;
  pinMode(this->pinDown, INPUT);
  buttons_bounce_ts[this->pinDown] = millis();
}

void Keyboard::setup(callback_void_t leftCallback, callback_void_t rightCallback, callback_void_t rotateCallback, callback_void_t downCallback ) {
  REGISTER_CALLBACK(this->pinLeft, leftCallback);
  REGISTER_CALLBACK(this->pinRight, rightCallback);
  REGISTER_CALLBACK(this->pinRotate, rotateCallback);
  REGISTER_CALLBACK(this->pinDown, downCallback);
}

void Keyboard::sync() {
  this->onPressed(this->pinLeft);
  this->onPressed(this->pinRight);
  this->onPressed(this->pinRotate);
  this->onPressed(this->pinDown);
}

void Keyboard::onPressed(uint8_t _pin) {
  uint32_t now = millis();
  int flag = digitalRead(_pin);
  // if (flag != buttons_flag[_pin]) {
    // buttons_flag[_pin] = 1 - buttons_flag[_pin];
    buttons_flag[_pin] = !flag;
    if (buttons_flag[_pin] && (now - buttons_bounce_ts[_pin]) > BOUNCE_TIMEOUT) {
      buttons_bounce_ts[_pin] = now;
      GET_CALLBACK(_pin, callback_void_t)();
    }
  // }
}

