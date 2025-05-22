#include "texture.h"
#include <stdio.h>

void loadGameTextures(GameTextures *textures) {
    char path[100];

    // Player
    textures->playerStand = LoadTexture("resource/player/stand.png");
    for (int i = 0; i < 9; i++) {
        sprintf(path, "resource/player/run%d.png", (i < 5) ? (i + 1) : (9 - i));
        textures->playerRun[i] = LoadTexture(path);
    }
    textures->shieldTexture = LoadTexture("resource/player/shield.png");
    textures->playerBullet = LoadTexture("resource/player/bullet.png");
    
    // Boss
    for (int i = 0; i < 6; i++) {
        sprintf(path, "resource/boss/attack/attack%d.png", i+1);
        textures->bossAttack[i] = LoadTexture(path);
    }

    // Drone
    for (int i = 0; i < 5; i++) {
        sprintf(path, "resource/drone/patrol%d.png", (i < 2) ? (i + 1) : (5 - i));
        textures->dronePatrol[i] = LoadTexture(path);
    }
    for (int i = 0; i < 9; i++) {
        sprintf(path, "resource/drone/chase%d.png", (i < 5) ? (i + 1) : (9 - i));
        textures->droneChase[i] = LoadTexture(path);
    }
    for (int i = 0; i < 4; i++) {
        sprintf(path, "resource/drone/attack%d.png", i + 1);
        textures->droneAttack[i] = LoadTexture(path);
    }
    textures->droneLaser = LoadTexture("resource/drone/laser.png");

    // Stage
    for (int i = 0; i < 5; i++) {
        sprintf(path, "resource/scene/1-%d.png", i + 1);
        textures->stage1Background[i] = LoadTexture(path);
    }
    for (int i = 0; i < 2; i++) {
        sprintf(path, "resource/scene/2-%d.png", i + 1);
        textures->stage2Background[i] = LoadTexture(path);
    }
    textures->menuBackground = LoadTexture("resource/scene/background.png");
}

void unloadGameTextures(GameTextures *textures) {
    UnloadTexture(textures->playerStand);
    for (int i = 0; i < 9; i++) UnloadTexture(textures->playerRun[i]);

    for (int i = 0; i < 6; i++) UnloadTexture(textures->bossAttack[i]);

    for (int i = 0; i < 5; i++) UnloadTexture(textures->dronePatrol[i]);
    for (int i = 0; i < 9; i++) UnloadTexture(textures->droneChase[i]);
    for (int i = 0; i < 4; i++) UnloadTexture(textures->droneAttack[i]);
    UnloadTexture(textures->droneLaser);

    for (int i = 0; i < 5; i++) UnloadTexture(textures->stage1Background[i]);
    for (int i = 0; i < 2; i++) UnloadTexture(textures->stage2Background[i]);
    UnloadTexture(textures->menuBackground);
}