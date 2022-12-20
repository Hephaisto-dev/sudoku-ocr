//
// Created by Mathias FILLIOL on 25/10/2022.
//

#ifndef OCR_GRIDDETECTION_H
#define OCR_GRIDDETECTION_H

#include "working_SDL2/SDL.h"
#include "working_SDL2/SDL_image.h"

#include "utils/vector.h"
#include "graphic/image.h"


typedef struct grid_s {
    union {
        vect2i32_t upleft;
        vect2i32_t upmost;
    };

    union {
        vect2i32_t upright;
        vect2i32_t rightmost;
    };

    union {
        vect2i32_t downleft;
        vect2i32_t downmost;
    };

    union {
        vect2i32_t downright;
        vect2i32_t leftmost;
    };
} grid_t;


typedef struct {
    int32_t x1, y1;
    int32_t x2, y2;
} rect_t;


// Detect the left up corner and right up coordinates of the grid within a given surface
grid_t grid_detect(image_t img);

#include "graphic/renderer.h"
#include "system/window.h"

// Extract and correct the grid from an image
image_t grid_extract(image_t img, renderer_t rndr, window_t win);

image_t grid_extract_v2(image_t src, renderer_t rndr, window_t win);

#endif //OCR_GRIDDETECTION_H
