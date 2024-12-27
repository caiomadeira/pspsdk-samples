#include "menu.h"

void mainMenuScreen(ScreenState* currentScreen, SceCtrlData* pad, int* oldButtons, int* option)
{
    const char* text = "THE TEST GAME";
    int calcTextW = calculateTextWidth(text, 0);
    drawStr(text, 10,  (SCR_HEIGHT / 4), (unsigned int) RAW_BLACK, 0);
    drawStr("Created by Caio Madeira.", (SCR_WIDTH - (calcTextW*1.8)), 258, (unsigned int) RAW_BLACK, 0);    

    if (*option == 0)
    {
        drawStr("> New Game", 10, (SCR_HEIGHT / 2), (unsigned int) RAW_BLUE, 0);
        drawStr("Load Game", 10, (SCR_HEIGHT / 2) + 20, (unsigned int) RAW_BLACK, 0);
        drawStr("Credits", 10, (SCR_HEIGHT / 2) + 40, (unsigned int) RAW_BLACK, 0);
        drawStr("Exit Game", 10, (SCR_HEIGHT / 2) + 80, (unsigned int) RAW_BLACK, 0);
    }
    else if (*option == 1)
    {
        drawStr("New Game", 10, (SCR_HEIGHT / 2) , (unsigned int) RAW_BLACK, 0);
        drawStr("> Load Game", 10, (SCR_HEIGHT / 2) + 20, (unsigned int) RAW_BLUE, 0);
        drawStr("Credits", 10, (SCR_HEIGHT / 2) + 40, (unsigned int) RAW_BLACK, 0);
        drawStr("Exit Game", 10, (SCR_HEIGHT / 2) + 80, (unsigned int) RAW_BLACK, 0);
    }
    else if (*option == 2) 
    {
        drawStr("New Game", 10, (SCR_HEIGHT / 2) , (unsigned int) RAW_BLACK, 0);
        drawStr("Load Game", 10, (SCR_HEIGHT / 2) + 20, (unsigned int) RAW_BLACK, 0);
        drawStr("> Credits", 10, (SCR_HEIGHT / 2) + 40, (unsigned int) RAW_BLUE, 0);
        drawStr("Exit Game", 10, (SCR_HEIGHT / 2) + 80, (unsigned int) RAW_BLACK, 0);
    }
    else if (*option == 3)
    {
        drawStr("New Game", 10, (SCR_HEIGHT / 2), (unsigned int) RAW_BLACK, 0);
        drawStr("Load Game", 10, (SCR_HEIGHT / 2) + 20, (unsigned int) RAW_BLACK, 0);
        drawStr("Credits", 10, (SCR_HEIGHT / 2) + 40, (unsigned int) RAW_BLACK, 0);
        drawStr("> Exit Game", 10, (SCR_HEIGHT / 2) + 80, (unsigned int) RAW_BLUE, 0);
    }
    
    sceDisplayWaitVblankStart();
    //sceGuSwapBuffers();

    // Navegação do menu
    if ((pad->Buttons & PSP_CTRL_DOWN) && !(*oldButtons & PSP_CTRL_DOWN))
    {
        if (*option < MENU_MAX_OPTIONS -1)
            (*option)++;
    }

    if ((pad->Buttons & PSP_CTRL_UP) && !(*oldButtons & PSP_CTRL_DOWN))
    {
        if (*option > 0)
            (*option)--;
    }

    if ((pad->Buttons & PSP_CTRL_CROSS))
    {
        if (*option == 0) 
            *currentScreen = SCREEN_NEW_GAME;
        else if (*option == 1)
            *currentScreen = SCREEN_LOAD_GAME;
        else if (*option == 2) 
            *currentScreen = SCREEN_CREDITS;
        else if (*option == 3)
            *currentScreen = SCREEN_EXIT;
    }
}

void creditMenuScreen(ScreenState* currentScreen, SceCtrlData* pad, int* oldButtons)
{
    int posY1 = 300;
    int posY2 = 280;
    while(1) 
    {   
        readButtonStateContinuous(pad, 1);
        startFrame(RAW_RED);
        drawStr("Credits", 0, 0, (unsigned int) RAW_WHITE, 0);
        drawStr("Press (O) to back to main menu.", 5, 250, (unsigned int) RAW_BLACK, 0);
        drawStr("Thanks to PSPDEV Community", (SCR_HEIGHT / 2), posY1, (unsigned int) RAW_BLACK, 0); 
        drawStr("Created by Caio Madeira", (SCR_HEIGHT / 2), posY2, (unsigned int) RAW_BLACK, 0); 
        posY1--;
        posY2--;
        // Sincroniza o frame
        sceDisplayWaitVblankStart();
        // Atualiza estado dos controles

        // Verifica se o usuário pressionou "O" ou se os créditos saíram da tela
        if ((pad->Buttons & PSP_CTRL_CIRCLE) && !(*oldButtons & PSP_CTRL_CIRCLE))
            break;
        else if (posY1 < -20) // Créditos saíram da tela
            break;

        *oldButtons = pad->Buttons; // Atualiza estado anterior dos botões
        endFrame();
    }
    *currentScreen = SCREEN_MENU;
}