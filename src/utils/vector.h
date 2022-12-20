#pragma once

#include "common.h"

#include <math.h>


typedef struct vect2i32_s {
    int32_t x;
    int32_t y;
} vect2i32_t;


typedef struct vect2f_s {
    float x;
    float y;
} vect2f_t;


static inline vect2f_t vect_normalize_2f(vect2f_t v) {
    float len = sqrt(v.x * v.x + v.y * v.y);
    v.x /= len;
    v.y /= len;
    return v;
}

static inline vect2i32_t vect_2f_to_2i32(vect2f_t v) {
    vect2i32_t r = {
        .x = (int) v.x,
        .y = (int) v.y,
    };
    return r;
}
