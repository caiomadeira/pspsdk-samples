#include "src/common.h"

PSP_MODULE_INFO("test3d", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

typedef enum GameScreen {
    ENTRY = 0,
    TITLE,
    GAMEPLAY
} GameScreen;

Vector3 playerPosition = { 0.0f, 1.0f, 0.0f };
float playerRotation = 0.0f;

const float PLAYER_MOVE_SPEED = 0.05f;
const float PLAYER_ROTATION_SPEED = 2.0f;
const float CAMERA_DISTANCE_BEHIND = 5.0f; 
const float CAMERA_HEIGHT_ABOVE = 3.0f;

int main() {
    setup_callbacks();
    
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, APP_NAME);

    GameScreen currentScreen = ENTRY;

    int fpsCount = 0;
    const int screenWidth = SCREEN_WIDTH;
    const int screenHeight = SCREEN_HEIGHT;
    const char menuTitle[10] = "Title 01";
    Model playerModel = LoadModel(PLAYER_MODEL);  
    
    Font menuFont = LoadFont("assets/anonymous_pro_bold.ttf");
    Vector2 fontPosition1 = { screenWidth/2.0f - MeasureTextEx(menuFont, menuTitle, (float)menuFont.baseSize, -3).x/2,
                              screenHeight/2.0f - menuFont.baseSize/2.0f - 80.0f };

    

    Camera3D camera = { 0 };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f }; 
    camera.fovy = 45.0f;                       
    camera.projection = CAMERA_PERSPECTIVE;  

    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
    SceCtrlData pad;

    SetTargetFPS(60);

    while (running) {
        sceCtrlReadBufferPositive(&pad, 1);


        switch (currentScreen) 
        {
            case ENTRY:
            {
                fpsCount++;
                if (fpsCount > 120) {
                    currentScreen = TITLE;
                }
            } break;
            case TITLE:
            {
                if (pad.Buttons & PSP_CTRL_CROSS) {
                    currentScreen = GAMEPLAY;
                }
            } break;
            case GAMEPLAY:
            {
                if (pad.Buttons & PSP_CTRL_LTRIGGER) playerRotation -= PLAYER_ROTATION_SPEED;
                if (pad.Buttons & PSP_CTRL_RTRIGGER) playerRotation += PLAYER_ROTATION_SPEED;
                
                if (pad.Ly < 100) { 
                    playerPosition.x += sinf(playerRotation * DEG2RAD) * PLAYER_MOVE_SPEED;
                    playerPosition.z += cosf(playerRotation * DEG2RAD) * PLAYER_MOVE_SPEED;
                }
                if (pad.Ly > 150) { 
                    playerPosition.x -= sinf(playerRotation * DEG2RAD) * PLAYER_MOVE_SPEED;
                    playerPosition.z -= cosf(playerRotation * DEG2RAD) * PLAYER_MOVE_SPEED;
                }
                camera.target = (Vector3){ playerPosition.x, playerPosition.y + 1.5f, playerPosition.z };
                camera.position.x = playerPosition.x - (sinf(playerRotation * DEG2RAD) * CAMERA_DISTANCE_BEHIND);
                camera.position.z = playerPosition.z - (cosf(playerRotation * DEG2RAD) * CAMERA_DISTANCE_BEHIND);
                camera.position.y = playerPosition.y + CAMERA_HEIGHT_ABOVE;
            } break;
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
                {
                    DrawText("By Caio Madeira", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 40, LIGHTGRAY);
                } break;
                case TITLE:
                {
                    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GREEN);
                    DrawTextEx(menuFont, menuTitle, fontPosition1, (float)menuFont.baseSize, -3, WHITE);
                    Vector2 posicao = {190, 200};
                    DrawTextEx(menuFont, "PRESS X to JUMP", posicao, 20, 2, WHITE);
                } break;
                case GAMEPLAY:
                {
                    BeginMode3D(camera);
                        DrawModelEx(playerModel, playerPosition, {0.0f, 1.0f, 0.0f}, playerRotation, {1.0f, 1.0f, 1.0f}, WHITE);

                        DrawGrid(20, 1.0f);
                    EndMode3D();

                    DrawText("Analogico: Mover | L/R: Girar", 10, 10, 20, BLACK);
                    DrawFPS(SCREEN_WIDTH - 100, 10);

                } break;
            }

        EndDrawing();
    }

    UnloadFont(menuFont);
    UnloadModel(playerModel);
    CloseWindow();

    sceKernelExitGame();
    return 0;
}


