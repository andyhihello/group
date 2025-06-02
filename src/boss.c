#include "boss.h"
#include "raylib.h"
#include "stage.h"
#include "main.h"
#include <stdio.h>
#include <math.h>

void DrawDebuffStatus(Player* player);

void boss_init(Boss *boss) {
    // 调整初始位置到偏左侧
    boss->position = (Vector2){1000, screenHeight - 400};
    boss->hitbox = (Rectangle){boss->position.x - 350, boss->position.y - 350, 700, 700};
       
    // 动画相关初始化
    boss->currentFrame = 0;
    boss->frameCounter = 0;
    boss->frameSpeed = 0.3f;  // 将动画速度从0.1f改为0.2f，使动画更慢
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
    boss->dataWaveStartTimer = 0;  // 初始化計時器
    boss->canUseDataWave = false;  // 初始化時設為false
    boss->phase1HealthThreshold = BOSS_HEALTH * 0.5f;  // 50%血量进入第二阶段

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

    boss->health = BOSS_HEALTH;  // 確保初始化時設置為最大血量
    boss->isPhase2 = false;      // 確保初始化時不是第二階段
    boss->shieldActive = false;
    boss->shieldTimer = 0.0f;
    boss->shieldCooldown = 0.0f;
    boss->absorbedDamage = 0.0f;
    boss->shieldExplosionActive = false;
    boss->shieldExplosionRadius = 0.0f;
    boss->shieldExplosionPosition = (Vector2){0, 0};
    boss->isActive = false;  // 初始化時boss未激活
}

void boss_update(Boss *boss, Player *player, GameSounds *sounds) {
    if (boss->isDead) return;

    // 更新动画帧
    boss->frameCounter += GetFrameTime();
    if (boss->frameCounter >= boss->frameSpeed) {
        boss->frameCounter = 0;
        boss->currentFrame = (boss->currentFrame + 1) % 5;  // 5帧动画循环
    }

    if (!boss->isActive) {
        float dx = boss->position.x - player->position.x;
        float dy = boss->position.y - player->position.y;
        float distance = sqrt(dx * dx + dy * dy);
        
        if (distance <= BOSS_ACTIVATION_RANGE) {
            boss->isActive = true;

        }
        return;
    }
    
    // 持续更新boss房背景音乐
    UpdateMusicStream(sounds->bossMusic);

    // 更新防護罩狀態
    if (boss->shieldActive) {
        boss->shieldTimer -= GetFrameTime();
        if (boss->shieldTimer <= 0) {
            boss->shieldActive = false;
            // 觸發爆炸
            boss->shieldExplosionActive = true;
            boss->shieldExplosionPosition = boss->position;
            boss->shieldExplosionRadius = BOSS_SHIELD_EXPLOSION_RADIUS;
            
            // 檢查玩家是否在爆炸範圍內
            float dx = player->position.x - boss->shieldExplosionPosition.x;
            float dy = player->position.y - boss->shieldExplosionPosition.y;
            float distance = sqrtf(dx * dx + dy * dy);
            
            if (distance <= BOSS_SHIELD_EXPLOSION_RADIUS) {
                float damage = boss->absorbedDamage * BOSS_SHIELD_DAMAGE_RATIO;
                player->hp -= damage;
                player->hurtTimer = 0.1f;
            }
            
            boss->absorbedDamage = 0.0f;
        }
    } else {
        boss->shieldCooldown -= GetFrameTime();
        if (boss->shieldCooldown <= 0 && boss->isPhase2) {
            boss->shieldActive = true;
            boss->shieldTimer = BOSS_SHIELD_DURATION;
            boss->shieldCooldown = BOSS_SHIELD_COOLDOWN;
        }
    }

    // 更新爆炸效果
    if (boss->shieldExplosionActive) {
        boss->shieldExplosionRadius -= GetFrameTime() * 500.0f; // 爆炸效果收縮速度
        if (boss->shieldExplosionRadius <= 0) {
            boss->shieldExplosionActive = false;
        }
    }

    // 只有在沒有防護罩和爆炸效果時才執行其他動作
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
    if (fabs(dx) > attackTriggerDistance && !boss->isAttacking && !boss->shieldActive && !boss->shieldExplosionActive && !boss->energyPulseActive) {
        float baseSpeed = 75.0f * GetFrameTime();  // 基礎移動速度
        float moveSpeed = boss->isPhase2 ? baseSpeed * 1.4f : baseSpeed;  // 第二階段提高50%速度
        
        if (boss->facingLeft) {
            boss->position.x -= moveSpeed;
        } else {
            boss->position.x += moveSpeed;
        }
    }
    
    // 當進入攻擊範圍時，開始攻擊
    if (fabs(dx) <= attackTriggerDistance && !boss->isAttacking && !boss->shieldActive && !boss->shieldExplosionActive) {
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

    // 更新攻击状态
    if (boss->isAttacking) {
        boss->attackTimer += GetFrameTime();
        if (boss->attackTimer >= BOSS_ATTACK_DURATION) {
            boss->isAttacking = false;  // 攻击结束后切换回移动状态
            boss->attackTimer = 0;
        }
    }

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
                    attackWidth = 600;
                    attackArea = (Rectangle){
                        boss->position.x - attackWidth - 150,
                        boss->position.y - attackHeight/2,
                        attackWidth,
                        attackHeight
                    };
                } else {
                    attackWidth = 300;  // 朝右時更小的攻擊範圍
                    attackArea = (Rectangle){
                        boss->position.x + 200,
                        boss->position.y - attackHeight/2,
                        attackWidth,
                        attackHeight
                    };
                }
                
                if (CheckCollisionRecs(attackArea, player->hitbox)) {
                    if (!player->invincible) {  // 只有在非無敵狀態下才造成傷害
                        player->hp -= boss->attackDamage;
                        player->hurtTimer = 0.1f;
                    }
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
                if (boss->shieldActive) {
                    // 在防護罩期間，吸收傷害並累積
                    boss->absorbedDamage += player->damage;
                } else {
                    // 沒有防護罩時才減少血量
                    boss->health -= player->damage;
                    
                    if (boss->health <= 0) {
                        boss->isDead = true;
                    }
                }
                player->bullets[i].active = false;
                break;
            }
        }
    }

    // 第一階段技能：數據波
    if (boss->health > boss->phase1HealthThreshold) {
        // 如果boss剛激活，開始計時
        if (boss->isActive) {
            boss->dataWaveStartTimer += GetFrameTime();
            if (boss->dataWaveStartTimer >= 10.0f) {  // 10秒後
                boss->canUseDataWave = true;
                boss->dataWaveStartTimer = 0;  // 重置計時器
            }
        }

        // 更新數據波冷卻
        boss->dataWaveTimer -= GetFrameTime();
        if (boss->dataWaveTimer <= 0 && boss->canUseDataWave) {  // 只有在canUseDataWave為true時才施放
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
                    if (!player->invincible) {  // 只有在非無敵狀態下才造成傷害
                        player->hp -= boss->attackDamage;  // 使用相同的攻擊傷害
                        player->hurtTimer = 0.1f;
                    
                    player->controlsReversed = true;
                    player->controlReverseTimer = DATA_WAVE_DURATION;
                    // 立即反轉移動方向
                    player->velocity.x = -player->velocity.x;
                    player->velocity.y = -player->velocity.y;
                    boss->dataWaves[i].active = false;
                    }
                }

                // 檢查是否超出與boss的距離
                float dx = boss->dataWaves[i].position.x - boss->position.x;
                float dy = boss->dataWaves[i].position.y - boss->position.y;
                float distance = sqrtf(dx * dx + dy * dy);
                
                if (distance > 1000.0f) {
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
                
                // 更新子弹位置，傳入player參數
                boss_updateDroneBullet(&boss->drones[i].bullet, GetFrameTime(), player);
                
                boss_checkDroneBulletHit(&boss->drones[i].bullet, player);
            }
        }
    }

    // 檢查是否進入第二階段
    if (boss->health <= BOSS_PHASE2_HEALTH && !boss->isPhase2) {
        printf("Boss entering Phase 2! Health: %f, Threshold: %f\n", boss->health, BOSS_PHASE2_HEALTH);
        boss->isPhase2 = true;
        boss->canUseEnergyPulse = true;
    }
    
    // 第二階段技能邏輯
    if (boss->isPhase2) {
        printf("Phase 2 active, can use energy pulse: %d\n", boss->canUseEnergyPulse);
        if (boss->energyPulseActive) {
            printf("Energy pulse active, radius: %f\n", boss->energyPulseRadius);
        }
        // 更新能量脈衝冷卻
        if (!boss->canUseEnergyPulse) {
            boss->energyPulseCooldown -= GetFrameTime();
            if (boss->energyPulseCooldown <= 0) {
                boss->canUseEnergyPulse = true;
            }
        }
        
        // 釋放能量脈衝
        if (boss->canUseEnergyPulse && !boss->energyPulseActive) {
            boss->energyPulseActive = true;
            boss->energyPulseRadius = 0;
            
            // 根據Boss面向決定脈衝生成位置
            float offset = 100.0f;  // 偏移距離
            if (boss->facingLeft) {
                // Boss朝左，脈衝在左側
                boss->energyPulsePosition = (Vector2){
                    boss->position.x - offset,
                    boss->position.y
                };
            } else {
                // Boss朝右，脈衝在右側
                boss->energyPulsePosition = (Vector2){
                    boss->position.x + offset,
                    boss->position.y
                };
            }
            
            boss->canUseEnergyPulse = false;
            boss->energyPulseCooldown = ENERGY_PULSE_COOLDOWN;
        }
        
        // 更新能量脈衝效果
        if (boss->energyPulseActive) {
            boss->energyPulseRadius += ENERGY_PULSE_SPEED * GetFrameTime();
            
            // 檢查是否擊中玩家
            float dx = player->position.x - boss->energyPulsePosition.x;
            float dy = player->position.y - boss->energyPulsePosition.y;
            float distanceToPlayer = sqrtf(dx * dx + dy * dy);
            
            // 根據boss朝向調整碰撞檢測位置
            float collisionOffset = boss->facingLeft ? -300 : +450;  // 與繪製位置保持一致
            float adjustedDx = player->position.x - (boss->energyPulsePosition.x + collisionOffset);
            float adjustedDistanceToPlayer = sqrtf(adjustedDx * adjustedDx + dy * dy);
            
            if (adjustedDistanceToPlayer <= boss->energyPulseRadius) {
                if (!player->invincible) {  // 只有在非無敵狀態下才施加debuff
                    // 只在debuff結束時才重新施加效果
                    if (player->weakenTimer <= 0) {
                        player->damage = player->originalDamage * 0.5f;
                        player->speed = 150;
                        player->weakenTimer = ENERGY_PULSE_DURATION;
                    }
                }
            }
            
            // 脈衝結束
            if (boss->energyPulseRadius >= ENERGY_PULSE_RADIUS) {
                boss->energyPulseActive = false;
            }
        }
    }

    // 更新武器禁用計時器
    if (player->weaponDisabled) {
        player->weaponDisableTimer -= GetFrameTime();
        if (player->weaponDisableTimer <= 0) {
            player->weaponDisabled = false;
            player->weaponDisableTimer = 0;
        }
    }

    // 在更新boss邏輯後顯示debuff狀態
    DrawDebuffStatus(player);
}

void boss_draw(Boss *boss, GameTextures *textures, Vector2 screenTopLeft, Vector2 screenBottomRight, Player *player) {
    if (boss->isDead) return;
    
    // 根據狀態選擇要繪製的紋理
    Texture2D currentTexture;
    Rectangle dest;
    if (boss->isAttacking) {
        // 使用攻擊動畫幀
        currentTexture = textures->bossAttack[boss->currentFrame % 2];  // 確保只使用前兩幀
        dest = (Rectangle){boss->position.x, boss->position.y, (float)currentTexture.width, (float)currentTexture.height};
    } else {
        // 使用移動圖片
        if (boss->isPhase2) {
            currentTexture = textures->bossMovePhase2[boss->currentFrame % 5];  // 使用5帧动画
        } else {
            currentTexture = textures->bossMovePhase1[boss->currentFrame % 5];  // 使用5帧动画
        }
        // 根据朝向调整偏移
        float offset = boss->facingLeft ? -100 : 100;
        dest = (Rectangle){boss->position.x + offset, boss->position.y, (float)currentTexture.width, (float)currentTexture.height};
    }
    
    // 设置繪製參數
    Rectangle source = {0, 0, (float)currentTexture.width, (float)currentTexture.height};
    Vector2 origin = {currentTexture.width / 2, currentTexture.height / 2};

    // 根據朝向翻轉圖片
    if (!boss->facingLeft) {
        source.width = -source.width;
    }

    DrawTexturePro(currentTexture, source, dest, origin, 0, WHITE);
    
    // DEBUG: 显示攻击范围和碰撞箱
    /*if (IsKeyDown(KEY_TAB)) {
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
    }*/

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
            // 若在鏡頭內，繪製數據波
            if (boss->dataWaves[i].position.x >= screenTopLeft.x && 
                boss->dataWaves[i].position.x <= screenBottomRight.x &&
                boss->dataWaves[i].position.y >= screenTopLeft.y && 
                boss->dataWaves[i].position.y <= screenBottomRight.y) {

                // 設定旋轉角度
                float angle = atan2f(boss->dataWaves[i].direction.y, 
                                   boss->dataWaves[i].direction.x) * RAD2DEG;

                // 設定貼圖繪製資訊
                Rectangle source = { 0, 0, (float)textures->soldierbossbullet.width, 
                                   (float)textures->soldierbossbullet.height };
                Rectangle dest = {
                    boss->dataWaves[i].position.x,
                    boss->dataWaves[i].position.y,
                    51.0f,
                    50.0f
                };
                Vector2 origin = { 25.5f, 25.0f };  // 旋轉中心點

                // 更新 hitbox
                boss->dataWaves[i].hitbox = (Rectangle){
                    boss->dataWaves[i].position.x - 25.5f,
                    boss->dataWaves[i].position.y - 25.0f,
                    51.0f,
                    50.0f
                };

                // 繪製數據波圖片（可旋轉）
                DrawTexturePro(textures->soldierbossbullet, source, dest, origin, angle, WHITE);
            }
        }
    }

    
    // 繪製無人機
    for (int i = 0; i < BOSS_DRONE_COUNT; i++) {
        if (boss->drones[i].active) {
            // 若在鏡頭內，繪製無人機
            if (boss->drones[i].position.x >= screenTopLeft.x && 
                boss->drones[i].position.x <= screenBottomRight.x &&
                boss->drones[i].position.y >= screenTopLeft.y && 
                boss->drones[i].position.y <= screenBottomRight.y) {

                // 設定旋轉角度
                float angle = atan2f(boss->drones[i].direction.y, 
                                   boss->drones[i].direction.x) * RAD2DEG;

                // 設定貼圖繪製資訊
                Rectangle source = { 0, 0, 90.0f, 90.0f };
                Rectangle dest = {
                    boss->drones[i].position.x,
                    boss->drones[i].position.y,
                    90.0f,
                    90.0f
                };
                Vector2 origin = { 45.0f, 45.0f };  // 旋轉中心點

                // 更新 hitbox
                boss->drones[i].hitbox = (Rectangle){
                    boss->drones[i].position.x - 45.0f,
                    boss->drones[i].position.y - 45.0f,
                    90.0f,
                    90.0f
                };

                // 繪製無人機圖片（可旋轉）
                DrawTexturePro(textures->soldiertexture, source, dest, origin, angle, WHITE);
            }
        }
    }

    // 繪製無人機子彈
    for (int i = 0; i < BOSS_DRONE_COUNT; i++) {
        BossDroneBullet *bullet = &boss->drones[i].bullet;
        if (!bullet->active) continue;
        
        // 檢查是否超出與玩家的距離
        float dx = bullet->position.x - player->position.x;
        float dy = bullet->position.y - player->position.y;
        float distance = sqrtf(dx * dx + dy * dy);
        
        if (distance > 1000.0f) {
            bullet->active = false;
            continue;
        }

        // 計算旋轉角度
        float angle = atan2f(bullet->direction.y, bullet->direction.x) * RAD2DEG;

        // 繪製子彈
        DrawTexturePro(
            textures->soldierstagebullet,
            (Rectangle){ 0, 0, 51.0f, 50.0f },
            (Rectangle){ bullet->position.x, bullet->position.y, 51.0f, 50.0f },
            (Vector2){ 25.5f, 25.0f },
            angle,
            WHITE
        );
    }

    // 繪製能量脈衝效果
    if (boss->energyPulseActive) {
        // 繪製能量脈衝效果
        Rectangle source = { 0, 0, 782, 182 };  // 更新為新的圖片尺寸
        Rectangle dest = {
            boss->facingLeft ? 
                boss->energyPulsePosition.x - 600 :  // 朝左時的偏移調整為600
                boss->energyPulsePosition.x - 50,   // 朝右時的偏移保持不變
            boss->energyPulsePosition.y + 100,   // 調低位置
            782,  // 更新寬度
            182   // 更新高度
        };
        Vector2 origin = { 0, 0 };

        // 根據boss朝向調整圖片 - 反轉邏輯
        if (!boss->facingLeft) {
            source.width = -source.width;  // 右翻轉
        }

        DrawTexturePro(textures->bossLight, source, dest, origin, 0.0f, WHITE);
    }

    // 繪製防護罩
    if (boss->shieldActive) {
        float shieldOffset = boss->facingLeft ? 0 : -200;
        DrawCircleGradient(
            (int)(boss->position.x + BOSS_WIDTH/2 + shieldOffset),
            (int)(boss->position.y + BOSS_HEIGHT/2),
            BOSS_WIDTH * 1.5f,
            (Color){0, 255, 255, 100},
            (Color){0, 255, 255, 0}
        );
    }


    // 繪製爆炸效果
    if (boss->shieldExplosionActive) {
        float explosionOffset = boss->facingLeft ? 0 : -200;  // 爆炸效果也需要相同的偏移
        DrawCircleGradient(
            (int)(boss->shieldExplosionPosition.x + explosionOffset),
            (int)boss->shieldExplosionPosition.y,
            boss->shieldExplosionRadius,
            (Color){255, 0, 0, 150},
            (Color){255, 0, 0, 0}
        );
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
void boss_updateDroneBullet(BossDroneBullet *bullet, float deltaTime, Player *player) {
    if (!bullet->active) return;
    
    bullet->position.x += bullet->direction.x * bullet->speed * deltaTime;
    bullet->position.y += bullet->direction.y * bullet->speed * deltaTime;
    
    // 檢查是否超出與玩家的距離
    float dx = bullet->position.x - player->position.x;
    float dy = bullet->position.y - player->position.y;
    float distance = sqrtf(dx * dx + dy * dy);
    
    if (distance > 1000.0f) {
        bullet->active = false;
    }
}

// 检查子弹是否击中玩家
void boss_checkDroneBulletHit(BossDroneBullet *bullet, Player *player) {
    if (player->invincible) return;  // 如果玩家處於無敵狀態，直接返回
    
    if (!bullet->active) return;
    
    if (CheckCollisionCircleRec(bullet->position, BOSS_DRONE_BULLET_RADIUS, player->hitbox)&& !player->invincible) {
        player->hp -= BOSS_DRONE_DAMAGE;
        player->hurtTimer = 0.1f;
        // 禁用武器2秒
        player->weaponDisabled = true;
        player->weaponDisableTimer = WEAPON_DISABLE_TIME;
        bullet->active = false;
    }
}

// 修改玩家傷害處理函數
void boss_takeDamage(Boss *boss, float damage) {
    if (boss->shieldActive) {
        boss->absorbedDamage += damage;
    } else {
        boss->health -= damage;
    }
}

void DrawDebuffStatus(Player* player) {

}

