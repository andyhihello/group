#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#define GRAVITY 0.5f   //  模擬重力
#define JUMP_STRENGTH -10.0f  //  跳躍的初始速度
#define GROUND_Y (screenHeight / 2)

typedef struct {
    Vector2 position;
    float speed;
    float velocityY;  // 用來處理跳躍
    bool isJumping;   // 防止二段跳
    Texture2D texture;
} Player;

typedef struct {
    Vector2 position;
    Vector2 speed;
    bool active;
} Bullet;

void Initplayer(Player *player);
void Moveplayer(Player *player);
void playerattrack(Player *player);

#endif