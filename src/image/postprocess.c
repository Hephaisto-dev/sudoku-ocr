#include "postprocess.h"

#include <math.h>

#include "utils/vector.h"



uint8_t _img_get_grayscale(color_t c) {
    return (uint8_t) (0.3*c.r + 0.59*c.g + 0.11*c.b);
}


static inline color_t _color_to_grayscale(color_t color)
{
    uint8_t average = _img_get_grayscale(color);
    color.r = average;
    color.g = average;
    color.b = average;
    return color;
}


void img_grayscale(image_t img)
{
    color_t* pixels = img->data;
    int len = img->width * img->height;
    img_lock(img);
    for (int i = 0; i < len; i++){
        pixels[i] = _color_to_grayscale(pixels[i]);
    }
    img_unlock(img);
}


static inline color_t _color_to_blackandwhite(color_t color, uint8_t threshold)
{
    uint8_t average = _img_get_grayscale(color);
    return (average < threshold) ?
        RGB_TO_COLOR(0, 0, 0) :
        RGB_TO_COLOR(0xFF, 0xFF, 0xFF);
}


uint8_t _img_guess_bw_threshold(image_t img) {
    // Generate a histograme

    int32_t histograme[256] = {0};
    color_t* pixels = img->data;
    for (size_t i = img->width * img->height; i--;){
        uint8_t v = _img_get_grayscale(pixels[i]);
        histograme[v]++;
    }

    // Split this histograme in 2 on a specific position called the cursor and
    // move this cursor from left to right until the sum of all values of
    // the histograme on the left part are the nearest of the sum of values of
    // the right part.

    int32_t rightsum = 0;
    int32_t leftsum  = 0;

    // Initialize sum
    for (size_t i = 1; i < 256; i++)
        rightsum += histograme[i];

    // Move cursor
    // (the cursor position is the number of histograme values on left)

    float best_diff1 = +INFINITY;
    size_t best_cursor1 = 0;

    for (size_t cursor = 1; cursor < 255; cursor++) {
        // Compute the new sums
        leftsum  += histograme[cursor - 1];
        rightsum -= histograme[cursor];

        float leftavg = 0;
        for (size_t i = 0; i < cursor; i++)
            leftavg += i * histograme[i];
        leftavg /= leftsum;

        float rightavg = 0;
        for (size_t i = cursor + 1; i < 256; i++)
            rightavg += i * histograme[i];
        rightavg /= rightsum;

        // Check if the difference is minimal
        float diff1 = abs_f(abs_f(leftavg - cursor) - abs_f(rightavg - cursor));
        if (diff1 < best_diff1) {
            best_cursor1 = cursor;
            best_diff1 = diff1;
        }
    }

    return (uint8_t) best_cursor1;
}


void img_blackandwhite(image_t img)
{
    uint8_t threshold = _img_guess_bw_threshold(img);
    //mprintf("Use threshold : %i\n", (int) threshold);
    color_t* pixels = img->data;
    int len = img->width * img->height;
    img_lock(img);
    for (int i = 0; i < len; i++){
        pixels[i] = _color_to_blackandwhite(pixels[i], threshold);
    }
    img_unlock(img);
}



image_t img_cut(image_t src, int32_t x1, int32_t y1, int32_t x2, int32_t y2) {
    int32_t dst_w = max_i32(0, x2 - x1);
    int32_t dst_h = max_i32(0, y2 - y1);

    int32_t src_w = src->width;

    image_t dst = img_new(dst_w, dst_h, RGB_TO_COLOR(0,0,0));

    img_lock(src);
    img_lock(dst);

    color_t* dst_data = dst->data;
    color_t* src_data = src->data + (src_w * y1);

    for (int32_t y = 0; y < dst_h; y++) {
        for (int32_t x = 0; x < dst_w; x++) {
            dst_data[x] = src_data[x + x1];
        }
        dst_data += dst_w;
        src_data += src_w;
    }

    img_unlock(src);
    img_unlock(dst);

    return dst;
}


void img_rotate_to(image_t src, image_t dst, float angle) {
    color_t* src_pixels = src->data;
    color_t* dst_pixels = dst->data;

    const int src_w = src->width;
    const int src_h = src->height;
    const int dst_w = dst->width;
    const int dst_h = dst->height;

    const int src_w_2 = src_w / 2;
    const int src_h_2 = src_h / 2;
    const int dst_w_2 = dst_w / 2;
    const int dst_h_2 = dst_h / 2;

    float radians = angle * PI / 180.0;
    float cos_angle = cos(radians);
    float sin_angle = sin(radians);

    img_lock(src);
    img_lock(dst);

    for (int i = 0; i < dst_w; i++) {
        for (int j = 0; j < dst_h; j++) {
            int x = (i - dst_w_2) * cos_angle -
                    (j - dst_h_2) * sin_angle + src_w_2;
            int y = (i - dst_w_2) * sin_angle +
                    (j - dst_h_2) * cos_angle + src_h_2;
            if (x >= 0 && x < src_w && y >= 0 && y < src_h) {
                dst_pixels[j * dst_w + i] = src_pixels[y * src_w + x];
            }
        }
    }

    img_unlock(src);
    img_unlock(dst);
}


image_t img_rotate(image_t src, float angle) {
    image_t dst = img_new(src->width, src->height, RGB_TO_COLOR(0, 0, 0));

    color_t* src_pixels = src->data;
    color_t* dst_pixels = dst->data;

    int src_w = src->width;
    int src_h = src->height;
    int dst_w = dst->width;
    int dst_h = dst->height;

    float radians = angle * PI / 180.0;
    float cos_angle = cos(radians);
    float sin_angle = sin(radians);

    img_lock(src);
    img_lock(dst);

    for (int i = 0; i < dst_w; i++) {
        for (int j = 0; j < dst_h; j++) {
            int x = (i - dst_w / 2) * cos_angle -
                    (j - dst_h / 2) * sin_angle + src_w / 2;
            int y = (i - dst_w / 2) * sin_angle +
                    (j - dst_h / 2) * cos_angle + src_h / 2;
            if (x >= 0 && x < src_w && y >= 0 && y < src_h) {
                dst_pixels[j * dst_w + i] = src_pixels[y * src_w + x];
            }
        }
    }

    img_unlock(src);
    img_unlock(dst);

    return dst;
}



image_t img_resize(image_t src, int32_t new_width, int32_t new_height) {
    int32_t width = src->width;
    int32_t height = src->height;

    float x_scale = (float) width / (float) new_width;
    float y_scale = (float) height / (float) new_height;

    image_t dst = img_new(new_width, new_height, HEX_TO_COLOR(0));

    color_t* srcdata = src->data;
    color_t* dstdata = dst->data;

    img_lock(src);
    img_lock(dst);

    for (int32_t y = 0; y < new_height; y++) {
        for (int32_t x = 0; x < new_width; x++) {
            int32_t srcx = (int32_t) (x * x_scale);
            int32_t srcy = (int32_t) (y * y_scale);
            if (srcx < 0 || srcx >= width || srcy < 0 || srcy >= height)
                continue;
            dstdata[y * new_width + x] = srcdata[srcy * width + srcx];
        }
    }

    img_unlock(src);
    img_unlock(dst);

    return dst;
}




static inline vect2i32_t _grid_find_intersection(vect2i32_t a1, vect2i32_t a2, vect2i32_t b1, vect2i32_t b2) {
    int32_t ax = a1.x - a2.x;
    int32_t ay = a1.y - a2.y;
    int32_t bx = b1.x - b2.x;
    int32_t by = b1.y - b2.y;
    int32_t q = ax*by - ay*bx;
    int32_t sa = a1.x*a2.y - a1.y*a2.x;
    int32_t sb = b1.x*b2.y - b1.y*b2.x;
    if (q == 0) return a2;
    vect2i32_t r = {
        .x = (sa * bx - sb * ax) / q,
        .y = (sa * by - sb * ay) / q
    };
    return r;
}



/*
    (ax,ay), (bx,by), (cx,cy) les trois sommets
    (x,y) le point a bouger

    On a:
    dy = dx * ((ay-y)/(ax-x)) + C
    dy = dx * ((cy-by)/(cx-bx))

    k1 = ((ay-y)/(ax-x))
    k2 = ((cy-by)/(cx-bx))

    dy = dx * k1 + C1   <=>   ay = ax * k1 + C1
    dy = dx * k2 + C2   <=>   by = bx * k2 + C2

    C1 = ay - ax * k1
    C2 = by - bx * k2

    System:

    dx * k1 + C1 = dy
    et
    dx * k2 + C2 = dy

    dx * k1 + C1 = dy
    et
    dx * k2 + C2 = dx * k1 + C1
    =>
    dx * (k2 - k1) = C1 - C2
    =>
    dx  = (C1 - C2) / (k2 - k1)
    et
    dy = dx * k2 + C2

    a = (x - dx) / (ax - dx)
    et
    a = (y - dy) / (ay - dy)
    et
    a = sqrt((ax-x)^2 + (ay-y)^2) / sqrt((dx-ax)^2 + (dy-ay)^2)

    Donc:
    a = (x - dx + y - dy) / (ax - dx + ay - dy)
*/
static inline vect2i32_t _grid_deform_point(
    vect2i32_t a, vect2i32_t b, vect2i32_t c,
    vect2i32_t va, vect2i32_t vb, vect2i32_t vc,
    vect2i32_t p
) {
    vect2i32_t d;

    /*float kpa = (a.y - p.y) / (a.x - p.x);
    float dpa = a.y - a.x * kpa;

    float kbc = (c.y - b.y) / (c.x - b.x);
    float dbc = c.y - c.x * kbc;

    d.x = (dpa - dbc) / (kbc - kpa);
    d.y = dx * kbc + dbc;*/

    float ka = 1;
    if (a.x != p.x || a.y != p.y) {
        d = _grid_find_intersection(a, p, b, c);
        ka = (float) (abs_i32(p.x - d.x) + abs_i32(p.y - d.y)) / (float) (abs_i32(a.x - d.x) + abs_i32(a.y - d.y));
    }


    /*float kpb = (b.y - p.y) / (b.x - p.x);
    float dpb = b.y - b.x * kpb;

    float kac = (c.y - a.y) / (c.x - a.x);
    float dac = c.y - c.x * kac;

    d.x = (dpb - dac) / (kac - kpb);
    d.y = dx * kac + dac;*/

    float kb = 1;
    if (b.x != p.x || b.y != p.y) {
        d = _grid_find_intersection(b, p, a, c);
        kb = (float) (abs_i32(p.x - d.x) + abs_i32(p.y - d.y)) / (float) (abs_i32(b.x - d.x) + abs_i32(b.y - d.y));
    }


    /*float kpc = (c.y - p.y) / (c.x - p.x);
    float dpc = c.y - c.x * kpc;

    float kab = (b.y - a.y) / (b.x - a.x);
    float dab = b.y - b.x * kab;

    d.x = (dpc - dab) / (kab - kpc);
    d.y = dx * kab + dab;*/

    float kc = 1;
    if (c.x != p.x || c.y != p.y) {
        d = _grid_find_intersection(c, p, b, a);
        kc = (float) (abs_i32(p.x - d.x) + abs_i32(p.y - d.y)) / (float) (abs_i32(c.x - d.x) + abs_i32(c.y - d.y));
    }

    //if (ka == 0 && kb == 0 && kc == 0) {
    //    mprintf("Error\n");
    //}

    vect2i32_t r = {
        .x = p.x + (int32_t) (ka * va.x + kb * vb.x + kc * vc.x),
        .y = p.y + (int32_t) (ka * va.y + kb * vb.y + kc * vc.y)
    };

    return r;
}



// Deform image with a grid to have a square shape
void img_deform_square(image_t srcimg, image_t dstimg, grid_t* origin, rect_t target, color_t background) {
    int32_t srcw = srcimg->width;
    int32_t srch = srcimg->height;

    int32_t dstw = dstimg->width;

    color_t* dst = dstimg->data;
    color_t* src = srcimg->data;

    int32_t targetw = target.x2 - target.x1;
    int32_t targeth = target.y2 - target.y1;

    // First triangle
    {
        vect2i32_t a = {.x = target.x1, .y = target.y1};
        vect2i32_t b = {.x = target.x2-1, .y = target.y1};
        vect2i32_t c = {.x = target.x2-1, .y = target.y2-1};

        vect2i32_t va = {.x = (origin->upleft.x - a.x), .y = (origin->upleft.y - a.y)};
        vect2i32_t vb = {.x = (origin->upright.x - b.x), .y = (origin->upright.y - b.y)};
        vect2i32_t vc = {.x = (origin->downright.x - c.x), .y = (origin->downright.y - c.y)};

        for (int32_t y = target.y1; y < target.y2; y++) {
            int32_t mid_x = (y - target.y1) * (targetw - 1) / (targeth - 1) + target.x1;
            for (int32_t x = mid_x; x < target.x2; x++) {
                vect2i32_t dstp = {.x = x, .y = y};
                vect2i32_t srcp = _grid_deform_point(a, b, c, va, vb, vc, dstp);
                if (srcp.x < 0 || srcp.x >= srcw || srcp.y < 0 || srcp.y >= srch) {
                    dst[dstp.x + dstp.y * dstw] = background;
                    continue;
                }
                dst[dstp.x + dstp.y * dstw] = src[srcp.x + srcp.y * srcw];
            }
        }
    }

    // Second triangle
    {
        vect2i32_t a = {.x = target.x1, .y = target.y1};
        vect2i32_t b = {.x = target.x1, .y = target.y2-1};
        vect2i32_t c = {.x = target.x2-1, .y = target.y2-1};

        vect2i32_t va = {.x = origin->upleft.x - a.x, .y = origin->upleft.y - a.y};
        vect2i32_t vb = {.x = origin->downleft.x - b.x, .y = origin->downleft.y - b.y};
        vect2i32_t vc = {.x = origin->downright.x - c.x, .y = origin->downright.y - c.y};

        for (int32_t y = target.y1; y < target.y2; y++) {
            int32_t mid_x = (y - target.y1) * (targetw - 1) / (targeth - 1) + target.x1;
            for (int32_t x = target.x1; x < mid_x; x++) {
                vect2i32_t dstp = {.x = x, .y = y};
                vect2i32_t srcp = _grid_deform_point(a, b, c, va, vb, vc, dstp);
                if (srcp.x < 0 || srcp.x >= srcw || srcp.y < 0 || srcp.y >= srch) {
                    dst[dstp.x + dstp.y * dstw] = background;
                    continue;
                }
                dst[dstp.x + dstp.y * dstw] = src[srcp.x + srcp.y * srcw];
            }
        }
    }
}




image_t img_blur(image_t src, int radius) {
    image_t dst = img_new(src->width, src->height, HEX_TO_COLOR(0));
    const int count = ((radius*2+1)*(radius*2+1));

    uint8_t* srcdata = (uint8_t*) src->data;
    uint8_t* dstdata = (uint8_t*) dst->data;

    img_lock(dst);
    img_lock(src);

    int32_t rows = src->height;
    int32_t cols = src->width * 4;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            uint32_t sum = 0;
            for (int x = i - radius; x <= i + radius; x++) {
                for (int y = j - radius; y <= j + radius; y++) {
                    if (x >= 0 && x < rows && y >= 0 && y < cols) {
                        sum += srcdata[x * cols + y];
                    }
                }
            }
            dstdata[i * cols + j] = sum / count;
        }
    }

    img_unlock(dst);
    img_unlock(src);

    return dst;
}
