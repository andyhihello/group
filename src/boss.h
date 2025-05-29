#ifndef BOSS_H
#define BOSS_H

#include "raylib.h"
#include "player.h"
#include <stdio.h>

#define BOSS_WIDTH 200
#define BOSS_HEIGHT 300
#define BOSS_SPEED 3
#define BOSS_ATTACK_FRAME_COUNT 6  // 实际帧数为6（1,3,5,7,9,11）
#define BOSS_ATTACK_RANGE 150.0f  // Boss攻击范围
#define BOSS_ATTACK_COOLDOWN 2.0f
#define BOSS_HEALTH 1000
#define BOSS_ATTACK_DAMAGE 5
#define BOSS_PHASE1_THRESHOLD_DRONE 0.8f  // 新增：80%血量閾值
#define MAX_DATA_WAVES 3
#define DATA_WAVE_SPEED 500.0f
#define DATA_WAVE_COOLDOWN 4.0f
#define DATA_WAVE_DURATION 5.0f
#define DATA_WAVE_RADIUS 20.0f

// 新增無人機相關參數
#define BOSS_DRONE_COUNT 1  // 改為1個無人機
#define BOSS_DRONE_WIDTH 100
#define BOSS_DRONE_HEIGHT 100
#define BOSS_DRONE_SPEED 200.0f
#define BOSS_DRONE_HEALTH 30
#define BOSS_DRONE_DAMAGE 2
#define BOSS_DRONE_ATTACK_RANGE 500.0f  // 增加攻击范围到300
#define BOSS_DRONE_ATTACK_COOLDOWN 1.5f
#define BOSS_DRONE_MOVE_RANGE 300.0f
#define BOSS_DRONE_DIRECTION_CHANGE_TIME 1.0f
#define BOSS_DRONE_DISTANCE_THRESHOLD 50.0f  // 新增：距離判斷的閾值
#define BOSS_DRONE_MIN_Y 100.0f  // 新增：Y軸最小值
#define BOSS_DRONE_MAX_Y 500.0f  // 新增：Y軸最大值
#define BOSS_DRONE_BULLET_SPEED 300.0f
#define BOSS_DRONE_BULLET_RADIUS 5.0f
#define BOSS_DRONE_BULLET_COOLDOWN 3.0f

#define BOSS_PHASE2_THRESHOLD 0.5f  // 50%血量閾值
#define BOSS_PHASE2_HEALTH (BOSS_HEALTH * 0.5f)  // 第二階段觸發血量（50%）
#define ENERGY_PULSE_DAMAGE 10
#define ENERGY_PULSE_DURATION 3.0f  // 脈衝持續時間
#define ENERGY_PULSE_COOLDOWN 20.0f  // 能量脉冲冷却时间
#define ENERGY_PULSE_RADIUS 300.0f  // 脈衝範圍
#define ENERGY_PULSE_SPEED 300.0f   // 能量脉冲扩散速度

#define BOSS_SHIELD_DURATION 5.0f  // 防護罩持續時間
#define BOSS_SHIELD_COOLDOWN 60.0f  // 防護罩冷卻時間
#define BOSS_SHIELD_EXPLOSION_RADIUS 3000.0f  // 爆炸範圍
#define BOSS_SHIELD_DAMAGE_RATIO 1.0f  // 傷害轉換比例

#define BOSS_ACTIVATION_RANGE 800.0f  // 新增：boss激活範圍

// Boss攻击相关常量
#define BOSS_ATTACK_DURATION 0.5f  // 攻击动画持续时间（秒）

typedef struct {
    Vector2 position;
    Vector2 direction;
    bool active;
    float speed;
} DataWave;

// 新增子弹结构体
typedef struct {
    Vector2 position;
    Vector2 direction;
    bool active;
    float speed;
} BossDroneBullet;

// 修改無人機結構體
typedef struct {
    Vector2 position;
    Rectangle hitbox;
    bool active;
    float health;
    float attackTimer;
    bool isAttacking;
    Vector2 direction;  // 移動方向
    float directionTimer;  // 方向計時器
    BossDroneBullet bullet;  // 新增子弹属性
} BossDrone;

typedef struct {
    Vector2 position;
    Rectangle hitbox;
    int currentFrame;
    float frameCounter;
    float frameSpeed;
    bool isAttacking;
    bool facingLeft;
    float attackTimer;
    float attackCooldown;
    float health;
    bool isDead;
    float attackRange;     // 攻击范围
    int attackDamage;      // 攻击伤害
    DataWave dataWaves[MAX_DATA_WAVES];
    float dataWaveTimer;
    bool canUseDataWave;
    float phase1HealthThreshold;  // 第一階段血量閾值
    
    // 新增無人機相關屬性
    BossDrone drones[BOSS_DRONE_COUNT];
    bool dronesActive;
    float droneSpawnTimer;
    float droneSpawnCooldown;
    bool isPhase2;
    bool canUseEnergyPulse;
    float energyPulseTimer;
    float energyPulseCooldown;
    bool energyPulseActive;
    float energyPulseRadius;
    Vector2 energyPulsePosition;
    bool shieldActive;
    float shieldTimer;
    float shieldCooldown;
    float absorbedDamage;
    bool shieldExplosionActive;
    float shieldExplosionRadius;
    Vector2 shieldExplosionPosition;
    bool isActive;  // 新增：boss是否已激活
} Boss;

void boss_init(Boss *boss);
void boss_update(Boss *boss, Player *player, GameSounds *sounds);
void boss_draw(Boss *boss, GameTextures *textures);
void boss_drawhitbox(Boss *boss);

// 新增无人机子弹相关函数声明
void boss_initDroneBullet(BossDroneBullet *bullet);
void boss_droneFireBullet(BossDrone *drone, Player *player);
void boss_updateDroneBullet(BossDroneBullet *bullet, float deltaTime);
void boss_checkDroneBulletHit(BossDroneBullet *bullet, Player *player);

#endif 