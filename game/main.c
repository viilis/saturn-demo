#include <jo/jo.h>
#include "src/player/player.h"

jo_camera cam;
jo_vertice cube_vertices[] = JO_3D_CUBE_VERTICES(32);
jo_3d_quad cube_quads[6];
jo_vertice plane_vertices[] = JO_3D_PLANE_VERTICES(104);
jo_3d_quad plane;

// Create player and attach camera to it
player p = {true, {-100, 0, 0}, &cam};

void create_cube(void) {
    jo_3d_create_cube(cube_quads, cube_vertices);

    for (int i = 0; i < 6; ++i) {
        jo_3d_set_texture(&cube_quads[i], 0);
    }
}

void create_plane(void) {
    jo_3d_create_plane(&plane, plane_vertices);
    jo_3d_set_texture(&plane, 1);
}

void draw_floor(void) {
    jo_3d_push_matrix();
    {
        jo_3d_rotate_matrix_x(0);
        jo_3d_draw(&plane);

        jo_3d_rotate_matrix_y(90);
        jo_3d_draw(&plane);
    }
    jo_3d_pop_matrix();
}

void draw_cube(void) {
    static short rx, ry, rz;

    jo_3d_push_matrix();
    {
        jo_3d_rotate_matrix(rx, ry, rz);
        rx += 1;
        ry += 1;
        rz += 1;
        jo_3d_draw_array(cube_quads, 6);
    }
    jo_3d_pop_matrix();
}

void draw(void) {
    jo_3d_camera_look_at(&cam);

    draw_floor();
    draw_cube();

    jo_printf(0, 20, "Polygon count: %d  ", jo_3d_get_polygon_count());
    jo_printf(0, 21, "Sprite usage: %d", jo_sprite_usage_percent());
}

void update(void) {
    player_move(&p);
}

void start(void) {
    jo_3d_camera_init(&cam);
    jo_3d_perspective_angle(80);

    jo_sprite_add_tga(JO_ROOT_DIR, "BOX.TGA", JO_COLOR_Transparent);
    jo_sprite_add_tga(JO_ROOT_DIR, "FLR.TGA", JO_COLOR_Transparent);

    create_cube();
    create_plane();
}

void jo_main(void) {
    // Setup Saturn console
    jo_core_init(JO_COLOR_Gray);

    // Run game logic start once
    start();

    // Update cycle
    jo_core_add_callback(update);

    // Draw cycle
    jo_core_add_callback(draw);

    jo_core_run();
}