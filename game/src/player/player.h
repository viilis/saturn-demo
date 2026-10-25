#include <jo/jo.h>

#ifndef PLAYER_H
#define PLAYER_H

/**
 * @brief player camera settings and states.
 */
typedef struct
{
    jo_camera *cam;
    jo_fixed sensitivity;
    jo_rot3D cameraRot; // x-axis for pitch, y-axis for yaw
    jo_fixed lookDistance;
} playerCamera;

typedef struct
{
    jo_fixed speed;
} playerMovement;

/**
 * @brief player state
 */
typedef struct {
    bool debug;
    jo_pos3D_fixed playerPos;
    playerMovement playerMov;
    playerCamera playerCam;
} player;

/**
 * @brief read inputs from pad1 and moves player accordingly.
 * @param player
 */
void player_move(player *const player);

/**
 * @brief init player struct with given camera.
 *
 * @param cam
 */
player player_init(jo_camera *const cam);

#endif