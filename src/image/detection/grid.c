#include "image/detection/grid.h"

#include "image/detection/edges.h"

#include "math.h"

#include "image/postprocess.h"
#include "graphic/draw.h"


#define MAX_SIZE 700

#define NB_ADJACENTS 4
static const vect2i32_t ADJACENTS[NB_ADJACENTS] = {
    { 1,  0},
    { 0,  1},
    {-1,  0},
    { 0, -1}
};



/*static inline float _grid_color_distance(color_t x, color_t y) {
    int32_t rmean = ((int32_t) (x.r + y.r)) >> 1;
    int32_t r = (int32_t) (x.r - y.r);
    int32_t g = (int32_t) (x.g - y.g);
    int32_t b = (int32_t) (x.b - y.b);
    return sqrtf( (((512+rmean)*r*r)>>8) + 4*g*g + (((767-rmean)*b*b)>>8) );
}*/



static inline int16_t _grid_color_distance(color_t x, color_t y) {
    return max_i16(abs_i16((int16_t) x.r - (int16_t) y.r), max_i16(abs_i16((int16_t) x.g - (int16_t) y.g), abs_i16((int16_t) x.b - (int16_t) y.b)));
}



static void _grid_magic_wand(image_t img, uint8_t* visited, uint8_t* selected, vect2i32_t* queue, int32_t sx, int32_t sy, float min_d, grid_t* r1, grid_t* r2) {
    color_t* const data = img->data;
    const int32_t w = img->width;
    const int32_t h = img->height;

    size_t queue_len = 1;
    queue[0].x = sx;
    queue[0].y = sy;
    visited[sx + sy * w] = 1;

    vect2i32_t upright = {0, h};
    vect2i32_t upleft = {w, h};
    vect2i32_t downright = {0, 0};
    vect2i32_t downleft = {w, 0};

    vect2i32_t upmost = {0, h};
    vect2i32_t leftmost = {w, 0};
    vect2i32_t downmost = {0, 0};
    vect2i32_t rightmost = {0, 0};

    const color_t ref = data[sx + sy * w];

    while (queue_len > 0) {
        const vect2i32_t coord = queue[--queue_len];
        const color_t self_color = data[coord.x + coord.y * w];

        // Check all adjacent pixels
        for (size_t i = NB_ADJACENTS; i--;) {
            const vect2i32_t adj = {coord.x + ADJACENTS[i].x, coord.y + ADJACENTS[i].y};
            if (adj.x < 0 || adj.x >= w || adj.y < 0 || adj.y >= h)
                continue;

            const size_t adj_index = adj.x + adj.y * w;
            if (!visited[adj_index]) {
                //float d = max_f(_grid_color_distance(data[adj_index], self_color), _grid_color_distance(data[adj_index], ref) / 2);
                int16_t d = _grid_color_distance(data[adj_index], ref);
                //mprintf("d = %f\n", d);
                if (d < min_d) {
                    if (adj.y - adj.x < upright.y - upright.x) {
                        upright = adj;
                    }

                    if (adj.x + adj.y < upleft.x + upleft.y) {
                        upleft = adj;
                    }

                    if (-adj.x - adj.y < -downright.x - downright.y) {
                        downright = adj;
                    }

                    if (adj.x - adj.y < downleft.x - downleft.y) {
                        downleft = adj;
                    }

                    if (adj.y < upmost.y) {
                        upmost = adj;
                    }

                    if (adj.x < leftmost.x) {
                        leftmost = adj;
                    }

                    if (adj.y > downmost.y) {
                        downmost = adj;
                    }

                    if (adj.x > rightmost.x) {
                        rightmost = adj;
                    }

                    queue[queue_len] = adj;
                    queue_len++;
                    visited[adj_index] = 1;
                    selected[adj_index] = 1;
                }
            }
        }
    }

    r1->downleft = downleft;
    r1->downright = downright;
    r1->upleft = upleft;
    r1->upright = upright;

    r2->downmost = downmost;
    r2->leftmost = leftmost;
    r2->upmost = upmost;
    r2->rightmost = rightmost;
}



static void _grid_magic_wand_dbg(image_t img, uint8_t* visited, uint8_t* selected, vect2i32_t* queue, int32_t sx, int32_t sy, float min_d, grid_t* r1, grid_t* r2, color_t rep) {
    color_t* const data = img->data;
    const int32_t w = img->width;
    const int32_t h = img->height;

    size_t queue_len = 1;
    queue[0].x = sx;
    queue[0].y = sy;
    visited[sx + sy * w] = 1;

    vect2i32_t upright = {0, h};
    vect2i32_t upleft = {w, h};
    vect2i32_t downright = {0, 0};
    vect2i32_t downleft = {w, 0};

    vect2i32_t upmost = {0, h};
    vect2i32_t leftmost = {w, 0};
    vect2i32_t downmost = {0, 0};
    vect2i32_t rightmost = {0, 0};

    const color_t ref = data[sx + sy * w];

    while (queue_len > 0) {
        const vect2i32_t coord = queue[--queue_len];
        //const color_t self_color = data[coord.x + coord.y * w];

        // Check all adjacent pixels
        for (size_t i = NB_ADJACENTS; i--;) {
            const vect2i32_t adj = {coord.x + ADJACENTS[i].x, coord.y + ADJACENTS[i].y};
            if (adj.x < 0 || adj.x >= w || adj.y < 0 || adj.y >= h)
                continue;

            const size_t adj_index = adj.x + adj.y * w;
            if (!visited[adj_index]) {
                //float d = _grid_color_distance(data[adj_index], ref);
                int16_t d = _grid_color_distance(data[adj_index], ref);
                //mprintf("d = %f\n", d);
                if (d < min_d) {
                    if (adj.y - adj.x < upright.y - upright.x) {
                        upright = adj;
                    }

                    if (adj.x + adj.y < upleft.x + upleft.y) {
                        upleft = adj;
                    }

                    if (-adj.x - adj.y < -downright.x - downright.y) {
                        downright = adj;
                    }

                    if (adj.x - adj.y < downleft.x - downleft.y) {
                        downleft = adj;
                    }

                    if (adj.y < upmost.y) {
                        upmost = adj;
                    }

                    if (adj.x < leftmost.x) {
                        leftmost = adj;
                    }

                    if (adj.y > downmost.y) {
                        downmost = adj;
                    }

                    if (adj.x > rightmost.x) {
                        rightmost = adj;
                    }

                    queue[queue_len] = adj;
                    queue_len++;
                    visited[adj_index] = 1;
                    selected[adj_index] = 1;
                    data[adj_index] = rep;
                }
            }
        }
    }

    r1->downleft = downleft;
    r1->downright = downright;
    r1->upleft = upleft;
    r1->upright = upright;

    r2->downmost = downmost;
    r2->leftmost = leftmost;
    r2->upmost = upmost;
    r2->rightmost = rightmost;
}



static float _grid_evaluate_part(image_t img, int32_t sx, int32_t sy, int32_t w, int32_t h, color_t ref) {
    float sum = 0;
    color_t* data = img->data;
    int32_t imgw = img->width;
    int32_t imgh = img->height;
    for (int32_t mx = min_i32(sx + w, imgw), x = sx; x < mx; x++) {
        for (int32_t my = min_i32(sy + h, imgh), y = sy; y < my; y++) {
            sum += _grid_color_distance(ref, data[x + y * imgw]);
        }
    }
    return sum;
}



// Score an image to test if it's look like a Sodoku
float _grid_evaluate(image_t img, rect_t rect, color_t fg, color_t bg) {
    float score = 0;

    int32_t w = rect.x2 - rect.x1;
    int32_t h = rect.y2 - rect.y1;
    int32_t small_line_width = min_i32(w,h) * 3 / 100;
    int32_t big_line_width   = min_i32(w,h) * 4 / 100;
    int32_t cell_border = min_i32(w,h) * 4 / 100;
    int32_t cell_width = (w - 4 * big_line_width - 6 * small_line_width) / 9;
    int32_t cell_height = (h - 4 * big_line_width - 6 * small_line_width) / 9;

    // Check horizontal and verticals lines
    for (int i = 0; i < 10; i++) {
        int32_t line_width = (i % 3) ? small_line_width : big_line_width;
        int nb_big = (i+2) / 3;
        int32_t xoffset = big_line_width + i * cell_width + (i - nb_big) * small_line_width + nb_big * big_line_width;
        int32_t yoffset = big_line_width + i * cell_height + (i - nb_big) * small_line_width + nb_big * big_line_width;
        score += _grid_evaluate_part(img, xoffset, 0, line_width, h, fg);
        score += _grid_evaluate_part(img, 0, yoffset, w, line_width, fg);
    }

    // Check each cells (check the white) in the cells
    for (int x = 0; x < 9; x++) {
        for (int y = 0; y < 9; y++) {
            int nb_big_x = (x+2) / 3;
            int nb_big_y = (y+2) / 3;
            int32_t xoffset = big_line_width + x * cell_width + (x - nb_big_x) * small_line_width + nb_big_x * big_line_width;
            int32_t yoffset = big_line_width + y * cell_height + (y - nb_big_y) * small_line_width + nb_big_y * big_line_width;
            score += _grid_evaluate_part(img, xoffset, yoffset, cell_width, cell_border, bg);
            score += _grid_evaluate_part(img, xoffset, yoffset, cell_border, cell_height, bg);
            score += _grid_evaluate_part(img, xoffset + cell_width - cell_border, yoffset, cell_border, cell_height, bg);
            score += _grid_evaluate_part(img, xoffset, yoffset + cell_height - cell_border, cell_width, cell_border, bg);
        }
    }

    return score / ((4*big_line_width+6*small_line_width)*w + 9*9*(2*cell_width+2*cell_height)*cell_border);
}



static rect_t _grid_get_bbox(grid_t* grid1, grid_t* grid2) {
    rect_t bbox;
    bbox.x1 = min_i32(grid2->leftmost.x, min_i32(grid1->upleft.x, grid1->downleft.x));
    bbox.x2 = max_i32(grid2->rightmost.x, max_i32(grid1->upright.x, grid1->downright.x));
    bbox.y1 = min_i32(grid2->upmost.y, min_i32(grid1->upright.y, grid1->upleft.y));
    bbox.y2 = max_i32(grid2->downmost.y, max_i32(grid1->downright.y, grid1->downleft.y));
    return bbox;
}



static rect_t _grid_square_bbox(rect_t bbox) {
    //bbox.x1 /= 2;
    //bbox.x2 /= 2;
    //bbox.y1 /= 2;
    //bbox.y2 /= 2;

    int32_t bw = bbox.x2 - bbox.x1;
    int32_t bh = bbox.y2 - bbox.y1;

    if (bw > bh) {
        int32_t diff = (bw - bh) / 2;
        bbox.x1 += diff;
        bbox.x2 -= diff;
    } else {
        int32_t diff = (bh - bw) / 2;
        bbox.y1 += diff;
        bbox.y2 -= diff;
    }

    return bbox;
}



static void _grid_detect_colors(image_t img, rect_t rect, uint8_t* selecteds, color_t* fg, color_t* bg) {
    uint32_t fgr = 0, fgg = 0, fgb = 0;
    uint32_t bgr = 0, bgg = 0, bgb = 0;
    uint32_t nb_fg = 0, nb_bg = 0;
    int32_t w = img->width;
    color_t* data = img->data;
    for (int32_t x = rect.x1; x < rect.x2; x++) {
        for (int32_t y = rect.y1; y < rect.y2; y++) {
            color_t c = data[w * y + x];
            if (selecteds[w * y + x]) {
                fgr += c.r;
                fgg += c.g;
                fgb += c.b;
                nb_fg++;
            } else {
                bgr += c.r;
                bgg += c.g;
                bgb += c.b;
                nb_bg++;
            }
        }
    }

    if (nb_fg > 0) {
        fg->r = fgr / nb_fg;
        fg->g = fgg / nb_fg;
        fg->b = fgb / nb_fg;
    } else {
        fg->r = 0xFF - bgr;
        fg->g = 0xFF - bgg;
        fg->b = 0xFF - bgb;
    }
    fg->a = 0xFF;

    if (nb_bg > 0) {
        bg->r = bgr / nb_bg;
        bg->g = bgg / nb_bg;
        bg->b = bgb / nb_bg;
    } else {
        bg->r = 0xFF - fgr;
        bg->g = 0xFF - fgg;
        bg->b = 0xFF - fgb;
    }
    bg->a = 0xFF;
}



#include "graphic/texture.h"

image_t grid_extract(image_t src, renderer_t rndr, window_t win) {
    // Resize input to speed up extraction
    image_t img = NULL;
    if (src->width > src->height) {
        if (src->width > MAX_SIZE) {
            img = img_resize(src, MAX_SIZE, src->height * MAX_SIZE / src->width);
        }
    } else {
        if (src->height > MAX_SIZE) {
            img = img_resize(src, src->width * MAX_SIZE / src->height, MAX_SIZE);
        }
    }
    if (!img) {
        img = img_clone(src);
    }

    int32_t w = img->width;
    int32_t h = img->height;

    int32_t min_area = w * h / 8;

    uint8_t* visiteds = (uint8_t*) calloc(w * h, 1);
    uint8_t* selecteds = (uint8_t*) malloc(w * h);
    vect2i32_t* queue = (vect2i32_t*) malloc((sizeof(vect2i32_t) * w * h) / 2);

    image_t buff1 = img_new(img->width, img->height, HEX_TO_COLOR(0));
    image_t buff2 = img_new(img->width, img->height, HEX_TO_COLOR(0));
    image_t result = img_new(img->width, img->height, HEX_TO_COLOR(0));

    float best_score = INFINITY;
    rect_t best_rect = {};

    img_lock(img);
    img_lock(buff1);
    img_lock(buff2);
    img_lock(result);

    uint32_t cnt = 0xFF0000;

    for (int32_t y = 0; y < h; y++) {
        //mprintf("Line %i\n", y);
        for (int32_t x = 0; x < w; x++) {
            if (!visiteds[x + y * w]) {
                grid_t grid1, grid2;

                // Get the area that contain the continuous block
                for (size_t i = 0, m = w*h; i < m; i++) selecteds[i] = 0;
                _grid_magic_wand(img, visiteds, selecteds, queue, x, y, 20, &grid1, &grid2);
                //_grid_magic_wand_dbg(img, visiteds, selecteds, queue, x, y, 100, &grid1, &grid2, HEX_TO_COLOR(cnt));

                if (
                    grid1.upright.x == 0 && grid1.upright.y == h &&
                    grid1.upleft.x == w && grid1.upleft.y == h &&
                    grid1.downright.x == 0 && grid1.downright.y == 0 &&
                    grid1.downleft.x == w && grid1.downleft.y == 0
                ) {
                    continue;
                }

                if (
                    grid1.upmost.x == 0 && grid1.upmost.y == h &&
                    grid1.leftmost.x == w && grid1.leftmost.y == 0 &&
                    grid1.downmost.x == 0 && grid1.downmost.y == 0 &&
                    grid1.rightmost.x == 0 && grid1.rightmost.y == 0
                ) {
                    continue;
                }

                /*cnt += 127l + (64l << 8) + (32l << 16);
                texture_t tex = tex_new_from_img(rndr, img);
                rndr_clip(rndr, tex, 0, 0);
                win_update(win);
                tex_free(tex);*/

                // Get BBOX that contains all visiteds pixels
                rect_t rect = _grid_get_bbox(&grid1, &grid2);

                // Detect optimal target rect for deformations
                rect_t orect = _grid_square_bbox(rect);

                // Check if the area is not too small
                if ((rect.x2 - rect.x1) * (rect.y2 - rect.y1) < min_area) {
                    continue;
                }

                //mprintf("Test deform\n");

                // Detect foreground and background colors
                color_t fg, bg;
                _grid_detect_colors(img, rect, selecteds, &fg, &bg);

                // Test first deform
                img_deform_square(img, buff1, &grid1, orect, bg);
                float score1 = _grid_evaluate(buff1, orect, fg, bg);

                // Test second deform
                img_deform_square(img, buff2, &grid2, orect, bg);
                float score2 = _grid_evaluate(buff2, orect, fg, bg);

                /*
                texture_t tex2 = tex_new_from_img(rndr, buff1);
                rndr_clip(rndr, tex2, 0, 0);
                win_update(win);
                tex_clip_image(tex2, buff2, 0, 0);
                rndr_clip(rndr, tex2, 0, 0);
                win_update(win);
                tex_free(tex2);
                */

                if (score1 < score2) {
                    if (score1 < best_score) {
                        best_score = score1;
                        best_rect = orect;
                        img_clip(buff1, result, 0, 0);
                    }
                } else {
                    if (score2 < best_score) {
                        best_score = score2;
                        best_rect = orect;
                        img_clip(buff2, result, 0, 0);
                    }
                }
            }
        }
    }

    img_unlock(buff1);
    img_unlock(buff2);
    img_unlock(img);

    img_free(buff1);
    img_free(buff2);
    img_free(img);

    free(queue);
    free(visiteds);

    image_t r = img_new(best_rect.x2 - best_rect.x1, best_rect.y2 - best_rect.y1, HEX_TO_COLOR(0));
    img_lock(r);
    img_clip(result, r, 0, 0);
    img_unlock(r);
    img_unlock(result);
    img_free(result);

    return r;
}



typedef struct {
    rect_t rect;
    vect2i32_t pixel;
} _grid_part_t;


#define _GRID_TOLERANCE 20
#define _GRID_MAX_COLOR_DIFF 30

image_t grid_extract_v2(image_t src, renderer_t rndr, window_t win) {
    //src = img_blur(src, 1);

    // Resize input to speed up extraction
    image_t img = NULL;
    if (src->width > src->height) {
        if (src->width > MAX_SIZE) {
            img = img_resize(src, MAX_SIZE, src->height * MAX_SIZE / src->width);
        }
    } else {
        if (src->height > MAX_SIZE) {
            img = img_resize(src, src->width * MAX_SIZE / src->height, MAX_SIZE);
        }
    }
    if (!img) {
        img = img_clone(src);
    }

    //img_free(src);

    int32_t w = img->width;
    int32_t h = img->height;

    //int32_t min_area = w * h / 8;
    int32_t min_area = 10;

    uint8_t* visiteds = (uint8_t*) calloc(w * h, 1);
    uint8_t* selecteds = (uint8_t*) malloc(w * h);
    vect2i32_t* queue = (vect2i32_t*) malloc((sizeof(vect2i32_t) * w * h) / 2);

    image_t buff1 = img_new(img->width, img->height, HEX_TO_COLOR(0));
    image_t buff2 = img_new(img->width, img->height, HEX_TO_COLOR(0));
    image_t result = img_new(img->width, img->height, HEX_TO_COLOR(0));

    float best_score = INFINITY;
    rect_t best_rect = {};

    img_lock(img);
    img_lock(buff1);
    img_lock(buff2);
    img_lock(result);

    uint32_t cnt = 0xFF0000;
    int32_t nb_sizes = max_i32(w, h);
    size_t* sizes = calloc(nb_sizes, sizeof(size_t));
    _grid_part_t* parts = malloc(sizeof(_grid_part_t) * (w * h * 2 / min_area));
    size_t nb_parts = 0;

    for (int32_t y = 0; y < h; y++) {
        //mprintf("Line %i\n", y);
        for (int32_t x = 0; x < w; x++) {
            if (!visiteds[x + y * w]) {
                grid_t grid1, grid2;

                // Get the area that contain the continuous block
                for (size_t i = 0, m = w*h; i < m; i++) selecteds[i] = 0;
                _grid_magic_wand(img, visiteds, selecteds, queue, x, y, _GRID_MAX_COLOR_DIFF, &grid1, &grid2);
                //_grid_magic_wand_dbg(img, visiteds, selecteds, queue, x, y, _GRID_MAX_COLOR_DIFF, &grid1, &grid2, HEX_TO_COLOR(cnt));

                if (
                    grid1.upright.x == 0 && grid1.upright.y == h &&
                    grid1.upleft.x == w && grid1.upleft.y == h &&
                    grid1.downright.x == 0 && grid1.downright.y == 0 &&
                    grid1.downleft.x == w && grid1.downleft.y == 0
                ) {
                    continue;
                }

                if (
                    grid1.upmost.x == 0 && grid1.upmost.y == h &&
                    grid1.leftmost.x == w && grid1.leftmost.y == 0 &&
                    grid1.downmost.x == 0 && grid1.downmost.y == 0 &&
                    grid1.rightmost.x == 0 && grid1.rightmost.y == 0
                ) {
                    continue;
                }

                // Get BBOX that contains all visiteds pixels
                rect_t rect = _grid_get_bbox(&grid1, &grid2);

                // Check if the area is not too small
                int32_t area = (rect.x2 - rect.x1) * (rect.y2 - rect.y1);
                if (area < min_area) {
                    continue;
                }

                /*if (area > 400) {
                    cnt += 127l + (64l << 8) + (32l << 16);
                    texture_t tex = tex_new_from_img(rndr, img);
                    rndr_clip(rndr, tex, 0, 0);
                    win_update(win);
                    tex_free(tex);
                }*/

                // Check if it's a square (approximatively)
                int32_t size = abs_i32(((rect.x2 - rect.x1) + (rect.y2 - rect.y1)) / 2);
                if (abs_i32((rect.x2 - rect.x1) - (rect.y2 - rect.y1)) > size) {
                    continue;
                }

                parts[nb_parts].pixel.x = x;
                parts[nb_parts].pixel.y = y;
                parts[nb_parts].rect = rect;
                nb_parts++;

                for (int32_t i = max_i32(0, size - _GRID_TOLERANCE), m = min_i32(nb_sizes, size + _GRID_TOLERANCE); i < m; i++)
                    sizes[i]++;
            }
        }
    }

    // Get the cells size for which the number of cell of this size is nearly 81
    int32_t best_diff = INT32_MAX;
    int32_t best_size = -1;
    for (int32_t size = 0; size < nb_sizes; size++) {
        int32_t diff = abs_i32(81 - sizes[size]);
        if (diff < best_diff) {
            best_size = size;
            best_diff = diff;
        }
    }

    // Compute the grid size size and choose the one that fit the best
    int32_t grid_size = best_size * 9;
    _grid_part_t best_part = {.pixel.x = -1, .pixel.y = -1};
    best_diff = INT32_MAX;
    for (size_t i = 0; i < nb_parts; i++) {
        rect_t rect = parts[i].rect;
        int32_t size = abs_i32(((rect.x2 - rect.x1) + (rect.y2 - rect.y1)) / 2);
        int32_t diff = abs_i32(grid_size - size);
        if (diff < best_diff) {
            best_diff = diff;
            best_part = parts[i];
        }
    }

    // Transform
    // Re-detect selection
    grid_t grid1, grid2;
    for (size_t i = 0, m = w*h; i < m; i++) selecteds[i] = 0;
    _grid_magic_wand(img, selecteds, selecteds, queue, best_part.pixel.x, best_part.pixel.y, _GRID_MAX_COLOR_DIFF, &grid1, &grid2);

    // Detect optimal target rect for deformations
    rect_t rect = best_part.rect;
    rect_t orect = _grid_square_bbox(rect);

    mprintf("Rect = (%d,%d) : (%d,%d)\n", rect.x1, rect.y1, rect.x2, rect.y2);
    mprintf("ORect = (%d,%d) : (%d,%d)\n", orect.x1, orect.y1, orect.x2, orect.y2);
    mprintf("Grid1 = (%d,%d); (%d,%d); (%d,%d); (%d,%d)\n", grid1.upleft.x, grid1.upleft.y, grid1.upright.x, grid1.upright.y, grid1.downleft.x, grid1.downleft.y, grid1.downright.x, grid1.downright.y);

    // Detect foreground and background colors
    color_t fg, bg;
    _grid_detect_colors(img, rect, selecteds, &fg, &bg);

    // Test first deform
    img_deform_square(img, buff1, &grid1, orect, bg);
    float score1 = _grid_evaluate(buff1, orect, fg, bg);

    // Test second deform
    img_deform_square(img, buff2, &grid2, orect, bg);
    float score2 = _grid_evaluate(buff2, orect, fg, bg);

    if (score1 < score2) {
        if (score1 < best_score) {
            best_score = score1;
            best_rect = orect;
            img_clip(buff1, result, 0, 0);
        }
    } else {
        if (score2 < best_score) {
            best_score = score2;
            best_rect = orect;
            img_clip(buff2, result, 0, 0);
        }
    }

    free(parts);
    free(sizes);

    img_unlock(buff1);
    img_unlock(buff2);
    img_unlock(img);

    img_free(buff1);
    img_free(buff2);
    img_free(img);

    free(queue);
    free(visiteds);

    image_t r = img_new(best_rect.x2 - best_rect.x1, best_rect.y2 - best_rect.y1, HEX_TO_COLOR(0));
    img_lock(r);
    img_clip(result, r, 0, 0);
    img_unlock(r);
    img_unlock(result);
    img_free(result);

    return r;
}




grid_t grid_detect(image_t img) {
    edge_lines_t data = edge_detect(img);

    // Detect 4 corners

    edge_point_t* upright   = data.lines[0].first;
    edge_point_t* upleft    = data.lines[0].first;
    edge_point_t* downright = data.lines[0].first;
    edge_point_t* downleft  = data.lines[0].first;

    for (size_t i = 0; i < data.nb_lines; i++) {
        edge_line_t* line = &data.lines[i];
        for (
            edge_point_t* p = line->first;
            p != NULL && p != line->last;
            p = p->next
        ) {
            if (p->position.y - p->position.x < upright->position.y - upright->position.x) {
                upright = p;
            }
            if (p->position.x + p->position.y < upleft->position.x + upleft->position.y) {
                upleft = p;
            }
            if (-p->position.x - p->position.y < -downright->position.x - downright->position.y) {
                downright = p;
            }
            if (p->position.x - p->position.y < downleft->position.x - downleft->position.y) {
                downleft = p;
            }
        }
    }

    grid_t grid = {
        .upleft = vect_2f_to_2i32(upleft->position),
        .upright = vect_2f_to_2i32(upright->position),
        .downleft = vect_2f_to_2i32(downleft->position),
        .downright = vect_2f_to_2i32(downright->position)
    };

    return grid;
}
