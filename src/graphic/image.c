#include "image.h"

#include <stdlib.h>


#ifdef IMG_USE_FREEIMAGE

#define FREEIMAGE_LIB
#include <FreeImage.h>


void img_init() {
    FreeImage_Initialise(false);
}

void img_destroy() {
    FreeImage_DeInitialise();
}


void img_lock(image_t img) {}
void img_unlock(image_t img) {}


image_t img_load_from_file(const char* filename) {
    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	// Check the file signature and deduce its format
	fif = FreeImage_GetFileType(filename, 0);
	if(fif == FIF_UNKNOWN) {
		// Try to guess the file format from the file extension
		fif = FreeImage_GetFIFFromFilename(filename);
	}
	// Check that the plugin has reading capabilities ...
	if((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif)) {
		// Ok then load the file
		FIBITMAP *dib = FreeImage_Load(fif, filename, 0);

        // Convert to RGBA

        // Convert to 32 bit
        FIBITMAP *src = FreeImage_ConvertTo32Bits(dib);
        FreeImage_Unload(dib);

        // Allocate a raw buffer
        int width = FreeImage_GetWidth(src);
        int height = FreeImage_GetHeight(src);
        color_t *data = (color_t*) malloc(width * height * sizeof(color_t));

        // Convert the bitmap to raw bits (top-left pixel first)
        FreeImage_ConvertToRawBits((BYTE*) data, src, width * sizeof(color_t), 32,
                IMG_RED_MASK, IMG_GREEN_MASK, IMG_BLUE_MASK, TRUE);
        FreeImage_Unload(src);

        image_t img = (image_t) malloc(sizeof(struct image_s));
        img->data = data;
        img->width = width;
        img->height = height;

		return img;
	}
	return NULL;
}


void img_free(image_t img) {
    free(img->data);
    free(img);
}

#else

#include "working_SDL2/SDL.h"
#include "working_SDL2/SDL_image.h"


void img_init() {}
void img_destroy() {}


void img_lock(image_t img) {
    if (img->_surf && SDL_MUSTLOCK(img->_surf))
        SDL_LockSurface(img->_surf);
    img->data = (color_t*) img->_surf->pixels;
}

void img_unlock(image_t img) {
    if (img->_surf && SDL_MUSTLOCK(img->_surf))
        SDL_UnlockSurface(img->_surf);
}


image_t img_new(int32_t width, int32_t height, color_t color) {
    SDL_Surface* _surf = SDL_CreateRGBSurfaceWithFormat(
        0, // Flags (unused and should bbe set to 0)
        width, height,
        32, // Color depth in bits
        SDL_PIXELFORMAT_RGBA32
    );

    image_t img = (image_t) malloc(sizeof(struct image_s));
    img->data = (color_t*) _surf->pixels;
    img->width = _surf->w;
    img->height = _surf->h;
    img->_surf = _surf;

    img_lock(img);
    color_t* pixels = img->data;
    for (size_t i = img->width * img->height; i--;)
        pixels[i] = color;
    img_unlock(img);

    return img;
}


image_t img_clone(image_t src) {
    SDL_Surface* _surf = SDL_CreateRGBSurfaceWithFormat(
        0, // Flags (unused and should bbe set to 0)
        src->width, src->height,
        32, // Color depth in bits
        SDL_PIXELFORMAT_RGBA32
    );

    image_t img = (image_t) malloc(sizeof(struct image_s));
    img->data = (color_t*) _surf->pixels;
    img->width = _surf->w;
    img->height = _surf->h;
    img->_surf = _surf;

    img_lock(img);
    img_lock(src);

    color_t* srcdata = src->data;
    color_t* dstdata = img->data;
    for (size_t i = img->width * img->height; i--;)
        dstdata[i] = srcdata[i];

    img_unlock(src);
    img_unlock(img);

    return img;
}


void img_clip(image_t src, image_t dst, int32_t dx, int32_t dy) {
    int32_t x_start = max_i32(0, -dx);
    int32_t y_start = max_i32(0, -dy);
    int32_t x_end = min_i32(dst->width, dx + src->width) - dx;
    int32_t y_end = min_i32(dst->height, dy + src->height) - dy;
    for (int32_t y = y_start; y < y_end; y++) {
        for (int32_t x = x_start; x < x_end; x++)
            img_set_pixel(dst, x + dx, y + dy, img_get_pixel(src, x, y));
    }
}


image_t img_load_from_file(const char* filename) {
    SDL_Surface *src = IMG_Load(filename);
    if (src == NULL) return NULL;
    SDL_Surface *converted = SDL_ConvertSurfaceFormat(src, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(src);

    image_t img = (image_t) malloc(sizeof(struct image_s));
    img->data = (color_t*) converted->pixels;
    img->width = converted->w;
    img->height = converted->h;
    img->_surf = converted;

    return img;
}


void img_save_to_file(image_t img, const char* filename) {
    // TODO : Check if img->_surf is NULL
    IMG_SavePNG(img->_surf, filename);
}


void img_free(image_t img) {
    if (img->_surf)
        SDL_FreeSurface(img->_surf);
    else
        free(img->data);
    free(img);
}

#endif
