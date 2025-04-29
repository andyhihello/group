#include "menu.h"
#include "main.h"



// 選單內部使用的變數
static Rectangle startButton;
static Color startButtonColor = LIGHTGRAY;
static Color startButtonHoverColor = GRAY;
static Rectangle settingsButton;
static Color settingsButtonColor = LIGHTGRAY;
static Color settingsButtonHoverColor = GRAY;
static Texture2D backgroundTexture; // 用於儲存背景圖片

// 初始化選單
void initMenu(Texture2D background) {
    backgroundTexture = background; // 儲存背景圖片

    // 計算start button的位置
    int startButtonX = (screenWidth - BUTTON_WIDTH) / 2;
    int startButtonY = screenHeight / 2 - BUTTON_HEIGHT - BUTTON_PADDING / 2;
    startButton = (Rectangle){ startButtonX, startButtonY, BUTTON_WIDTH, BUTTON_HEIGHT };

    //計算setting button的位置
    int settingsButtonX = (screenWidth - BUTTON_WIDTH) / 2;
    int settingsButtonY = screenHeight / 2 + BUTTON_PADDING / 2;
    settingsButton = (Rectangle){ settingsButtonX, settingsButtonY, BUTTON_WIDTH, BUTTON_HEIGHT };
}

// 是否按按鈕，更新現在的選單狀態
void updateMenu(GameState *currentGameState, Player *player, Boss *boss, Drone *drone) {
    Vector2 mousePoint = GetMousePosition();

    // 檢查滑鼠是否懸停在 "Start" 按鈕上
    if (CheckCollisionPointRec(mousePoint, startButton)) {
        startButtonColor = startButtonHoverColor;
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            player_init(player);
            TraceLog(LOG_INFO, "Start Button Pressed");
            
            *currentGameState = GAME;  // 切換場景
        }
    } else {
        startButtonColor = LIGHTGRAY;
    }

    // 檢查滑鼠是否懸停在 "Settings" 按鈕上
    if (CheckCollisionPointRec(mousePoint, settingsButton)) {
        settingsButtonColor = settingsButtonHoverColor;
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            *currentGameState = SETTINGS; // 切換到設定狀態
            TraceLog(LOG_INFO, "Settings Button Pressed");
            // 在這裡載入設定相關的資源或進行初始化(可能要修改)
        }
    } else {
        settingsButtonColor = LIGHTGRAY;
    }
}

// 繪製選單畫面
void drawMenu() {

    DrawTexture(backgroundTexture, 0, 0, WHITE);//畫主畫面的背景圖片
    DrawText("Game", screenWidth / 2 - MeasureText("Game", 40) / 2, 100, 40, BLACK);//畫Game(遊戲名)
    DrawRectangleRec(startButton, startButtonColor); // 使用動態顏色
    DrawText("Start", startButton.x + (BUTTON_WIDTH - MeasureText("Start", 20)) / 2, startButton.y + (BUTTON_HEIGHT - 20) / 2, 20, BLACK);//畫start 按鍵

    DrawRectangleRec(settingsButton, settingsButtonColor); // 使用動態顏色
    DrawText("Settings", settingsButton.x + (BUTTON_WIDTH - MeasureText("Settings", 20)) / 2, settingsButton.y + (BUTTON_HEIGHT - 20) / 2, 20, BLACK);//畫setting按鍵
}