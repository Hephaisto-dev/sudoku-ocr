#pragma once

#include "common.h"

#include "graphic/color.h"
#include "utils/vector.h"

#include "working_SDL2/SDL.h"



#if !defined(USE_SOFTWARE_RENDERER) && !defined(USE_HARDWARE_RENDERER)
    #define USE_HARDWARE_RENDERER
#endif


// Don't include texture.h because it include itself this file
struct texture_s;


typedef struct renderer_s {
    #ifdef USE_HARDWARE_RENDERER
        SDL_Renderer* _rndr;
    #else
        image_t _img;
    #endif
}* renderer_t;


// Various

void rndr_clip(renderer_t rndr, struct texture_s* tex, int32_t x, int32_t y);

void rndr_clip_rect(
    renderer_t rndr, struct texture_s* tex,
    int32_t srcx, int32_t srcy,
    int32_t w, int32_t h,
    int32_t dstx, int32_t dsty
);

void rndr_set_mul_color(renderer_t rndr, struct texture_s* tex, color_t color);

void rndr_clear(renderer_t rndr, color_t color);


// Getters

vect2i32_t rndr_get_size(renderer_t rndr);


// Draw functions

void rndr_draw_rect(
    renderer_t rndr,
    int32_t x1, int32_t y1,
    int32_t x2, int32_t y2,
    color_t color
);

void rndr_draw_line(
    renderer_t rndr,
    int32_t x1, int32_t y1,
    int32_t x2, int32_t y2,
    int32_t width, color_t color
);

void rndr_draw_circle(
    renderer_t rndr,
    int32_t x, int32_t y, int32_t r,
    color_t color
);
