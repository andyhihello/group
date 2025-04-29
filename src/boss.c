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
}

void boss_update(Boss *boss, Player *player) {
    float leftRangeStart = boss->position.x - BOSS_ATTACK_RANGE;
    float rightRangeEnd = boss->position.x + BOSS_ATTACK_RANGE;
    float playerX = player->position.x;
    bool inRange = (playerX >= leftRangeStart && playerX <= rightRangeEnd);

    boss->facingLeft = (playerX < boss->position.x);

    if (inRange && !boss->isAttacking && boss->attackTimer <= 0) {
        boss->isAttacking = true;
        boss->currentFrame = 0;
        boss->frameCounter = 0;
        boss->attackTimer = boss->attackCooldown;
    }

    if (boss->isAttacking) {
        boss->frameCounter += GetFrameTime();
        if (boss->frameCounter >= boss->frameSpeed) {
            boss->frameCounter = 0;
            boss->currentFrame++;
            if (boss->currentFrame >= BOSS_ATTACK_FRAME_COUNT * 2) {
                boss->currentFrame = 0;
                boss->isAttacking = false;
            }
        }
    }

    if (boss->attackTimer > 0) boss->attackTimer -= GetFrameTime();

    boss->hitbox.x = boss->position.x - 350;
    boss->hitbox.y = boss->position.y - 350;
}


void boss_draw(Boss *boss, GameTextures *textures) {
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

