#include "stage.h"
#include "main.h"
#include "setting.h"

void stage_drawtutorial(Player *player) {
    // 背景遮罩
    // 背景遮罩
    int boxWidth = 1200;
    int boxHeight = 350;
    int boxHeight = 350;
    int boxX = 200;
    int boxY = 120;
    int boxY = 120;

    DrawRectangle(boxX, boxY, boxWidth, boxHeight, Fade(BLACK, 0.95f));
    DrawText("GAME TUTORIAL", boxX + boxWidth / 2 - MeasureText("GAME TUTORIAL", 40) / 2, boxY + 20, 40, WHITE);
    DrawRectangle(boxX, boxY, boxWidth, boxHeight, Fade(BLACK, 0.95f));
    DrawText("GAME TUTORIAL", boxX + boxWidth / 2 - MeasureText("GAME TUTORIAL", 40) / 2, boxY + 20, 40, WHITE);

    int textStartX = boxX + 40;
    int textStartY = boxY + 80;

    switch (player->tutorial) {
    case 1:
        DrawText("Basic Controls:", textStartX, textStartY, 28, YELLOW);
        DrawText("A / D       - Move Left / Right", textStartX + 20, textStartY + 40, 22, WHITE);
        DrawText("W           - Jump", textStartX + 20, textStartY + 70, 22, WHITE);
        DrawText("Mouse Left  - Shoot", textStartX + 20, textStartY + 100, 22, WHITE);
        DrawText("R           - Reload Ammo", textStartX + 20, textStartY + 130, 22, WHITE);
        DrawText("E           - Activate Shield (invincibility)", textStartX + 20, textStartY + 160, 22, WHITE);
        DrawText("C           - Spend 1 coin to heal 60 HP (HP will not exceed 100)", textStartX + 20, textStartY + 190, 22, WHITE);
        break;

    case 2:
        DrawText("Skill Upgrade System:", textStartX, textStartY, 28, YELLOW);
        DrawText("Collect COINS to upgrade your abilities! PRESS [num] to upgrade", textStartX + 20, textStartY + 40, 22, YELLOW);
        DrawText("[1] Reload Speed    - Faster reloading", textStartX + 20, textStartY + 70, 22,  WHITE);
        DrawText("[2] Ammo Capacity   - Carry more bullets", textStartX + 20, textStartY + 100, 22,  WHITE);
        DrawText("[3] Shield Cooldown - Shorter invincibility cooldown", textStartX + 20, textStartY + 130, 22,  WHITE);
        DrawText("Each upgrade costs coins and can be leveled up twice.", textStartX + 20, textStartY + 170, 22, RED);
        break;

    case 3:
        DrawText("Mission Objective:", textStartX, textStartY, 28, YELLOW);
        DrawText("Infiltrate NEX Corp's cyber base and stop the Neural Override Project.", textStartX + 20, textStartY + 40, 22, WHITE);
        DrawText("Defeat all enemies including the boss to save humanity.", textStartX + 20, textStartY + 70, 22, WHITE);
        DrawText("If you die, you need to pay 2 coins to revive, or it's game over.", textStartX + 20, textStartY + 150, 32,  RED);
        break;
    }

    // 指示文字
    // 指示文字
    const char *continueText = "Press [SPACE] to continue...";
    DrawText(continueText, boxX + boxWidth - MeasureText(continueText, 25) - 20, boxY + boxHeight - 30, 25, GREEN);
    DrawText(continueText, boxX + boxWidth - MeasureText(continueText, 25) - 20, boxY + boxHeight - 30, 25, GREEN);
}

void stage_door(Player *player,GameSounds *sounds){
    Rectangle stage1_door = { 14050, 0, 480, 900 };
    if(CheckCollisionRecs(player->hitbox, stage1_door) && IsKeyPressed(KEY_SPACE)){
        PlaySound(sounds->enterbossstage);
        WaitTime(0.5);
        player->stage = 2;
        player->position = (Vector2){100, 300};
    }
}

void stage_drawdoortext(){
    DrawText("Press SPACE to enter", 14150, 100, 25, WHITE);
}

void stage_drawhitbox(){
    Rectangle stage1_door = { 14050, 0, 480, 900 };
    DrawRectangleLinesEx(stage1_door, 2, (Color){255, 0, 0, 180});
}

void stage2_init(Texture2D *backgrounds) {
    for (int i = 0; i < stage2backgroundCount; i++) {
        char path[100];
        sprintf(path, "resource/scene/2-%d.png", i + 1);
        backgrounds[i] = LoadTexture(path);
    }
}

void stage2_draw(Texture2D *backgrounds) {
    for (int i = 0; i < stage2backgroundCount; i++) {
        DrawTexture(backgrounds[i], i * stage2pictureWidth, 0, WHITE);
    }
}

void stage2_update(Boss *boss, Player *player, GameSounds *sounds) {
    // 在进入stage2时播放音效
    static bool firstEnter = true;
    if (firstEnter) {
        PlayMusicStream(sounds->bossMusic); // 开始播放boss房背景音乐
        firstEnter = false;
    }
    
    // 持续更新boss房背景音乐
    UpdateMusicStream(sounds->bossMusic);
    
    boss_update(boss, player, sounds);
}

void stage_drawgridlines() {
    int gridSize = 1000;
    Color lineColor = Fade(GRAY, 0.5f);  // 半透明灰色

    for (int x = 0; x <= stage1Width; x += gridSize) {
        DrawLine(x, 0, x, stage1Width, lineColor);
    }

    for (int y = 0; y <= screenHeight; y += gridSize) {
        DrawLine(0, y, screenWidth, y, lineColor);
    }
}

// 儲存通關時間到檔案
void stage_saveCompletionTime(float completeTime) {
    FILE *file = fopen("compeletetime.txt", "a");
    if (file) {
        fprintf(file, "%.2f\n", completeTime);
        fclose(file);
    }
}

// 讀取並顯示前 5 名最快時間
void stage_displayTopCompletionTimes() {
    float times[100];
    int count = 0;

    FILE *file = fopen("compeletetime.txt", "r");
    if (file) {
        while (fscanf(file, "%f", &times[count]) != EOF && count < 1000) {
            count++;
        }
        fclose(file);
    }

    // 排序（氣泡排序）
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (times[j] > times[j + 1]) {
                float temp = times[j];
                times[j] = times[j + 1];
                times[j + 1] = temp;
            }
        }
    }

    // 顯示前 5 名
    DrawText("Top 5 Completion Times:", 1500, 120, 50, GOLD);
    for (int i = 0; i < count && i < 5; i++) {
        char buffer[64];
        sprintf(buffer, "%d. %.2f seconds", i + 1, times[i]);
        DrawText(buffer, 1520, 220 + i * 80, 70, WHITE);
    }
}

void stage_exitdoor(Player *player,GameState *currentGameState,bool *Isinit){
    Rectangle exitdoor = { 2500, 300, 400, 700 };
    if(CheckCollisionRecs(player->hitbox, exitdoor) && IsKeyPressed(KEY_SPACE)){
        *currentGameState = MENU;
        *Isinit = false;
    }
}
