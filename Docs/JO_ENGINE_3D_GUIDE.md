# Jo Engine 3D: quick guide

Notes from reading `jo_engine/` (mainly `jo/3d.h`, `jo/core.h`, `core.c`, `3d.c`) and the samples
`demo - 3D - programmatically`, `demo - advanced 3D`, `demo - 3D map` and this project's `main.c`.

Jo Engine wraps SEGA's SGL library. Most `jo_3d_*` functions are thin inlines over `sl*` calls.

---

## 1. Engine hooks (the "Unity lifecycle")

Jo Engine has no components or `GameObject`s. Instead you register plain `void f(void)` functions.

| Unity            | Jo Engine                                   | When it runs                                                         |
|------------------|---------------------------------------------|----------------------------------------------------------------------|
| `Awake`/`Start`  | `jo_main()`                                 | Once at boot. You write it, the engine calls it from `main()`.       |
| `Update`         | `jo_core_add_callback(fn)`                  | Every frame, in the order you added them.                            |
| `FixedUpdate`    | *(none, use `jo_core_add_callback`)*        | The game loop is already locked to a fixed rate, see below.          |
| `LateUpdate`-ish | `jo_core_add_vblank_callback(fn)`           | Inside the VBLANK interrupt, after the frame is sent to the screen.  |
| worker thread    | `jo_core_add_slave_callback(fn)`            | On the 2nd CPU, in parallel with your callbacks (needs dual-CPU build flag). |
| custom           | `jo_core_set_restart_game_callback(fn)`     | When pad 1 holds A+B+C+Start.                                        |
| `OnDisable`      | `jo_core_remove_callback(id)` / `jo_core_remove_vblank_callback(id)` | Stops a callback. The `id` is what `add` returned. |

```c
void update(void) { /* input, game logic */ }
void draw(void)   { /* 3D drawing */ }

void jo_main(void)
{
    jo_core_init(JO_COLOR_Black);   // must be FIRST
    /* load sprites, build meshes, set up the camera ... */
    jo_core_add_callback(update);   // runs 1st every frame
    jo_core_add_callback(draw);     // runs 2nd every frame
    jo_core_run();                  // must be LAST, never returns
}
```

### What one frame does (`jo_core_run()` in `core.c`)

```
loop forever:
    slUnitMatrix(0)            // reset the 3D matrix to identity
    storyboards                // if any
    start slave CPU callbacks  // if any
    restart-game check         // A+B+C+Start
    file system background jobs
    YOUR callbacks             // jo_core_add_callback, in order
    wait for slave CPU
    slSynch()                  // wait for VBLANK and render the frame; vblank callbacks run here
```

### Frame rate / delta time

The loop runs at `60 / JO_FRAMERATE` fps (50 on PAL). `JO_FRAMERATE` defaults to `1` (see
`Compiler/COMMON/jo_engine_makefile`); you can set it in the `makefile`, e.g. `JO_FRAMERATE = 2` for 30 fps.
So a plain callback already acts like `FixedUpdate`: moving "+1 per frame" is fine.

If you want real seconds, call `jo_fixed_point_time()` once per frame and read `delta_time`
(16.16 fixed-point seconds):

```c
void update(void)
{
    jo_fixed_point_time();                     // once per frame!
    pos_x += jo_fixed_mult(speed, delta_time); // speed in units per second
}
```

---

## 2. Numbers: integers, fixed point, angles

The Saturn has no FPU, so SGL uses **16.16 fixed point** (`jo_fixed` / `FIXED`): `1.0 == 65536`.

```c
jo_fixed a = jo_int2fixed(10);   // 10.0  (== 10 << 16)
int      b = jo_fixed2int(a);    // 10
jo_fixed c = toFIXED(1.5);       // 1.5 (macro, fine for constants)
```

Most `jo_3d_*` helpers take **plain ints** and convert for you (`jo_3d_translate_matrix(10, 0, 0)`).
Functions ending in `_fixed` take `jo_fixed`, functions ending in `f` take `float`.
Rotation helpers take **degrees** (`jo_3d_rotate_matrix_x(45)`), `_rad` variants take radians.

---

## 3. World space and positioning things

### Axes

Default camera (`jo_3d_camera_init`) sits at `(0, 0, -150)` looking at `(0, 0, 0)`, so:

- **+X** = right
- **+Y** = down (SGL convention, same as 2D screen coordinates)
- **+Z** = into the screen, away from the default camera

### There are no object positions, only a matrix stack

An object has no stored transform. Its position is whatever the **current matrix** is at the moment
you call a draw function. You build that matrix every frame with translate/rotate/scale calls:

```c
jo_3d_push_matrix();               // save current matrix (like entering a child Transform)
{
    jo_3d_translate_matrix(50, 0, 0);  // move 50 units right
    jo_3d_rotate_matrix_y(angle);      // spin in place around its own Y
    jo_3d_set_scale(2, 2, 2);          // twice as big
    jo_3d_draw(&quad);                 // drawn with all of the above
}
jo_3d_pop_matrix();                // restore, so the next object is not affected
```

Rules:

- Every transform **multiplies onto** the current matrix, and acts in the object's **local** space.
- **Order matters.** `translate` then `rotate` = move there, then spin in place.
  `rotate` then `translate` = orbit around the origin.
- Always wrap one object in `push`/`pop`, otherwise the next object inherits its transform.
- Nesting push/pop gives you parent/child hierarchy:

```c
jo_3d_push_matrix();                   // parent: ship
    jo_3d_translate_matrix(ship_x, ship_y, ship_z);
    jo_3d_rotate_matrix_y(ship_yaw);
    jo_3d_mesh_draw(ship);

    jo_3d_push_matrix();               // child: turret, relative to ship
        jo_3d_translate_matrix(0, -20, 0);
        jo_3d_rotate_matrix_y(turret_yaw);
        jo_3d_mesh_draw(turret);
    jo_3d_pop_matrix();
jo_3d_pop_matrix();
```

Store your own positions in variables/structs and re-apply them each frame:

```c
typedef struct { int x, y, z; short ry; } thing;
thing box = { 0, 0, 100, 0 };

void draw_thing(thing *t, jo_3d_quad *quads)
{
    jo_3d_push_matrix();
    jo_3d_translate_matrix(t->x, t->y, t->z);
    jo_3d_rotate_matrix_y(t->ry);
    jo_3d_draw_array(quads, 6);
    jo_3d_pop_matrix();
}
```

Shortcut for sprites: `jo_3d_draw_sprite_at(id, x, y, z)` does push/translate/draw/pop for you.

---

## 4. Camera

```c
typedef struct {
    FIXED viewpoint[XYZ];   // where the camera is
    FIXED target[XYZ];      // what it looks at
    ANGLE z_angle;          // roll
} jo_camera;
```

```c
jo_camera cam;

void jo_main(void)
{
    jo_core_init(JO_COLOR_Black);
    jo_3d_camera_init(&cam);                      // viewpoint (0,0,-150), target (0,0,0), roll 0
    jo_3d_camera_set_viewpoint(&cam, 0, -50, -250); // a bit up and further back
    jo_3d_camera_set_target(&cam, 0, 0, 0);
    jo_3d_camera_set_z_angle(&cam, 0);            // roll in degrees
    ...
}

void draw(void)
{
    jo_3d_camera_look_at(&cam);   // FIRST thing in the draw callback
    /* draw objects after this */
}
```

How it works: `jo_3d_camera_look_at()` calls `slLookAt()`, which multiplies the view transform onto the
current matrix. The engine resets the matrix to identity at the start of each frame, so you must call
`look_at` **every frame, before drawing anything**. Anything drawn before it is not affected by the camera.

Moving the camera = change `viewpoint`/`target` and the next `look_at` uses them:

```c
void update(void)
{
    if (jo_is_pad1_key_pressed(JO_KEY_LEFT))  cam_x -= 2;
    if (jo_is_pad1_key_pressed(JO_KEY_RIGHT)) cam_x += 2;
    jo_3d_camera_set_viewpoint(&cam, cam_x, -50, -250);
    jo_3d_camera_set_target(&cam, cam_x, 0, 0);   // keep looking straight ahead
}
```

Lens settings:

```c
jo_3d_perspective_angle(60);   // field of view in degrees (10..160)
jo_3d_display_level(3);        // near distance level, engine default is 3
```

---

## 5. 3D geometry

Everything is made of **quads** (4-vertex polygons). A triangle is a quad whose last two vertices are equal.
Vertices are `jo_vertice` = `{x, y, z}`, in the object's local space around its own origin.

### 5a. Built-in shapes (`jo_3d_quad`)

```c
jo_vertice cube_vertices[] = JO_3D_CUBE_VERTICES(32);    // 24 verts, half-size 32
jo_3d_quad cube_quads[6];

jo_vertice plane_vertices[] = JO_3D_PLANE_VERTICES(64);  // 4 verts in the XY plane
jo_3d_quad plane;

void setup(void)
{
    jo_3d_create_cube(cube_quads, cube_vertices);
    jo_3d_create_plane(&plane, plane_vertices);
    jo_3d_set_texture(&cube_quads[0], 0);   // sprite id 0 on one face
    jo_3d_set_color(&plane, JO_COLOR_Red);  // or a flat color
}

void draw(void)
{
    jo_3d_draw_array(cube_quads, 6);
    jo_3d_draw(&plane);
}
```

Gotcha: `jo_3d_create_*` converts the vertex array **in place** from ints to fixed point (`<< 16`).
Call it **once** per array, and don't share one vertex array between two create calls.

### 5b. Custom meshes (`jo_3d_mesh`)

```c
jo_3d_mesh *mesh = jo_3d_create_mesh(2);   // 2 quads = 8 vertices

// vertices here are jo_fixed, index = quad * 4 + corner
jo_3d_set_mesh_vertice(mesh, jo_int2fixed(-50), jo_int2fixed(-50), 0, 0);
jo_3d_set_mesh_vertice(mesh, jo_int2fixed( 50), jo_int2fixed(-50), 0, 1);
jo_3d_set_mesh_vertice(mesh, jo_int2fixed( 50), jo_int2fixed( 50), 0, 2);
jo_3d_set_mesh_vertice(mesh, jo_int2fixed(-50), jo_int2fixed( 50), 0, 3);
/* ... indices 4..7 for the second quad ... */

jo_3d_set_mesh_polygon_color(mesh, JO_COLOR_Yellow, 0);  // per quad
jo_3d_set_mesh_polygon_color(mesh, JO_COLOR_Cyan, 1);

// in draw:
jo_3d_mesh_draw(mesh);

// when done:
jo_3d_free_mesh(mesh);
```

For real models, convert `.obj`/Collada files with **Jo Map Editor** into a header (see
`Samples/demo - advanced 3D/model.h`) and draw with `jo_3d_mesh_draw(&MeshName)`.

### 5c. Textures, colors and effects

Textures are just sprite ids from `jo_sprite_add_tga()`, numbered in load order:

```c
jo_sprite_add_tga(JO_ROOT_DIR, "BOX.TGA", JO_COLOR_Transparent);  // id 0
jo_sprite_add_tga(JO_ROOT_DIR, "FLR.TGA", JO_COLOR_Transparent);  // id 1
```

| Quad                                   | Mesh (whole / one polygon)                                         |
|----------------------------------------|--------------------------------------------------------------------|
| `jo_3d_set_texture(&q, id)`            | `jo_3d_set_mesh_texture(m, id)` / `jo_3d_set_mesh_polygon_texture(m, id, i)` |
| `jo_3d_set_color(&q, color)`           | `jo_3d_set_mesh_color(m, c)` / `jo_3d_set_mesh_polygon_color(m, c, i)` |
| `jo_3d_set_transparency(&q, true)`     | `jo_3d_set_mesh_transparency(m, true)`                             |
| `jo_3d_set_screen_doors(&q, true)`     | `jo_3d_set_mesh_screen_doors(m, true)` (dithered transparency)     |
| `jo_3d_set_light(&q, true)`            | `jo_3d_set_mesh_light(m, true)` + `jo_3d_light(x, y, z)` for direction |

### 5d. Sprites and billboards in 3D

```c
jo_3d_draw_sprite_at(2, 0, 10, -40);     // flat sprite quad placed in 3D (rotates with the matrix)
jo_3d_draw_billboard(2, 0, 10, -40);     // always faces the camera
jo_3d_draw_scaled_billboard(2, 0, 10, -40, 2);
```

### 5e. Stats

Call after drawing:

```c
jo_printf(0, 20, "polys: %d shown: %d  ",
          jo_3d_get_polygon_count(), jo_3d_get_displayed_polygon_count());
```

---

## 6. Putting it together (this project's `main.c`, trimmed)

```c
jo_camera cam;
jo_vertice cube_vertices[] = JO_3D_CUBE_VERTICES(32);
jo_3d_quad cube_quads[6];

void my_draw(void)
{
    static short rx, ry, rz;

    jo_3d_camera_look_at(&cam);        // 1. camera first

    jo_3d_push_matrix();               // 2. one object
    jo_3d_rotate_matrix(rx, ry, rz);
    jo_3d_draw_array(cube_quads, 6);
    jo_3d_pop_matrix();

    rx += 3; ry += 2; rz += 1;         // 3. animate (frame-locked)
}

void jo_main(void)
{
    jo_core_init(JO_COLOR_Black);
    jo_3d_camera_init(&cam);
    jo_sprite_add_tga(JO_ROOT_DIR, "BOX.TGA", JO_COLOR_Transparent);
    jo_3d_create_cube(cube_quads, cube_vertices);
    for (int i = 0; i < 6; ++i)
        jo_3d_set_texture(&cube_quads[i], 0);
    jo_core_add_callback(my_draw);
    jo_core_run();
}
```

To enable 3D, the `makefile` needs `JO_COMPILE_WITH_3D_MODULE = 1` (already set here).
