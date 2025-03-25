#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"


typedef struct {
    Vector2 position; //角色位置
    float speed;      //行走速度
    float velocityY;  // 用來處理跳躍
    bool isJumping;   // 防止二段跳
    int ammo;               // 當前子彈數量
    int maxAmmo;            // 當前最大子彈數量
    bool reloading;
    float reloadtime;
    float reloadTimeLeft;   // 換彈倒數時間
    Texture2D texture;   //圖片
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