#include "src/common.h"
#include "src/camera.h"
#include "src/player.h"

PSP_MODULE_INFO("test3d", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

int main() {
    setup_callbacks();
    
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, APP_NAME);

    GameScreen currentScreen = ENTRY;
    int fpsCount = 0;
    const char menuTitle[10] = "Title 01";
        
    Font menuFont = LoadFont("assets/anonymous_pro_bold.ttf");
    Vector2 fontPosition1 = { SCREEN_WIDTH/2.0f - MeasureTextEx(menuFont, menuTitle, (float)menuFont.baseSize, -3).x/2,
                              SCREEN_HEIGHT/2.0f - menuFont.baseSize/2.0f - 80.0f };

    

    Camera3D camera = { 0 };
    InitGameCamera(&camera);

    Player player;
    InitPlayer(&player, PLAYER_MODEL);

    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
    SceCtrlData pad;

    SetTargetFPS(60);

    while (running) {
        sceCtrlReadBufferPositive(&pad, 1);


        switch (currentScreen) 
        {
            case ENTRY:
                fpsCount++;
                if (fpsCount > 120) currentScreen = TITLE; 
                break;

            case TITLE:
                if (pad.Buttons & PSP_CTRL_CROSS) currentScreen = GAMEPLAY; 
                break;

            case GAMEPLAY:
                UpdatePlayer(&player, pad);
                UpdateGameCamera(&camera, player.position, player.rotation);
                break;

            default: break;
        }
        //----------------------------------------------------------------------------------
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(RAYWHITE);
            switch(currentScreen) 
            {
                case ENTRY:
                    DrawText("By Caio Madeira", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 40, LIGHTGRAY);
                    break;
                case TITLE:
                    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GREEN);
                    DrawTextEx(menuFont, menuTitle, fontPosition1, (float)menuFont.baseSize, -3, WHITE);
                    DrawTextEx(menuFont, "PRESS X to JUMP", (Vector2){190, 200}, 20, 2, WHITE);
                    break;
                case GAMEPLAY:
                    BeginMode3D(camera);
                        DrawPlayer(&player);
                        DrawGrid(20, 1.0f);
                    EndMode3D();
                    DrawText("Analogico: Mover | L/R: Girar", 10, 10, 20, BLACK);
                    DrawFPS(SCREEN_WIDTH - 100, 10);
                    break;
            }

        EndDrawing();
    }

    UnloadFont(menuFont);
    UnloadPlayer(&player);
    CloseWindow();

    sceKernelExitGame();
    return 0;
}
