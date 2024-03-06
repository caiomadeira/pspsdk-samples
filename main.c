#include "sources/internal/pspvram.h"
#include "sources/internal/pspgraphics.h"

// Include Graphics Libraries // Once included in pspvram.c
#include <pspdisplay.h>
#include <pspgu.h>
#include <pspgum.h>

// PSP Module Info
PSP_MODULE_INFO("Context Sample", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

// Define PSP Width / Height
#define PSP_BUF_WIDTH (512)
#define PSP_SCR_WIDTH (480)
#define PSP_SCR_HEIGHT (272)

// Global variables
int running = 1;

int main() {
    // Boilerplate
    SetupCallbacks();

    // Initialize Graphics
    initGraphics();

    
    //Main program loop
    while(running){
        startFrame();
    
        //Clear background to Green
        sceGuClearColor(0xFF00FF00);
        sceGuClear(GU_COLOR_BUFFER_BIT);
    
        endFrame();
    }

    // Terminate Graphics
    termGraphics();

    // Exit Game
    sceKernelExitGame();
    return 0;
}