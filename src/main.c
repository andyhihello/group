#include "player.h"
#include "main.h"

int main(void)
{

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second

    Player player;
    Initplayer(&player);

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {

        BeginDrawing();

            ClearBackground(RAYWHITE);

            Moveplayer(&player);
            playerattrack(&player);

        EndDrawing();
    }

    CloseWindow();       

    return 0;
}