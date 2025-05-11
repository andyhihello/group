#ifndef HACK_H
#define HACK_H

#include "raylib.h"
#include "boss.h"
#include "player.h"

// 黑客小游戏场景的尺寸
#define HACK_SCENE_WIDTH 1600
#define HACK_SCENE_HEIGHT 900

// 游戏设置
#define MAX_ATTEMPTS 3
#define TIME_LIMIT 10.0f
#define MAX_STRING_LENGTH 20

// 黑客小游戏状态
typedef enum {
    HACK_IDLE,
    HACK_ACTIVE,
    HACK_COMPLETE,
    HACK_FAILED
} HackState;

// 黑客小游戏结构体
typedef struct {
    bool isActive;
    HackState state;
    float timer;
    Rectangle bounds;
    
    // 新增游戏相关变量
    char targetString[MAX_STRING_LENGTH];
    char inputString[MAX_STRING_LENGTH];
    int currentAttempt;
    int stringLength;
    bool isGameOver;
} HackScene;

// 函数声明
void hack_init(HackScene *scene);
void hack_update(HackScene *scene, Boss *boss, Player *player);
void hack_draw(HackScene *scene);
void hack_generate_new_string(HackScene *scene);
void hack_reset_game(HackScene *scene);

#endif 