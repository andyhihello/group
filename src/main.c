#include "raylib.h"
#include "menu.h" // 引入 menu.h

int main() {
    // 初始化
    const int screenWidth = 1600;
    const int screenHeight = 900;
    InitWindow(screenWidth, screenHeight, "Game"); // 設定初始視窗；遊戲名稱設定為 "Game"
    SetTargetFPS(60);//(raylib)每秒鐘跑60個畫面

    Texture2D background = LoadTexture("resource/background.png");//下載背景圖片

    GameState currentGameState = MENU;

    // 初始化選單
    initMenu(screenWidth, screenHeight, background);

    // 主遊戲迴圈
    while (!WindowShouldClose()) {
        // 輸入處理 (選單的輸入處理在 updateMenu 中)
        // 更新
        if (currentGameState == MENU) {
            updateMenu(&currentGameState); 
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
            drawMenu();
        } else if (currentGameState == GAME) {
            // 在這裡繪製遊戲畫面
        } else if (currentGameState == SETTINGS) {
            // 在這裡繪製設定畫面
        }

        EndDrawing();
    }

    // 釋放資源並關閉
    CloseWindow();
    UnloadTexture(background);

    return 0;
}