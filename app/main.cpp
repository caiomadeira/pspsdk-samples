#include "src/common.h"

PSP_MODULE_INFO("test3d", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

typedef enum GameScreen {
    ENTRY = 0,
    TITLE,
    GAMEPLAY
} GameScreen;

// Player configs
Vector3 playerPosition = { 0.0f, 1.0f, 0.0f };
float playerRotation = 0.0f;

// Constantes para o comportamento da câmera e do jogador
const float PLAYER_MOVE_SPEED = 0.05f;
const float PLAYER_ROTATION_SPEED = 2.0f;
const float CAMERA_DISTANCE_BEHIND = 5.0f; // Distância da câmera atrás do jogador
const float CAMERA_HEIGHT_ABOVE = 3.0f;    // Altura da câmera acima do jogador


// Função principal do nosso aplicativo
int main() {
    // Configuração padrão para qualquer homebrew do PSP
    setup_callbacks();
    
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, APP_NAME);

    GameScreen currentScreen = LOGO;

    int fpsCount = 0;

    Model playerModel = LoadModel(PLAYER_MODEL);  
    
    Camera3D camera = { 0 };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f }; // Vetor "para cima"
    camera.fovy = 45.0f;                       // Campo de visão
    camera.projection = CAMERA_PERSPECTIVE;    // Projeção em perspectiva

    // Habilita o controle analógico
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
    SceCtrlData pad;

    SetTargetFPS(60);

    // Loop principal do aplicativo
    while (running) {
        // --- Lógica de Atualização ---
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
                
                if (pad.Ly < 100) { // Analógico para cima
                    playerPosition.x += sinf(playerRotation * DEG2RAD) * PLAYER_MOVE_SPEED;
                    playerPosition.z += cosf(playerRotation * DEG2RAD) * PLAYER_MOVE_SPEED;
                }
                if (pad.Ly > 150) { // Analógico para baixo
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
                    // TODO: Draw LOGO screen here!
                    DrawText("LOGO SCREEN", 20, 20, 40, LIGHTGRAY);
                    DrawText("WAIT for 2 SECONDS...", 290, 220, 20, GRAY);
                } break;
                case TITLE:
                {
                    // TODO: Draw TITLE screen here!
                    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GREEN);
                    DrawText("TITLE SCREEN", 20, 20, 40, DARKGREEN);
                    DrawText("PRESS X to JUMP to GAMEPLAY SCREEN", 120, 220, 20, DARKGREEN);

                } break;
                case GAMEPLAY:
                {
                    BeginMode3D(camera);
                        // Desenha o jogador na sua posição e rotação atuais
                        DrawModelEx(playerModel, playerPosition, {0.0f, 1.0f, 0.0f}, playerRotation, {1.0f, 1.0f, 1.0f}, WHITE);

                        // Desenha um chão para dar referência
                        DrawGrid(20, 1.0f);
                    EndMode3D();

                    // Desenha texto de ajuda na tela
                    DrawText("Analogico: Mover | L/R: Girar", 10, 10, 20, BLACK);
                    DrawFPS(SCREEN_WIDTH - 100, 10);

                } break;
            }

        EndDrawing();
    }

    // Limpeza
    UnloadModel(playerModel);
    CloseWindow();

    sceKernelExitGame();
    return 0;
}


