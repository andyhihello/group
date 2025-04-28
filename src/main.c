#include "raylib.h"
#include "menu.h" 
#include "main.h"
#include "player.h"
#include "stage.h"
#include "enemy.h"
#include "boss.h"

int main() {
    InitWindow(screenWidth, screenHeight, "Game"); // 設定初始視窗；遊戲名稱設定為 "Game"
    SetTargetFPS(60);//(raylib)每秒鐘跑60個畫面

    //圖檔載入
    char path[100];
    Texture2D stage1_background[stage1backgroundCount];
    for (int i = 0; i < stage1backgroundCount; i++) {        
        sprintf(path, "resource/scene/1-%d.png", i + 1);
        stage1_background[i] = LoadTexture(path);
    }
    Texture2D stage2_background[stage2backgroundCount];
    for (int i = 0; i < stage2backgroundCount; i++) {
        char path[100];
        sprintf(path, "resource/scene/2-%d.png", i + 1);
        stage2_background[i] = LoadTexture(path);
    }
    Texture2D menu_background = LoadTexture("resource/scene/background.png");//下載背景圖片

    enemyTextures dronetexture;
    for (int i = 0; i < 5; i++) {
        sprintf(path, "resource/drone/patrol%d.png", (i < 2) ? (i + 1) : (5 - i));
        dronetexture.patrolFrames[i] = LoadTexture(path);
    }
    for (int i = 0; i < 9; i++) {
        sprintf(path, "resource/drone/chase%d.png", (i < 5) ? (i + 1) : (9 - i));
        dronetexture.chaseFrames[i] = LoadTexture(path);
    }
    for (int i = 0; i < 4; i++) {
        sprintf(path, "resource/drone/attack%d.png", i + 1);
        dronetexture.attackFrames[i] = LoadTexture(path);
    }
    dronetexture.laserFrame = LoadTexture("resource/drone/laser.png");

    // 初始化
    initMenu(menu_background);
    GameState currentGameState = MENU;
    Player player;
    Drone drone[MAX_DRONES];
    Soldier soldier;
    player_init(&player);
    for (int i = 0; i < MAX_DRONES; i++) {
        enemy_initDrone(&drone[i],&dronetexture);
        drone[i].position = (Vector2){2000 + i * 2000, 200};  // 每台 Drone 分開一點
    }
    enemy_initSoldier(&soldier);
    Camera2D camera = { 0 };
    camera.target = player.position;
    camera.offset = (Vector2){ screenWidth * 0.4f, screenHeight / 2.0f };
    camera.zoom = 1.0f;
    float camX = player.position.x;
    float halfScreen = screenWidth * 0.4f;
    Boss boss;
    boss_init(&boss);

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
        float deltaTime = GetFrameTime();

        if (currentGameState == MENU) {
            // 輸入處理 (選單的輸入處理在 updateMenu 中)
            updateMenu(&currentGameState); 
        } 

        else if (currentGameState == GAME) {
            if(IsKeyPressed(KEY_H)) debug = (debug +1)%2;
            if (player.stage == 1){
                player_move(&player, deltaTime);  
                for (int i = 0; i < MAX_DRONES; i++) {
                    enemy_updateDrone(&drone[i], &player, deltaTime); 
                }  
                
                enemy_updateSoldier(&soldier, &player, deltaTime); 
                camX = player.position.x;
                //使背景不跑出畫面
                if (camX < halfScreen) camX = halfScreen;
                if (camX > stage1Width - screenWidth * 0.6f) camX = stage1Width - screenWidth * 0.6f;
                camera.target = (Vector2){ camX, screenHeight / 2.0f  };
                player_attack(&player, camera);
                stage_door(&player);

                for (int i = 0; i < MAX_DRONES; i++) {
                    enemy_laserDamagePlayer(&drone[i], &player);
                    enemy_bulletDamageDrone(&player, &drone[i]); 
                } 
                
            }

            else if (player.stage == 2) {
                player_move(&player,deltaTime);
                camX = player.position.x;
                // 使背景不跑出畫面
                if (camX < halfScreen) camX = halfScreen;
                if (camX > stage2Width - screenWidth * 0.8f) camX = stage2Width - screenWidth * 0.8f;
                camera.target = (Vector2){ camX, screenHeight / 2.0f };
                player_attack(&player, camera);
                stage2_update(&player, &boss);
                boss_update(&boss,&player);
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
                for (int i = 0; i < MAX_DRONES; i++) {
                    enemy_drawDrone(&drone[i]);
                    enemy_drawLaser(&drone[i]);
                }
                
                player_drawbullet(&player,camera);
                
                if(debug){
                    player_drawhitbox(&player);
                    stage_drawhitbox();
                    for (int i = 0; i < MAX_DRONES; i++) {
                        enemy_hitbox(&drone[i]);
                    }
                }
                      
            }
            else if (player.stage == 2) {
                stage2_draw(stage2_background);
                boss_draw(&boss);
                player_draw(&player);
                player_drawbullet(&player, camera);
                
                if(debug) {
                    player_drawhitbox(&player);
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

    for (int i = 0; i < stage2backgroundCount; i++) {
        UnloadTexture(stage2_background[i]);
    }

    player_unload(&player);
    boss_unload(&boss);

    CloseWindow();
    return 0;
}