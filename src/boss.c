#include "boss.h"
#include "raylib.h"
#include "stage.h"
#include "main.h"
#include <stdio.h>
#include <math.h>


void boss_init(Boss *boss) {
    // 调整初始位置到偏左侧
    boss->position = (Vector2){1000, screenHeight - 400};
    boss->hitbox = (Rectangle){boss->position.x - 350, boss->position.y - 350, 700, 700};
       
    boss->currentFrame = 0;
    boss->frameCounter = 0;
    boss->frameSpeed = 0.1f;
    boss->isAttacking = false;
    boss->facingLeft = false;
    boss->attackTimer = 0;
    boss->attackCooldown = 2.0f;
    boss->health = BOSS_HEALTH;
    boss->isDead = false;
    boss->attackRange = BOSS_ATTACK_RANGE;
    boss->attackDamage = BOSS_ATTACK_DAMAGE;
    
    // 碰撞箱设置 - 根据700x700的图片尺寸调整
    boss->hitbox.width = 300;     // 宽度保持300像素
    boss->hitbox.height = 600;    // 高度保持600像素
    
    // 碰撞箱位置偏移（相对于boss位置）
    float offsetX = 140;   // 向右偏移140像素（1/5图片宽度）
    float offsetY = -300;  // 垂直偏移保持不变
    
    boss->hitbox.x = boss->position.x + offsetX;
    boss->hitbox.y = boss->position.y + offsetY;

    // 初始化數據波
    for (int i = 0; i < MAX_DATA_WAVES; i++) {
        boss->dataWaves[i].active = false;
        boss->dataWaves[i].speed = DATA_WAVE_SPEED;
    }
    boss->dataWaveTimer = 0;
    boss->canUseDataWave = true;
    boss->phase1HealthThreshold = BOSS_HEALTH * 0.5f;  // 50%血量為第一階段
    
    // 初始化無人機相關屬性
    boss->dronesActive = false;
    boss->droneSpawnTimer = 0;
    boss->droneSpawnCooldown = 2.0f;
    
    for (int i = 0; i < BOSS_DRONE_COUNT; i++) {
        boss->drones[i].active = false;
        boss->drones[i].health = BOSS_DRONE_HEALTH;
        boss->drones[i].attackTimer = 0;
        boss->drones[i].isAttacking = false;
        boss->drones[i].direction = (Vector2){0, 0};
        boss->drones[i].directionTimer = 0;
        boss_initDroneBullet(&boss->drones[i].bullet);
    }
}

void boss_update(Boss *boss, Player *player) {
    if (boss->isDead) return;
    
    // 檢查子彈是否超出場景邊界
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (player->bullets[i].active) {
            // 添加場景邊界檢查
            if (player->bullets[i].position.x < 0 ||
                player->bullets[i].position.x > stage2Width ||  // 使用第二階段的寬度
                player->bullets[i].position.y < 0 ||
                player->bullets[i].position.y > screenHeight) {
                player->bullets[i].active = false;
            }
        }
    }
    
    // 計算與玩家的距離
    float dx = player->position.x - boss->position.x;
    boss->facingLeft = dx < 0;  // 如果玩家在左邊，boss朝左
    
    // 根據朝向設定不同的觸發攻擊距離
    float attackTriggerDistance;
    if (boss->facingLeft) {
        attackTriggerDistance = 400.0f;
    } else {
        attackTriggerDistance = 200.0f;
    }
    
    // 如果不在攻擊範圍內，移動boss
    if (fabs(dx) > attackTriggerDistance && !boss->isAttacking) {
        float moveSpeed = 100.0f * GetFrameTime();  // 移動速度降低到100
        if (boss->facingLeft) {
            boss->position.x -= moveSpeed;
        } else {
            boss->position.x += moveSpeed;
        }
    }
    
    // 當進入攻擊範圍時，開始攻擊
    if (fabs(dx) <= attackTriggerDistance && !boss->isAttacking) {
        boss->isAttacking = true;
        boss->attackTimer = 0;
    }

    // 根據朝向更新碰撞箱位置
    if (boss->facingLeft) {
        boss->hitbox.x = boss->position.x - 150 - 140;
    } else {
        boss->hitbox.x = boss->position.x - 150 + 140;
    }
    boss->hitbox.y = boss->position.y - 300;

    // 攻击逻辑
    if (boss->isAttacking) {
        boss->frameCounter += GetFrameTime();
        if (boss->frameCounter >= boss->frameSpeed) {
            boss->frameCounter = 0;
            boss->currentFrame++;
            
            if (boss->currentFrame == 5) {
                Rectangle attackArea;
                float attackWidth;
                float attackHeight = 200;
                
                if (boss->facingLeft) {
                    attackWidth = 400;
                    attackArea = (Rectangle){
                        boss->position.x - attackWidth - 100,
                        boss->position.y - attackHeight/2,
                        attackWidth,
                        attackHeight
                    };
                } else {
                    attackWidth = 200;  // 朝右時更小的攻擊範圍
                    attackArea = (Rectangle){
                        boss->position.x + 100,
                        boss->position.y - attackHeight/2,
                        attackWidth,
                        attackHeight
                    };
                }
                
                if (CheckCollisionRecs(attackArea, player->hitbox)) {
                    player->hp -= boss->attackDamage;
                }
                
                if (IsKeyDown(KEY_TAB)) {
                    DrawRectangleRec(attackArea, (Color){255, 0, 0, 128});
                }
            }
            
            if (boss->currentFrame >= BOSS_ATTACK_FRAME_COUNT) {
                boss->currentFrame = 0;
                boss->isAttacking = false;
            }
        }
    }

    // 攻击冷却
    if (!boss->isAttacking) {
        boss->attackCooldown -= GetFrameTime();
        if (boss->attackCooldown <= 0) {
            boss->attackCooldown = 2.0f;  // 2秒冷却时间
        }
    }

    // 检查是否被玩家子弹击中
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (player->bullets[i].active) {
            Rectangle bulletRect = {
                player->bullets[i].position.x,
                player->bullets[i].position.y,
                SIZE_BULLET,
                SIZE_BULLET
            };

            // 根据朝向调整碰撞箱位置
            Rectangle adjustedHitbox;
            if (boss->facingLeft) {
                adjustedHitbox = (Rectangle){
                    boss->hitbox.x + 300,  // 朝左时向右偏移
                    boss->hitbox.y,
                    boss->hitbox.width,
                    boss->hitbox.height
                };
            } else {
                adjustedHitbox = (Rectangle){
                    boss->hitbox.x - 300,  // 朝右时向左偏移
                    boss->hitbox.y,
                    boss->hitbox.width,
                    boss->hitbox.height
                };
            }

            if (CheckCollisionRecs(bulletRect, adjustedHitbox)) {
                boss->health -= player->damage;
                player->bullets[i].active = false;
                
                if (boss->health <= 0) {
                    boss->isDead = true;
                }
                break;
            }
        }
    }

    // 第一階段技能：數據波
    if (boss->health > boss->phase1HealthThreshold) {
        // 更新數據波冷卻
        if (!boss->canUseDataWave) {
            boss->dataWaveTimer -= GetFrameTime();
            if (boss->dataWaveTimer <= 0) {
                boss->canUseDataWave = true;
            }
        }

        // 發射數據波
        if (boss->canUseDataWave) {
            for (int i = 0; i < MAX_DATA_WAVES; i++) {
                if (!boss->dataWaves[i].active) {
                    boss->dataWaves[i].position = boss->position;
                    Vector2 direction = {
                        player->position.x - boss->position.x,
                        player->position.y - boss->position.y
                    };
                    float length = sqrt(direction.x * direction.x + direction.y * direction.y);
                    boss->dataWaves[i].direction = (Vector2){
                        direction.x / length,
                        direction.y / length
                    };
                    boss->dataWaves[i].active = true;
                    break;
                }
            }
            boss->canUseDataWave = false;
            boss->dataWaveTimer = DATA_WAVE_COOLDOWN;
        }

        // 更新數據波位置和碰撞檢測
        for (int i = 0; i < MAX_DATA_WAVES; i++) {
            if (boss->dataWaves[i].active) {
                boss->dataWaves[i].position.x += boss->dataWaves[i].direction.x * boss->dataWaves[i].speed * GetFrameTime();
                boss->dataWaves[i].position.y += boss->dataWaves[i].direction.y * boss->dataWaves[i].speed * GetFrameTime();

                // 檢查是否擊中玩家
                if (CheckCollisionCircleRec(
                    boss->dataWaves[i].position,
                    DATA_WAVE_RADIUS,
                    player->hitbox
                )) {
                    player->controlsReversed = true;
                    player->controlReverseTimer = DATA_WAVE_DURATION;
                    // 立即反轉移動方向
                    player->velocity.x = -player->velocity.x;
                    player->velocity.y = -player->velocity.y;
                    boss->dataWaves[i].active = false;
                }

                // 檢查是否超出屏幕
                if (boss->dataWaves[i].position.x < -500 || 
                    boss->dataWaves[i].position.x > screenWidth + 500 ||
                    boss->dataWaves[i].position.y < -500 || 
                    boss->dataWaves[i].position.y > screenHeight + 500) {
                    boss->dataWaves[i].active = false;
                }
            }
        }
    }
    // 當boss血量低於80%時啟動無人機系統
    if (boss->health <= BOSS_HEALTH * BOSS_PHASE1_THRESHOLD_DRONE && !boss->dronesActive) {
        boss->dronesActive = true;
        }
    // 當boss血量低於50%時關閉無人機系統
    if (boss->health <= BOSS_HEALTH * 0.5f && boss->dronesActive) {
        boss->dronesActive = false;
        // 關閉所有無人機
        for (int i = 0; i < BOSS_DRONE_COUNT; i++) {
            boss->drones[i].active = false;
            boss->drones[i].bullet.active = false;
        }
    }
    
    // 更新無人機
    if (boss->dronesActive) {
        // 更新無人機生成邏輯
        boss->droneSpawnTimer += GetFrameTime();
        
        if (boss->droneSpawnTimer >= boss->droneSpawnCooldown) {
            // 尋找未激活的無人機
            for (int i = 0; i < BOSS_DRONE_COUNT; i++) {
                if (!boss->drones[i].active) {
                    // 在boss周圍隨機位置生成無人機
                    float angle = GetRandomValue(0, 360) * DEG2RAD;
                    float distance = GetRandomValue(150, 250);  // 距離boss 150-250像素
                    
                    boss->drones[i].position.x = boss->position.x + cosf(angle) * distance;
                    boss->drones[i].position.y = boss->position.y + sinf(angle) * distance;
                    
                    boss->drones[i].hitbox = (Rectangle){
                        boss->drones[i].position.x - BOSS_DRONE_WIDTH/2,
                        boss->drones[i].position.y - BOSS_DRONE_HEIGHT/2,
                        BOSS_DRONE_WIDTH,
                        BOSS_DRONE_HEIGHT
                    };
                    
                    boss->drones[i].active = true;
                    boss->droneSpawnTimer = 0;
                    break;
                }
            }
        }
        
        // 更新每個無人機的移動
        for (int i = 0; i < BOSS_DRONE_COUNT; i++) {
            if (boss->drones[i].active) {
                // 更新方向計時器
                boss->drones[i].directionTimer += GetFrameTime();
                
                // 每秒進行一次距離判斷和方向調整
                if (boss->drones[i].directionTimer >= BOSS_DRONE_DIRECTION_CHANGE_TIME) {
                    // 計算與玩家的距離和方向
                    Vector2 toPlayer = {
                        player->position.x - boss->drones[i].position.x,
                        player->position.y - boss->drones[i].position.y
                    };
                    float distanceToPlayer = sqrtf(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);
                    
                    // 計算目標距離（在BOSS_DRONE_MOVE_RANGE的基礎上加上一些隨機性）
                    float targetDistance = BOSS_DRONE_MOVE_RANGE * (0.8f + (float)GetRandomValue(0, 40) / 100.0f);
                    
                    // 如果距離超出目標範圍，調整方向
                    if (fabsf(distanceToPlayer - targetDistance) > BOSS_DRONE_DISTANCE_THRESHOLD) {
                        // 歸一化向量
                        float length = distanceToPlayer;
                        toPlayer.x /= length;
                        toPlayer.y /= length;
                        
                        // 根據距離決定是靠近還是遠離玩家
                        if (distanceToPlayer > targetDistance) {
                            boss->drones[i].direction = toPlayer;
                        } else {
                            boss->drones[i].direction.x = -toPlayer.x;
                            boss->drones[i].direction.y = -toPlayer.y;
                        }
                    } else {
                        // 在目標距離範圍內時，隨機改變方向
                        float angle = GetRandomValue(0, 360) * DEG2RAD;
                        boss->drones[i].direction.x = cosf(angle);
                        boss->drones[i].direction.y = sinf(angle);
                    }
                    
                    boss->drones[i].directionTimer = 0;
                }
                
                // 更新位置
                float newX = boss->drones[i].position.x + boss->drones[i].direction.x * BOSS_DRONE_SPEED * GetFrameTime();
                float newY = boss->drones[i].position.y + boss->drones[i].direction.y * BOSS_DRONE_SPEED * GetFrameTime();
                
                // 限制Y軸範圍
                if (newY < BOSS_DRONE_MIN_Y) {
                    newY = BOSS_DRONE_MIN_Y;
                    boss->drones[i].direction.y = 0;  // 碰到邊界時停止垂直移動
                } else if (newY > BOSS_DRONE_MAX_Y) {
                    newY = BOSS_DRONE_MAX_Y;
                    boss->drones[i].direction.y = 0;  // 碰到邊界時停止垂直移動
                }
                
                boss->drones[i].position.x = newX;
                boss->drones[i].position.y = newY;
                
                // 更新碰撞箱
                boss->drones[i].hitbox.x = boss->drones[i].position.x - BOSS_DRONE_WIDTH/2;
                boss->drones[i].hitbox.y = boss->drones[i].position.y - BOSS_DRONE_HEIGHT/2;
            }
        }
    }

    // 更新无人机子弹
    if (boss->dronesActive) {  // 只在無人機系統激活時更新子彈
        for (int i = 0; i < BOSS_DRONE_COUNT; i++) {
            if (boss->drones[i].active) {
                boss->drones[i].attackTimer += GetFrameTime();
                
                // 每3秒发射一次子弹
                if (boss->drones[i].attackTimer >= BOSS_DRONE_BULLET_COOLDOWN) {
                    boss_droneFireBullet(&boss->drones[i], player);
                    boss->drones[i].attackTimer = 0.0f;
                }
                
                // 更新子弹位置
                boss_updateDroneBullet(&boss->drones[i].bullet, GetFrameTime());
                
                // 只检查子弹是否击中玩家，忽略与boss的碰撞
                boss_checkDroneBulletHit(&boss->drones[i].bullet, player);
            }
        }
    }
}

void boss_draw(Boss *boss, GameTextures *textures) {
    if (boss->isDead) return;
    
    int frameIndex = boss->currentFrame;

    frameIndex %= BOSS_ATTACK_FRAME_COUNT;

    Texture2D frame = textures->bossAttack[frameIndex];
    float width = (float)frame.width;
    float height = (float)frame.height;

    Rectangle source = {0, 0, boss->facingLeft ? width : -width, height};
    Rectangle dest = {boss->position.x, boss->position.y, width, height};
    Vector2 origin = {width / 2, height / 2};

    DrawTexturePro(frame, source, dest, origin, 0, WHITE);
    // DEBUG: 显示攻击范围和碰撞箱
    if (IsKeyDown(KEY_TAB)) {
        // 显示攻击范围
        Color rangeColor = {255, 0, 0, 128};
        DrawRectangle(boss->position.x - BOSS_ATTACK_RANGE,
                     boss->position.y - 100,
                     BOSS_ATTACK_RANGE * 2,
                     200,
                     rangeColor);
        
        // 显示原始碰撞箱
        DrawRectangleRec(boss->hitbox, (Color){0, 255, 0, 128});
        
        // 根据朝向显示调整后的子弹碰撞箱
        Rectangle bulletHitbox;
        if (boss->facingLeft) {
            bulletHitbox = (Rectangle){
                boss->hitbox.x + 300,  // 朝左时向右偏移
                boss->hitbox.y,
                boss->hitbox.width,
                boss->hitbox.height
            };
        } else {
            bulletHitbox = (Rectangle){
                boss->hitbox.x - 300,  // 朝右时向左偏移
                boss->hitbox.y,
                boss->hitbox.width,
                boss->hitbox.height
            };
        }
        DrawRectangleRec(bulletHitbox, (Color){0, 0, 255, 128});
    }

    // 绘制血条
    float barWidth = 200.0f;    // 血条总宽
    float barHeight = 20.0f;    // 血条高度
    float hpRatio = boss->health / BOSS_HEALTH;   // 目前血量百分比
    float barX = boss->position.x - barWidth/2;   // 中央对齐
    float barY = boss->position.y - 400;          // 头上位置

    // 画血条底色（灰色）
    DrawRectangle(barX, barY, barWidth, barHeight, GRAY);
    // 画血量（红色）
    DrawRectangle(barX, barY, barWidth * hpRatio, barHeight, RED);
    // 血条外框
    DrawRectangleLines(barX, barY, barWidth, barHeight, BLACK);

    // 繪製數據波
    for (int i = 0; i < MAX_DATA_WAVES; i++) {
        if (boss->dataWaves[i].active) {
            DrawCircle(
                (int)boss->dataWaves[i].position.x,
                (int)boss->dataWaves[i].position.y,
                DATA_WAVE_RADIUS,
                BLUE
            );
        }
    }
    
    // 繪製無人機
    for (int i = 0; i < BOSS_DRONE_COUNT; i++) {
        if (boss->drones[i].active) {
            DrawRectangleRec(boss->drones[i].hitbox, BLUE);
        }
    }

    // 绘制无人机子弹
    for (int i = 0; i < BOSS_DRONE_COUNT; i++) {
        if (boss->drones[i].bullet.active) {
            DrawCircleV(boss->drones[i].bullet.position, BOSS_DRONE_BULLET_RADIUS, YELLOW);
        }
    }
}

void boss_drawhitbox(Boss *boss) {
    // 使用红线框显示碰撞箱
    DrawRectangleLines(
        boss->hitbox.x,
        boss->hitbox.y,
        boss->hitbox.width,
        boss->hitbox.height,
        RED
    );
}

// 初始化Boss无人机的子弹
void boss_initDroneBullet(BossDroneBullet *bullet) {
    bullet->active = false;
    bullet->speed = BOSS_DRONE_BULLET_SPEED;
}

// 发射子弹
void boss_droneFireBullet(BossDrone *drone, Player *player) {
    if (!drone->active) return;
    
    // 计算与玩家的距离
    float dx = player->position.x - drone->position.x;
    float dy = player->position.y - drone->position.y;
    float distance = sqrt(dx * dx + dy * dy);
    
    // 只有当玩家在攻击范围内才发射子弹
    if (distance <= BOSS_DRONE_ATTACK_RANGE) {
        Vector2 direction = {dx, dy};
        
        // 标准化方向向量
        float length = sqrt(direction.x * direction.x + direction.y * direction.y);
        if (length > 0) {
            direction.x /= length;
            direction.y /= length;
        }
        
        drone->bullet.position = drone->position;
        drone->bullet.direction = direction;
        drone->bullet.active = true;
    }
}

// 更新子弹位置
void boss_updateDroneBullet(BossDroneBullet *bullet, float deltaTime) {
    if (!bullet->active) return;
    
    bullet->position.x += bullet->direction.x * bullet->speed * deltaTime;
    bullet->position.y += bullet->direction.y * bullet->speed * deltaTime;
    
    // 检查子弹是否超出屏幕
    if (bullet->position.x < 0 || bullet->position.x > screenWidth ||
        bullet->position.y < 0 || bullet->position.y > screenHeight) {
        bullet->active = false;
    }
}

// 检查子弹是否击中玩家
void boss_checkDroneBulletHit(BossDroneBullet *bullet, Player *player) {
    if (!bullet->active || player->invincible) return;
    
    if (CheckCollisionCircleRec(bullet->position, BOSS_DRONE_BULLET_RADIUS, player->hitbox)) {
        player->hp -= BOSS_DRONE_DAMAGE;
        // 禁用武器2秒
        player->weaponDisabled = true;
        player->weaponDisableTimer = WEAPON_DISABLE_TIME;
        bullet->active = false;
    }
}

