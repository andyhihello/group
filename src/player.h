#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#define MAX_BULLETS 50 // 螢幕可容納最多子彈
#define BULLET_SPEED 20
#define GRAVITY 0.5f   //  模擬重力
#define JUMP_STRENGTH -10.0f  //  跳躍的初始速度
#define GROUND_Y 520



typedef struct {
    Vector2 position; //角色位置
    float speed;      //行走速度
    float velocityY;  // 用來處理跳躍
    bool isJumping;   // 防止二段跳
    int ammo;               // 當前子彈數量
    int maxAmmo;            // 當前最大子彈數量
    bool reloading;         //是否在reload
    float reloadtime;       //換彈所需時間
    float reloadTimeLeft;   // 換彈倒數時間
    Texture2D stand;   //圖片
    Texture2D runFrames[9];
    int currentFrame;
    float frameTimer;
    bool isRunning;
    bool facingRight;
} Player;

typedef struct {
    Vector2 position;
    Vector2 speed;
    bool active;
} Bullet;

void Initplayer(Player *player);
void Moveplayer(Player *player);
void reload(Player *player);         // 換彈邏輯
void attrackdata(Player *player);    // 顯示玩家狀態
void playerattrack(Player *player);  // 玩家射擊與狀態更新
void drawplayerbullet(); //子彈繪製



#endif