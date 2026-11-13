#include <jo/jo.h>
#include <stdlib.h>
#include "obj_to_mesh.h"

jo_vertice read_v_line(const char *line) {
    jo_vertice v;
    char *end;

    v.x = (int)strtol(line, &end, 10);
    line = end;

    v.y = (int)strtol(line, &end, 10);
    line = end;

    v.z = (int)strtol(line, &end, 10);
    line = end;

    return v;
}

void read_vertice_data(const char *file, const int file_length, const int v_count, jo_vertice *v) {
    bool skip_line = false;
    int len = 0;

    for (int i = 0; i < file_length && len < v_count; i++) {
        if (skip_line) {
            if (file[i] == '\n') {
                skip_line = false;
            }
            continue;
        }
        if (file[i] == 'v' && file[i + 1] == ' ') {
            i = i + 2;
            v[len] = read_v_line(&file[i]);
            len++;
        } else {
            skip_line = true;
        }
    }
}

void read_face_data(const char *file, const int file_length, const int f_count, face_data *f) {
    bool skip_line = false;
    int len = 0;

    for (int i = 0; i < file_length && len < f_count; i++) {
        if (skip_line) {
            if (file[i] == '\n') {
                skip_line = false;
            }
            continue;
        }
        if (file[i] == 'f' && file[i + 1] == ' ') {
            i = i + 3; // skip whitespace
            char *end;

            f->v = v[(int)strtol(&file[i], &end, 10)];

            file = end + 1;
            jo_printf(22, 1, "f:%i", (int)strtol(&file[i], &end, 10));
            file = end + 1;
            jo_printf(22, 2, "f:%i", (int)strtol(&file[i], &end, 10));

            len++;
        } else {
            skip_line = true;
        }
    }
}

obj_counts init_face_and_vertices(const char *file, const int file_length) {
    // we will skip all lines that don't start with v or f
    // this allows us to ignore v and f letters on comments, texture names etc.
    bool skip_line = false;
    obj_counts counts = {0};

    // get counts
    for (int i = 0; i < file_length; i++) {
        if (skip_line) {
            if (file[i] == '\n') {
                skip_line = false;
            }
            continue;
        }

        switch (file[i]) {
        case 'v':
            if (file[i + 1] == 't') {
                counts.vt_count++;
                break;
            }
            if (file[i + 1] == 'n') {
                counts.vn_count++;
                break;
            }
            if (file[i + 1] == ' ') {
                counts.v_count++;
                break;
            }
            break;
        case 'f':
            counts.f_count++;
        default:
            skip_line = true;
        }
    }

    return counts;
}

jo_3d_mesh *obj_to_mesh(const char *const filename, const char *subfolder) {
    int file_length;
    jo_3d_mesh *mesh;

    // get stream and file length
    char *file = jo_fs_read_file_in_dir(filename, subfolder, &file_length);

    // count v,vt,vn,f amounts for allocation
    obj_counts counts = init_face_and_vertices(file, file_length);

    jo_vertice v[counts.v_count];
    read_vertice_data(file, file_length, counts.v_count, v);

    face_data f[counts.f_count];
    read_face_data(file, file_length, counts.f_count, f);

    jo_printf(0, 1, "sf:%s,fn:%s", subfolder, filename);
    jo_printf(0, 2, "data_len:%i", file_length);
    jo_printf(0, 3, "v_count:%i", counts.v_count);

    for (int i = 0; i < counts.v_count; i++) {
        jo_printf(0, i + 4, "v_data:%4d,%4d,%4d", v[i].x, v[i].y, v[i].z);
    }

    // face count as quads
    mesh = jo_3d_create_mesh(2);

    for (int i = 0; i < counts.v_count; i++) {
        jo_printf(0, i + 4, "v_data:%4d,%4d,%4d", v[i].x, v[i].y, v[i].z);
        jo_3d_set_mesh_vertice(mesh, jo_int2fixed(v[i].x), jo_int2fixed(v[i].y), jo_int2fixed(v[i].z), i);
    }

    jo_3d_set_mesh_polygon_color(mesh, JO_COLOR_Blue, 0);
    jo_3d_set_mesh_polygon_color(mesh, JO_COLOR_Yellow, 1);

    // exit from dir and free file stream from memory
    jo_fs_cd("..");
    jo_free(file);

    return mesh;
}