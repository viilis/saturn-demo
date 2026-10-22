# Jo Engine 3D: quick guide

Jo Engine wraps SEGA's SGL. Most `jo_3d_*` calls are thin wrappers over `sl*`.

## 1. Hooks

| Unity         | Jo Engine                          | Runs                                |
|---------------|------------------------------------|-------------------------------------|
| `Start`       | `jo_main()`                        | once at boot                        |
| `Update`      | `jo_core_add_callback(fn)`         | every frame, in order added         |
| `FixedUpdate` | same as `Update`                   | loop is already fixed-rate          |
| `LateUpdate`  | `jo_core_add_vblank_callback(fn)`  | in VBLANK, after the frame          |
| `OnDisable`   | `jo_core_remove_callback(id)`      | `id` = what `add` returned          |

```c
void jo_main(void)
{
    jo_core_init(JO_COLOR_Black);   // FIRST: sets up memory, video, input
    /* load sprites, build meshes, init camera */
    jo_core_add_callback(update);
    jo_core_add_callback(draw);
    jo_core_run();                  // LAST: never returns
}
```

Frame rate is `60 / JO_FRAMERATE` (default 60). For seconds: call `jo_fixed_point_time()` once per frame, read `delta_time`.

## 2. Numbers

No FPU, so 16.16 fixed point: `1.0 == 65536`.

```c
jo_fixed a = jo_int2fixed(10);
int      b = jo_fixed2int(a);
```

Most `jo_3d_*` take ints and degrees. `_fixed` = fixed, `f` = float, `_rad` = radians.

## 3. World space

Axes: **+X** right, **+Y** down, **+Z** into the screen.

Objects have no position. They're drawn wherever the current matrix is. The matrix resets each frame.

```c
jo_3d_push_matrix();               // save
    jo_3d_translate_matrix(x, y, z);
    jo_3d_rotate_matrix_y(angle);  // degrees
    jo_3d_draw(&quad);
jo_3d_pop_matrix();                // restore
```

- Order matters: translate→rotate = spin in place; rotate→translate = orbit.
- Nest push/pop for parent/child.
- Keep positions in your own variables.

## 4. Camera

```c
jo_camera cam;

jo_3d_camera_init(&cam);                       // at (0,0,-150) looking at (0,0,0)
jo_3d_camera_set_viewpoint(&cam, 0, -50, -250);
jo_3d_camera_set_target(&cam, 0, 0, 0);

void draw(void)
{
    jo_3d_camera_look_at(&cam);    // every frame, before drawing
    /* draw */
}
```

FOV: `jo_3d_perspective_angle(60);`

## 5. Geometry

Everything is quads. Triangle = quad with last two vertices equal.

```c
jo_vertice verts[] = JO_3D_CUBE_VERTICES(32);
jo_3d_quad cube[6];

jo_3d_create_cube(cube, verts);    // once per array: converts verts in place
jo_3d_set_texture(&cube[0], 0);    // sprite id 0
jo_3d_draw_array(cube, 6);         // in draw
```

Custom mesh:

```c
jo_3d_mesh *m = jo_3d_create_mesh(1);                         // 1 quad
jo_3d_set_mesh_vertice(m, jo_int2fixed(-50), jo_int2fixed(-50), 0, 0);  // index 0..3
/* ...indices 1..3 */
jo_3d_set_mesh_polygon_color(m, JO_COLOR_Yellow, 0);
jo_3d_mesh_draw(m);
```

Models: convert with Jo Map Editor (see `Samples/demo - advanced 3D` in upstream [Jo Engine](https://github.com/johannes-fetz/joengine)).

Textures = sprite ids from `jo_sprite_add_tga()` in load order. Also: `jo_3d_set_color`, `jo_3d_set_transparency`, `jo_3d_set_light`.

Sprites: `jo_3d_draw_sprite_at(id, x, y, z)`, `jo_3d_draw_billboard(id, x, y, z)`.

## 6. Input

Read once per frame automatically.

```c
jo_is_pad1_key_pressed(JO_KEY_A);  // held
jo_is_pad1_key_down(JO_KEY_A);     // just pressed
jo_get_input_direction_pressed(0); // UP, UP_LEFT, ... NONE
```

Keys: `UP DOWN LEFT RIGHT A B C X Y Z L R START` (prefix `JO_KEY_`).
Other ports: `jo_is_input_key_pressed(port, key)`.
Analog: `jo_get_input_axis(0, JoAxis1) - 128`.
No key-up in SGL builds; compare with last frame yourself.

## 7. Example

```c
jo_camera cam;
jo_vertice verts[] = JO_3D_CUBE_VERTICES(32);
jo_3d_quad cube[6];
short ry;

void update(void)
{
    if (jo_is_pad1_key_pressed(JO_KEY_LEFT))  ry -= 2;
    if (jo_is_pad1_key_pressed(JO_KEY_RIGHT)) ry += 2;
}

void draw(void)
{
    jo_3d_camera_look_at(&cam);
    jo_3d_push_matrix();
        jo_3d_rotate_matrix_y(ry);
        jo_3d_draw_array(cube, 6);
    jo_3d_pop_matrix();
}

void jo_main(void)
{
    jo_core_init(JO_COLOR_Black);
    jo_3d_camera_init(&cam);
    jo_sprite_add_tga(JO_ROOT_DIR, "BOX.TGA", JO_COLOR_Transparent);
    jo_3d_create_cube(cube, verts);
    for (int i = 0; i < 6; ++i)
        jo_3d_set_texture(&cube[i], 0);
    jo_core_add_callback(update);
    jo_core_add_callback(draw);
    jo_core_run();
}
```
