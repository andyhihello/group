#include "raylib.h"
#include "menu.h" 
#include "main.h"
#include "player.h"

int main() {
    InitWindow(screenWidth, screenHeight, "Game"); // 設定初始視窗；遊戲名稱設定為 "Game"
    SetTargetFPS(60);//(raylib)每秒鐘跑60個畫面

    //圖檔載入
    Texture2D scene1_1 = LoadTexture("resource/scene/1-1.png");
    Texture2D background = LoadTexture("resource/scene/background.png");//下載背景圖片
    // 初始化
    initMenu(screenWidth, screenHeight, background);
    GameState currentGameState = MENU;
    Player player;
    Initplayer(&player);
    Camera2D camera = { 0 };
    camera.target = player.position;
    camera.offset = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f };
    camera.zoom = 1.0f;
    float camX = player.position.x;
    float halfScreen = screenWidth / 2.0f;
    int stage = 1;

    // 主遊戲迴圈
    while (!WindowShouldClose()) {
        // 輸入處理 (選單的輸入處理在 updateMenu 中)
        // 更新
        if (currentGameState == MENU) {
            updateMenu(&currentGameState); 
        } 
        else if (currentGameState == GAME) {
            if (stage == 1){
                TraceLog(LOG_INFO, "進入 GAME 狀態！");
                Moveplayer(&player);
                playerattrack(&player);
                camX = player.position.x;
                //使背景不跑出畫面
                if (camX < halfScreen) camX = halfScreen;
                if (camX > stage1Width - halfScreen) camX = stage1Width - halfScreen;
                camera.target = (Vector2){ camX, screenHeight / 2.0f  };
            }
        } 
        else if (currentGameState == SETTINGS) {
            // 設定邏輯
            // ...
        }
            
        

        // 開始繪製
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (currentGameState == MENU) {
            drawMenu();
        } 
        else if (currentGameState == GAME) {
            BeginMode2D(camera);
            if (stage == 1){
                    DrawTexture(scene1_1, 0, 0, WHITE); 
                    //腳色繪製
                Texture2D frame = player.isRunning ? player.runFrames[player.currentFrame] : player.stand;
                if (player.facingRight) {
                    DrawTextureV(frame, player.position, WHITE);
                } 
                else {
                    DrawTexturePro(
                        frame,
                        (Rectangle){ 0, 0, (float)-frame.width, (float)frame.height },  // 負寬度實現鏡像
                        (Rectangle){ player.position.x, player.position.y, (float)frame.width, (float)frame.height },
                        (Vector2){ 0, 0 }, 0.0f, WHITE
                    );
                    }

                drawplayerbullet();       
            }
            EndMode2D();
            attrackdata(&player);
        } 
        else if (currentGameState == SETTINGS) {
            // 在這裡繪製設定畫面
        }

        EndDrawing();
    }

    // 釋放資源並關閉
    CloseWindow();
    UnloadTexture(background);
    UnloadTexture(scene1_1);
    UnloadTexture(player.stand);
    for (int i = 0; i < 9; i++) UnloadTexture(player.runFrames[i]);

    return 0;
}