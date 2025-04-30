#ifndef MAIN_H
#define MAIN_H

#include "raylib.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

typedef struct {
    Texture2D patrolFrames[5];
    Texture2D chaseFrames[9];
    Texture2D attackFrames[4];
    Texture2D laserFrame;
} enemyTextures;

typedef enum { MENU, GAME, SETTINGS } GameState;

#define screenWidth 1600
#define screenHeight 900

// 第一阶段
#define stage1backgroundCount 5
#define stage1pictureWidth 3000
#define stage1Width (stage1backgroundCount * stage1pictureWidth)

// 第二阶段
#define stage2backgroundCount 2
#define stage2pictureWidth 3000
#define stage2Width (stage2backgroundCount * stage2pictureWidth)  // 改为动态计算

#define MAX_DRONES 7



#endif