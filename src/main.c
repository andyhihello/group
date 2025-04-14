#include "raylib.h"

#define BUTTON_WIDTH  450
#define BUTTON_HEIGHT 90
#define BUTTON_PADDING 50

//***現在是在什麼遊戲狀態
typedef enum { MENU, GAME, SETTINGS } GameState;

int main() {
    // 初始化
    const int screenWidth = 1600;
    const int screenHeight = 900;
    InitWindow(screenWidth, screenHeight, "Game"); // 設定初始視窗；遊戲名稱設定為 "Game"
    SetTargetFPS(60);//(raylib)每秒鐘跑60個畫面

    Texture2D background = LoadTexture("resource/background.png");//下載背景圖片


    GameState currentGameState = MENU;

    // 計算start button的位置
    int startButtonX = (screenWidth - BUTTON_WIDTH) / 2;
    int startButtonY = screenHeight / 2 - BUTTON_HEIGHT - BUTTON_PADDING / 2;   //?
    Rectangle startButton = { startButtonX, startButtonY, BUTTON_WIDTH, BUTTON_HEIGHT }; //初始的點，往右下長出寬、長
    Color startButtonColor = LIGHTGRAY;       // 原始顏色
    Color startButtonHoverColor = GRAY;   // 滑鼠懸停時的顏色
 
    //計算setting button的位置
    int settingsButtonX = (screenWidth - BUTTON_WIDTH) / 2;
    int settingsButtonY = screenHeight / 2 + BUTTON_PADDING / 2;
    Rectangle settingsButton = { settingsButtonX, settingsButtonY, BUTTON_WIDTH, BUTTON_HEIGHT };
    Color settingsButtonColor = LIGHTGRAY;    // 原始顏色
    Color settingsButtonHoverColor = GRAY; // 滑鼠懸停時的顏色

    // 主遊戲迴圈
    while (!WindowShouldClose()) {
        // 輸入處理
        Vector2 mousePoint = GetMousePosition();

        // 更新
        if (currentGameState == MENU) {
            // 檢查滑鼠是否懸停在 "Start" 按鈕上
            if (CheckCollisionPointRec(mousePoint, startButton)) {//若點擊左鍵
                startButtonColor = startButtonHoverColor;//按鈕顏色改變
                if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                    currentGameState = GAME; // 切換到遊戲狀態
                    TraceLog(LOG_INFO, "Start Button Pressed");
                    // ***在這裡載入遊戲相關的資源或進行初始化  (可能要修改)
                }
            } else {
                startButtonColor = LIGHTGRAY; // 恢復原始顏色(可能要改)
            }

            // 檢查滑鼠是否懸停在 "Settings" 按鈕上
            if (CheckCollisionPointRec(mousePoint, settingsButton)) {
                settingsButtonColor = settingsButtonHoverColor;
                if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                    currentGameState = SETTINGS; // 切換到設定狀態
                    TraceLog(LOG_INFO, "Settings Button Pressed");
                    // 在這裡載入設定相關的資源或進行初始化(可能要修改)
                }
            } else {
                settingsButtonColor = LIGHTGRAY; // 恢復原始顏色
            }
        } else if (currentGameState == GAME) {
            // 遊戲邏輯
            // ...
        } else if (currentGameState == SETTINGS) {
            // 設定邏輯
            // ...
        }

        // 開始繪製
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (currentGameState == MENU) {
            DrawText("Game", screenWidth / 2 - MeasureText("Game", 40) / 2, 100, 40, BLACK);//畫Game(遊戲名)
            DrawTexture(background, 0, 0, WHITE);//畫主畫面的背景圖片
            DrawRectangleRec(startButton, startButtonColor); // 使用動態顏色
            DrawText("Start", startButtonX + (BUTTON_WIDTH - MeasureText("Start", 20)) / 2, startButtonY + (BUTTON_HEIGHT - 20) / 2, 20, BLACK);//畫start 按鍵

            DrawRectangleRec(settingsButton, settingsButtonColor); // 使用動態顏色
            DrawText("Settings", settingsButtonX + (BUTTON_WIDTH - MeasureText("Settings", 20)) / 2, settingsButtonY + (BUTTON_HEIGHT - 20) / 2, 20, BLACK);//畫setting按鍵
        } else if (currentGameState == GAME) {
            DrawText("Playing Game...", 100, 100, 20, BLACK);
            // 在這裡繪製遊戲畫面
        } else if (currentGameState == SETTINGS) {
            DrawText("Settings Menu...", 100, 100, 20, BLACK);
            // 在這裡繪製設定畫面
        }

        EndDrawing();
    }

    // 釋放資源並關閉
    CloseWindow();
    UnloadTexture(background);

    return 0;
}