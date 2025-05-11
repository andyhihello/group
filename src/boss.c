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
}

void boss_draw(Boss *boss, GameTextures *textures) {
    if (boss->isDead) return;
    
    int frameIndex = boss->currentFrame < BOSS_ATTACK_FRAME_COUNT
                   ? boss->currentFrame
                   : (BOSS_ATTACK_FRAME_COUNT * 2 - 1) - boss->currentFrame;

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

