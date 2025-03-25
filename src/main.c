#include "raylib.h"
#include "menu.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void)
{

    const int screenWidth = 2000;
    const int screenHeight = 1200;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second


    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        

        BeginDrawing();

            ClearBackground(RAYWHITE);

            menu();
            a();

        EndDrawing();
    }

    CloseWindow();       

    return 0;
}