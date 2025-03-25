
#include "main.h"
#include "player.h"
#include "player_attrack.h"


#define MAX_BULLETS 50 // 螢幕可容納最多子彈
#define BULLET_SPEED 20
Bullet bullets[MAX_BULLETS] = { 0 };

void reload(Player *player){
    // 換彈倒數
    if (player->reloadTimeLeft > 0) {
        player->reloadTimeLeft -= GetFrameTime(); // 每幀減少時間

        printf("Reloading time: %.1f\n", player->reloadTimeLeft);
    }

    // 如果換彈完成，重設子彈數量
    if (player->reloadTimeLeft <= 0) {
        player->ammo = player->maxAmmo;  // 填滿子彈
        player->reloading = false;
    }
}

void attrackdata(Player *player){
   
    char ammoText[20];
    snprintf(ammoText, sizeof(ammoText), "Ammo: %d/%d", player->ammo, player->maxAmmo);
    DrawText(ammoText, 10, 30, 20, DARKGRAY);

    // 顯示換彈倒數
    if (player->reloadTimeLeft > 0) {
        char reloadText[30];
        snprintf(reloadText, sizeof(reloadText), "Reloading... %.1f", player->reloadTimeLeft);
        DrawText(reloadText, screenWidth - 200, 30, 20, DARKGRAY);
    }
}

void playerattrack(Player *player){


    // 換彈邏輯：按 R 鍵開始換彈
    if (IsKeyPressed(KEY_R) && player->ammo < player->maxAmmo && player->reloadTimeLeft <= 0) {
        player->reloadTimeLeft = player->reloadtime;  
        player->reloading = true;
    }

    if(player->reloading) reload(player);

    
    
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && player->ammo > 0 && player->reloadTimeLeft <= 0) {
        
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
                bullets[i].speed = (Vector2){ direction.x * BULLET_SPEED, direction.y * BULLET_SPEED };
                bullets[i].active = true;
                player->ammo--;
                printf("Ammo after shot: %d\n", player->ammo);
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

    attrackdata(player);
    
}

