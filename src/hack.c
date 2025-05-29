#include "hack.h"
#include "main.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>

// 生成随机字符串
void hack_generate_new_string(HackScene *scene) {
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    scene->stringLength = 4;  // 修改為4個字符
    scene->inputString[0] = '\0';  // 清空輸入字符串
    
    for (int i = 0; i < scene->stringLength; i++) {
        int index = rand() % (sizeof(charset) - 1);
        scene->targetString[i] = charset[index];
    }
    scene->targetString[scene->stringLength] = '\0';
}

void hack_reset_game(HackScene *scene) {
    scene->currentAttempt = 0;
    scene->timer = TIME_LIMIT;
    scene->isGameOver = false;
    scene->state = HACK_ACTIVE;
    hack_generate_new_string(scene);
}

void hack_init(HackScene *scene) {
    scene->isActive = false;
    scene->state = HACK_IDLE;
    scene->timer = TIME_LIMIT;
    scene->bounds = (Rectangle){
        0,
        0,
        HACK_SCENE_WIDTH,
        HACK_SCENE_HEIGHT
    };
    scene->currentAttempt = 0;
    scene->isGameOver = false;
    memset(scene->inputString, 0, sizeof(scene->inputString));  // 確保輸入字符串被初始化為空
    srand(time(NULL));
}

void hack_update(HackScene *scene, Boss *boss, Player *player) {
    static double lastTime = 0;
    double currentTime = GetTime();
    double deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    // 檢查Boss血量是否低於10%
    if (boss->health <= BOSS_HEALTH * 0.1f && !scene->isActive) {
        scene->isActive = true;
        hack_reset_game(scene);
        player->stage = 3;
    }

    // 只在遊戲激活且處於活動狀態時更新計時器
    if (scene->isActive && scene->state == HACK_ACTIVE) {
        // 更新計時器
        scene->timer -= deltaTime;
        
        // 檢查是否超時
        if (scene->timer <= 0) {
            scene->currentAttempt++;
            if (scene->currentAttempt >= MAX_ATTEMPTS) {
                scene->state = HACK_FAILED;
                boss->health = BOSS_HEALTH * 0.3f;
                scene->isActive = false;
                player->stage = 2;
            } else {
                scene->timer = TIME_LIMIT;
                hack_generate_new_string(scene);
            }
        }

        // 新的輸入處理邏輯
        if (IsKeyPressed(KEY_BACKSPACE)) {
            int len = strlen(scene->inputString);
            if (len > 0) {
                scene->inputString[len - 1] = '\0';
            }
        }

        int key = GetCharPressed();
        if (key > 0) {
            int len = strlen(scene->inputString);
            if (len < scene->stringLength) {
                scene->inputString[len] = (char)key;
                scene->inputString[len + 1] = '\0';
            }
        }

        // 檢查輸入是否正確
        if (strcmp(scene->inputString, scene->targetString) == 0) {
            scene->state = HACK_COMPLETE;
            boss->health = 0;
            boss->isDead = true;
            scene->isActive = false;
            scene->state = HACK_IDLE;
            player->stage = 4;
            player->stageChanged = true;  // 设置stageChanged标志
        }
    }
}

void hack_draw(HackScene *scene) {
    if (!scene->isActive) return;

    // 绘制黑客场景背景
    DrawRectangleRec(scene->bounds, BLACK);
    
    // 绘制目标字符串
    const char* targetText = scene->targetString;
    int fontSize = 40;
    Vector2 targetSize = MeasureTextEx(GetFontDefault(), targetText, fontSize, 2);
    Vector2 targetPos = {
        (HACK_SCENE_WIDTH - targetSize.x) / 2,
        (HACK_SCENE_HEIGHT - targetSize.y) / 2 - 50
    };
    DrawText(targetText, targetPos.x, targetPos.y, fontSize, GREEN);

    // 绘制输入字符串
    const char* inputText = scene->inputString;
    if (inputText[0] != '\0') {  // 只在有輸入時顯示
        Vector2 inputSize = MeasureTextEx(GetFontDefault(), inputText, fontSize, 2);
        Vector2 inputPos = {
            (HACK_SCENE_WIDTH - inputSize.x) / 2,
            (HACK_SCENE_HEIGHT - inputSize.y) / 2 + 50
        };
        DrawText(inputText, inputPos.x, inputPos.y, fontSize, YELLOW);
    }

    // 绘制剩余时间和尝试次数
    char timeText[32];
    sprintf(timeText, "Time: %.1f", scene->timer);
    int timeWidth = MeasureText(timeText, 30);
    DrawText(timeText, 
        (HACK_SCENE_WIDTH - timeWidth) / 2,
        100,
        30, WHITE);

    char attemptText[32];
    sprintf(attemptText, "Attempts: %d/%d", scene->currentAttempt + 1, MAX_ATTEMPTS);
    int attemptWidth = MeasureText(attemptText, 30);
    DrawText(attemptText, 
        (HACK_SCENE_WIDTH - attemptWidth) / 2,
        150,
        30, WHITE);

    // 如果游戏结束，显示结果
    if (scene->state == HACK_COMPLETE) {
        DrawText("HACK SUCCESSFUL!", 
            (HACK_SCENE_WIDTH - MeasureText("HACK SUCCESSFUL!", 60)) / 2,
            HACK_SCENE_HEIGHT / 2 - 150,
            60, GREEN);
    } else if (scene->state == HACK_FAILED) {
        DrawText("HACK FAILED!", 
            (HACK_SCENE_WIDTH - MeasureText("HACK FAILED!", 60)) / 2,
            HACK_SCENE_HEIGHT / 2 - 150,
            60, RED);
    }
} 