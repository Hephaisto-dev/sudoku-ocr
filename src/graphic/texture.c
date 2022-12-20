#include "texture.h"


texture_t tex_new_from_img(renderer_t rndr, image_t img) {
    texture_t tex = (texture_t) malloc(sizeof(struct texture_s));
    tex->copied = false;
    tex->readonly = false;
    tex->_img = img;
    tex->width = img->width;
    tex->height = img->height;
    tex->rndr = rndr;
    tex->_tex = SDL_CreateTextureFromSurface(rndr->_rndr, img->_surf);
    return tex;
}


texture_t tex_new(renderer_t rndr, int32_t w, int32_t h, color_t color) {
    texture_t tex = (texture_t) malloc(sizeof(struct texture_s));
    tex->_img = img_new(w, h, color);
    tex->_tex = SDL_CreateTextureFromSurface(rndr->_rndr, tex->_img->_surf);
    tex->copied = true;
    tex->readonly = false;
    tex->width = w;
    tex->height = h;
    tex->rndr = rndr;
    return tex;
}


texture_t tex_new_from_img_cpy(renderer_t rndr, image_t img) {
    image_t copied = img_clone(img);
    texture_t tex = (texture_t) malloc(sizeof(struct texture_s));
    tex->copied = true;
    tex->readonly = false;
    tex->_img = copied;
    tex->width = copied->width;
    tex->height = copied->height;
    tex->rndr = rndr;
    tex->_tex = SDL_CreateTextureFromSurface(rndr->_rndr, copied->_surf);
    return tex;
}



void tex_update(texture_t tex) {
    if (tex->readonly) {
        // TODO : Error
        return;
    }
    img_lock(tex->_img);
    SDL_UpdateTexture(
        tex->_tex,
        NULL,
        tex->_img->data,
        tex->width * sizeof(color_t)
    );
    img_unlock(tex->_img);
}



image_t tex_edit(texture_t tex) {
    if (tex->readonly) {
        // TODO : Error
        return NULL;
    }
    return tex->_img;
}



void tex_clip_image(texture_t tex, image_t img, int32_t x, int32_t y) {
    SDL_Rect dst = {
        .x = x,
        .y = y,
        .w = img->width,
        .h = img->height
    };
    SDL_UpdateTexture(tex->_tex, &dst, img->data, img->width * sizeof(color_t));
}



void tex_free(texture_t tex) {
    SDL_DestroyTexture(tex->_tex);
    if (tex->copied && !tex->readonly) img_free(tex->_img);
    free(tex);
}
