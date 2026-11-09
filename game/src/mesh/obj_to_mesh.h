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

jo_3d_mesh *obj_to_mesh(const char *const filename, const char *subfolder);

#endif