#pragma once

#include "common.h"

#include "graphic/color.h"


/**
 * @file image.h
 * @brief This file provide stuff to load and manipulate images.
 */


#ifndef IMG_USE_FREEIMAGE
    #include "working_SDL2/SDL.h"
#endif


#define IMG_RED_MASK   0xFF000000
#define IMG_GREEN_MASK 0x00FF0000
#define IMG_BLUE_MASK  0x0000FF00
#define IMG_ALPHA_MASK 0x000000FF


// Allwas in RGBA format
// 8 highest bit for the alpha channel
// and the 8 lower bit for the blue channel
#ifndef IMG_USE_FREEIMAGE

typedef struct image_s {
    int32_t width;
    int32_t height;
    color_t* data;
    SDL_Surface* _surf;
} *image_t;

#else

typedef struct image_s {
    int16_t width;
    int16_t height;
    color_t* data;
} *image_t;

#endif


/**
 * @brief Init the image loading module
 *
 */
void img_init();


/**
 * @brief Uninit the image loading module
 *
 */
void img_destroy();


/**
 * @brief Lock an image for editing or reading (as with the SDL)
 *
 * @param img
 */
void img_lock(image_t img);


/**
 * @brief Unlock image when finished editing or reading (as with the SDL)
 *
 * @param img
 */
void img_unlock(image_t img);


/**
 * @brief Create a new image of specific color and size
 *
 * @param width The new image width
 * @param height The new image height
 * @param color The color to fill the new image with
 * @return image_t The new image
 */
image_t img_new(int32_t width, int32_t height, color_t color);


/**
 * @brief Clone an existing image
 *
 * @param img The image to clone
 * @return image_t The cloned image
 */
image_t img_clone(image_t img);


/**
 * @brief Clip an image on another image
 *
 * @param src The image to clip
 * @param dst The image where to clip
 * @param x The X coordinate of the clip destination
 * @param y The Y coordinate of the clip destination
 */
void img_clip(image_t src, image_t dst, int32_t x, int32_t y);


/**
 * @brief Load an image from a file
 *
 * @param filename The path to the image file to load
 * @return image_t The loaded image or \b NULL if an error occured
 */
image_t img_load_from_file(const char* filename);


/**
 * @brief Save an image to a file
 *
 * @param img The image to save
 * @param filename The filename (where to save the image)
 */
void img_save_to_file(image_t img, const char* filename);


/**
 * @brief Free an image from memory
 *
 * @param img The image to free
 */
void img_free(image_t img);


/**
 * @brief Helper function to set a pixel value (fast inline function)
 *
 * @param img The image where to set the pixel color
 * @param x X
 * @param y Y
 * @param color The pixel's color to set
 */
static inline void img_set_pixel(image_t img, int32_t x, int32_t y, color_t color) {
    img->data[y * img->width + x] = color;
}


/**
 * @brief Helper function to get a pixel value (fast inline function)
 *
 * @param img The image to get the pixel from
 * @param x X
 * @param y Y
 * @return color_t The pixel's color
 */
static inline color_t img_get_pixel(image_t img, int32_t x, int32_t y) {
    return img->data[y * img->width + x];
}
