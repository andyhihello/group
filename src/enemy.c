#include "enemy.h"
#include "main.h"
 

// 計算兩點之間的距離
float enemy_distance(Vector2 p1, Vector2 p2) {
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    return sqrt(dx * dx + dy * dy);
}

float enemy_distanceX(Vector2 p1, Vector2 p2) {
    return fabs(p2.x - p1.x);  // fabs 是浮點數絕對值
}

Rectangle enemy_Dronehitbox(Drone *drone) {
    return (Rectangle){
        drone->position.x + 120,  
        drone->position.y + 120,  
        200,                   
        530                     
    };
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
void enemy_initDrone(Drone* drone) {
    char path[100];
    for (int i = 0; i < 5; i++) {       
        sprintf(path, "resource/drone/patrol%d.png", (i < 2) ? (i + 1) : (5 - i));
        drone->patrolFrames[i] = LoadTexture(path);
    }
    for (int i = 0; i < 9; i++) {
        sprintf(path, "resource/drone/chase%d.png", (i < 5) ? (i + 1) : (9 - i));
        drone->chaseFrames[i] = LoadTexture(path);
    }
    drone->attackFrames= LoadTexture("resource/drone/attack.png");
    drone->laserFrame = LoadTexture("resource/drone/laser.png");

    drone->active = true;
    drone->position = (Vector2){1000, 300};
    drone->health = 100.0f;
    drone->attackCooldown = 2.0f;           // 每2秒攻擊一次
    drone->timeSinceLastAttack = 0.0f;
    drone->type = SECURITY_DRONE;
    drone->laser = (LaserShot){0};
    drone->laser.damage = 25.0f;            // 雷射傷害
    drone->laser.duration = 0.5f;           // 雷射持續時間
    drone->laser.active = false;
}

// 初始化機器人士兵（Robot Soldier）
void enemy_initSoldier(Soldier* soldier) {
    
    soldier->active = true;
    soldier->position = (Vector2){2000, 500};
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
void enemy_DroneFireLaser(Drone* drone, Player *player) {
    if (!drone->active) return;

    drone->laser.start = drone->position;

    float laserWidth = 563.0f;
    float laserHeight = 79.0f;
    float fixedLaserY = drone->position.y + 400;  

    // 判斷玩家在左還是右
    if (player->hitbox.x + player->hitbox.width / 2 > drone->laser.start.x) {
        // 向右射擊
        drone->laserhitbox = (Rectangle){
            drone->laser.start.x + drone->hitbox.width + drone->hitbox.width /2,
            fixedLaserY,
            laserWidth,
            laserHeight
        };
    } else {
        // 向左射擊
        drone->laserhitbox = (Rectangle){
            drone->laser.start.x - laserWidth + drone->hitbox.width - drone->hitbox.width /2,
            fixedLaserY,
            laserWidth,
            laserHeight
        };
    }

    drone->laser.active = true;
    drone->laser.timeActive = 0.0f;
}

// 機器人士兵發射散彈（向玩家，三向散射）
void enemy_SoldierFireScatterShot(Soldier* soldier, Player *player) {
    if (soldier->active == false) return;
    Vector2 baseDirection = enemy_normalizeVector((Vector2){
        player->position.x - soldier->position.x,
        player->position.y - soldier->position.y
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
void enemy_laserDamagePlayer(Drone *drone, Player *player) {
    if (CheckCollisionRecs(drone->laserhitbox, player->hitbox)) {
        player->hp -= drone->laser.damage;
    }
}

// 判斷散彈是否擊中玩家
void enemy_bulletDamagePlayer(ScatterBullet* bullet, Player *player,Soldier* soldier) {
    if (bullet == NULL || !bullet->active) return;

    float bulletRadius = 5.0f;  // 你子彈的半徑

    if (CheckCollisionCircleRec(bullet->position, bulletRadius, player->hitbox)) {
        float damage = enemy_calculateDistanceBasedDamage(bullet, player, soldier);
        player->hp -= damage;
        bullet->active = false;  // 子彈命中後消失
    }
}

// 根據距離計算傷害
float enemy_calculateDistanceBasedDamage(ScatterBullet* bullet, Player *player, Soldier* soldier) {
    if (bullet == NULL || soldier == NULL) return 0.0f;
    float dist = enemy_distance(bullet->position, player->position);
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
void enemy_updateDrone(Drone* drone, Player* player, float deltaTime) {
    if (!drone->active) return;

    Vector2 drone_center = {
        drone->hitbox.x + drone->hitbox.width / 2,
        drone->hitbox.y + drone->hitbox.height / 2
    };

    Vector2 player_center = {
        player->hitbox.x + player->hitbox.width / 2,
        player->hitbox.y + player->hitbox.height / 2
    };

    float dist = enemy_distanceX(drone_center, player_center);  // 只算 X 軸

    drone->facingRight = (player_center.x < drone_center.x);

    EnemyState previousState = drone->state;  // 新增：記錄上一個狀態

    // 狀態切換邏輯
    if (dist > 800) {
        drone->state = PATROL;
    } else if (dist <= 800 && dist > 500) {
        drone->state = CHASE;
    } else if (dist <= 500) {
        drone->state = ATTACK;
    }
    
    // 狀態改變時，重設動畫
    if (drone->state != previousState) {
        drone->currentFrame = 0;
        drone->frameTimer = 0;
    }

    TraceLog(LOG_INFO, "%d",drone->state);

    switch (drone->state) {
        case PATROL:
            drone->position.x += sin(GetTime()) * 4.0f;
            break;
        case CHASE:
            if ((player->hitbox.x + player->hitbox.width / 2) > drone->position.x) {
                drone->position.x += 200.0f * deltaTime;
            } else {
                drone->position.x -= 200.0f * deltaTime;
            }
            break;
        case ATTACK:
            drone->timeSinceLastAttack += deltaTime;
            if (drone->timeSinceLastAttack >= drone->attackCooldown) {
                enemy_DroneFireLaser(drone, player);
                drone->timeSinceLastAttack = 0.0f;
            }
            break;
    }

    if (drone->laser.active) {
        drone->laser.timeActive += deltaTime;
        if (drone->laser.timeActive >= drone->laser.duration) {
            drone->laser.active = false;
        }
    }
    drone->hitbox = enemy_Dronehitbox(drone);
}

// 更新士兵狀態（攻擊計時與子彈移動）
void enemy_updateSoldier(Soldier* soldier, Player *player, float deltaTime) {
    if (soldier->active == false) return;
    int sceneWidth;
    if(player->stage == 1) sceneWidth = stage1Width;
    soldier->timeSinceLastAttack += deltaTime;
    if (soldier->timeSinceLastAttack >= soldier->attackCooldown) {
        enemy_SoldierFireScatterShot(soldier, player);
        soldier->timeSinceLastAttack = 0.0f;
    }
    for (int i = 0; i < 3; i++) {
        if (soldier->bullets[i].active) {
            soldier->bullets[i].position.x += soldier->bullets[i].direction.x * soldier->bullets[i].speed * deltaTime;
            soldier->bullets[i].position.y += soldier->bullets[i].direction.y * soldier->bullets[i].speed * deltaTime;
            if (soldier->bullets[i].position.x < 0 ||
                soldier->bullets[i].position.x > sceneWidth ||
                soldier->bullets[i].position.y < 0 ||
                soldier->bullets[i].position.y > screenHeight) {
                soldier->bullets[i].active = false;
            }
        }
    }
}

void enemy_drawDrone(Drone* drone) {
    Texture2D frame;

    switch (drone->state) {
        case PATROL:
            frame = drone->patrolFrames[drone->currentFrame];
            break;
        case CHASE:
            frame = drone->chaseFrames[drone->currentFrame];
            break;
        case ATTACK:
            frame = drone->attackFrames;
            break;
    }

    if (drone->state == PATROL) {
        drone->frameTimer += GetFrameTime();
        if (drone->frameTimer >= 0.1f) {
            drone->currentFrame = (drone->currentFrame + 1) % 5;
            drone->frameTimer = 0;
        }
    } 
    else if (drone->state == CHASE) {
        drone->frameTimer += GetFrameTime();
        if (drone->frameTimer >= 0.1f) {
            drone->currentFrame = (drone->currentFrame + 1) % 9;
            drone->frameTimer = 0;
        }
    } 
    else {
        drone->currentFrame = 0;  // ATTACK 單張圖
    }

    Rectangle source = { 0, 0, (float)frame.width, (float)frame.height };
    Rectangle dest = { drone->position.x, drone->position.y, (float)frame.width, (float)frame.height };
    Vector2 origin = { 0, 0 };

    // 根據 facingRight 決定是否左右翻轉
    if (!drone->facingRight) {
        source.width = -source.width;  // 翻轉圖像
    }

    DrawTexturePro(frame, source, dest, origin, 0.0f, WHITE);
}

void enemy_drawLaser(Drone* drone) {
    if (drone->laser.active) {
        Rectangle source = { 0, 0, (float)drone->laserFrame.width, (float)drone->laserFrame.height };
        Rectangle dest = drone->laserhitbox;
        Vector2 origin = { 0, 0 };

        if (!drone->facingRight) {
            source.width = -source.width;  // 左翻轉
        }

        DrawTexturePro(drone->laserFrame, source, dest, origin, 0.0f, WHITE);
    }
}

void enemy_hitbox(Drone* drone){
    DrawRectangleLinesEx(drone->hitbox, 2, RED);
    DrawRectangleLinesEx(drone->laserhitbox, 2, YELLOW);
}