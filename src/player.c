#ifndef PLAYER
#define PLAYER

#include "player.h"
#include "raylib.h"

void Initplayer(Player *player){
    // 載入圖片
    player->texture = LoadTexture("resource/123.png");

    // 設定圖片初始位置
    player->position = (Vector2){600 / 2.0f, 600 / 2.0f};
    player->speed = 5.0f;
}

void Moveplayer(Player *player){
    
    if (IsKeyDown(KEY_W)) player->position.y -= player->speed;
    if (IsKeyDown(KEY_S)) player->position.y += player->speed;
    if (IsKeyDown(KEY_A)) player->position.x -= player->speed;
    if (IsKeyDown(KEY_D)) player->position.x += player->speed;
    DrawTexture(player->texture, player->position.x, player->position.y, WHITE);
}

#endif