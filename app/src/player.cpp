#include "player.h"

const float PLAYER_MOVE_SPEED = 0.05f;
const float PLAYER_ROTATION_SPEED = 2.0f;

void InitPlayer(Player* player, const char* modelPath) {
    player->position = (Vector3){ 0.0f, 1.0f, 0.0f };
    player->rotation = 0.0f;
    player->model = LoadModel(modelPath); // loads player model
}

void UpdatePlayer(Player* player, SceCtrlData pad) {
    if (pad.Buttons & PSP_CTRL_LTRIGGER) { 
        player->rotation -= PLAYER_ROTATION_SPEED;
    }
    if (pad.Buttons & PSP_CTRL_RTRIGGER) { 
        player->rotation += PLAYER_ROTATION_SPEED;
    }
    if (pad.Ly < 100) { 
        player->position.x += sinf(player->rotation * DEG2RAD) * PLAYER_MOVE_SPEED;
        player->position.z += cosf(player->rotation * DEG2RAD) * PLAYER_MOVE_SPEED;
    }
    if (pad.Ly > 150) { 
        player->position.x -= sinf(player->rotation * DEG2RAD) * PLAYER_MOVE_SPEED;
        player->position.z -= cosf(player->rotation * DEG2RAD) * PLAYER_MOVE_SPEED;
    }
}
void DrawPlayer(Player* player) {
    DrawModelEx(player->model, player->position, {0.0f, 1.0f, 0.0f}, player->rotation, {1.0f, 1.0f, 1.0f}, WHITE);
}
void UnloadPlayer(Player* player) {
    UnloadModel(player->model);
}