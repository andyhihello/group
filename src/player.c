
#include "player.h"
#include "main.h"

void Initplayer(Player *player){
    // 載入圖片
    player->texture = LoadTexture("resource/123.png");
    // 設定圖片初始位置
    player->position = (Vector2){screenWidth / 5, screenHeight / 2};
    player->reloadtime = 3;
    player->reloadTimeLeft = 0;
    player->ammo = 10;
    player->maxAmmo = 10;
    player->speed = 5;
}

#define GRAVITY 0.5f   //  模擬重力
#define JUMP_STRENGTH -10.0f  //  跳躍的初始速度
#define GROUND_Y (screenHeight / 2)

void Moveplayer(Player *player){
    
    if (IsKeyDown(KEY_A)) player->position.x -= player->speed;
    if (IsKeyDown(KEY_D)) player->position.x += player->speed;

    if (IsKeyPressed(KEY_W) && !player->isJumping) {
        player->velocityY = JUMP_STRENGTH;  
        player->isJumping = true;
    }

    
    player->position.y += player->velocityY;
    player->velocityY += GRAVITY;  

    
    if (player->position.y > GROUND_Y) {
        player->position.y = GROUND_Y;
        player->velocityY = 0;
        player->isJumping = false;  
    }

    DrawTextureV(player->texture, player->position, WHITE);
}
