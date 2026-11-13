#include <jo/jo.h>

#ifndef OBJ_TO_MESH_H
#define OBJ_TO_MESH_H

typedef struct
{
    int v_count;
    int vt_count;
    int vn_count;
    int f_count;
} obj_counts;

/* vertex texture aka UV */
typedef struct
{
    int u;
    int v;
} vt_data;

/* vertex normal coordinates */
typedef struct
{
    int i;
    int j;
    int k;
    /* data */
} vn_data;

/*format: v / vt / vn */
typedef struct
{
    jo_vertice v;
    vt_data vt;
    vn_data vn;
} face_data;

jo_3d_mesh *obj_to_mesh(const char *const filename, const char *subfolder);

#endif