#include <jo/jo.h>
#include "player.h"
#include "../utils/vec_utils.h"

player player_init(jo_camera *const cam) {
    return (player){
        .debug = true,
        .playerPos = {JO_INT_TO_FIXED(0), JO_INT_TO_FIXED(0), JO_INT_TO_FIXED(-100)},
        .playerMov = {.speed = JO_INT_TO_FIXED(1)},
        .playerCam = {
            .cam = cam,
            .sensitivity = JO_INT_TO_FIXED(1),
            .cameraRot = {0},
            .lookDistance = JO_INT_TO_FIXED(100)}};
}

void player_move(player *const p) {
    // Move back or forth
    jo_vector moveVecI = {0};

    // Cam ref
    jo_camera *cam = p->playerCam.cam;

    // Move player back and forth on z-axis
    if (jo_is_pad1_key_pressed(JO_KEY_DOWN)) {
        moveVecI.z--;
    }
    if (jo_is_pad1_key_pressed(JO_KEY_UP)) {
        moveVecI.z++;
    }

    // Left right rotates player around y-axis
    if (jo_is_pad1_key_pressed(JO_KEY_LEFT)) {
        if (p->playerCam.cameraRot.ry > -359) {
            p->playerCam.cameraRot.ry--;
        } else {
            p->playerCam.cameraRot.ry = 0;
        }
    }
    if (jo_is_pad1_key_pressed(JO_KEY_RIGHT)) {
        if (p->playerCam.cameraRot.ry < 359) {
            p->playerCam.cameraRot.ry++;
        } else {
            p->playerCam.cameraRot.ry = 0;
        }
    }

    // Back buttons that control pitch around x-axis
    if (jo_is_pad1_key_pressed(JO_KEY_L)) {
        p->playerCam.cameraRot.rx++;
    }
    if (jo_is_pad1_key_pressed(JO_KEY_R)) {
        p->playerCam.cameraRot.rx--;
    }

    // Limit pitch angle between 89 and -89
    if (p->playerCam.cameraRot.rx > 89) {
        p->playerCam.cameraRot.rx = 89;
    }
    if (p->playerCam.cameraRot.rx < -89) {
        p->playerCam.cameraRot.rx = -89;
    }

    // 1. lets rotate the player
    if (cam != JO_NULL) {
        // Skip jo_engine func to reduce integer jitter
        cam->viewpoint_pos.x = p->playerPos.x;
        cam->viewpoint_pos.y = p->playerPos.y;
        cam->viewpoint_pos.z = p->playerPos.z;

        // forward direction vec
        jo_vector_fixed forwardVecF = {jo_sin(p->playerCam.cameraRot.ry), 0, jo_cos(p->playerCam.cameraRot.ry)};
        jo_vector_fixed scaledForwardF = {0};

        // Scales forward based on lookDistance and writes result down
        jo_vector_fixed_muls(&forwardVecF, p->playerCam.lookDistance, &scaledForwardF);

        jo_vector_fixed targetVecF = {0};
        jo_vector_fixed vpVecF = {p->playerPos.x, p->playerPos.y, p->playerPos.z};
        // Adds fixed viewpoint vec into scaled forward, writes result down
        jo_vector_fixed_add(&vpVecF, &scaledForwardF, &targetVecF);

        cam->target_pos.x = targetVecF.x;
        cam->target_pos.y = targetVecF.y;
        cam->target_pos.z = targetVecF.z;
    }

    // 2. move the player
    if (!vec_is_zero_int(moveVecI)) {
        // Speed scale
        jo_fixed speed = p->playerMov.speed;

        // Apply rotations and movement speed, set players new position
        p->playerPos.z += jo_fixed_mult(jo_cos(p->playerCam.cameraRot.ry), speed) * moveVecI.z;
        p->playerPos.x += jo_fixed_mult(jo_sin(p->playerCam.cameraRot.ry), speed) * moveVecI.z;
    }

    if (p->debug) {
        jo_printf(0, 22, "p_pos: %4d, %4d, %4d", jo_fixed2int(p->playerPos.x), jo_fixed2int(p->playerPos.y), jo_fixed2int(p->playerPos.z));
        jo_printf(0, 23, "p_rot: %4d, %4d, %4d", p->playerCam.cameraRot.rx, p->playerCam.cameraRot.ry, p->playerCam.cameraRot.rz);
    }
}