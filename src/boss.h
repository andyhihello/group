#ifndef BOSS_H
#define BOSS_H

#include "raylib.h"
#include "player.h"
#include <stdio.h>

#define BOSS_WIDTH 200
#define BOSS_HEIGHT 300
#define BOSS_SPEED 3
#define BOSS_ATTACK_FRAME_COUNT 6  // 实际帧数为6（1,3,5,7,9,11）
#define BOSS_ATTACK_RANGE 300.0f
#define BOSS_ATTACK_COOLDOWN 2.0f
#define BOSS_HEALTH 100

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
} Boss;

void boss_init(Boss *boss);
void boss_update(Boss *boss, Player *player);
void boss_draw(Boss *boss, GameTextures *textures);

#endif 