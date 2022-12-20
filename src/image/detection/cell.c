#include "cell.h" 

#include "image/postprocess.h"


cells_t cells_detect(image_t img){
    grid_t grid = grid_detect(img);

    int32_t cell_w = (grid.downright.x - grid.upleft.x) / 9;
    int32_t cell_h = (grid.downright.y - grid.upleft.y) / 9;

    int32_t off_x = grid.upleft.x;
    int32_t off_y = grid.upleft.y;

    cells_t cells = {
        .cells = (image_t*) malloc(sizeof(image_t) * 9 * 9),
        .nb_cells = 9 * 9,
        .grid = grid
    };

    for (int32_t x = 0; x < 9; x++) {
        for (int32_t y = 0; y < 9; y++) {
            int32_t rx = off_x + x * cell_w;
            int32_t ry = off_y + y * cell_h;
            cells.cells[y * 9 + x] = 
                img_cut(img, rx, ry, rx + cell_w, ry + cell_h);
        }
    }

    return cells;
}
