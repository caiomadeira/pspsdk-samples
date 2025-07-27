#ifndef common_h
#define common_h

#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <pspsdk.h>

#include "raylib.h"
#include "raymath.h"

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 272

#define APP_NAME "Test3d"

#define PLAYER_MODEL "assets/greenman.glb"

static volatile int running = 1;

int exit_callback(int arg1, int arg2, void* common);
int callback_thread(SceSize args, void* argp);
void setup_callbacks(void);

#endif