#ifndef PLAYER_H
#define PLAYER_H

#include "common.h"

typedef struct Player {
    Vector3 position;
    float rotation;
    Model model;
} Player;


void InitPlayer(Player* player, const char* modelPath);
void UpdatePlayer(Player* player, SceCtrlData pad);
void DrawPlayer(Player* player);
void UnloadPlayer(Player* player);

#endif