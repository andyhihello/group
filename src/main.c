#include "player.h"
#include "main.h"

int main(void)
{

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
    Texture2D scene1_1 = LoadTexture("resource/scene/1-1.png");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second

    Player player;
    Initplayer(&player);

    Camera2D camera = { 0 };
    camera.target = player.position;
    camera.offset = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f };
    camera.zoom = 1.0f;
    float camX = player.position.x;
    float halfScreen = screenWidth / 2.0f;
    int stage = 1;

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {

        BeginDrawing();

            ClearBackground(RAYWHITE);

            if (stage == 1){
                Moveplayer(&player);
                playerattrack(&player);
                camX = player.position.x;
                //使背景不跑出畫面
                if (camX < halfScreen) camX = halfScreen;
                if (camX > stage1Width - halfScreen) camX = stage1Width - halfScreen;
                camera.target = (Vector2){ camX, screenHeight / 2.0f  };
                //移動視角
                BeginMode2D(camera);
                    DrawTexture(scene1_1, 0, 0, WHITE); 
                    //腳色繪製
                    Texture2D frame = player.isRunning ? player.runFrames[player.currentFrame] : player.texture;
                    if (player.facingRight) {
                        DrawTextureV(frame, player.position, WHITE);
                    } else {
                        DrawTexturePro(
                            frame,
                            (Rectangle){ 0, 0, (float)-frame.width, (float)frame.height },  // 負寬度實現鏡像
                            (Rectangle){ player.position.x, player.position.y, (float)frame.width, (float)frame.height },
                            (Vector2){ 0, 0 }, 0.0f, WHITE
                        );
                    }

                    drawbullet();
                EndMode2D();

                attrackdata(&player);

            }
            
        EndDrawing();
    }

    CloseWindow();       

    return 0;
}