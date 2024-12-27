#include "menu.h"

static unsigned int __attribute__((aligned(16))) list[262144];

float posX = 0.0f;
float posY = 0.0f; 
float posZ = -3.5f;

PSP_MODULE_INFO("Psp test", PSP_MODULE_USER, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_VFPU | THREAD_ATTR_USER);
static unsigned int __attribute__((aligned(16))) list2[1024] = {0};

// Background texture
//extern unsigned char background_texture[]; 

void startGame(ScreenState* currentScreen, SceCtrlData* pad, int* oldButtons);

int main()
{
    pspDebugScreenInitEx(0x0, PSP_DISPLAY_PIXEL_FORMAT_8888, 0);
    pspDebugScreenEnableBackColor(0);

    setupCallbacks();
    SceCtrlData pad = initControl();
    int option = 0;
    int oldButtons = 0; // Debounce
    ScreenState currentScreen = SCREEN_MENU;

    // Setup frame buffers
    void* frameBuffer0 = guGetStaticVramBuffer(BUF_WIDTH,SCR_HEIGHT,GU_PSM_8888); //  (void*)0x0;
    void* frameBuffer1 = guGetStaticVramBuffer(BUF_WIDTH,SCR_HEIGHT,GU_PSM_8888); // (void*)0x88000;
    void* depthBuffer = guGetStaticVramBuffer(BUF_WIDTH,SCR_HEIGHT,GU_PSM_4444); // (void*)0x110000;

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

            case SCREEN_NEW_GAME: 
            {
                startGame(&currentScreen, &pad, &oldButtons);
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

void startGame(ScreenState* currentScreen, SceCtrlData* pad, int* oldButtons)
{
    int minX;
    int maxX;
    startFrameCube(RAW_BLUE);
    //startFrameCubeNoBackground();
    renderCube(posX, posY, posZ);

    // Aumentando os limites de movimento em X de acordo com Z
    minX = -1.0f;
    maxX = 1.0f;
    if (posZ < -3.5f)
    {
        float adjustmentFactor = (-posZ - 3.5f); // Quanto mais profundo, maior o ajuste
        minX = -1.0f - adjustmentFactor;
        maxX = 1.0f + adjustmentFactor;
    }

    // Movimento lateral
    if (pad->Buttons & PSP_CTRL_RIGHT)
    {
        if (posX < maxX)
            posX++;
    }
    if (pad->Buttons & PSP_CTRL_LEFT)
    {   
        if (posX > minX)
            posX--;
    }

    // Profundidade
    if (pad->Buttons & PSP_CTRL_UP)
    {
        if (posZ > -10.0f)
        {
            posZ--;
        }
    }
    if (pad->Buttons & PSP_CTRL_DOWN)
        posZ++;
    if ((pad->Buttons & PSP_CTRL_CIRCLE) && !(*oldButtons & PSP_CTRL_CIRCLE))
        *currentScreen = SCREEN_MENU;  // Volta ao menu
}
