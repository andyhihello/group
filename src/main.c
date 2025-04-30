#include "raylib.h"
#include "main.h"
#include "player.h"
#include "boss.h"
#include "enemy.h"
#include "stage.h"
#include "menu.h"
#include "texture.h"


int main() {
    InitWindow(screenWidth, screenHeight, "Game"); // 設定初始視窗；遊戲名稱設定為 "Game"
    SetTargetFPS(60);//(raylib)每秒鐘跑60個畫面

    GameTextures textures;
    loadGameTextures(&textures);
    

    // 初始化
    initMenu(textures.menuBackground);
    GameState currentGameState = MENU;
    Player player;
    player_init(&player);
    Drone drone[MAX_DRONES];
    Soldier soldier;

    for (int i = 0; i < MAX_DRONES; i++) {
        enemy_initDrone(&drone[i]);
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

    


    // 主遊戲迴圈
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        if (currentGameState == MENU) {
            // 輸入處理 (選單的輸入處理在 updateMenu 中)
            updateMenu(&currentGameState, &player, &boss, drone);
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
                player_skillupgrade(&player);
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
                    DrawTexture(textures.stage1Background[i], i * stage1pictureWidth, 0, WHITE);
                }

                stage_drawdoortext(); 
                player_draw(&player, &textures);
                for (int i = 0; i < MAX_DRONES; i++) {
                    enemy_drawDrone(&drone[i], &textures);
                    enemy_drawLaser(&drone[i], &textures);      
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
                stage2_draw(textures.stage2Background);
                boss_update(&boss, &player);
                player_drawbullet(&player, camera);
                boss_draw(&boss, &textures);
                player_draw(&player, &textures);
                
                if(debug) {
                    player_drawhitbox(&player);
                    boss_drawhitbox(&boss);
                    // 修改子弹碰撞箱的显示
                    for (int i = 0; i < MAX_BULLETS; i++) {
                        if (player.bullets[i].active) {
                            DrawRectangleLines(
                                player.bullets[i].position.x,
                                player.bullets[i].position.y,
                                BULLET_WIDTH,
                                BULLET_HEIGHT,
                                RED
                            );
                        }
                    }
                }
                
                // 检查Boss是否死亡
                if (boss.isDead) {
                    const char* text = "GAME COMPLETE!";
                    int fontSize = 60;
                    Vector2 textSize = MeasureTextEx(GetFontDefault(), text, fontSize, 2);
                    Vector2 textPos = {
                        screenWidth/2 - textSize.x/2,
                        screenHeight/2 - textSize.y/2
                    };
                    DrawText(text, textPos.x, textPos.y, fontSize, WHITE);
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
    unloadGameTextures(&textures);

    CloseWindow();
    return 0;
}