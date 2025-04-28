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
    
    // 加载攻击动画帧，修改路径格式以匹配其他资源的加载方式
    const char* frameFiles[] = {
        "attack_0001.png",
        "attack_0003.png",
        "attack_0005.png",
        "attack_0007.png",
        "attack_0009.png",
        "attack_0011.png"
    };

    for (int i = 0; i < BOSS_ATTACK_FRAME_COUNT; i++) {
        char path[100];
        sprintf(path, "resource/boss/attack/%s", frameFiles[i]);  // 移除 "../"
        printf("Trying to load: %s\n", path);
        boss->attackFrames[i] = LoadTexture(path);
        
        // 检查图片是否成功加载
        if (boss->attackFrames[i].id == 0) {
            printf("Failed to load boss attack frame: %s\n", path);
        } else {
            printf("Successfully loaded boss attack frame: %s\n", path);
            printf("Texture size: %dx%d\n", boss->attackFrames[i].width, boss->attackFrames[i].height);
        }
    }
    
    boss->currentFrame = 0;
    boss->frameCounter = 0;
    boss->frameSpeed = 0.1f;
    boss->isAttacking = false;
    boss->facingLeft = false;
    boss->attackTimer = 0;
    boss->attackCooldown = 2.0f;
}

void boss_update(Boss *boss, Player *player) {
    // 计算boss左右两侧的攻击范围
    float leftRangeStart = boss->position.x - BOSS_ATTACK_RANGE;
    float rightRangeEnd = boss->position.x + BOSS_ATTACK_RANGE;
    float playerX = player->position.x;
    
    // 检查玩家是否在攻击范围内（左右两侧都检查）
    bool inRange = (playerX >= leftRangeStart && playerX <= rightRangeEnd);
    
    // 更新朝向：玩家在左边时boss朝左，玩家在右边时boss朝右
    boss->facingLeft = (playerX < boss->position.x);
    
    // 在攻击范围内且不在攻击冷却时开始攻击
    if (inRange && !boss->isAttacking) {
        if (boss->attackTimer <= 0) {
            boss->isAttacking = true;
            boss->currentFrame = 0;
            boss->frameCounter = 0;
            boss->attackTimer = boss->attackCooldown;
        }
    }
    
    // 更新攻击动画
    if (boss->isAttacking) {
        boss->frameCounter += GetFrameTime();
        if (boss->frameCounter >= boss->frameSpeed) {
            boss->frameCounter = 0;
            boss->currentFrame++;
            
            // 完整的动画序列：1-3-5-7-9-11-9-7-5-3-1
            if (boss->currentFrame >= BOSS_ATTACK_FRAME_COUNT * 2) {
                boss->currentFrame = 0;
                boss->isAttacking = false;
            }
        }
    }
    
    // 更新攻击冷却时间
    if (boss->attackTimer > 0) {
        boss->attackTimer -= GetFrameTime();
    }
    
    // 更新hitbox位置
    boss->hitbox.x = boss->position.x - 350;
    boss->hitbox.y = boss->position.y - 350;
    
    // DEBUG: 打印调试信息
    if (IsKeyDown(KEY_TAB)) {
        printf("Player X: %.2f, Boss X: %.2f, In Range: %d, Facing Left: %d\n",
               playerX, boss->position.x, inRange, boss->facingLeft);
        printf("Attack Range: %.2f - %.2f\n", leftRangeStart, rightRangeEnd);
    }
}

void boss_draw(Boss *boss) {
    // 获取当前帧的索引
    int frameIndex;
    if (boss->currentFrame < BOSS_ATTACK_FRAME_COUNT) {
        frameIndex = boss->currentFrame;
    } else {
        frameIndex = (BOSS_ATTACK_FRAME_COUNT * 2 - 1) - boss->currentFrame;
    }
    frameIndex = frameIndex % BOSS_ATTACK_FRAME_COUNT;
    
    if (boss->attackFrames[frameIndex].id != 0) {
        float width = (float)boss->attackFrames[frameIndex].width;
        float height = (float)boss->attackFrames[frameIndex].height;
        
        Rectangle source = {
            0, 0,
            boss->facingLeft ? width : -width,
            height
        };
        
        Rectangle dest = {
            boss->position.x,
            boss->position.y,
            width,
            height
        };
        
        Vector2 origin = {width/2, height/2};
        
        DrawTexturePro(boss->attackFrames[frameIndex],
                       source, dest, origin, 0, WHITE);
        
        // DEBUG: 显示攻击范围
        if (IsKeyDown(KEY_TAB)) {
            Color rangeColor = {255, 0, 0, 128};
            DrawRectangle(boss->position.x - BOSS_ATTACK_RANGE,
                         boss->position.y - 100,
                         BOSS_ATTACK_RANGE * 2,
                         200,
                         rangeColor);
            DrawRectangleRec(boss->hitbox, (Color){0, 255, 0, 128});
        }
    }
}

void boss_unload(Boss *boss) {
    for (int i = 0; i < BOSS_ATTACK_FRAME_COUNT; i++) {
        if (boss->attackFrames[i].id != 0) {
            UnloadTexture(boss->attackFrames[i]);
        }
    }
} 