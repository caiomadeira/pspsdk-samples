#ifndef player_h
#define player_h

#include "common.h"

typedef struct player {
    float x;
    float y;
    float hp;
    float speed;
    int inventory_size;
    int* inventory;
} Player;

Player* init_player(void);

#endif