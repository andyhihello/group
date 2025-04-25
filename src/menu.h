#ifndef MENU_H
#define MENU_H

#include "main.h"

#define BUTTON_WIDTH  450
#define BUTTON_HEIGHT 90
#define BUTTON_PADDING 50

// 函數
void initMenu(Texture2D background);
void updateMenu(GameState *currentGameState); // 注意這裡傳遞的是指標，以便在 menu.c 中修改 main.c 的變數
void drawMenu();

#endif