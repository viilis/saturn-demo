#include <jo/jo.h>

#ifndef VEC_UTILS_H
#define VEC_UTILS_H

static __jo_force_inline bool vec_is_zero_int(jo_vector v) { return !(v.x | v.y | v.z); }
static __jo_force_inline bool vec_is_zero_fixed(jo_vector_fixed v) { return !(v.x | v.y | v.z); }

static __jo_force_inline jo_pos3D fixed_pos3D_to_int(jo_pos3D_fixed v) {
    return (jo_pos3D){JO_FIXED_TO_INT(v.x), JO_FIXED_TO_INT(v.y), JO_FIXED_TO_INT(v.z)};
}
static __jo_force_inline jo_vector fixed_vec3_to_int(jo_vector_fixed v) {
    return (jo_vector){JO_FIXED_TO_INT(v.x), JO_FIXED_TO_INT(v.y), JO_FIXED_TO_INT(v.z)};
}

static __jo_force_inline void vec_int_add(const jo_vector *const a, const jo_vector *const b, jo_vector *const result) {
    result->x = a->x + b->x;
    result->y = a->y + b->y;
    result->z = a->z + b->z;
}

#endif