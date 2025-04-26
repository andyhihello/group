#include "main.h"
#include "stage1.h"


void stage1_drawtutorial(Player *player) {
    // 背景半透明遮罩
    int boxWidth = 1200;
    int boxHeight = 300;
    int boxX = 200;
    int boxY = 150;

    DrawRectangle(boxX, boxY, boxWidth, boxHeight, Fade(BLACK, 0.8f));

    // 標題置中
    DrawText("STAGE 1: INFILTRATION", boxX + boxWidth / 2 - MeasureText("STAGE 1: INFILTRATION", 30) / 2, boxY + 20, 30, YELLOW);

    int textStartX = boxX + 40;
    int textStartY = boxY + 80;

    if (player->tutorial == 1) {
        DrawText("Basic Controls:", textStartX, textStartY, 25, WHITE);
        DrawText("A / D - Move Left / Right", textStartX + 20, textStartY + 40, 20, WHITE);
        DrawText("W - Jump", textStartX + 20, textStartY + 70, 20, WHITE);
        DrawText("Left Mouse Button - Shoot", textStartX + 20, textStartY + 100, 20, WHITE);
        DrawText("R - Reload", textStartX + 20, textStartY + 130, 20, WHITE);
    }

    if (player->tutorial == 2) {
        DrawText("Mission Objective:", textStartX, textStartY, 25, WHITE);
        DrawText("Infiltrate NEX Corp headquarters, defeat the enemies, and move forward.", textStartX + 20, textStartY + 40, 20, WHITE);

        DrawText("Warning:", textStartX, textStartY + 100, 25, WHITE);
        DrawText("Limited ammo. Watch your bullets! And use \"R\" to reload.", textStartX + 20, textStartY + 140, 20, WHITE);
        DrawText("Avoid enemy attacks. Proceed with caution!", textStartX + 20, textStartY + 170, 20, WHITE);
    }

    // 提示文字置中在畫面最下方
    const char *continueText = "Press [SPACE] to continue...";
    DrawText(continueText, boxX + boxWidth - MeasureText(continueText, 25) -20 , boxY + boxHeight - 30, 25, GREEN);
}

void stage1_door(Player *player){
    Rectangle stage1_door = { 14050, 0, 480, 900 };
    if(CheckCollisionRecs(player->hitbox, stage1_door) && IsKeyPressed(KEY_SPACE)){
        player->stage = 2;
    }
}

void stage1_drawdoortext(){
    DrawText("Press SPACE to enter", 14150, 100, 25, WHITE);
}

void stage1_drawhitbox(){
    Rectangle stage1_door = { 14050, 0, 480, 900 };
    DrawRectangleLinesEx(stage1_door, 2, (Color){255, 0, 0, 180});
}