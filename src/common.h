#ifndef common_h
#define common_h

#include <pspkernel.h>
#include <pspgu.h>
#include <pspgum.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <pspctrl.h>

#include "color.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#define readButtonState sceCtrlReadBufferPositive
#define readButtonStateContinuous sceCtrlPeekBufferPositive
// #define SceCtrlData PSPControl

// Declara running como extern p/ evitar multilas definicoes
extern int running;

int exit_callback(int arg1, int arg2, void *common);
int callback_thread(SceSize args, void *argp);
int setupCallbacks(void);
SceCtrlData initControl(void);

#endif 