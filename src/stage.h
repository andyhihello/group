#ifndef STAGE_H
#define STAGE_H

#include "raylib.h"
#include "player.h"
#include "main.h"
#include "boss.h"

void stage_drawtutorial(Player *player);
void stage_door(Player *player);
void stage_drawdoortext();
void stage_drawhitbox();

void stage2_init(Texture2D *backgrounds);
void stage2_draw(Texture2D *backgrounds);
void stage2_update(Player *player, Boss *boss);
void stage_drawgridlines();
#endif