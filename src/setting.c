#include "raylib.h"
#include "main.h"
#include "menu.h"
#include "texture.h"

// 音量數值 (0.0f ~ 1.0f)
float bgmVolume = 0.5f;
float sfxVolume = 0.5f;

// 每條滑桿的位置與寬度
#define SLIDER_WIDTH 400
#define SLIDER_HEIGHT 40
#define SLIDER_X 800

static Rectangle bgmSlider = { SLIDER_X, 350, SLIDER_WIDTH, SLIDER_HEIGHT };
static Rectangle sfxSlider = { SLIDER_X, 450, SLIDER_WIDTH, SLIDER_HEIGHT };
static Rectangle backButton = { screenWidth / 2 - 100, screenHeight - 100, 200, 50 };
Color backButtonColor = LIGHTGRAY;

// 更新滑桿邏輯
void updateSettings(GameState *currentGameState) {
    Vector2 mouse = GetMousePosition();\

    if (CheckCollisionPointRec(mouse, backButton)) {
        backButtonColor = GRAY;
    }
    else backButtonColor = LIGHTGRAY;

    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse, bgmSlider)) {
            bgmVolume = (mouse.x - bgmSlider.x) / SLIDER_WIDTH;
            if (bgmVolume < 0) bgmVolume = 0;
            if (bgmVolume > 1) bgmVolume = 1;
        } else if (CheckCollisionPointRec(mouse, sfxSlider)) {
            sfxVolume = (mouse.x - sfxSlider.x) / SLIDER_WIDTH;
            if (sfxVolume < 0) sfxVolume = 0;
            if (sfxVolume > 1) sfxVolume = 1;
        } else if (CheckCollisionPointRec(mouse, backButton)) {
            *currentGameState = MENU;
        }
    }

}

// 繪製滑桿與標籤
void drawSettings(GameTextures *textures) {

    DrawTexture(textures->settingBackground, 0, 0, WHITE);//背景圖片
    DrawText("Settings - Adjust Volume", 420, 150, 60, WHITE);

    DrawText("BGM Volume", 400, 350, 50, WHITE);
    DrawRectangleRec(bgmSlider, GRAY);
    DrawRectangle(SLIDER_X, 350, bgmVolume * SLIDER_WIDTH, SLIDER_HEIGHT, GREEN);

    DrawText("SFX Volume", 400, 450, 50, WHITE);
    DrawRectangleRec(sfxSlider, GRAY);
    DrawRectangle(SLIDER_X, 450, sfxVolume * SLIDER_WIDTH, SLIDER_HEIGHT, GREEN);


    // 畫按鈕背景
    
    DrawRectangleRec(backButton, backButtonColor);

    // 畫按鈕文字置中
    const char *text = "Back to Menu";
    int textWidth = MeasureText(text, 20);
    DrawText(text, backButton.x + (backButton.width - textWidth) / 2, backButton.y + 15, 20, BLACK);
}