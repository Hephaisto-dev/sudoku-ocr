#include "renderer.h"
#include "graphic/texture.h"
#include "graphic/draw.h"



void rndr_clip(renderer_t rndr, texture_t tex, int32_t x, int32_t y) {
    SDL_Rect dst = {
        .x = x,
        .y = y,
        .w = tex->width,
        .h = tex->height
    };
    SDL_RenderCopy(rndr->_rndr, tex->_tex, NULL, &dst);
}



void rndr_clip_rect(
    renderer_t rndr, struct texture_s* tex,
    int32_t srcx, int32_t srcy,
    int32_t w, int32_t h,
    int32_t dstx, int32_t dsty
) {
    SDL_Rect src = {
        .x = srcx,
        .y = srcy,
        .w = w,
        .h = h
    };
    SDL_Rect dst = {
        .x = dstx,
        .y = dsty,
        .w = w,
        .h = h
    };
    SDL_RenderCopy(rndr->_rndr, tex->_tex, &src, &dst);
}



void rndr_set_mul_color(renderer_t rndr, texture_t tex, color_t color) {
    (void) rndr;
    SDL_SetTextureColorMod(tex->_tex, color.r, color.g, color.b);
}



void rndr_clear(renderer_t rndr, color_t color) {
    SDL_SetRenderDrawColor(rndr->_rndr, color.r, color.g, color.b, color.a);
    SDL_RenderClear(rndr->_rndr);
}



vect2i32_t rndr_get_size(renderer_t rndr) {
    vect2i32_t r;
    SDL_GetRendererOutputSize(rndr->_rndr, &r.x, &r.y);
    return r;
}



void rndr_draw_rect(
    renderer_t rndr,
    int32_t x1, int32_t y1,
    int32_t x2, int32_t y2,
    color_t color
) {
    SDL_Rect rect = {
        .x = x1,
        .y = y1,
        .w = x2 - x1,
        .h = y2 - y1
    };
    SDL_SetRenderDrawColor(rndr->_rndr, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(rndr->_rndr, &rect);
}



void rndr_draw_line(
    renderer_t rndr,
    int32_t x1, int32_t y1,
    int32_t x2, int32_t y2,
    int32_t width, color_t color
) {
    SDL_SetRenderDrawColor(rndr->_rndr, color.r, color.g, color.b, color.a);
    SDL_RenderDrawLine(rndr->_rndr, x1, y1, x2, y2);
}



void rndr_draw_circle(
    renderer_t rndr,
    int32_t x, int32_t y, int32_t r,
    color_t color
) {
    image_t img = img_new(r*2+2, r*2+2, RGBA_TO_COLOR(0,0,0,0));
    draw_circle(img, r, r, r, color);
    texture_t tex = tex_new_from_img(rndr, img);
    rndr_clip(rndr, tex, x - r - 1, y - r - 1);
    tex_free(tex);
    img_free(img);
}
