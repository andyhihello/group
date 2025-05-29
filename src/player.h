#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "texture.h"
#include "main.h"

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
    // 移動與位置
    Vector2 position;          // 角色座標位置
    Rectangle hitbox;          // 碰撞箱（更新碰撞邏輯時要同步）
    float speed;               // 水平移動速度
    float velocityY;           // 垂直速度（用於跳躍與重力）
    bool isJumping;            // 是否正在跳躍
    bool isGrounded;           // 是否接觸地面（跳躍控制用）
    Vector2 velocity;          // 移動向量
    float jumpForce;           // 跳躍力道

    // 生命與狀態
    int hp;                    // 血量
    bool dead;                 // 是否死亡
    bool invincible;           // 是否處於無敵狀態
    float invincibleTimeLeft;  // 無敵剩餘時間
    float invincibleDuration;  // 無敵持續時間
    float invincibleCooldown;  // 無敵技能冷卻時間
    float invincibleCooldownLeft; // 無敵技能剩餘冷卻時間

    // 金錢與升級
    int coin;                      // 擁有金幣數

    int upgrade_reload_cost;       // 換彈升級花費
    int upgrade_ammo_cost;         // 子彈容量升級花費
    int upgrade_invincible_cost;   // 無敵冷卻縮短升級花費

    float reload_upgrade_ratio;    // 每次升級換彈速度提升倍率（<1）
    int ammo_upgrade_amount;       // 每次升級子彈數增加量
    float invincible_upgrade_ratio;// 每次升級冷卻縮短倍率（<1）

    int reload_upgrade_times;      // 換彈升級次數（最多 2 次）
    int ammo_upgrade_times;        // 子彈容量升級次數（最多 2 次）
    int invincible_upgrade_times;  // 無敵冷卻升級次數（最多 2 次）

    // 子彈與攻擊
    Bullet bullets[MAX_BULLETS];   // 子彈陣列
    int damage;                    // 每發子彈傷害
    int ammo;                      // 當前子彈數
    int maxAmmo;                   // 最大彈匣容量
    bool reloading;               // 是否正在換彈
    float reloadtime;             // 換彈總時間
    float reloadTimeLeft;         // 換彈剩餘時間
    float shootEffectTimer;       // 開火閃光特效倒數

    float weaponDisableTimer;     // 武器禁用剩餘時間
    bool weaponDisabled;          // 是否無法射擊

    int originalDamage;
    float originalSpeed;
    float debuffTimer;

    // 動畫與方向
    int currentFrame;             // 當前動畫幀
    float frameTimer;             // 幀時間控制器
    bool isRunning;               // 是否奔跑中
    bool facingRight;             // 朝向（true: 右）

    // 控制與干擾狀態
    bool controlsReversed;        // 控制反轉（來自BOSS技能）
    float controlReverseTimer;    // 控制反轉剩餘時間

    // 教學與關卡
    int tutorial;                 // 教學階段（0: 無教學）
    int stage;                    // 關卡階段（1: 普通, 2: BOSS）
    bool stageChanged;  // 添加stageChanged标志

    float weakenTimer;      // 虛弱效果計時器
    float controlTimer;     // 控制反轉計時器
    float weaponTimer;      // 武器禁用計時器

} Player;

void player_hitbox(Player *player); // 玩家碰撞箱
void player_init(Player *player);            // 玩家初始化（載入圖片、設定初始狀態）
void player_update(Player *player,float deltaTime);            // 玩家移動與跳躍控制（含動畫）
void player_reload(Player *player);          // 玩家換彈邏輯（倒數 reload 時間）
void player_UI(Player *player);              // 畫面顯示玩家子彈數、reload 倒數
void player_attack(Player *player, Camera2D camera); // 玩家射擊（子彈產生）
void player_skillupgrade(Player *player,GameSounds *sounds);
void player_drawbullet(Player *player, Camera2D camera ,GameTextures *textures);              // 子彈移動與繪製
void player_draw(Player *player, GameTextures *textures);     // 玩家角色繪製（靜止 / 跑步動畫）
void player_drawhitbox(Player *player);      //debug
void player_dead(Player *player,GameState *currentGameState,bool *Isinit,GameSounds *sounds,bool *deadsound); // 死亡復活或回menu
void player_deadUI(Player *player); //死亡介面

#endif