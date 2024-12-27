// psp libs
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspgu.h>
#include <pspgum.h>
#include <pspdisplay.h>
#include <pspge.h>
#include <pspctrl.h>
#include<string.h>
#include <png.h>

// c libs
#include <stdio.h>
#include <stdlib.h>

// ALIAS
#define printDebug pspDebugScreenPrintf
#define clearScreen pspDebugScreenClear
#define setScreenBackColor pspDebugScreenSetBackColor
#define setTextColor pspDebugScreenSetTextColor

// COLORS
typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
} RGB32;

// Define o módulo principal do PSP
PSP_MODULE_INFO("My PSP App", PSP_MODULE_USER, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);

// Cria cor rgba
u32 newColorRGB32(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
    return (alpha << 24) | (red << 16) | (green << 8) | blue;
}

// PSP CRAZY STUFF FUNCTIONS
/* Exit callback */
int exit_callback(int arg1, int arg2, void *common) {
    sceKernelExitGame();
    return 0;
}

/* Callback thread */
int CallbackThread(SceSize args, void *argp) {
          int cbid;

          cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
          sceKernelRegisterExitCallback(cbid);

          sceKernelSleepThreadCB();

          return 0;
}

/* Sets up the callback thread and returns its thread id */
int SetupCallbacks(void) {
          int thid = 0;

          thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
          if(thid >= 0) {
                    sceKernelStartThread(thid, 0, 0);
          }

          return thid;
}

int main() {
    pspDebugScreenInit();
    SetupCallbacks();
    clearScreen();
    u32 blue = newColorRGB32(255, 100, 0, 255);
    u32 green = newColorRGB32(0, 255, 0, 0);
    setScreenBackColor(blue);
    setTextColor(green);

    printDebug("TIMER\n");
    printDebug("Press TRIANGLE TO START THE TIMER\n");
    SceCtrlData pad;
    int i;
    int counter = 0;

    while(1)
    {
        
        sceCtrlReadBufferPositive(&pad, 1);
        if (pad.Buttons & PSP_CTRL_TRIANGLE)
        {
            break;
        }
    }

    while(1) 
    {
        sceCtrlReadBufferPositive(&pad, 1);
        if(pad.Buttons & PSP_CTRL_CIRCLE) 
                    break;
        clearScreen();
        printDebug("Press [0] to stop the timer\n");
        printDebug("Counter: %i\n", counter);
        counter++;

        for(i = 0; i < 5; i++)
            sceDisplayWaitVblankStart();
    }
    clearScreen();
    printDebug("Counter Finished.");
    printDebug("Final Count: %i", counter);

    sceKernelSleepThread(); // Mantém a aplicação ativa

    //sceKernelExitGame(); //Finaliza o aplicativo e retorna ao menu XMB do PSP.

    return 0;
}
