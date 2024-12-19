#include "common.h"
#include "color.h"
#include "graphic.h"
#include "util.h"

#define MENU_MAX_OPTIONS 3
#define readButtonState sceCtrlReadBufferPositive

PSP_MODULE_INFO("Psp test", PSP_MODULE_USER, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_VFPU | THREAD_ATTR_USER);
static unsigned int __attribute__((aligned(16))) listFont[262144];

typedef enum 
{
    SCREEN_MENU,
    SCREEN_NEW_GAME,
    SCREEN_EXIT
} ScreenState;

SceCtrlData initControl()
{
    SceCtrlData pad;
    sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
    return pad;
}

int main()
{
    void* txtBuff;
    SceCtrlData pad = initControl();
    int option = 0;
    int oldButtons = 0; // Debounce
    ScreenState currentScreen = SCREEN_MENU;

    setup_callbacks();
    initGuForFont();

    int calcTextW;
    running = 1;
    while (running) 
    {
        /*
        int sceCtrlReadBufferPositive(SceCtrlData *ctrlData, int numControls);

        a função sceCtrlReadBufferPositive é usada para ler os estados dos botões do controle (botões da PSP) em um 
        determinado momento. Essa função retorna um buffer de controles, permitindo que você verifique quais botões 
        foram pressionados ou liberados durante a execução do código.
        */
        readButtonState(&pad, 1);

        //drawStrWithEffectRainbowEffect("EU SOU LGBT!!!!!!!!!", 0, 244, 0, t);
        startFrame(RAW_WHITE);

        switch(currentScreen)
        {
            case SCREEN_MENU: 
            {
                const char* text = "THE TEST GAME";
                calcTextW = calculateTextWidth(text, 0);
                drawStr(text, 10,  (SCR_HEIGHT / 4), (unsigned int) RAW_BLACK, 0);
                drawStr("Created by Caio Madeira.", (SCR_WIDTH - (calcTextW*1.8)), 258, (unsigned int) RAW_BLACK, 0);    

                if (option == 0)
                {
                    drawStr("> New Game", 10, (SCR_HEIGHT / 2), (unsigned int) RAW_BLUE, 0);
                    drawStr("Load Game", 10, (SCR_HEIGHT / 2) + 20, (unsigned int) RAW_BLACK, 0);
                    drawStr("Exit Game", 10, (SCR_HEIGHT / 2) + 40, (unsigned int) RAW_BLACK, 0);
                }
                else if (option == 1)
                {
                    drawStr("New Game", 10, (SCR_HEIGHT / 2) , (unsigned int) RAW_BLACK, 0);
                    drawStr("> Load Game", 10, (SCR_HEIGHT / 2) + 20, (unsigned int) RAW_BLUE, 0);
                    drawStr("Exit Game", 10, (SCR_HEIGHT / 2) + 40, (unsigned int) RAW_BLACK, 0);
                }
                else if (option == 2) 
                {
                    drawStr("New Game", 10, (SCR_HEIGHT / 2) , (unsigned int) RAW_BLACK, 0);
                    drawStr("Load Game", 10, (SCR_HEIGHT / 2) + 20, (unsigned int) RAW_BLACK, 0);
                    drawStr("> Exit Game", 10, (SCR_HEIGHT / 2) + 40, (unsigned int) RAW_BLUE, 0);
                }        
                
                sceDisplayWaitVblankStart();
                //sceGuSwapBuffers();

                // Navegação do menu
                if ((pad.Buttons & PSP_CTRL_DOWN) && !(oldButtons & PSP_CTRL_DOWN))
                {
                    if (option < MENU_MAX_OPTIONS -1)
                        option++;
                }

                if ((pad.Buttons & PSP_CTRL_UP) && !(oldButtons & PSP_CTRL_DOWN))
                {
                    if (option > 0)
                        option--;
                }

                if ((pad.Buttons & PSP_CTRL_CROSS))
                {
                    if (option == 0)
                    {
                        currentScreen = SCREEN_NEW_GAME;
                    } else if (option == 2)
                    {
                        currentScreen = SCREEN_EXIT;
                    }
                }
                break;
            }

            case SCREEN_NEW_GAME: {
                drawStr("New Screen", 10,  (SCR_HEIGHT / 4), (unsigned int) RAW_BLACK, 0);
                drawStr("Press (O) to back to main menu.", 5, 250, (unsigned int) RAW_BLACK, 0);    

                // Verificar se o usuário quer voltar ao menu
                if ((pad.Buttons & PSP_CTRL_CIRCLE)) {
                    currentScreen = SCREEN_MENU;
                }

                break;
            }

            case SCREEN_EXIT:
            {
                running = 0;
                break;
            }
        }

        oldButtons = pad.Buttons;
        endFrame();
    }   

    endGu();
    return 0;
}
