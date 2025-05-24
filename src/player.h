#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "texture.h"

#define playerWidth 140
#define playerHeight 310
#define playerXoffset 50
#define playerYoffset 15
#define MAX_BULLETS 100 // 螢幕可容納最多子彈
#define SIZE_BULLET 50
#define BULLET_SPEED 20
#define GRAVITY 30.0f   //  模擬重力
#define JUMP_STRENGTH -900.0f  //  跳躍的初始速度
#define GROUND_Y 520
#define BULLET_WIDTH 10
#define BULLET_HEIGHT 10
#define WEAPON_DISABLE_TIME 2.0f  // 武器禁用時間（秒）
#define shootEffectDuration 0.1f

typedef struct {
    Vector2 position;
    Vector2 startPosition;
    Rectangle hitbox;
    Vector2 speed;
    bool active;
} Bullet;

typedef struct {
    Vector2 position;         // 角色的位置
    Rectangle hitbox;         // 碰撞
    float speed;              // 角色移動速度
    float velocityY;          // 垂直方向速度，用於跳躍和重力影響
    bool isJumping;           // 是否正在跳躍中

    int hp;
    int coin;
    bool invincible;            // 是否無敵中
    float invincibleTimeLeft;   // 無敵剩餘時間
    float invincibleDuration;   // 每次無敵持續時間
    float invincibleCooldown;   // 冷卻時間
    float invincibleCooldownLeft; // 冷卻剩餘時間

    Bullet bullets[MAX_BULLETS];
    int damage;
    int ammo;                 // 剩餘的子彈數量
    int maxAmmo;              // 彈匣容量
    bool reloading;           // 是否正在換彈中
    float reloadtime;         // 換彈總時間（單位：秒）
    float reloadTimeLeft;     // 換彈剩餘時間

    int currentFrame;         // 當前跑步動畫播放到第幾幀
    float frameTimer;         // 控制動畫播放速度
    bool isRunning;           // 是否奔跑中
    bool facingRight;         // 角色朝向（true：朝右，false：朝左）

    int tutorial;
    int stage;

    int upgrade_reload_cost;      // 換彈速度升級花多少coin
    int upgrade_ammo_cost;         // 子彈容量升級花多少coin
    int upgrade_invincible_cost;   // 無敵CD升級花多少coin

    float reload_upgrade_ratio;    // 換彈速度每次升級的倍率（例如0.9 = 變快10%）
    int ammo_upgrade_amount;        // 子彈容量每次增加多少
    float invincible_upgrade_ratio; // 無敵CD每次縮短倍率

    int reload_upgrade_times;     // 換彈已升級幾次
    int ammo_upgrade_times;       // 容量已升級幾次
    int invincible_upgrade_times; // 無敵CD已升級幾次

    bool controlsReversed;     // 控制是否反轉
    float controlReverseTimer; // 控制反轉計時器

    bool isGrounded;
    Vector2 velocity;
    float jumpForce;

    float weaponDisableTimer;  // 武器禁用倒計時
    bool weaponDisabled;       // 武器是否被禁用

    float shootEffectTimer;

    float debuffTimer;  // 用於追蹤debuff持續時間
    float originalDamage;  // 保存原始攻击力
    float originalSpeed;   // 保存原始移动速度
} Player;

void player_hitbox(Player *player);
void player_init(Player *player);            // 玩家初始化（載入圖片、設定初始狀態）
void player_move(Player *player,float deltaTime);            // 玩家移動與跳躍控制（含動畫）
void player_reload(Player *player);          // 玩家換彈邏輯（倒數 reload 時間）
void player_UI(Player *player);              // 畫面顯示玩家子彈數、reload 倒數
void player_attack(Player *player, Camera2D camera); // 玩家射擊（子彈產生）
void player_skillupgrade(Player *player);
void player_drawbullet(Player *player, Camera2D camera ,GameTextures *textures);              // 子彈移動與繪製
void player_draw(Player *player, GameTextures *textures);     // 玩家角色繪製（靜止 / 跑步動畫）
void player_drawhitbox(Player *player);      //debug
void player_update(Player *player);  // 添加這行

#endif