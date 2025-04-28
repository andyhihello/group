#include "enemy.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// 定義圓周率常數（若標準庫未定義）
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// 計算兩點之間的距離
float enemy_distance(Vector2 p1, Vector2 p2) {
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    return sqrt(dx * dx + dy * dy);
}

// 將向量標準化（單位向量）
Vector2 enemy_normalizeVector(Vector2 v) {
    Vector2 result = {0};
    float length = sqrt(v.x * v.x + v.y * v.y);
    if (length > 0) {
        result.x = v.x / length;
        result.y = v.y / length;
    }
    return result;
}

// 初始化無人機敵人（Security Drone）
void enemy_initSecurityDrone(SecurityDrone* drone, float x, float y) {
    if (drone == NULL) return;
    drone->position = (Vector2){x, y};
    drone->health = 100.0f;
    drone->attackCooldown = 2.0f;           // 每2秒攻擊一次
    drone->timeSinceLastAttack = 0.0f;
    drone->type = SECURITY_DRONE;
    drone->laser = (LaserShot){0};
    drone->laser.damage = 25.0f;            // 雷射傷害
    drone->laser.width = 5.0f;              // 雷射寬度
    drone->laser.duration = 0.5f;           // 雷射持續時間
    drone->laser.active = false;
}

// 初始化機器人士兵（Robot Soldier）
void enemy_initRobotSoldier(RobotSoldier* soldier, float x, float y) {
    if (soldier == NULL) return;
    soldier->position = (Vector2){x, y};
    soldier->health = 120.0f;
    soldier->attackCooldown = 3.0f;         // 每3秒攻擊一次
    soldier->timeSinceLastAttack = 0.0f;
    soldier->type = ROBOT_SOLDIER;
    soldier->maxDamageDistance = 100.0f;    // 最大傷害距離
    soldier->minDamageDistance = 500.0f;    // 最小傷害距離
    soldier->maxDamage = 20.0f;             // 最大傷害
    soldier->minDamage = 5.0f;              // 最小傷害

    // 初始化子彈
    for (int i = 0; i < 3; i++) {
        soldier->bullets[i].active = false;
        soldier->bullets[i].speed = 300.0f;
        soldier->bullets[i].damage = soldier->maxDamage;
    }
}

// 無人機發射雷射（朝向玩家）
void enemy_securityDroneFireLaser(SecurityDrone* drone, Vector2 playerPosition) {
    if (drone == NULL) return;
    drone->laser.start = drone->position;
    Vector2 direction = enemy_normalizeVector((Vector2){
        playerPosition.x - drone->position.x,
        playerPosition.y - drone->position.y
    });
    float laserLength = 1000.0f;
    drone->laser.end = (Vector2){
        drone->position.x + direction.x * laserLength,
        drone->position.y + direction.y * laserLength
    };
    drone->laser.active = true;
    drone->laser.timeActive = 0.0f;
}

// 機器人士兵發射散彈（向玩家，三向散射）
void enemy_robotSoldierFireScatterShot(RobotSoldier* soldier, Vector2 playerPosition) {
    if (soldier == NULL) return;
    Vector2 baseDirection = enemy_normalizeVector((Vector2){
        playerPosition.x - soldier->position.x,
        playerPosition.y - soldier->position.y
    });
    float baseAngle = atan2(baseDirection.y, baseDirection.x);
    for (int i = 0; i < 3; i++) {
        float angleOffset = (i - 1) * 0.1f; // 左、中、右
        float finalAngle = baseAngle + angleOffset;
        soldier->bullets[i].position = soldier->position;
        soldier->bullets[i].direction = (Vector2){
            cos(finalAngle),
            sin(finalAngle)
        };
        soldier->bullets[i].active = true;
    }
}

// 判斷雷射是否擊中玩家
bool enemy_checkLaserHit(LaserShot* laser, Vector2 playerPosition, float playerRadius) {
    if (laser == NULL || !laser->active) return false;
    Vector2 lineVector = {
        laser->end.x - laser->start.x,
        laser->end.y - laser->start.y
    };
    Vector2 pointVector = {
        playerPosition.x - laser->start.x,
        playerPosition.y - laser->start.y
    };
    float lineLength = sqrt(lineVector.x * lineVector.x + lineVector.y * lineVector.y);
    if (lineLength < 0.001f) {
        return enemy_distance(laser->start, playerPosition) <= playerRadius + laser->width / 2;
    }
    lineVector.x /= lineLength;
    lineVector.y /= lineLength;
    float projection = pointVector.x * lineVector.x + pointVector.y * lineVector.y;
    if (projection < 0) {
        return enemy_distance(laser->start, playerPosition) <= playerRadius + laser->width / 2;
    } else if (projection > lineLength) {
        return enemy_distance(laser->end, playerPosition) <= playerRadius + laser->width / 2;
    }
    Vector2 closestPoint = {
        laser->start.x + lineVector.x * projection,
        laser->start.y + lineVector.y * projection
    };
    return enemy_distance(closestPoint, playerPosition) <= playerRadius + laser->width / 2;
}

// 判斷散彈是否擊中玩家
bool enemy_checkBulletHit(ScatterBullet* bullet, Vector2 playerPosition, float playerRadius) {
    if (bullet == NULL || !bullet->active) return false;
    return enemy_distance(bullet->position, playerPosition) <= playerRadius;
}

// 根據距離計算傷害
float enemy_calculateDistanceBasedDamage(ScatterBullet* bullet, Vector2 playerPosition, RobotSoldier* soldier) {
    if (bullet == NULL || soldier == NULL) return 0.0f;
    float dist = enemy_distance(bullet->position, playerPosition);
    if (dist <= soldier->maxDamageDistance) {
        return soldier->maxDamage;
    } else if (dist >= soldier->minDamageDistance) {
        return soldier->minDamage;
    } else {
        float t = (dist - soldier->maxDamageDistance) / (soldier->minDamageDistance - soldier->maxDamageDistance);
        return soldier->maxDamage + t * (soldier->minDamage - soldier->maxDamage);
    }
}

// 更新無人機狀態（攻擊計時與雷射狀態）
void enemy_updateSecurityDrone(SecurityDrone* drone, Vector2 playerPosition, float deltaTime) {
    if (drone == NULL) return;
    drone->timeSinceLastAttack += deltaTime;
    if (drone->timeSinceLastAttack >= drone->attackCooldown) {
        enemy_securityDroneFireLaser(drone, playerPosition);
        drone->timeSinceLastAttack = 0.0f;
    }
    if (drone->laser.active) {
        drone->laser.timeActive += deltaTime;
        if (drone->laser.timeActive >= drone->laser.duration) {
            drone->laser.active = false;
        }
    }
}

// 更新士兵狀態（攻擊計時與子彈移動）
void enemy_updateRobotSoldier(RobotSoldier* soldier, Vector2 playerPosition, float deltaTime) {
    if (soldier == NULL) return;
    soldier->timeSinceLastAttack += deltaTime;
    if (soldier->timeSinceLastAttack >= soldier->attackCooldown) {
        enemy_robotSoldierFireScatterShot(soldier, playerPosition);
        soldier->timeSinceLastAttack = 0.0f;
    }
    for (int i = 0; i < 3; i++) {
        if (soldier->bullets[i].active) {
            soldier->bullets[i].position.x += soldier->bullets[i].direction.x * soldier->bullets[i].speed * deltaTime;
            soldier->bullets[i].position.y += soldier->bullets[i].direction.y * soldier->bullets[i].speed * deltaTime;
            float screenBoundary = 1000.0f; // 調整為地圖大小
            if (soldier->bullets[i].position.x < -screenBoundary ||
                soldier->bullets[i].position.x > screenBoundary ||
                soldier->bullets[i].position.y < -screenBoundary ||
                soldier->bullets[i].position.y > screenBoundary) {
                soldier->bullets[i].active = false;
            }
        }
    }
}
