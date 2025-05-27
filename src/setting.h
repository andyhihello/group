#ifndef SETTING_H
#define SETTING_H

void updateSettings(GameState *currentGameState);
void drawSettings(GameTextures *textures);

extern float bgmVolume;
extern float sfxVolume;
extern float uiVolume;

#endif