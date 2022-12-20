#include "draw.h"


void draw_rect(image_t dst, int32_t x1, int32_t y1, int32_t x2, int32_t y2, color_t color) {
    int32_t j_start = max_i32(0, y1);
    int32_t j_end = min_i32(y2+1, dst->height);
    img_lock(dst);
    for (int32_t i = max_i32(0, x1), i_end = min_i32(x2+1, dst->width); i < i_end; i++) {
        for (int32_t j = j_start; j < j_end; j++)
            img_set_pixel(dst, i, j, color);
    }
    img_unlock(dst);
}


void draw_line(image_t dst, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t width, color_t color) {
    // Suppress unused warning
    (void) width;

    img_lock(dst);
    // FIXME : Some weird behavor some time

    int32_t dx = abs_i32(x2 - x1);
    int32_t dy = abs_i32(y2 - y1);

    if (dx > 0 || dy > 0) {
        if (dx > dy) {
            if (x1 > x2) {
                int32_t t = x1;
                x1 = x2;
                x2 = t;
                t = y1;
                y1 = y2;
                y2 = t;
            }
            float k = ((float) (y2 - y1)) / ((float) dx);
            x1 = max_i32(x1, 0);
            x2 = min_i32(x2, dst->width - 1);
            for (int32_t x = x1; x <= x2; x++) {
                int32_t y = (int32_t) (((float)x) * k) + y1;
                if (y >= 0 && y < dst->height)
                    img_set_pixel(dst, x, y, color);
            }
        } else {
            if (y1 > y2) {
                int32_t t = y1;
                y1 = y2;
                y2 = t;
                t = x1;
                x1 = x2;
                x2 = t;
            }
            float k = ((float) (x2 - x1)) / ((float) dy);
            y1 = max_i32(y1, 0);
            y2 = min_i32(y2, dst->height - 1);
            for (int32_t y = y1; y <= y2; y++) {
                int32_t x = (int32_t) (((float) y) * k) + x1;
                if (x >= 0 && x < dst->width)
                    img_set_pixel(dst, x, y, color);
            }
        }
    }

    img_unlock(dst);
}


void draw_circle(image_t dst, int32_t x, int32_t y, int32_t r, color_t color) {
    int32_t sr = r * r;
    int32_t j_start = max_i32(0, y - r);
    int32_t j_end = min_i32(y + r + 1, dst->height);
    img_lock(dst);
    for (int32_t i = max_i32(0, x - r), i_end = min_i32(x + r + 1, dst->width); i < i_end; i++) {
        for (int32_t j = j_start; j < j_end; j++) {
            int32_t dx = x - i;
            int32_t dy = y - j;
            if ((dx*dx) + (dy*dy) < sr)
                img_set_pixel(dst, i, j, color);
        }
    }
    img_unlock(dst);
}
