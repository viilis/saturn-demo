#include <jo/jo.h>
#include "player.h"

/**
 * @brief read inputs from pad1 and moves player accordingly.
 * Also
 * @param player
 */
void player_move(player *const player) {
    jo_vector moveVec = {0, 0, 0}; // values as int

    if (jo_is_pad1_key_pressed(JO_KEY_LEFT))
        moveVec.z++;
    if (jo_is_pad1_key_pressed(JO_KEY_RIGHT))
        moveVec.z--;
    if (jo_is_pad1_key_pressed(JO_KEY_DOWN))
        moveVec.x--;
    if (jo_is_pad1_key_pressed(JO_KEY_UP))
        moveVec.x++;

    player->playerPos.x += moveVec.x;
    player->playerPos.z += moveVec.z;

    if (player->playerCam != JO_NULL) {
        jo_3d_camera_set_viewpoint(
            player->playerCam,
            player->playerPos.x, player->playerPos.y, player->playerPos.z);

        // returns fixed point, not int
        jo_pos3D oldTargetPos = player->playerCam->target_pos;
        oldTargetPos.x += JO_INT_TO_FIXED(moveVec.x);
        oldTargetPos.z += JO_INT_TO_FIXED(moveVec.z);

        if (player->debug)
            jo_printf(0, 23, "cam_target: %4d, %4d, %4d", jo_fixed2int(oldTargetPos.x), jo_fixed2int(oldTargetPos.y), jo_fixed2int(oldTargetPos.z));

        // takes ints
        jo_3d_camera_set_target(
            player->playerCam,
            jo_fixed2int(oldTargetPos.x), jo_fixed2int(oldTargetPos.y), jo_fixed2int(oldTargetPos.z));
    }

    if (player->debug) {
        jo_printf(0, 22, "p_pos: %4d, %4d, %4d", player->playerPos.x, player->playerPos.y, player->playerPos.z);
    }
}

void player_rotate(player *const player) {
}
