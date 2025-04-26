#ifndef STAGE_H
#define STAGE_H

#include "raylib.h"
#include "player.h"



void stage_drawtutorial(Player *player);
void stage_door(Player *player);
void stage_drawdoortext();
void stage_drawhitbox();

#endif