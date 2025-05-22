#ifndef TEXTURE_H
#define TEXTURE_H

#include "raylib.h"

typedef struct {
    Texture2D playerStand;
    Texture2D playerRun[9];
    Texture2D shieldTexture;
    Texture2D playerbullet;

    Texture2D bossAttack[6];

    Texture2D dronePatrol[5];
    Texture2D droneChase[9];
    Texture2D droneAttack[4];
    Texture2D droneLaser;

    Texture2D stage1Background[5];
    Texture2D stage2Background[2];
    Texture2D menuBackground;
} GameTextures;

void loadGameTextures(GameTextures *textures);
void unloadGameTextures(GameTextures *textures);

#endif