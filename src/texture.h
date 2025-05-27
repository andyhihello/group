#ifndef TEXTURE_H
#define TEXTURE_H

#include "raylib.h"

typedef struct {
    // Player textures
    Texture2D playerStand;
    Texture2D playerRun[9];
    Texture2D shieldTexture;
    Texture2D playerBullet;
    Texture2D shooting;

    // Boss textures
    Texture2D boss;         // boss移動時的圖片
    Texture2D bossAttack[2];  // boss攻擊動畫幀
    Texture2D bossShield;   // boss護盾效果
    Texture2D bossLight;    // boss能量脈衝效果

    // Drone textures
    Texture2D dronePatrol[5];
    Texture2D droneChase[9];
    Texture2D droneAttack[4];
    Texture2D droneLaser;

    // Background textures
    Texture2D stage1Background[5];
    Texture2D stage2Background[2];
    Texture2D menuBackground;
    Texture2D settingBackground;
} GameTextures;

typedef struct {
    Music menumusic;
    Music stagemusic;
    Music tutorialmusic;
    Sound upgrade;
    Sound attack;
    Sound playerdied;
    Sound enterbossstage;
    Sound win;
} GameSounds;

void loadGameTextures(GameTextures *textures, GameSounds *sounds);
void unloadGameTextures(GameTextures *textures, GameSounds *sounds);

#endif