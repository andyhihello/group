#ifndef ENEMY_H
#define ENEMY_H

#include <stdbool.h>
#include <time.h>

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

/// 二維座標
typedef struct {
    float x;
    float y;
} Vector2;

/// =============================
/// 攻擊結構體定義
/// =============================

/// 雷射攻擊的資料結構
typedef struct {
    Vector2 start;        // 雷射起點
    Vector2 end;          // 雷射終點
    float damage;         // 傷害值
    float width;          // 雷射寬度
    float duration;       // 持續時間（秒）
    bool active;          // 是否啟動中
    float timeActive;     // 已啟動的時間
} LaserShot;

/// 散射子彈的資料結構
typedef struct {
    Vector2 position;     // 子彈位置
    Vector2 direction;    // 子彈方向
    float speed;          // 子彈速度
    float damage;         // 傷害值
    bool active;          // 是否啟動中
} ScatterBullet;


/// 安全無人機（敵人）
typedef struct {
    Vector2 position;           // 無人機位置
    float health;               // 血量
    float attackCooldown;       // 攻擊冷卻時間（秒）
    float timeSinceLastAttack;  // 距離上次攻擊經過的時間
    LaserShot laser;            // 雷射攻擊資料
    EnemyType type;             // 敵人類型
} SecurityDrone;

/// 機器士兵（敵人）
typedef struct {
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
} RobotSoldier;



/// 工具函式（計算用）

/// 計算兩點之間的距離
float enemy_distance(Vector2 p1, Vector2 p2);

/// 將一個向量正規化（單位向量）
Vector2 enemy_normalizeVector(Vector2 v);


/// 初始化無人機
void enemy_initSecurityDrone(SecurityDrone* drone, float x, float y);

/// 初始化機器士兵
void enemy_initRobotSoldier(RobotSoldier* soldier, float x, float y);


/// 無人機發射雷射
void enemy_securityDroneFireLaser(SecurityDrone* drone, Vector2 playerPosition);

/// 機器士兵發射散射子彈
void enemy_robotSoldierFireScatterShot(RobotSoldier* soldier, Vector2 playerPosition);


/// 更新無人機行動（攻擊冷卻、雷射狀態）
void enemy_updateSecurityDrone(SecurityDrone* drone, Vector2 playerPosition, float deltaTime);

/// 更新機器士兵行動（攻擊冷卻、子彈更新）
void enemy_updateRobotSoldier(RobotSoldier* soldier, Vector2 playerPosition, float deltaTime);



/// 判斷雷射是否擊中玩家
bool enemy_checkLaserHit(LaserShot* laser, Vector2 playerPosition, float playerRadius);

/// 判斷子彈是否擊中玩家
bool enemy_checkBulletHit(ScatterBullet* bullet, Vector2 playerPosition, float playerRadius);



/// 根據距離計算子彈對玩家的傷害（越近傷害越高）
float enemy_calculateDistanceBasedDamage(ScatterBullet* bullet, Vector2 playerPosition, RobotSoldier* soldier);

#endif 
