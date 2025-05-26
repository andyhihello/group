#include "raylib.h"
#include "player.h"
#include "boss.h"
#include "main.h"
#include <math.h>
Rectangle stage1_wall = { 9010, 0, 70, 455 };
float upgradeEffectTimer = 0.0f; // 特效剩餘時間
const float upgradeEffectDuration = 1.5f; // 特效持續秒數
float upgradeFailTimer = 0.0f;        // 顯示失敗訊息用的計時器
const float upgradeFailDuration = 1.5f; // 顯示 1.5 秒
float healEffectTimer = 0.0f;
const float healEffectDuration = 1.5f;

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
    // 基本屬性初始化
    player->position = (Vector2){300, 300};   // 起始位置
    player->hp = 100000000;                         // 初始血量
    player->coin = 0;                         // 初始金幣
    player->damage = 5;                       // 子彈傷害
    player->dead = false;                     // 是否死亡
    player->invincible = false;               // 是否無敵
    player->invincibleDuration = 3.0f;        // 無敵持續時間
    player->invincibleTimeLeft = 0.0f;        // 無敵剩餘時間
    player->invincibleCooldown = 15.0f;       // 無敵技能冷卻時間
    player->invincibleCooldownLeft = 0.0f;    // 無敵剩餘冷卻
    memset(player->bullets, 0, sizeof(player->bullets)); // 清空子彈狀態

    // 子彈與移動相關設定
    player->reloadtime = 3;                   // 換彈所需時間（秒）
    player->reloadTimeLeft = 0;               // 換彈倒數時間
    player->ammo = 100;                       // 當前子彈數
    player->maxAmmo = 100;                    // 最大子彈數
    player->speed = 300;                      // 移動速度
    player->stage = 1;                        // 起始關卡
    player->tutorial = 0;                     // 教學進度
    player->controlsReversed = false;         // 是否控制反轉
    player->controlReverseTimer = 0.0f;       // 控制反轉剩餘時間

    // 升級系統參數初始化
    player->upgrade_reload_cost = 2;          // 第一次升級換彈所需金幣
    player->upgrade_ammo_cost = 1;            // 第一次升級彈容量所需金幣
    player->upgrade_invincible_cost = 3;      // 第一次升級無敵CD所需金幣

    player->reload_upgrade_ratio = 0.9f;      // 每次升級後換彈時間變成原來的90%
    player->ammo_upgrade_amount = 5;          // 每次升級增加5發彈量
    player->invincible_upgrade_ratio = 0.85f; // 每次升級無敵CD變成原來的85%

    player->reload_upgrade_times = 0;         // 換彈升級次數
    player->ammo_upgrade_times = 0;           // 彈量升級次數
    player->invincible_upgrade_times = 0;     // 無敵CD升級次數

    player->shootEffectTimer = 0.0f;          // 開槍特效倒數時間
}

void player_update(Player *player, float deltaTime) {
    // 更新無敵狀態與冷卻
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

    // 控制反轉更新
    if (player->controlsReversed) {
        player->controlReverseTimer -= deltaTime;
        if (player->controlReverseTimer <= 0) {
            player->controlsReversed = false;
            player->controlReverseTimer = 0;
        }
    }

    // 控制反轉後左右鍵對調
    if (player->controlsReversed){
        // 移動反轉從左移改成右移
        if (IsKeyDown(KEY_A)) {
            player->position.x += player->speed * deltaTime;

            // 如果碰到牆，就把角色卡在牆的左側
            if(player->stage == 1 && CheckCollisionRecs(player->hitbox, stage1_wall))player->position.x = stage1_wall.x - stage1_wall.width - playerXoffset;

            player->facingRight = true;
        }

        // 移動反轉從右移改成左移
        if (IsKeyDown(KEY_D)) {
            player->position.x -= player->speed * deltaTime;

            // 如果碰到牆，就把角色卡在牆的右側
            if(player->stage == 1 && CheckCollisionRecs(player->hitbox, stage1_wall))player->position.x = stage1_wall.x + playerWidth - playerXoffset;           

            player->facingRight = false;
        }
    }
    else{
        // 左移
        if (IsKeyDown(KEY_A)) {
        player->position.x -= player->speed * deltaTime;

        // 如果碰到牆，就把角色卡在牆的右側
        if(player->stage == 1 && CheckCollisionRecs(player->hitbox, stage1_wall))player->position.x = stage1_wall.x + stage1_wall.width - playerXoffset;

        player->facingRight = false;
        }

        // 右移
        if (IsKeyDown(KEY_D)) {
            player->position.x += player->speed * deltaTime;

            // 如果碰到牆，就把角色卡在牆的左側
            if(player->stage == 1 && CheckCollisionRecs(player->hitbox, stage1_wall))player->position.x = stage1_wall.x - playerWidth - playerXoffset;           

            player->facingRight = true;
        }
    }
    // 跑步動畫控制
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_D)) {
        player->frameTimer += deltaTime;
        player->isRunning = true;
        if (player->frameTimer >= 0.1f) {
            player->currentFrame = (player->currentFrame + 1) % 9;
            player->frameTimer = 0;
        }
    } else {
        player->currentFrame = 0;
        player->isRunning = false;
    }

    // 跳躍控制
    if (!player->controlsReversed) {
        if (IsKeyPressed(KEY_W) && !player->isJumping) {
            player->velocityY = JUMP_STRENGTH;
            player->isJumping = true;
        }
    } 
    else {
        if (IsKeyPressed(KEY_S) && !player->isJumping) {
            player->velocityY = JUMP_STRENGTH;
            player->isJumping = true;
        }
    }

    player->position.y += player->velocityY * deltaTime;
    player->velocityY += GRAVITY;

    if (player->position.y > GROUND_Y) {
        player->position.y = GROUND_Y;
        player->velocityY = 0;
        player->isJumping = false;
    }

    // 限制不能超出地圖
    if (player->position.x < -50) player->position.x = -50;
    if (player->position.x > stage1Width - playerWidth) player->position.x = stage1Width - playerWidth;

    // 更新碰撞矩形
    player_hitbox(player);

    // 撞到牆底
    if (player->stage == 1 && CheckCollisionRecs(player->hitbox, stage1_wall) && player->velocityY < 0) {
        player->position.y = stage1_wall.y + stage1_wall.height;  
        player->velocityY = 0;
    }

    if(player->hp <= 0) player->dead = true;

    if(IsKeyPressed(KEY_C)){
        if(player->coin > 0){
            player->hp += 60;
            if(player->hp >100) player->hp = 100;
            healEffectTimer = healEffectDuration;
            player->coin --;
        }
        else upgradeFailTimer = upgradeFailDuration;
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
    // 基本資訊
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

    if (upgradeFailTimer > 0.0f) {
        float alpha = upgradeFailTimer / upgradeFailDuration;
        Color alertColor = (Color){255, 0, 0, (unsigned char)(alpha * 255)};
        DrawText("Not enough coins!", screenWidth / 2 - MeasureText("Not enough coins!", 30) / 2, screenHeight / 2 - 150, 30, alertColor);
    
        upgradeFailTimer -= GetFrameTime();
    }

    // 補血文字特效顯示
    if (healEffectTimer > 0.0f) {
        float alpha = healEffectTimer / healEffectDuration;  // 1.0秒顯示時間
        Color healColor = (Color){0, 255, 0, (unsigned char)(alpha * 255)};  // 綠色透明淡出

        char healText[30];
        snprintf(healText, sizeof(healText), "Heal Success");
        DrawText(healText,
                screenWidth / 2 - MeasureText(healText, 30) / 2,
                screenHeight / 2 - 200,
                30,
                healColor);

        healEffectTimer -= GetFrameTime();
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
                upgradeFailTimer = upgradeFailDuration;
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
                upgradeFailTimer = upgradeFailDuration;
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
                upgradeFailTimer = upgradeFailDuration;
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

    if (player->shootEffectTimer > 0.0f) {
        Vector2 effectPos = player->position;

        if (player->facingRight) {
            effectPos.x += 200;
        } else {
            effectPos.x -= 30;  // shooting.png 寬 60，向左縮一點
        }
        effectPos.y += 160; // 可視情況微調 Y 值

        Rectangle src = { 0, 0, 60, 29 };
        if (!player->facingRight) src.width = -60; // 左翻轉

        Rectangle dest = { effectPos.x, effectPos.y, 60, 29 };
        Vector2 origin = { 0, 0 };

        DrawTexturePro(textures->shooting, src, dest, origin, 0.0f, WHITE);

        player->shootEffectTimer -= GetFrameTime();
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

void player_dead(Player *player,GameState *currentGameState,bool *Isinit){
    

    if (IsKeyPressed(KEY_SPACE)) {
        
        if(player->coin > 1){
            player->coin -= 2;
            player->hp = 100;
            player->position = (Vector2){300, 300}; // 重生點
            player->dead = false;
        }
        else{
            *currentGameState = MENU;
            *Isinit = false;
        }
        
    }

}

void player_deadUI(Player *player){
    ClearBackground(BLACK);
    const char *title = "YOU DIED";
    int titleFontSize = 80;
    int titleWidth = MeasureText(title, titleFontSize);
    DrawText(title, (screenWidth - titleWidth) / 2, screenHeight / 2 - 120, titleFontSize, RED);

    // 有錢：黃色提示
    if (player->coin >= 2) {
        const char *reviveText = "Press [SPACE] to revive (DEDUCT 2 COIN)";
        int textWidth = MeasureText(reviveText, 30);
        DrawText(reviveText, (screenWidth - textWidth) / 2, screenHeight / 2, 30, YELLOW);
    } 
    // 沒錢：灰色 GAME OVER 提示
    else {
        const char *noMoneyText = "Not enough coins. Press [SPACE] and GAMEOVER";
        int textWidth = MeasureText(noMoneyText, 30);
        DrawText(noMoneyText, (screenWidth - textWidth) / 2, screenHeight / 2, 30, GRAY);
    }
}