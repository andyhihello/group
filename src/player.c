#ifndef PLAYER
#define PLAYER

#include "player.h"
#include "main.h"

void Initplayer(Player *player){
    // 載入圖片
    player->texture = LoadTexture("resource/123.png");
    // 設定圖片初始位置
    player->position = (Vector2){screenWidth / 5, screenHeight / 2};
    player->speed = 5;
}

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

    DrawTexture(player->texture, player->position.x, player->position.y, WHITE);
}


#define MAX_BULLETS 20
Bullet bullets[MAX_BULLETS] = { 0 };

void playerattrack(Player *player){
    
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (!bullets[i].active) {
                Vector2 mouse = GetMousePosition();
                Vector2 direction = { mouse.x - player->position.x, mouse.y - player->position.y };
                float length = sqrtf(direction.x * direction.x + direction.y * direction.y);
                if (length > 0) {  // avoid x/0 error
                    direction.x /= length;
                    direction.y /= length;
                }

                bullets[i].position = player->position;
                bullets[i].speed = (Vector2){ direction.x * 20, direction.y * 20 };
                bullets[i].active = true;
                break;
            }
        }
    }

    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            bullets[i].position.x += bullets[i].speed.x;
            bullets[i].position.y += bullets[i].speed.y;

            if (bullets[i].position.x < 0 || bullets[i].position.x > screenWidth ||
                bullets[i].position.y < 0 || bullets[i].position.y > screenHeight) {
                bullets[i].active = false;
            }
            if (bullets[i].active) DrawCircleV(bullets[i].position, 10, RED);

        }
    }

    
}

#endif