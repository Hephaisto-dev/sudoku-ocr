#pragma once

#include "common.h"

#include "graphic/image.h"
#include "image/detection/grid.h"


image_t img_rotate(image_t src, float angle);

void img_rotate_to(image_t src, image_t dst, float angle);

image_t img_cut(image_t src, int32_t x1, int32_t y1, int32_t x2, int32_t y2);

void img_blackandwhite(image_t img);

void img_grayscale(image_t img);

image_t img_resize(image_t src, int32_t new_width, int32_t new_height);

void img_deform_square(image_t srcimg, image_t dstimg, grid_t* origin, rect_t target, color_t background);

image_t img_blur(image_t src, int radius);
