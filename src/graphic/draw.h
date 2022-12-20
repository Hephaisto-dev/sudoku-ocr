#pragma once

#include "common.h"

#include "image.h"


void draw_rect(
    image_t dst,
    int32_t x1, int32_t y1,
    int32_t x2, int32_t y2,
    color_t color
);

void draw_line(
    image_t dst,
    int32_t x1, int32_t y1,
    int32_t x2, int32_t y2,
    int32_t width, color_t color
);

void draw_circle(
    image_t dst,
    int32_t x, int32_t y, int32_t r,
    color_t color
);
