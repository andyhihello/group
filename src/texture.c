#include "texture.h"
#include <stdio.h>

void loadGameTextures(GameTextures *textures, GameSounds *sounds) {
    char path[100];

    // Player
    textures->playerStand = LoadTexture("resource/player/stand.png");
    for (int i = 0; i < 9; i++) {
        sprintf(path, "resource/player/run%d.png", (i < 5) ? (i + 1) : (9 - i));
        textures->playerRun[i] = LoadTexture(path);
    }
    textures->shieldTexture = LoadTexture("resource/player/shield.png");
    textures->playerBullet = LoadTexture("resource/player/bullet.png");
    textures->shooting = LoadTexture("resource/player/shooting.png");
    
    // Boss
    textures->boss = LoadTexture("resource/boss/Boss.png");
    for(int i = 1; i <= 5; i++) {
        sprintf(path, "resource/boss/stage1/0%d.png", i);
        textures->bossMovePhase1[i-1] = LoadTexture(path);
    }
    for(int i = 1; i <= 5; i++) {
        sprintf(path, "resource/boss/stage2/0%d.png", i);
        textures->bossMovePhase2[i-1] = LoadTexture(path);  // 第二阶段移动动画
    }
    
    // Load boss attack animation frames
    for (int i = 0; i < 2; i++) {
        sprintf(path, "resource/boss/Boss attack/Boss atack 0%d.png", i+1);
        textures->bossAttack[i] = LoadTexture(path);
    }
    
    textures->bossLight = LoadTexture("resource/boss/stage 2 light.png");

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

    //soldier

    textures->soldierstagebullet = LoadTexture("resource/soldier/stagebullet.png");
    textures->soldierbossbullet = LoadTexture("resource/soldier/bossbullet.png");
    textures->soldiertexture = LoadTexture("resource/soldier/soldier.png");

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
    textures->settingBackground = LoadTexture("resource/scene/settingbackground.png");
    textures->stage4Background = LoadTexture("resource/scene/2-5.png");

    textures->dataWaveTexture = LoadTexture("resource/boss/soldier/bossbullet.png");  // 載入數據波動畫圖片

    //sound
    sounds->menumusic = LoadMusicStream("resource/sound/menumusic.mp3");
    sounds->stagemusic = LoadMusicStream("resource/sound/stagemusic.mp3");
    sounds->tutorialmusic = LoadMusicStream("resource/sound/tutorialmusic.mp3");
    sounds->endingmusic = LoadMusicStream("resource/sound/endingmusic.mp3");
    sounds->upgrade = LoadSound("resource/sound/upgrade.mp3");
    sounds->attack = LoadSound("resource/sound/attack.mp3");
    sounds->playerdied = LoadSound("resource/sound/playerdied.mp3");
    sounds->noammo = LoadSound("resource/sound/noammo.mp3");
    sounds->enterbossstage = LoadSound("resource/sound/enterbossstage.mp3");
    sounds->bossMusic = LoadMusicStream("resource/sound/boss ing.mp3");

}

void unloadGameTextures(GameTextures *textures, GameSounds *sounds) {
    // Player
    UnloadTexture(textures->playerStand);
    for (int i = 0; i < 9; i++) UnloadTexture(textures->playerRun[i]);
    UnloadTexture(textures->shieldTexture);
    UnloadTexture(textures->playerBullet);
    UnloadTexture(textures->shooting);

    // Boss
    UnloadTexture(textures->boss);
    for (int i = 0; i < 2; i++) UnloadTexture(textures->bossAttack[i]);
    UnloadTexture(textures->bossLight);

    // Drone
    for (int i = 0; i < 5; i++) UnloadTexture(textures->dronePatrol[i]);
    for (int i = 0; i < 9; i++) UnloadTexture(textures->droneChase[i]);
    for (int i = 0; i < 4; i++) UnloadTexture(textures->droneAttack[i]);
    UnloadTexture(textures->droneLaser);

    // Soldier
    UnloadTexture(textures->soldierstagebullet);
    UnloadTexture(textures->soldierbossbullet);
    UnloadTexture(textures->soldiertexture);

    // Stage
    for (int i = 0; i < 5; i++) UnloadTexture(textures->stage1Background[i]);
    for (int i = 0; i < 2; i++) UnloadTexture(textures->stage2Background[i]);
    UnloadTexture(textures->menuBackground);
    UnloadTexture(textures->settingBackground);
    UnloadTexture(textures->stage4Background);

    // Data wave
    UnloadTexture(textures->dataWaveTexture);

    // Sound
    UnloadMusicStream(sounds->menumusic);
    UnloadMusicStream(sounds->stagemusic);
    UnloadMusicStream(sounds->tutorialmusic);
    UnloadMusicStream(sounds->endingmusic);
    UnloadMusicStream(sounds->bossMusic);

    UnloadSound(sounds->upgrade);
    UnloadSound(sounds->attack);
    UnloadSound(sounds->playerdied);
    UnloadSound(sounds->noammo);
    UnloadSound(sounds->enterbossstage);

    // Close audio
    CloseAudioDevice();
}
