#include "raylib.h"
#include "main.h"
#include "player.h"
#include "boss.h"
#include "enemy.h"
#include "stage.h"
#include "menu.h"
#include "texture.h"
#include "hack.h"
#include "setting.h"


int main() {
    InitWindow(screenWidth, screenHeight, "Game"); // 設定初始視窗；遊戲名稱設定為 "Game"
    InitAudioDevice();
    SetTargetFPS(60);//(raylib)每秒鐘跑60個畫面

    GameTextures textures;
    GameSounds sounds;
    loadGameTextures(&textures,&sounds);
    double startTime;
    bool savingdata;

    // 初始化
    menu_init(textures.menuBackground);
    GameState currentGameState = MENU;
    Player player;
    Boss boss;
    Drone drone[MAX_DRONES];
    Soldier soldier[MAX_SOLDIERS];
    HackScene hackScene;
    Camera2D camera = { 0 };
    
    float camX;
    float halfScreen;
    
    int debug = 0;
    bool Isinit = false;
    bool deadsound = false;

    // 主遊戲迴圈
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        if (currentGameState == MENU) {
            // 輸入處理 (選單的輸入處理在 updateMenu 中)
            menu_update(&currentGameState);
            if (!IsMusicStreamPlaying(sounds.menumusic)) {
                PlayMusicStream(sounds.menumusic);
            }

            SetMusicVolume(sounds.menumusic, bgmVolume);
            UpdateMusicStream(sounds.menumusic);
        } 
        
        else if (currentGameState == SETTINGS) {
            updateSettings(&currentGameState);
            if (!IsMusicStreamPlaying(sounds.menumusic)) {
                PlayMusicStream(sounds.menumusic);
            }

            SetMusicVolume(sounds.menumusic, bgmVolume);
            UpdateMusicStream(sounds.menumusic);
        }

        else if (currentGameState == GAME) {
            if(!Isinit){
                player_init(&player);
                for (int i = 0; i < MAX_DRONES; i++) {
                    enemy_initDrone(&drone[i]);
                    drone[i].position = (Vector2){2000 + i * 2000, 200};  
                }
                for (int i = 0; i < MAX_SOLDIERS; i++) {
                    enemy_initSoldier(&soldier[i]);
                    soldier[i].position = (Vector2){ 3500 + i * 1500, 400 };
                }
                boss_init(&boss);
                hack_init(&hackScene);

                camera.target = player.position;
                camera.offset = (Vector2){ screenWidth * 0.4f, screenHeight / 2.0f };
                camera.zoom = 1.0f;
                camX = player.position.x;
                halfScreen = screenWidth * 0.4f;
                Isinit = true;

                startTime = GetTime();
                savingdata = false;
                
            }
            //if(IsKeyPressed(KEY_H)) debug = (debug +1)%2;

            //聲音設定
            SetSoundVolume(sounds.upgrade,sfxVolume);
            SetSoundVolume(sounds.attack,sfxVolume);
            SetSoundVolume(sounds.playerdied,sfxVolume);
            SetSoundVolume(sounds.enterbossstage,sfxVolume);
            SetSoundVolume(sounds.noammo,sfxVolume*3);
            SetMusicVolume(sounds.bossMusic,bgmVolume);       // 修正bossmusic为bossMusic

            if (player.stage == 1 && !player.dead){
                player_update(&player, deltaTime);  
            
                camX = player.position.x;
                //使背景不跑出畫面
                if (camX < halfScreen) camX = halfScreen;
                if (camX > stage1Width - screenWidth * 0.6f) camX = stage1Width - screenWidth * 0.6f;
                camera.target = (Vector2){ camX, screenHeight / 2.0f  };
                player_attack(&player, camera,&sounds);
                player_skillupgrade(&player,&sounds);
                stage_door(&player,&sounds);

                for (int i = 0; i < MAX_DRONES; i++) {
                    enemy_updateDrone(&drone[i], &player, deltaTime); 
                    enemy_laserDamagePlayer(&drone[i], &player,&sounds);
                    enemy_bulletDamageDrone(&player, &drone[i],&sounds); 
                } 

                for (int i = 0; i < MAX_SOLDIERS; i++) {
                    if (soldier[i].active){
                    enemy_updateSoldier(&soldier[i], &player, deltaTime);
                    enemy_bulletDamageSoldier(&player, &soldier[i], &sounds);
                    

                    for (int j = 0; j < 3; j++)
                        enemy_bulletDamagePlayer(&soldier[i].bullets[j], &player, &soldier[i]);
                    }
                }

                
                if(player.tutorial){
                    if (IsKeyPressed(KEY_SPACE)) {
                        player.tutorial++;
                        if(player.tutorial == 4)player.tutorial = false; 
                    }
                    if (!IsMusicStreamPlaying(sounds.tutorialmusic)) {
                        PlayMusicStream(sounds.tutorialmusic);
                    }

                    SetMusicVolume(sounds.tutorialmusic, bgmVolume);
                    UpdateMusicStream(sounds.tutorialmusic);
                    
                }
                else{
                    if (!IsMusicStreamPlaying(sounds.stagemusic)) {
                        PlayMusicStream(sounds.stagemusic);
                    }

                    SetMusicVolume(sounds.stagemusic, bgmVolume);
                    UpdateMusicStream(sounds.stagemusic);
                }
            }

            else if (player.stage == 2 && !player.dead) {
                player_update(&player,deltaTime);
                camX = player.position.x;
                // 使背景不跑出畫面
                if (camX < halfScreen) camX = halfScreen;
                if (camX > stage2Width - screenWidth * 0.8f) camX = stage2Width - screenWidth * 0.8f;
                camera.target = (Vector2){ camX, screenHeight / 2.0f };
                player_attack(&player, camera,&sounds);
                stage2_update(&boss, &player, &sounds);
                boss_update(&boss, &player, &sounds);
                
                // 只在boss未死亡时更新黑客小游戏
                if (!boss.isDead) {
                    hack_update(&hackScene, &boss, &player);
                }
                
                // 如果黑客场景激活，绘制它
                if (hackScene.isActive) {
                    hack_draw(&hackScene);
                }
            }
            else if (player.stage == 3) {
                // 在stage 3中更新和显示黑客场景
                hack_update(&hackScene, &boss, &player);
                hack_draw(&hackScene);
            }
            else if (player.stage == 4) {
                if (player.stageChanged) {  // 当stage刚改变时
                    player.position = (Vector2){ 300, 400 };  // 设置stage4的初始位置
                    player.stageChanged = false;
                }
                camX = player.position.x;
                //使背景不跑出畫面
                if (camX < halfScreen) camX = halfScreen;
                if (camX > 3000 * 0.7f-60) camX = 3000 * 0.7f-60;
                camera.target = (Vector2){ camX, screenHeight / 2.0f  };
                
                stage_exitdoor(&player,&currentGameState,&Isinit);
                player_update(&player, GetFrameTime());

                if (!IsMusicStreamPlaying(sounds.endingmusic)) {
                        PlayMusicStream(sounds.endingmusic);
                    }

                SetMusicVolume(sounds.endingmusic, bgmVolume*0.5);
                UpdateMusicStream(sounds.endingmusic);
                
            }

            if(player.dead){
                if(!deadsound) {
                    PlaySound(sounds.playerdied);
                    deadsound = true;
                }
                player_dead(&player, &currentGameState,&Isinit,&sounds,&deadsound);
                
                
            }   

            if(boss.isDead){
                if(!savingdata){
                    double completeTime = GetTime() - startTime;
                    stage_saveCompletionTime(completeTime);
                    savingdata = true;
                }
            }
        } 
        
            
        // 開始繪製
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        if (currentGameState == MENU) {
            menu_draw();
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

                for (int i = 0; i < MAX_SOLDIERS; i++) {
                    enemy_drawSoldier(&soldier[i], &textures);
                    enemy_drawSoldierBullets(&soldier[i], &textures);
                }
                
                player_drawbullet(&player,camera,&textures);
                
                if(debug){
                    player_drawhitbox(&player);
                    stage_drawhitbox();
                    for (int i = 0; i < MAX_DRONES; i++) {
                        enemy_hitbox(&drone[i], NULL, &textures); // 只畫 Drone
                    }
                    for (int i = 0; i < MAX_SOLDIERS; i++) {
                        enemy_hitbox(NULL, &soldier[i], &textures); // 只畫 Soldier
                    }

                    
                }

                
                      
            }
            else if (player.stage == 2) {
                stage2_draw(textures.stage2Background);
                boss_update(&boss, &player, &sounds);
                player_drawbullet(&player, camera,&textures);
                
                // 更新屏幕視角範圍
                Vector2 screenTopLeft = { camera.target.x - camera.offset.x, camera.target.y - camera.offset.y };
                Vector2 screenBottomRight = { 
                    screenTopLeft.x + GetScreenWidth(), 
                    screenTopLeft.y + GetScreenHeight() 
                };
                
                // 繪製boss
                boss_draw(&boss, &textures, screenTopLeft, screenBottomRight, &player);
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
            }
            else if (player.stage == 4){
                DrawTexture(textures.stage4Background, 0, 0, WHITE);
                DrawText("Congratulation! You stop the Neural Override Project", 550, 300, 32, WHITE);
                DrawText("Thank you for playing!", 680, 400, 50, WHITE);
                stage_displayTopCompletionTimes();
                player_draw(&player, &textures);
                
            }
            EndMode2D();
            if(player.stage == 4);
            else if(!player.dead)player_UI(&player);    
            else player_deadUI(&player);           
            if(player.tutorial){
                stage_drawtutorial(&player);
            }
            
        } 
        else if (currentGameState == SETTINGS) {
            drawSettings(&textures);
        }
        
        EndDrawing();
    }
    //stage_drawgridlines(); // 修正圖片用網格
    // 釋放資源並關閉  
    unloadGameTextures(&textures,&sounds);

    CloseWindow();
    return 0;
}