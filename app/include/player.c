#include "player.h"


Player* init_player(void) 
{
    Player* player = (Player*)malloc(sizeof(Player));
    if (player == NULL) return NULL;

    player->x = 0.0f;
    player->y = 0.0f;
    player->hp = 100.0f;
    player->inventory_size = 3;
    player->speed = 0.015f;

    player->inventory = (int*)malloc(player->inventory_size*sizeof(int));
    if (player->inventory == NULL) return NULL;

    for(int i = 0; i < player->inventory_size; i++)
        player->inventory[i] = 0;

    return player;
}