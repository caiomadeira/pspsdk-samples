#include "menu.h"
#include "complexgraphics.h"

static unsigned int __attribute__((aligned(16))) list[262144];

float posX = 0.0f;
float posY = 0.0f; 
float posZ = -3.5f;

PSP_MODULE_INFO("Psp test", PSP_MODULE_USER, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_VFPU | THREAD_ATTR_USER);
//static unsigned int __attribute__((aligned(16))) list2[1024] = {0};

void startGame(ScreenState* currentScreen, SceCtrlData* pad, int* oldButtons, 
               ScePspFMatrix4 *projection, ScePspFMatrix4 *view, Geometry* grid);

int main()
{
    setupCallbacks();

    int option = 0;
    int oldButtons = 0; // Debounce
    ScreenState currentScreen = SCREEN_MENU;

    float bgPlanePosZ = -2.5;
    int zoom = -1;

    SceCtrlData pad = initControl();

    // Setup frame buffers
    void* frameBuffer0 = guGetStaticVramBuffer(BUF_WIDTH,SCR_HEIGHT,GU_PSM_8888); //  (void*)0x0;
    void* frameBuffer1 = guGetStaticVramBuffer(BUF_WIDTH,SCR_HEIGHT,GU_PSM_8888); // (void*)0x88000;
    void* depthBuffer = guGetStaticVramBuffer(BUF_WIDTH,SCR_HEIGHT,GU_PSM_4444); // (void*)0x110000;
    initGu(frameBuffer0, frameBuffer1, depthBuffer, true);

    // Setup matrices
    ScePspFMatrix4 identity, projection, view;
    InitMatrices(&identity, &projection, &view);

    // Generate grid geometry
    genGrid(GRID_ROWS, GRID_COLUMNS, GRID_SIZE, grid_vertices, grid_indices);

    Geometry grid = {
        identity,
        sizeof(grid_indices) / sizeof(unsigned short),
        grid_indices,
        grid_vertices,
        0xff7777
    };

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
                // // Update grid position
                // ScePspFVector3 pos = {0, -1.5f, 0};
                // gumLoadIdentity(&grid.world);
                // gumTranslate(&grid.world, &pos);

                // // Render the scene
                // sceGuStart(GU_DIRECT, list);
                // sceGuClearColor(RAW_BLUE);
                // sceGuClearDepth(0);
                // sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT);
                
                // sceGuSetMatrix(GU_PROJECTION, &projection);
                // sceGuSetMatrix(GU_VIEW, &view);
                // drawGeometry(&grid);
                // endFrame();
                startGame(&currentScreen, &pad, &oldButtons, &projection, &view, &grid);
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

void startGame(ScreenState* currentScreen, SceCtrlData* pad, int* oldButtons, 
               ScePspFMatrix4 *projection, ScePspFMatrix4 *view, Geometry* grid)
{
    int minX;
    int maxX;

    startFrame(RAW_CLEAR_GRAY);
    renderScene(posX, posY, posZ);

    endFrame();

    // Ajusta limites de movimento X com base em Z
    minX = -1.0f;
    maxX = 1.0f;
    if (posZ < -3.5f)
    {
        float adjustmentFactor = (-posZ - 3.5f);
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
