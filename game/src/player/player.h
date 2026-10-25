#include <jo/jo.h>

#ifndef PLAYER_H
#define PLAYER_H

typedef struct {
    bool debug;
    jo_pos3D playerPos;
    jo_camera *playerCam;
} player;

void player_move(player *const player);

#endif