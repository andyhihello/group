#ifndef PLAYER_ATTRACK_H
#define PLAYER_ATTRACK_H
#include "raylib.h"
#include "player.h"

void reload(Player *player);         // 換彈邏輯
void attrackdata(Player *player);    // 顯示玩家狀態
void playerattrack(Player *player);  // 玩家射擊與狀態更新

extern Bullet bullets[];

#endif