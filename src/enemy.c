#include "enemy.h"
#include "main.h"
#include "setting.h"
 

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
    drone->active = true;
    drone->position = (Vector2){1000, 200};
    drone->health = 100.0f;
    drone->type = SECURITY_DRONE;
    drone->laser = (LaserShot){0};
    drone->laser.damage = 1.0f;            // 雷射傷害
    drone->laser.duration = 0.5f;           // 雷射持續時間
    drone->laser.active = false;
    drone->coin = 1;
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
    float fixedLaserY = drone->position.y + 530;  

    // 判斷玩家在左還是右
    if (player->hitbox.x + player->hitbox.width / 2 > drone->laser.start.x) {
        // 向右射擊
        drone->laserhitbox = (Rectangle){
            drone->laser.start.x + drone->hitbox.width + drone->hitbox.width /2,
            fixedLaserY-20,
            laserWidth,
            laserHeight-20
        };
    } else {
        // 向左射擊
        drone->laserhitbox = (Rectangle){
            drone->laser.start.x - laserWidth + drone->hitbox.width - drone->hitbox.width /2,
            fixedLaserY-20,
            laserWidth,
            laserHeight-20
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
void enemy_laserDamagePlayer(Drone *drone, Player *player,GameSounds *sounds) {
    
    if (drone->laser.active == false||player->invincible == true) return;
    if (CheckCollisionRecs(drone->laserhitbox, player->hitbox) ) {
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

    

    EnemyState previousState = drone->state;  // 新增：記錄上一個狀態

    // 狀態切換邏輯
    if (dist <= 800 && dist > 700 && (drone->facingRight == (player_center.x < drone_center.x))) drone->state = CHASE;
    else if (dist <= 700 && (drone->facingRight == (player_center.x < drone_center.x))) drone->state = ATTACK;
    else if (dist <= 500) drone->state = ATTACK;
    else drone->state = PATROL;
    
    // 狀態改變時，重設動畫
    if (drone->state != previousState) {
        drone->currentFrame = 0;
        drone->frameTimer = 0;
    }


    switch (drone->state) {
        case PATROL:
            float move = sin(0.7*GetTime()) * 4.0f;
            drone->position.x += move;
            if(move > 0)drone->facingRight = false;
            if(move < 0)drone->facingRight = true;        
            drone->frameTimer += GetFrameTime();
            if (drone->frameTimer >= 0.1f) {  // 控制攻擊動畫播放速度
                drone->currentFrame++;
                if (drone->currentFrame >= 4) drone->currentFrame = 0;  // 攻擊動畫有 4 張圖
                drone->frameTimer = 0;
            }
            break;
        case CHASE:
            drone->facingRight = (player_center.x < drone_center.x);
            if ((player->hitbox.x + player->hitbox.width / 2) > drone->position.x) {
                drone->position.x += 200.0f * deltaTime;
            } else {
                drone->position.x -= 200.0f * deltaTime;
            }

            drone->frameTimer += GetFrameTime();
            if (drone->frameTimer >= 0.1f) {
                drone->currentFrame = (drone->currentFrame + 1) % 9;
                drone->frameTimer = 0;
            }

            break;
        case ATTACK:
            drone->facingRight = (player_center.x < drone_center.x);
            drone->frameTimer += deltaTime;
            if (drone->frameTimer >= 0.4f) {
                drone->currentFrame++;
                if (drone->currentFrame >= 4) drone->currentFrame = 0;  
                drone->frameTimer = 0;
            }
        
            if (drone->currentFrame == 3 && drone->laser.active == false) {
                drone->laser.active = true;
                enemy_DroneFireLaser(drone, player);
            }
            else if(drone->currentFrame != 3){
                drone->laser.active = false;
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

void enemy_bulletDamageDrone(Player *player, Drone *drone,GameSounds *sounds) {

    SetSoundVolume(sounds->attack,sfxVolume);
    if (!drone->active) return;  // 跳過已經死掉的 Drone

    for (int i = 0; i < MAX_BULLETS; i++) {
        if (player->bullets[i].active) {
            Rectangle bulletRect = {
                player->bullets[i].position.x,
                player->bullets[i].position.y,
                SIZE_BULLET,
                SIZE_BULLET
            };

            if (CheckCollisionRecs(bulletRect, drone->hitbox)) {
                drone->health -= player->damage;     
                PlaySound(sounds->attack);                 // 扣血
                player->bullets[i].active = false;       // 子彈消失
    
                if (drone->health <= 0) {
                    drone->active = false;
                    player->coin += drone->coin;
                    drone->laser.active = false;         // 如果血量 <= 0 就不活躍
                }
                break; // 一顆子彈只能打中一個 Drone
 
            }
        }

        
    }
}

void enemy_drawDrone(Drone* drone, GameTextures *textures) {
    Texture2D frame;
    if(drone->active == false) return;
    switch (drone->state) {
        case PATROL:
            frame = textures->dronePatrol[drone->currentFrame];
            break;
        case CHASE:
            frame = textures->droneChase[drone->currentFrame];
            break;
        case ATTACK:
            frame = textures->droneAttack[drone->currentFrame];
            break;
    }

    Rectangle source = { 0, 0, (float)frame.width, (float)frame.height };
    Rectangle dest = { drone->position.x, drone->position.y, (float)frame.width, (float)frame.height };
    Vector2 origin = { 0, 0 };

    // 根據 facingRight 決定是否左右翻轉
    if (!drone->facingRight) {
        source.width = -source.width;  // 翻轉圖像
    }

    DrawTexturePro(frame, source, dest, origin, 0.0f, WHITE);

    float barWidth = 100.0f;    // 血條總寬
    float barHeight = 10.0f;    // 血條高度
    float hpRatio = drone->health / 100.0f;   // 目前血量百分比 (假設滿血是100)
    float barX = drone->position.x + frame.width/2 - barWidth/2 - 20 ;  // 中央對齊
    float barY = drone->position.y +100;      // 頭上20px

    // 畫血條底色（灰色）
    DrawRectangle(barX, barY, barWidth, barHeight, GRAY);

    // 畫血量（綠色）
    DrawRectangle(barX, barY, barWidth * hpRatio, barHeight, GREEN);

    // 血條外框
    DrawRectangleLines(barX, barY, barWidth, barHeight, BLACK);
}

void enemy_drawLaser(Drone* drone, GameTextures *textures) {
    if (drone->laser.active) {
        Rectangle source = { 0, 0, (float)textures->droneLaser.width, (float)textures->droneLaser.height };
        Rectangle dest = drone->laserhitbox;
        Vector2 origin = { 0, 0 };

        if (!drone->facingRight) {
            source.width = -source.width;  // 左翻轉
        }

        DrawTexturePro(textures->droneLaser, source, dest, origin, 0.0f, WHITE);
    }
}

void enemy_hitbox(Drone* drone){
    if(drone->active)DrawRectangleLinesEx(drone->hitbox, 2, RED);
    if(drone->laser.active)DrawRectangleLinesEx(drone->laserhitbox, 2, YELLOW);
}

