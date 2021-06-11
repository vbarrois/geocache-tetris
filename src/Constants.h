
#ifndef CONSTANTS_H
#define CONSTANTS_H

#define POINTS_TO_OPEN 1000
#define ADDR_HIGHSCORE 0 // 0->3
#define ADDR_OPENCOUNT 4 // 4->8

#define START_SPEED 1500
#define ACCELERATION_SPEED 50
#define MAX_SPEED 200

enum MODES {
  DEBUG,
  START,
  GAME,
  ENDGAME,
  MAX_MODES
};

#endif