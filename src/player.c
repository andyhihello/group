
#include "player.h"
#include "main.h"
Bullet bullets[MAX_BULLETS] = { 0 };


void Initplayer(Player *player){
    // 載入圖片
    player->texture = LoadTexture("resource/player/stand.png");
    //奔跑動畫載入
    player->runFrames[0] = LoadTexture("resource/player/run1.png");
    player->runFrames[1] = LoadTexture("resource/player/run2.png");
    player->runFrames[2] = LoadTexture("resource/player/run3.png");
    player->runFrames[3] = LoadTexture("resource/player/run4.png");
    player->runFrames[4] = LoadTexture("resource/player/run5.png");
    player->runFrames[5] = LoadTexture("resource/player/run4.png");
    player->runFrames[6] = LoadTexture("resource/player/run3.png");
    player->runFrames[7] = LoadTexture("resource/player/run2.png");
    player->runFrames[8] = LoadTexture("resource/player/run1.png");
    // 設定腳色初始設定
    player->position = (Vector2){400, 100};
    player->reloadtime = 3;
    player->reloadTimeLeft = 0;
    player->ammo = 10;
    player->maxAmmo = 10;
    player->speed = 5;
}

void Moveplayer(Player *player){
    
    if (IsKeyDown(KEY_A)) {
        player->position.x -= player->speed;
        player->facingRight = false;
    }
    if (IsKeyDown(KEY_D)){ 
        player->position.x += player->speed;
        player->facingRight = true;
    }
    //跑步動畫
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_D)) {
        player->frameTimer += GetFrameTime();
        player->isRunning = true;
        if (player->frameTimer >= 0.1f) {
            player->currentFrame++;
            if (player->currentFrame >= 9) player->currentFrame = 0;
            player->frameTimer = 0;
        }
    } else {
        player->currentFrame = 0;
        player->isRunning = false;
    }
    // 限制玩家不能超出背景範圍
    if (player->position.x < 0) player->position.x = 0;
    if (player->position.x > stage1Width - playerWidth) player->position.x = stage1Width - playerWidth;
    //跳躍
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
}

void reload(Player *player){
    // 換彈倒數
    if (player->reloadTimeLeft > 0) player->reloadTimeLeft -= GetFrameTime(); // 每幀減少時間

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
    Vector2 fireOrigin = (Vector2){ player->position.x + 240, player->position.y + 170 };

    // 換彈邏輯：按 R 鍵開始換彈
    if (IsKeyPressed(KEY_R) && player->ammo < player->maxAmmo && player->reloadTimeLeft <= 0) {
        player->reloadTimeLeft = player->reloadtime;  
        player->reloading = true;
    }

    if(player->reloading) reload(player);
 
    //射擊(按左鍵)
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && player->ammo > 0 && player->reloadTimeLeft <= 0) {
        //計算子彈移動
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (!bullets[i].active) {
                Vector2 mouse = GetMousePosition();
                Vector2 direction = { mouse.x - fireOrigin.x, mouse.y - fireOrigin.y };
                float length = sqrtf(direction.x * direction.x + direction.y * direction.y);
                if (length > 0) {  // avoid x/0 error
                    direction.x /= length;
                    direction.y /= length;
                }

                bullets[i].position = fireOrigin;
                bullets[i].speed = (Vector2){ direction.x * BULLET_SPEED, direction.y * BULLET_SPEED };
                bullets[i].active = true;
                player->ammo--;
                break;
            }
        }
    }

}

void drawplayerbullet(){
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            bullets[i].position.x += bullets[i].speed.x;
            bullets[i].position.y += bullets[i].speed.y;
            //子彈超出畫面清除
            if (bullets[i].position.x < 0 || bullets[i].position.x > screenWidth ||
                bullets[i].position.y < 0 || bullets[i].position.y > screenHeight) {
                bullets[i].active = false;
            }
            if (bullets[i].active) DrawCircleV(bullets[i].position, 10, RED);
        }
    }
}
