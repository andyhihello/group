#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

#define playerWidth 140
#define playerHeight 310
#define playerXoffset 50
#define playerYoffset 15
#define MAX_BULLETS 100 // 螢幕可容納最多子彈
#define SIZE_BULLET 10
#define BULLET_SPEED 20
#define GRAVITY 0.5f   //  模擬重力
#define JUMP_STRENGTH -13.0f  //  跳躍的初始速度
#define GROUND_Y 520

typedef struct {
    Vector2 position;
    Vector2 speed;
    bool active;
} Bullet;

typedef struct {
    Vector2 position;         // 角色的位置
    Rectangle hitbox;         // 碰撞
    float speed;              // 角色移動速度
    float velocityY;          // 垂直方向速度，用於跳躍和重力影響
    bool isJumping;           // 是否正在跳躍中

    Bullet bullets[MAX_BULLETS];
    int ammo;                 // 剩餘的子彈數量
    int maxAmmo;              // 彈匣容量
    bool reloading;           // 是否正在換彈中
    float reloadtime;         // 換彈總時間（單位：秒）
    float reloadTimeLeft;     // 換彈剩餘時間

    Texture2D stand;          // 角色靜止圖
    Texture2D runFrames[9];   // 角色奔跑動畫圖
    int currentFrame;         // 當前跑步動畫播放到第幾幀
    float frameTimer;         // 控制動畫播放速度
    bool isRunning;           // 是否奔跑中
    bool facingRight;         // 角色朝向（true：朝右，false：朝左）

    int tutorial;
    int stage;
} Player;

void player_hitbox(Player *player);
void player_init(Player *player);            // 玩家初始化（載入圖片、設定初始狀態）
void player_move(Player *player);            // 玩家移動與跳躍控制（含動畫）
void player_reload(Player *player);          // 玩家換彈邏輯（倒數 reload 時間）
void player_UI(Player *player);              // 畫面顯示玩家子彈數、reload 倒數
void player_attack(Player *player, Camera2D camera); // 玩家射擊（子彈產生）
void player_drawbullet(Player *player, Camera2D camera);              // 子彈移動與繪製
void player_draw(Player *player);            // 玩家角色繪製（靜止 / 跑步動畫）
void player_unload(Player *player);          // 釋放玩家資源（貼圖）
void player_drawhitbox(Player *player);      //debug

#endif