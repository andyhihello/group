#include "raylib.h"
#include "menu.h" 
#include "main.h"
#include "player.h"
#include "stage.h"

int main() {
    InitWindow(screenWidth, screenHeight, "Game"); // 設定初始視窗；遊戲名稱設定為 "Game"
    SetTargetFPS(60);//(raylib)每秒鐘跑60個畫面

    //圖檔載入
    Texture2D stage1_background[stage1backgroundCount];
    for (int i = 0; i < stage1backgroundCount; i++) {
        char path[100];
        sprintf(path, "resource/scene/1-%d.png", i + 1);
        stage1_background[i] = LoadTexture(path);
    }
    Texture2D menu_background = LoadTexture("resource/scene/background.png");//下載背景圖片

    // 初始化
    initMenu(menu_background);
    GameState currentGameState = MENU;
    Player player;
    player_init(&player);
    Camera2D camera = { 0 };
    camera.target = player.position;
    camera.offset = (Vector2){ screenWidth * 0.2f, screenHeight / 2.0f };
    camera.zoom = 1.0f;
    float camX = player.position.x;
    float halfScreen = screenWidth * 0.2f;
    int debug = 0;

    //檢查圖片載入
    bool loadError = false;
    for (int i = 0; i < stage1backgroundCount; i++) {
        if (stage1_background[i].id == 0) {
            loadError = true;
        }
    }
    if (menu_background.id == 0) {
        loadError = true;
    }
    if (player.stand.id == 0) {
        loadError = true;
    }
    for (int i = 0; i < 9; i++) {
        if (player.runFrames[i].id == 0) {
            loadError = true;
        }
    }
    if (loadError) {
        TraceLog(LOG_ERROR, "Resource loading failed");
        CloseWindow();
        return -1;
    }

    // 主遊戲迴圈
    while (!WindowShouldClose()) {

        if (currentGameState == MENU) {
            // 輸入處理 (選單的輸入處理在 updateMenu 中)
            updateMenu(&currentGameState); 
        } 

        else if (currentGameState == GAME) {
            if(IsKeyPressed(KEY_H)) debug = (debug +1)%2;
            if (player.stage == 1){
                player_move(&player);
                camX = player.position.x;
                //使背景不跑出畫面
                if (camX < halfScreen) camX = halfScreen;
                if (camX > stage1Width - screenWidth * 0.8f) camX = stage1Width - screenWidth * 0.8f;
                camera.target = (Vector2){ camX, screenHeight / 2.0f  };
                player_attack(&player, camera);
                stage_door(&player);
            }

            if(player.tutorial){
                if (IsKeyPressed(KEY_SPACE)) {
                    player.tutorial++;
                    if(player.tutorial == 3)player.tutorial = false; 
                }
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
            if (player.stage == 1){
                for (int i = 0; i < stage1backgroundCount; i++) {
                    DrawTexture(stage1_background[i], i * stage1pictureWidth, 0, WHITE);
                }

                stage_drawdoortext(); 
                player_draw(&player);
                player_drawbullet(&player,camera);
                
                if(debug){
                    player_drawhitbox(&player);
                    stage_drawhitbox();
                }     
            }
            EndMode2D();

            player_UI(&player);           
            if(player.tutorial){
                stage_drawtutorial(&player);
            }
            if(debug){
                
            }
        } 
        else if (currentGameState == SETTINGS) {
            // 在這裡繪製設定畫面
        }

        EndDrawing();
    }

    // 釋放資源並關閉  
    UnloadTexture(menu_background);
    for (int i = 0; i < stage1backgroundCount; i++) {
        UnloadTexture(stage1_background[i]);
    }
    player_unload(&player);

    CloseWindow();
    return 0;
}