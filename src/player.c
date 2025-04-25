
#include "player.h"
#include "main.h"
Rectangle stage1_wall = { 9010, 0, 70, 455 };

void player_hitbox(Player *player) {
    player->hitbox = (Rectangle){
        player->position.x + playerXoffset,
        player->position.y + playerYoffset,
        playerWidth,
        playerHeight
    };
}

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
    player->position = (Vector2){8500, 100};
    memset(player->bullets, 0, sizeof(player->bullets));
    player->reloadtime = 3;
    player->reloadTimeLeft = 0;
    player->ammo = 50;
    player->maxAmmo = 50;
    player->speed = 6;
}

void player_move(Player *player,int stage){

    // 左移
    if (IsKeyDown(KEY_A)) {
        player->position.x -= player->speed;

        // 更新玩家碰撞矩形
        player_hitbox(player);

        // 如果碰到牆，就把角色卡在牆的右側
        if(stage == 1 && CheckCollisionRecs(player->hitbox, stage1_wall))player->position.x = stage1_wall.x + stage1_wall.width - playerXoffset;

        player->facingRight = false;
    }

    // 右移
    if (IsKeyDown(KEY_D)) {
        player->position.x += player->speed;

        // 更新玩家碰撞矩形
        player_hitbox(player); 

        // 如果碰到牆，就把角色卡在牆的左側
        if(stage == 1 && CheckCollisionRecs(player->hitbox, stage1_wall))player->position.x = stage1_wall.x - playerWidth - playerXoffset;           

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

    // 更新碰撞矩形
    player_hitbox(player); 

    // 撞到牆底
    if (stage == 1 && CheckCollisionRecs(player->hitbox, stage1_wall) && player->velocityY < 0) {
        player->position.y = stage1_wall.y + stage1_wall.height;  
        player->velocityY = 0;
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
            if (!player->bullets[i].active) {
                Vector2 mouse = GetScreenToWorld2D(GetMousePosition(), camera);
                Vector2 direction = { mouse.x - fireOrigin.x, mouse.y - fireOrigin.y };
                float length = sqrtf(direction.x * direction.x + direction.y * direction.y);
                if (length > 0) {  // avoid x/0 error
                    direction.x /= length;
                    direction.y /= length;
                }

                player->bullets[i].position = fireOrigin;
                player->bullets[i].speed = (Vector2){ direction.x * BULLET_SPEED, direction.y * BULLET_SPEED };
                player->bullets[i].active = true;
                player->ammo--;
                break;
            }
        }
    }

}

void player_drawbullet(Player *player, Camera2D camera,int stage) {
    // 計算鏡頭的視野範圍（世界座標）
    int sceneWidth;
    if(stage == 1) sceneWidth = stage1Width;
    Vector2 screenTopLeft = GetScreenToWorld2D((Vector2){0, 0}, camera);
    Vector2 screenBottomRight = GetScreenToWorld2D((Vector2){screenWidth, screenHeight}, camera);

    for (int i = 0; i < MAX_BULLETS; i++) {
        Rectangle bulletRect = { player->bullets[i].position.x, player->bullets[i].position.y, SIZE_BULLET, SIZE_BULLET};
        if (player->bullets[i].active) {
            player->bullets[i].position.x += player->bullets[i].speed.x;
            player->bullets[i].position.y += player->bullets[i].speed.y;

            // 超出地圖範圍，停用子彈
            if (player->bullets[i].position.x < 0 || player->bullets[i].position.x > sceneWidth ||
                player->bullets[i].position.y < 0 || player->bullets[i].position.y > screenHeight || 
                (stage == 1 && CheckCollisionRecs(bulletRect, stage1_wall))) {
                player->bullets[i].active = false;
                continue;
            }

            // 在畫面內才繪製
            if (player->bullets[i].position.x >= screenTopLeft.x && player->bullets[i].position.x <= screenBottomRight.x &&
                player->bullets[i].position.y >= screenTopLeft.y && player->bullets[i].position.y <= screenBottomRight.y) {
                DrawCircleV(player->bullets[i].position, SIZE_BULLET, RED);
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
        player->bullets[i].active = false;
    }
}

void player_drawhitbox(Player *player){
    DrawRectangleLinesEx(player->hitbox, 2, (Color){255, 0, 0, 180});
}