#include "draw_grid.h"

int main()
{
    image_t img = draw_grid("tests/in/grids/test_00", "tests/out/grids/result_00", 1, 1);
    img_save_to_file(img, "tests/out/draw_grid/test_00x1.png");
    img_free(img);

    img = draw_grid("tests/in/grids/test_00", "tests/out/grids/result_00", 2, 1);
    img_save_to_file(img, "tests/out/draw_grid/test_00x2.png");
    img_free(img);

    img = draw_grid("tests/in/grids/test_00", "tests/out/grids/result_00", 3, 1);
    img_save_to_file(img, "tests/out/draw_grid/test_00x3.png");
    img_free(img);

    img = draw_grid("tests/in/grids/test_00", "tests/out/grids/result_00", 4, 1);
    img_save_to_file(img, "tests/out/draw_grid/test_00x4.png");
    img_free(img);
    return 0;
}