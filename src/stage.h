#ifndef STAGE_H
#define STAGE_H

#include "raylib.h"
#include "player.h"
#include "main.h"
#include "boss.h"

// 繪製教學場景
void stage_drawtutorial(Player *player);

// 教學場景中的門邏輯（開門音效 / 開門條件判定）
void stage_door(Player *player, GameSounds *sounds);

// 繪製門上的提示文字
void stage_drawdoortext();

// DEBUG 用：繪製碰撞箱（hitbox）
void stage_drawhitbox();

// 初始化 Stage2 背景
void stage2_init(Texture2D *backgrounds);

// 繪製 Stage2 背景
void stage2_draw(Texture2D *backgrounds);

// 更新 Stage2 遊戲邏輯（BOSS戰 + 玩家互動）
void stage2_update(Boss *boss, Player *player, GameSounds *sounds);

// 繪製網格線（DEBUG用）
void stage_drawgridlines();

// 存檔完成時間 (將過關時間存入檔案)
void stage_saveCompletionTime(float completeTime);

// 顯示最快完成時間排行榜 (Top 5 完成時間)
void stage_displayTopCompletionTimes();

// 通關後退出門邏輯（切換 GameState / 重設狀態）
void stage_exitdoor(Player *player, GameState *currentGameState, bool *Isinit);

#endif