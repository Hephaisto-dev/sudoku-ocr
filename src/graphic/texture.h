#pragma once

#include "common.h"

#include "graphic/color.h"
#include "graphic/image.h"
#include "graphic/renderer.h"

#include "working_SDL2/SDL.h"


typedef struct texture_s {
    int32_t width;
    int32_t height;
    bool readonly;
    bool copied;
    renderer_t rndr;
    SDL_Texture* _tex;
    image_t _img;
}* texture_t;


texture_t tex_new(renderer_t rndr, int32_t w, int32_t h, color_t color);

texture_t tex_new_from_img(renderer_t rndr, image_t img);

texture_t tex_new_from_img_cpy(renderer_t rndr, image_t img);

void tex_update(texture_t tex);

void tex_clip_image(texture_t tex, image_t img, int32_t x, int32_t y);

image_t tex_edit(texture_t tex);

void tex_free(texture_t tex);
