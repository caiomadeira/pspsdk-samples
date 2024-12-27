#include "graphic.h"

#define MENU_MAX_OPTIONS 4
#define readButtonState sceCtrlReadBufferPositive
#define readButtonStateContinuous sceCtrlPeekBufferPositive
static unsigned int __attribute__((aligned(16))) list[262144];

float posX = 0.0f;
float posY = 0.0f; 
float posZ = -3.5f;

PSP_MODULE_INFO("Psp test", PSP_MODULE_USER, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_VFPU | THREAD_ATTR_USER);
static unsigned int __attribute__((aligned(16))) list2[1024] = {0};

struct Vertex3D2 {
    u32 color;
    u16 x, y, z;
} __attribute__((aligned(4)));

typedef enum 
{
    SCREEN_MENU,
    SCREEN_NEW_GAME,
    SCREEN_LOAD_GAME,
    SCREEN_CREDITS,
    SCREEN_EXIT
} ScreenState;

void mainMenuScreen(ScreenState* currentScreen, SceCtrlData* pad, int* oldButtons, int* option);
void newGameScreen(ScreenState* currentScreen, SceCtrlData* pad, int* oldButtons, void* fbp0, void* fbp1, void* zbp);
void creditMenuScreen(ScreenState* currentScreen, SceCtrlData* pad, int* oldButtons);

SceCtrlData initControl()
{
    SceCtrlData pad;
    sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
    return pad;
}

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

int main()
{

    setup_callbacks();
    SceCtrlData pad = initControl();
    int option = 0;
    int oldButtons = 0; // Debounce
    ScreenState currentScreen = SCREEN_MENU;

    //initGuForFont();

    // // Setup frame buffers
    void* frameBuffer0 = guGetStaticVramBuffer(BUF_WIDTH,SCR_HEIGHT,GU_PSM_8888);
    void* frameBuffer1 = guGetStaticVramBuffer(BUF_WIDTH,SCR_HEIGHT,GU_PSM_8888);
    void* depthBuffer = guGetStaticVramBuffer(BUF_WIDTH,SCR_HEIGHT,GU_PSM_4444);
    // void* frameBuffer0 = (void*)0x0;
    // void* frameBuffer1 = (void*)0x88000;
    // void* depthBuffer = (void*)0x110000;
    initGu(frameBuffer0, frameBuffer1, depthBuffer, true);
    running = 1;
    while (running) 
    {
        readButtonState(&pad, 1);

        switch(currentScreen)
        {
            case SCREEN_MENU: 
            {
                startFrame(RAW_WHITE);
                mainMenuScreen(&currentScreen, &pad, &oldButtons, &option);
                break;
            }

            case SCREEN_NEW_GAME: {
                // endFrame();
                // configureGuForCube();
                // newGameScreen(&currentScreen, &pad, &oldButtons, frameBuffer0, frameBuffer1, depthBuffer);
                startFrameCube(RAW_BLUE);
                renderCube(posX, posY, posZ);
                if (pad.Buttons & PSP_CTRL_RIGHT)
                {
                    posX++;
                    //sceGumTranslate(&pos);
                }
                if (pad.Buttons & PSP_CTRL_LEFT)
                {
                    posX--;
                }
                if (pad.Buttons & PSP_CTRL_UP)
                {
                    posZ--;
                }
                if (pad.Buttons & PSP_CTRL_DOWN)
                {
                    posZ++;
                }
                if ((pad.Buttons & PSP_CTRL_CIRCLE) && !(oldButtons & PSP_CTRL_CIRCLE)) {
                    currentScreen = SCREEN_MENU;  // Volta ao menu
                }
                break;
            }

            case SCREEN_LOAD_GAME: {

                break;
            }

            case SCREEN_CREDITS: {
                endFrame();
                creditMenuScreen(&currentScreen, &pad, &oldButtons);
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

void newGameScreen(ScreenState* currentScreen, SceCtrlData* pad, int* oldButtons, void* fbp0, void* fbp1, void* zbp)
{
    
    while(1)
    {
        readButtonStateContinuous(pad, 1);
        startFrameCube(RAW_BLUE);
        

        //sceDisplayWaitVblankStart();
        if ((pad->Buttons & PSP_CTRL_CIRCLE) && !(*oldButtons & PSP_CTRL_CIRCLE))
        {
            break;
        }
        *oldButtons = pad->Buttons; // Atualiza estado anterior dos botões
        endFrame();

    }
    *currentScreen = SCREEN_MENU;
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
        {
            break;
        }
        else if (posY1 < -20) // Créditos saíram da tela
        {
            break;
        }

        *oldButtons = pad->Buttons; // Atualiza estado anterior dos botões
        endFrame();
    }
    *currentScreen = SCREEN_MENU;
}