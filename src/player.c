#include "raylib.h"
#include "player.h"
#include "boss.h"
#include "main.h"
#include <math.h>
Rectangle stage1_wall = { 9010, 0, 70, 455 };
float upgradeEffectTimer = 0.0f; // 特效剩餘時間
const float upgradeEffectDuration = 1.0f; // 特效持續秒數

// 在文件開頭添加靜態變量
static int frameCount = 0;

void player_hitbox(Player *player) {
    player->hitbox = (Rectangle){
        player->position.x + playerXoffset,
        player->position.y + playerYoffset,
        playerWidth,
        playerHeight
    };
}

void player_init(Player *player){
    // 設定腳色初始設定
    player->position = (Vector2){300, 300};
    player->hp = 100;
    player->coin = 100;
    player->damage = 5;
    player->invincible = false;
    player->invincibleDuration = 3.0f;
    player->invincibleTimeLeft = 0.0f;
    player->invincibleCooldown = 15.0f;
    player->invincibleCooldownLeft = 0.0f;
    memset(player->bullets, 0, sizeof(player->bullets));
    player->reloadtime = 3;
    player->reloadTimeLeft = 0;
    player->ammo = 15;
    player->maxAmmo = 15;
    player->speed = 300;
    player->stage = 1;
    player->tutorial = 0;
    player->controlsReversed = false;
    player->controlReverseTimer = 0.0f;

    // 設定升級用參數
    player->upgrade_reload_cost = 2;      // 換彈升級要20 coin
    player->upgrade_ammo_cost = 1;         // 容量升級要15 coin
    player->upgrade_invincible_cost = 3;   // 無敵CD升級要25 coin

    player->reload_upgrade_ratio = 0.9f;    // 每次換彈變成原本90%
    player->ammo_upgrade_amount = 5;         // 每次多5發
    player->invincible_upgrade_ratio = 0.85f; // 每次CD剩下85%

    player->reload_upgrade_times = 0;
    player->ammo_upgrade_times = 0;
    player->invincible_upgrade_times = 0;
}

void player_move(Player *player,float deltaTime){

    if (IsKeyPressed(KEY_E) && player->invincibleCooldownLeft <= 0.0f) {
        player->invincible = true;
        player->invincibleTimeLeft = player->invincibleDuration;
        player->invincibleCooldownLeft = player->invincibleCooldown;
    }

    if (player->invincible) {
        player->invincibleTimeLeft -= deltaTime;
        if (player->invincibleTimeLeft <= 0.0f) {
            player->invincible = false;
        }
    }
    
    if (player->invincibleCooldownLeft > 0.0f) {
        player->invincibleCooldownLeft -= deltaTime;
    }
    if (player->controlsReversed){
        // 移動反轉從左移改成右移
    if (IsKeyDown(KEY_A)) {
        player->position.x += player->speed * deltaTime;

        // 更新玩家碰撞矩形
        player_hitbox(player);

        // 如果碰到牆，就把角色卡在牆的左側
        if(player->stage == 1 && CheckCollisionRecs(player->hitbox, stage1_wall))player->position.x = stage1_wall.x - stage1_wall.width - playerXoffset;

        player->facingRight = true;
    }

    // 移動反轉從右移改成左移
    if (IsKeyDown(KEY_D)) {
        player->position.x -= player->speed * deltaTime;

        // 更新玩家碰撞矩形
        player_hitbox(player); 

        // 如果碰到牆，就把角色卡在牆的右側
        if(player->stage == 1 && CheckCollisionRecs(player->hitbox, stage1_wall))player->position.x = stage1_wall.x + playerWidth - playerXoffset;           

        player->facingRight = false;
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
    if (player->position.x < -50) player->position.x = -50;
    if (player->position.x > stage1Width - playerWidth) player->position.x = stage1Width - playerWidth;

    //跳躍從W改成S
    if (IsKeyPressed(KEY_S) && !player->isJumping) {
        player->velocityY = JUMP_STRENGTH;  
        player->isJumping = true;
    }
    player->position.y += player->velocityY * deltaTime ;
    player->velocityY += GRAVITY;  
    if (player->position.y > GROUND_Y) {
        player->position.y = GROUND_Y;
        player->velocityY = 0;
        player->isJumping = false;  
    }
    }
    
    else{
        // 左移
        if (IsKeyDown(KEY_A)) {
        player->position.x -= player->speed * deltaTime;

        // 更新玩家碰撞矩形
        player_hitbox(player);

        // 如果碰到牆，就把角色卡在牆的右側
        if(player->stage == 1 && CheckCollisionRecs(player->hitbox, stage1_wall))player->position.x = stage1_wall.x + stage1_wall.width - playerXoffset;

        player->facingRight = false;
    }

    // 右移
    if (IsKeyDown(KEY_D)) {
        player->position.x += player->speed * deltaTime;

        // 更新玩家碰撞矩形
        player_hitbox(player); 

        // 如果碰到牆，就把角色卡在牆的左側
        if(player->stage == 1 && CheckCollisionRecs(player->hitbox, stage1_wall))player->position.x = stage1_wall.x - playerWidth - playerXoffset;           

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
    if (player->position.x < -50) player->position.x = -50;
    if (player->position.x > stage1Width - playerWidth) player->position.x = stage1Width - playerWidth;

    //跳躍
    if (IsKeyPressed(KEY_W) && !player->isJumping) {
        player->velocityY = JUMP_STRENGTH;  
        player->isJumping = true;
    }
    player->position.y += player->velocityY * deltaTime ;
    player->velocityY += GRAVITY;  
    if (player->position.y > GROUND_Y) {
        player->position.y = GROUND_Y;
        player->velocityY = 0;
        player->isJumping = false;  
    }
    }
    // 更新碰撞矩形
    player_hitbox(player); 

    // 撞到牆底
    if (player->stage == 1 && CheckCollisionRecs(player->hitbox, stage1_wall) && player->velocityY < 0) {
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

void player_UI(Player *player) {
    // 基本資訊：子彈 / 血量 / 金幣
    char ammoText[20];
    snprintf(ammoText, sizeof(ammoText), "Ammo: %d/%d", player->ammo, player->maxAmmo);
    DrawText(ammoText, 10, 30, 20, WHITE);

    char hpText[20];
    snprintf(hpText, sizeof(hpText), "HP: %d", player->hp);
    DrawText(hpText, 10, 60, 20, WHITE);

    char coinText[20];
    snprintf(coinText, sizeof(coinText), "COIN: %d", player->coin);
    DrawText(coinText, 10, 90, 20, WHITE);

    // 顯示換彈倒數
    if (player->reloadTimeLeft > 0) {
        char reloadText[30];
        snprintf(reloadText, sizeof(reloadText), "Reloading... %.1f", player->reloadTimeLeft);
        DrawText(reloadText, screenWidth - 200, 30, 20, WHITE);
    }

    DrawText("Skill Upgrades:", 10, 140, 20, GOLD);

    // 升級項目 1：換彈速度
    if (player->reload_upgrade_times >= 2) {
        DrawText("[1] Reload Upgrade (MAX)", 10, 180, 20, RED);
    } else {
        char reloadUpgrade[50];
        snprintf(reloadUpgrade, sizeof(reloadUpgrade), "[1] Reload Upgrade (Cost: %d)", player->upgrade_reload_cost);
        DrawText(reloadUpgrade, 10, 180, 20, YELLOW);
    }

    // 升級項目 2：子彈容量
    if (player->ammo_upgrade_times >= 2) {
        DrawText("[2] Ammo Upgrade (MAX)", 10, 210, 20, RED);
    } else {
        char ammoUpgrade[50];
        snprintf(ammoUpgrade, sizeof(ammoUpgrade), "[2] Ammo Upgrade (Cost: %d)", player->upgrade_ammo_cost);
        DrawText(ammoUpgrade, 10, 210, 20, YELLOW);
    }

    // 升級項目 3：無敵CD
    if (player->invincible_upgrade_times >= 2) {
        DrawText("[3] Invincible Upgrade (MAX)", 10, 240, 20, RED);
    } else {
        char invincibleUpgrade[50];
        snprintf(invincibleUpgrade, sizeof(invincibleUpgrade), "[3] Invincible Upgrade (Cost: %d)", player->upgrade_invincible_cost);
        DrawText(invincibleUpgrade, 10, 240, 20, YELLOW);
    }

    // 升級特效繪製
    if (upgradeEffectTimer > 0.0f) {
        float alpha = upgradeEffectTimer / upgradeEffectDuration;  // alpha 越小越透明
        Color effectColor = (Color){255, 215, 0, (unsigned char)(alpha * 255)};  // 金黃色閃爍

        DrawText("Upgrade Success!", screenWidth/2 - MeasureText("Upgrade Success!", 40)/2, screenHeight/2 - 100, 40, effectColor);

        upgradeEffectTimer -= GetFrameTime();
    }

    if (player->invincibleCooldownLeft > 0.0f) {
        char cooldownText[40];
        snprintf(cooldownText, sizeof(cooldownText), "E: Shield Cooldown %.1f", player->invincibleCooldownLeft);
        DrawText(cooldownText, screenWidth - 280, 60, 20, SKYBLUE);
    } else {
        DrawText("E: Shield Ready!", screenWidth - 200, 60, 20, GREEN);
    }

    // 顯示控制反轉狀態和計時器
    char controlText[32];
    sprintf(controlText, "Controls Reversed: %s", player->controlsReversed ? "YES" : "NO");
    DrawText(controlText, 10, 100, 20, player->controlsReversed ? RED : GREEN);

    // 如果控制被反轉，顯示剩餘時間
    if (player->controlsReversed) {
        char timerText[32];
        sprintf(timerText, "Reverse Time: %.1f", player->controlReverseTimer);
        DrawText(timerText, 10, 130, 20, RED);
    }
}

void player_attack(Player *player, Camera2D camera) {
    Vector2 fireOrigin = (Vector2){ player->position.x + 240, player->position.y + 170 };

    // 更新武器禁用計時器
    if (player->weaponDisabled) {
        player->weaponDisableTimer -= GetFrameTime();
        if (player->weaponDisableTimer <= 0) {
            player->weaponDisabled = false;
            player->weaponDisableTimer = 0;
        }
    }

    // 換彈邏輯：按 R 鍵開始換彈
    if (IsKeyPressed(KEY_R) && player->ammo < player->maxAmmo && player->reloadTimeLeft <= 0) {
        player->reloadTimeLeft = player->reloadtime;  
        player->reloading = true;
    }

    if(player->reloading) player_reload(player);
 
    //射擊(按左鍵)，只有在武器未被禁用時才能射擊
    if (!player->weaponDisabled && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && player->ammo > 0 && player->reloadTimeLeft <= 0) {
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
                player->bullets[i].startPosition = fireOrigin;
                player->bullets[i].speed = (Vector2){ direction.x * BULLET_SPEED, direction.y * BULLET_SPEED };
                player->bullets[i].active = true;
                player->ammo--;
                break;
            }
        }
    }
}

void player_skillupgrade(Player *player) {
    const int upgradeCost = 10; // 每次升級花10個coin（可以自己改）
    int upgradeType;

    if (IsKeyPressed(KEY_ONE)) {  // 按 1 升級換彈速度
        upgradeType = 1;
    }
    else if (IsKeyPressed(KEY_TWO)) {  // 按 2 升級子彈容量
        upgradeType = 2;
    }
    else if (IsKeyPressed(KEY_THREE)) {  // 按 3 升級無敵CD
        upgradeType = 3;
    }
    else upgradeType = 0;

    if (player->coin < upgradeCost) {
        TraceLog(LOG_INFO, "金幣不足，無法升級！");
        return;
    }

    switch (upgradeType) {
        case 1: // 換彈速度增加
            if (player->reload_upgrade_times >= 2) {
                TraceLog(LOG_WARNING, "MAX reload level");
                break;
            }
            if (player->coin >= player->upgrade_reload_cost) {
                player->reloadtime *= player->reload_upgrade_ratio;
                player->coin -= player->upgrade_reload_cost;
                player->reload_upgrade_times++;
                player->upgrade_reload_cost += 2;
                upgradeEffectTimer = upgradeEffectDuration;  // 觸發特效！
                TraceLog(LOG_INFO, "reloadupgrade!");
            } else {
                TraceLog(LOG_WARNING, "No enough money!");
            }
            break;
        case 2: // 子彈容量增加
            if (player->ammo_upgrade_times >= 2) {
                TraceLog(LOG_WARNING, "MAX ammo level");
                break;
            }
            if (player->coin >= player->upgrade_ammo_cost) {
                player->maxAmmo += player->ammo_upgrade_amount;
                player->ammo = player->maxAmmo;
                player->coin -= player->upgrade_ammo_cost;
                player->ammo_upgrade_times++;
                player->upgrade_ammo_cost += 2;
                upgradeEffectTimer = upgradeEffectDuration;  // 觸發特效！
                TraceLog(LOG_INFO, "ammoupgrade!");
            } else {
                TraceLog(LOG_WARNING, "No enough money!");
            }
            break;
        case 3: // 無敵CD縮短
            if (player->invincible_upgrade_times >= 2) {
                TraceLog(LOG_WARNING, "MAX invincible level;");
                break;
            }
            if (player->coin >= player->upgrade_invincible_cost) {
                player->invincibleCooldown *= player->invincible_upgrade_ratio;
                player->coin -= player->upgrade_invincible_cost;
                player->upgrade_invincible_cost += 3;
                player->invincible_upgrade_times++;
                upgradeEffectTimer = upgradeEffectDuration;  // 觸發特效！
                TraceLog(LOG_INFO, "invincibleupgrade!");
            } else {
                TraceLog(LOG_WARNING, "No enough money!");
            }
            break;
        default:
            return;
    }

}

void player_drawbullet(Player *player, Camera2D camera, GameTextures *textures) {
    int sceneWidth = (player->stage == 1) ? stage1Width : stage2Width;

    Vector2 screenTopLeft = GetScreenToWorld2D((Vector2){0, 0}, camera);
    Vector2 screenBottomRight = GetScreenToWorld2D((Vector2){screenWidth, screenHeight}, camera);

    for (int i = 0; i < MAX_BULLETS; i++) {
        Bullet *b = &player->bullets[i];
        if (b->active) {
            // 更新子彈位置
            b->position.x += b->speed.x;
            b->position.y += b->speed.y;

            // 子彈飛出邊界或碰到牆壁則失效
            if (b->position.x < 0 || b->position.x > sceneWidth ||
                b->position.y < 0 || b->position.y > screenHeight ||
                (player->stage == 1 && CheckCollisionRecs(b->hitbox, stage1_wall))) {
                b->active = false;
                continue;
            }

            // 超出飛行距離則失效
            float dx = b->position.x - b->startPosition.x;
            float dy = b->position.y - b->startPosition.y;
            if (sqrtf(dx * dx + dy * dy) >= 500.0f) {
                b->active = false;
                continue;
            }

            // 若在鏡頭內，繪製子彈
            if (b->position.x >= screenTopLeft.x && b->position.x <= screenBottomRight.x &&
                b->position.y >= screenTopLeft.y && b->position.y <= screenBottomRight.y) {

                // 設定旋轉角度
                float angle = atan2f(b->speed.y, b->speed.x) * RAD2DEG;

                // 設定貼圖繪製資訊
                Rectangle source = { 0, 0, (float)textures->playerBullet.width, (float)textures->playerBullet.height };
                Rectangle dest = {
                    b->position.x,
                    b->position.y,
                    SIZE_BULLET,
                    SIZE_BULLET
                };
                Vector2 origin = { SIZE_BULLET / 2.0f, SIZE_BULLET / 2.0f };

                // 更新 hitbox 為未旋轉的矩形，與圖中央對齊
                b->hitbox = (Rectangle){
                    b->position.x - SIZE_BULLET / 2.0f,
                    b->position.y - SIZE_BULLET / 2.0f,
                    SIZE_BULLET,
                    SIZE_BULLET
                };

                // 繪製子彈圖片（可旋轉）
                DrawTexturePro(textures->playerBullet, source, dest, origin, angle, WHITE);

            }
        }
    }
}



void player_draw(Player *player, GameTextures *textures) {
    Texture2D frame;

    if (player->isRunning) {
        frame = textures->playerRun[player->currentFrame];
    } else {
        frame = textures->playerStand;
    }

    Rectangle source = { 0, 0, (float)frame.width, (float)frame.height };
    Rectangle dest = { player->position.x, player->position.y, (float)frame.width, (float)frame.height };
    Vector2 origin = { 0, 0 };

    if (!player->facingRight) {
        source.width = -source.width;
    }

    DrawTexturePro(frame, source, dest, origin, 0.0f, WHITE);

    if (player->invincible) {
        Rectangle dest = {
            player->position.x ,
            player->position.y - 20,
            player->hitbox.width + 100,
            player->hitbox.height + 60
        };
    
        DrawTexturePro(
            textures->shieldTexture,
            (Rectangle){0, 0, textures->shieldTexture.width, textures->shieldTexture.height},
            dest,
            (Vector2){0, 0},
            0.0f,
            (Color){255, 255, 255, 180}  // 半透明光盾
        );
        DrawTexturePro(
            textures->shieldTexture,
            (Rectangle){textures->shieldTexture.width, 0, -textures->shieldTexture.width, textures->shieldTexture.height},
            dest,
            (Vector2){0, 0},
            0.0f,
            (Color){255, 255, 255, 180}
        );
    }
}


void player_drawhitbox(Player *player){
    DrawRectangleLinesEx(player->hitbox, 2, (Color){255, 0, 0, 180});

    for (int i = 0; i < MAX_BULLETS; i++) {
        Bullet *b = &player->bullets[i];
        if (b->active) {
            DrawRectangleLinesEx(b->hitbox, 2, WHITE);
        }
    }
}

void player_update(Player *player) {
    frameCount++;
    
    // 更新控制反轉計時器
    if (player->controlsReversed) {
        player->controlReverseTimer -= GetFrameTime();
        if (player->controlReverseTimer <= 0) {
            player->controlsReversed = false;
            player->controlReverseTimer = 0;
        }
    }
    
    // 更新子彈位置
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (player->bullets[i].active) {
            // 只更新 y 坐標，保持 x 坐標不變
            player->bullets[i].position.y -= BULLET_SPEED * GetFrameTime();
            
            // 如果子彈超出屏幕頂部，停用子彈
            if (player->bullets[i].position.y < 0) {
                player->bullets[i].active = false;
            }
        }
    }

    // 根據控制反轉狀態調整移動方向
    float moveDirection = player->controlsReversed ? -1.0f : 1.0f;
    
    // 水平移動
    if (IsKeyDown(KEY_LEFT)) {
        player->position.x -= player->speed * GetFrameTime() * moveDirection;
    }
    if (IsKeyDown(KEY_RIGHT)) {
        player->position.x += player->speed * GetFrameTime() * moveDirection;
    }
    
    // 跳躍
    if (IsKeyDown(KEY_SPACE) && player->isGrounded) {
        player->velocity.y = -player->jumpForce * moveDirection;
        player->isGrounded = false;
    }

    // 更新碰撞矩形
    player_hitbox(player); 

    // 撞到牆底
    if (player->stage == 1 && CheckCollisionRecs(player->hitbox, stage1_wall) && player->velocityY < 0) {
        player->position.y = stage1_wall.y + stage1_wall.height;  
        player->velocityY = 0;
    }
}

