#ifndef ENEMY_H
#define ENEMY_H

#include <stdbool.h>
#include <time.h>
#include "raylib.h"
#include "player.h"
#include "texture.h"
#include "main.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


/// 敵人種類
typedef enum {
    SECURITY_DRONE,   // 安全無人機
    ROBOT_SOLDIER     // 機器士兵
} EnemyType;

/// 攻擊模式
typedef enum {
    LASER_SHOT,       // 雷射攻擊
    SCATTER_SHOT      // 散射子彈攻擊
} AttackMode;

/// 敵人狀態（巡邏、追擊、攻擊）
typedef enum {
    PATROL,
    CHASE,
    ATTACK
} EnemyState;

/// 雷射攻擊的資料結構
typedef struct {
    Vector2 start;        // 雷射起點
    Vector2 end;          // 雷射終點
    float damage;         // 傷害值
    float duration;       // 持續時間（秒）
    bool active;          // 是否啟動中
    float timeActive;     // 已啟動的時間
} LaserShot;

/// 散射子彈的資料結構
typedef struct {
    Vector2 position;     // 子彈位置
    Vector2 direction;    // 子彈方向
    EnemyState state;  
    float speed;          // 子彈速度
    float damage;         // 傷害值
    bool active;          // 是否啟動中
    Rectangle hitbox; 
    float lifetime;
    
} ScatterBullet;


/// 安全無人機（Drone 敵人）
typedef struct {
    bool active;
    Vector2 position; 
    Rectangle hitbox;              // 無人機碰撞箱
    Rectangle laserhitbox;         // 雷射碰撞箱
    float health;                  // 血量
    float attackCooldown;          // 攻擊冷卻
    float timeSinceLastAttack;     // 上次攻擊後的時間
    LaserShot laser;               // 雷射攻擊資料
    EnemyType type;                // 敵人類型
    bool facingRight;              // 朝向
    EnemyState state;              // 狀態
    int currentFrame;              // 當前動畫幀
    float frameTimer;              // 幀時間控制
    int coin;                      // 擊敗後掉落金幣
} Drone;

/// 機器士兵（敵人）
typedef struct {
    bool active;
    Rectangle hitbox;
    Vector2 position;           // 士兵位置
    float health;               // 血量
    float attackCooldown;       // 攻擊冷卻時間（秒）
    float timeSinceLastAttack;  // 距離上次攻擊經過的時間
    ScatterBullet bullets[3];   // 三發散射子彈
    float maxDamageDistance;    // 最大傷害距離（近距離）
    float minDamageDistance;    // 最小傷害距離（遠距離）
    float maxDamage;            // 最大傷害值
    float minDamage;            // 最小傷害值
    EnemyType type;             // 敵人類型
} Soldier;



// 工具函式

/// 計算兩點距離
float enemy_distance(Vector2 p1, Vector2 p2);

/// 計算水平距離
float enemy_distanceX(Vector2 p1, Vector2 p2);

/// 回傳無人機碰撞箱
Rectangle enemy_Dronehitbox(Drone *drone);

/// 向量正規化
Vector2 enemy_normalizeVector(Vector2 v);

// 初始化

/// 初始化無人機
void enemy_initDrone(Drone* drone);

/// 初始化士兵
void enemy_initSoldier(Soldier* soldier);

// 攻擊

/// 無人機發射雷射
void enemy_DroneFireLaser(Drone* drone, Player *player);

/// 士兵發射散射子彈
void enemy_SoldierFireScatterShot(Soldier* soldier, Player *player);

/// 無人機雷射攻擊玩家
void enemy_laserDamagePlayer(Drone *drone, Player *player, GameSounds *sounds);

/// 散射子彈攻擊玩家
void enemy_bulletDamagePlayer(ScatterBullet* bullet, Player *player, Soldier* soldier);

/// 玩家攻擊士兵
void enemy_bulletDamageSoldier(Player *player, Soldier* soldier, GameSounds *sounds);

/// 計算子彈距離傷害
float enemy_calculateDistanceBasedDamage(ScatterBullet* bullet, Player *player, Soldier* soldier);

// 更新

/// 更新無人機
void enemy_updateDrone(Drone* drone, Player *player, float deltaTime);

/// 更新士兵
void enemy_updateSoldier(Soldier* soldier, Player *player, float deltaTime);

/// 玩家攻擊無人機
void enemy_bulletDamageDrone(Player *player, Drone *drone, GameSounds *sounds);

// 繪製

/// 繪製士兵子彈
void enemy_drawSoldierBullets(Soldier* soldier, GameTextures *textures);

/// 繪製士兵
void enemy_drawSoldier(Soldier* soldier, GameTextures *textures);

/// 繪製無人機
void enemy_drawDrone(Drone* drone, GameTextures *textures);

/// 繪製無人機雷射
void enemy_drawLaser(Drone* drone, GameTextures *textures);

/// DEBUG - 繪製碰撞箱
void enemy_hitbox(Drone* drone, Soldier* soldier, GameTextures* textures);

#endif