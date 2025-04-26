#ifndef MAIN_H
#define MAIN_H

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>


#define screenWidth 1600
#define screenHeight 900
#define stage1backgroundCount 5
#define stage1pictureWidth 3000
#define stage1Width (stage1backgroundCount * stage1pictureWidth)

typedef enum { MENU, GAME, SETTINGS } GameState;

#endif