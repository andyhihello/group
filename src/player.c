
#include "player.h"
#include "main.h"
Bullet bullets[MAX_BULLETS] = { 0 };


void player_init(Player *player){
    // 載入圖片
    player->stand = LoadTexture("resource/player/stand.png");
    if (player->stand.id == 0) {
        TraceLog(LOG_ERROR, "stand.png 載入失敗！");
    }
    //奔跑動畫載入
    for (int i = 0; i < 9; i++) {
        char path[100];
        sprintf(path, "resource/player/run%d.png", (i < 5) ? (i + 1) : (9 - i));
        player->runFrames[i] = LoadTexture(path);
        if (player->runFrames[i].id == 0) {
            TraceLog(LOG_ERROR, "%s 載入失敗！", path);
        }
    }
    // 設定腳色初始設定
    player->position = (Vector2){400, 100};
    player->reloadtime = 3;
    player->reloadTimeLeft = 0;
    player->ammo = 100;
    player->maxAmmo = 100;
    player->speed = 7;
}

void player_move(Player *player){
    
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

void player_reload(Player *player){
    // 換彈倒數
    if (player->reloadTimeLeft > 0) player->reloadTimeLeft -= GetFrameTime(); // 每幀減少時間

    // 如果換彈完成，重設子彈數量
    if (player->reloadTimeLeft <= 0) {
        player->ammo = player->maxAmmo;  // 填滿子彈
        player->reloading = false;
    }
}

void player_UI(Player *player){
   
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

void player_attack(Player *player,Camera2D camera){
    Vector2 fireOrigin = (Vector2){ player->position.x + 240, player->position.y + 170 };

    // 換彈邏輯：按 R 鍵開始換彈
    if (IsKeyPressed(KEY_R) && player->ammo < player->maxAmmo && player->reloadTimeLeft <= 0) {
        player->reloadTimeLeft = player->reloadtime;  
        player->reloading = true;
    }

    if(player->reloading) player_reload(player);
 
    //射擊(按左鍵)
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && player->ammo > 0 && player->reloadTimeLeft <= 0) {
        //計算子彈移動
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (!bullets[i].active) {
                Vector2 mouse = GetScreenToWorld2D(GetMousePosition(), camera);
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

void player_drawbullet(Camera2D camera) {
    // 計算鏡頭的視野範圍（世界座標）
    Vector2 screenTopLeft = GetScreenToWorld2D((Vector2){0, 0}, camera);
    Vector2 screenBottomRight = GetScreenToWorld2D((Vector2){screenWidth, screenHeight}, camera);

    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            // 子彈位置更新
            bullets[i].position.x += bullets[i].speed.x;
            bullets[i].position.y += bullets[i].speed.y;

            // 判斷是否超出世界地圖範圍（不會因為鏡頭而判斷錯誤）
            if (bullets[i].position.x < 0 || bullets[i].position.x > stage1Width ||
                bullets[i].position.y < 0 || bullets[i].position.y > screenHeight) {
                bullets[i].active = false;
                continue;
            }

            // 只在鏡頭視野範圍內才繪製
            if (bullets[i].position.x >= screenTopLeft.x && bullets[i].position.x <= screenBottomRight.x &&
                bullets[i].position.y >= screenTopLeft.y && bullets[i].position.y <= screenBottomRight.y) {
                DrawCircleV(bullets[i].position, 10, RED);
            }
        }
    }
}

void player_draw(Player *player){
    Texture2D frame = player->isRunning ? player->runFrames[player->currentFrame] : player->stand;
    if (player->facingRight) {
        DrawTextureV(frame, player->position, WHITE);
    } 
    else {
        DrawTexturePro(
            frame,
            (Rectangle){ 0, 0, (float)-frame.width, (float)frame.height },  // 負寬度實現鏡像
            (Rectangle){ player->position.x, player->position.y, (float)frame.width, (float)frame.height },
            (Vector2){ 0, 0 }, 0.0f, WHITE
        );
    }
}

void player_unload(Player *player) {
    UnloadTexture(player->stand);
    for (int i = 0; i < 9; i++) {
        UnloadTexture(player->runFrames[i]);
    }
    for (int i = 0; i < MAX_BULLETS; i++) {
        bullets[i].active = false;
    }
}