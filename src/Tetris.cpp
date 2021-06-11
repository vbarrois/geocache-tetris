#include "Constants.h"
#include "Tetris.h"

#define POINTS_1ROW 40
#define POINTS_2ROW 100
#define POINTS_3ROW 300
#define POINTS_TETRIS 1200

#define REGISTER_TETRISCALLBACK(e,p) register_tetriscallback(e, (callback_tetris_t)p)
#define GET_TETRISCALLBACK(e,T) ((T)callback_tetrisfunc_ptrs[e])

typedef void (*callback_tetris_t)(void);

enum TETRIS_EVENT {
  POINTS,
  END,
  MAX_TETRISEVENT
};

static callback_tetris_t callback_tetrisfunc_ptrs[MAX_TETRISEVENT];

void register_tetriscallback(TETRIS_EVENT _event, callback_tetris_t _ptr) {
    callback_tetrisfunc_ptrs[_event] = _ptr;
}

Tetris::Tetris(uint8_t _rows, uint8_t _cols) {
  this->rows = _rows;
  this->cols = _cols;
  this->pixels = (uint8_t *)malloc(sizeof(uint8_t) * _rows); // columns are manage with uint8_t bits
  this->ts = millis(); // init timestamp
  this->highscore = 0;
  this->clear();
}

/**
 * init callbacks
 */
 void Tetris::init(LedMatrix *_matrix, callback_tetris_t _points, callback_tetris_t _end) {
  this->matrix = _matrix;
  REGISTER_TETRISCALLBACK(POINTS, _points);
  REGISTER_TETRISCALLBACK(END, _end);
}

/**
 * Create a new game
 */
void Tetris::newGame() {
  this->points = 0;
  this->clear();
  this->newRow();
  this->SPEED = START_SPEED;
  this->isPlaying = true;
}

/**
 * Calculate the height of a sprite
 */
uint8_t Tetris::spriteHeight(uint8_t * _sprite) {
  uint8_t h = 0;
  for (uint8_t r = 0; r < SPRITESIZE; r++ ) {
    if (_sprite[r] > 0) h = r + 1;
  }
  return h;
}

/**
 * Return the active sprite
 */
uint8_t * Tetris::getSprite() {
  return this->sprites[this->curElementIdx][this->curRot];
}

/**
 * Move to left
 */
void Tetris::left() {
  if (!this->hit(0, 1, this->curRot)) this->curCol++;
}

/**
 * Move to right
 */
void Tetris::right() {
  if (!this->hit(0, -1, this->curRot)) this->curCol--;
}

/**
 * Move down
 */
void Tetris::down() {
  this->forceFall = true;
}

/**
 * Rotate sprite
 */
void Tetris::rotate(int _dir) {
  uint8_t rot = this->curRot + _dir;
  if (rot < 0) rot = 3;
  else if (rot > 3) rot = 0;

  if (!this->hit(0, 0, rot)) {
    this->curRot = rot;
  }
}

/**
 * Points management
 */
void Tetris::addPoints(uint8_t _nbrows) {
  switch (_nbrows) {
    case 1:
      this->points += POINTS_1ROW;
      break;
    case 2:
      this->points += POINTS_2ROW;
      break;
    case 3:
      this->points += POINTS_3ROW;
      break;
    case 4:
      this->points += POINTS_TETRIS;
      break;
  }
  
  if (_nbrows > 0) {
    if (this->points > this->highscore) {
      this->highscore = this->points;
    }
    this->accelerate();
    GET_TETRISCALLBACK(POINTS, callback_tetris_t)();
  }
}

/**
 * Return the number of points
 */
uint32_t Tetris::getPoints() {
  return this->points;
}

/**
 * Return the high score
 */
uint32_t Tetris::getHighScore() {
  return this->highscore;
}

void Tetris::setHighScore(uint32_t _hs) {
  this->highscore = _hs;
}

/**
 * New falling row
 * Start from top of the screen
 */
void Tetris::newRow() {
  this->loop = 0; // loop counter

  this->curElementIdx = random(sizeof(sprites) / sizeof(sprites[0]));
  this->curRot = 0; // init rotation position

  this->curRow = this->rows - Tetris::spriteHeight(this->getSprite()); // on top of the sprite

  uint8_t width = Tetris::uppestBitPosition(this->getSprite()) - Tetris::lowestBitPosition(this->getSprite()) + 1;
  int centeredCol = (this->cols / 2) + (SPRITESIZE - (Tetris::uppestBitPosition(this->getSprite()) + 1)) + (width / 2);
  this->curCol = centeredCol; // center

  uint8_t delRows = 0;
  uint8_t rowIndex = 0;
  while (rowIndex < this->rows) {
    if (this->pixels[rowIndex] == 0xFF) {
      delRows++;
      this->removeRow(this->pixels, rowIndex);
    } else {
      rowIndex++;
    }
  }

  this->addPoints(delRows);
}

/**
 * Decrease the falling row
 * if bottom is reached, set new row
 */
bool Tetris::decreaseRow() {
  if (this->curRow == 0) {
    return false;
  } else {
    this->curRow--;
    return true;
  }
}

/**
 * Clear all pixels
 */
void Tetris::clear() {
  for (uint8_t r = 0; r < this->rows; r++) {
    this->pixels[r] = 0x00;
  }
}

void Tetris::freeze() {
  for (uint8_t r = 0; r < this->rows; r++) {
    this->pixels[r] = 0xFF;
  }
}

/**
 * Sprite colision detection
 */
bool Tetris::hit(int _rowDecay, int _colDecay, uint8_t _rotation) {
  bool isHit = false;
  
  uint8_t lb = Tetris::lowestBitPosition(this->sprites[this->curElementIdx][_rotation]);
  int hb = Tetris::uppestBitPosition(this->sprites[this->curElementIdx][_rotation]);
    
  if (_colDecay <= 0) {
    if (SPRITESIZE - lb > this->curCol + _colDecay) isHit = true; // right
  }
  if (_colDecay >= 0) {
    if (this->curCol + _colDecay > this->cols - 1 + SPRITESIZE - hb) isHit = true; // left
  }

  // hit displayed items ?
  uint8_t r = 0;
  while (!isHit && r < SPRITESIZE) {
    int slide = this->curCol - SPRITESIZE + _colDecay;
    uint8_t spr;
    if (slide >= 0) {
      spr = this->sprites[this->curElementIdx][_rotation][r] << slide;
    } else if (slide < 0) {
      spr = this->sprites[this->curElementIdx][_rotation][r] >> -slide;
    }

    if ((this->pixels[this->curRow + r + _rowDecay] & ~spr) != this->pixels[this->curRow + r + _rowDecay]) isHit = true;
    r++;
  }
  return isHit;
}

/**
 * Timestamp update and falling row speed
 */
void Tetris::sync() {
  uint8_t screen[this->rows];
  memcpy(screen, this->pixels, this->rows);
  this->drawPixels(screen);
  
  if (this->isPlaying) {
    bool updated = true;
    unsigned long m = millis();
    if (this->forceFall || (m - this->ts > SPEED)) {
      this->ts = m;
      this->forceFall = false;
      updated = this->fall();
      if (!updated) {
        if (this->playable()) {
          memcpy(this->pixels, screen, this->rows); // save screen
          this->newRow();
        } else {
          this->isPlaying = false;
          this->freeze();
          GET_TETRISCALLBACK(END, callback_tetris_t)();
        }
      }
    }
  }

  matrix->sync(screen);
}

/**
 * Freeze last sprite position on display
 */
bool Tetris::fall() {
  bool moved = false;
  if (!this->hit(-1, 0, this->curRot)) {
    moved = this->decreaseRow();
  }
  
  if (moved) {
    this->loop++;
  }

  return moved;
}

/**
 * Check if game has ended
 */
bool Tetris::playable() {
  return this->loop > 0;
}

void Tetris::removeRow(uint8_t * _display, uint8_t _index) {
  for (uint8_t r =_index; r < this->rows - 1; r++) {
    _display[r] = _display[r+1];
  }
}

/**
 * write the pixels states
 */
void Tetris::drawPixels(uint8_t * _display) {
  int slide = this->curCol - SPRITESIZE;
  for (uint8_t cpt = 0; cpt < SPRITESIZE; cpt++) {
    uint8_t sr = this->curRow + cpt;
    if (sr < this->rows) {
      if (slide >= 0) {
        _display[sr] |= this->getSprite()[cpt] << slide;
      } else if (slide < 0) {
        _display[sr] |= this->getSprite()[cpt] >> -slide;
      }
    }
  }
}

/**
 * Acceleration after each row completion
 */
void Tetris::accelerate() {
  if ((this->SPEED - ACCELERATION_SPEED) < MAX_SPEED) {
    this->SPEED = MAX_SPEED;
  } else {
    this->SPEED -= ACCELERATION_SPEED;
  }
}