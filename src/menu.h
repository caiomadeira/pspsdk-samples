#ifndef menu_h
#define menu_h

#include "common.h"
#include "graphic.h"

typedef enum 
{
    SCREEN_MENU,
    SCREEN_NEW_GAME,
    SCREEN_LOAD_GAME,
    SCREEN_CREDITS,
    SCREEN_EXIT
} ScreenState;

#define MENU_MAX_OPTIONS 4

void mainMenuScreen(ScreenState* currentScreen, SceCtrlData* pad, int* oldButtons, int* option);
void creditMenuScreen(ScreenState* currentScreen, SceCtrlData* pad, int* oldButtons);

#endif