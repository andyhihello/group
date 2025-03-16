#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

typedef struct {
    Vector2 position;
    float speed;
    Texture2D texture;
} Player;

void Initplayer(Player *player);
void Moveplayer(Player *player);

#endif