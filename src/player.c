#ifndef PLAYER
#define PLAYER

#include "raylib.h"

void player(){
    // 載入圖片
    Texture2D texture = LoadTexture("resource/123.png");

    // 設定圖片初始位置
    Vector2 position = {600 / 2.0f, 600 / 2.0f};
    float speed = 5.0f;

    if (IsKeyDown(KEY_W)) position.y -= speed;
    if (IsKeyDown(KEY_S)) position.y += speed;
    if (IsKeyDown(KEY_A)) position.x -= speed;
    if (IsKeyDown(KEY_D)) position.x += speed;
}

#endif