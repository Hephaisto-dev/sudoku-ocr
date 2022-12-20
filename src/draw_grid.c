#include "draw_grid.h"
#include "graphic/draw.h"
#include <stdio.h>


int sizes[] = { 32, 65, 98, 130 };


int read_file(char *filename, char *grid)
{
	FILE* ptr = fopen(filename, "r");

    if (ptr == NULL)
    {
        printf("Error, read fail!\n");
        return 1;
    }

    char ch = fgetc(ptr);
    while (ch != EOF)
    {
        if (ch == '.' || (ch >= '1' && ch <= '9')) *(grid++) = ch;
        ch = fgetc(ptr);
    }

    fclose(ptr);
    return 0;
}


void img_draw_number(image_t img, char number, int x, int y,
                     font_t f, int32_t case_width, int font_size)
{
    int32_t width = img->width;
    color_t* pixels = img->data;
    font_glyph_t g = font_render_glyph(f, number, font_size);
    int32_t w = g.img->width;
    int32_t h = g.img->height;
    color_t* new_pixels = g.img->data;

    x += (case_width - w) / 2;
    y += (case_width - h) / 2;

    img_lock(img);
    for (int i = 0; i < w; i++)
        for (int j = 0; j < h; j++)
            if (new_pixels[i+j*w].a > 0)
            {
                uint8_t a = 0xFF - new_pixels[i+j*w].a;
                color_t p = pixels[x+i + width*(y+j)];
                pixels[x+i + width*(y+j)] = RGB_TO_COLOR(0xE3 - p.r*a/255, 0xE3 - p.g*a/255, 0xE3 - p.b*a/255);
            }
    img_unlock(img);
    img_free(g.img);
}


image_t create_grid(int size)
{
    int32_t width = 406 * size;
    int32_t line_width = 2 * size;
    int32_t case_width = 42 * size;

    color_t white = RGB_TO_COLOR(0x54, 0x54, 0x54);

    image_t grid = img_new(width, width, RGB_TO_COLOR(0, 0, 0));
    int32_t x;
    int32_t y = line_width * 2;

    for (size_t i = 1; i < 10; i++)
    {
        x = line_width * 2;
        for (size_t j = 1; j < 10; j++)
        {
            draw_rect(grid, x, y, x+case_width, y+case_width, white);
            x += case_width + line_width;
            if (j % 3 == 0) x += line_width;
        }
        y += case_width + line_width;
        if (i % 3 == 0) y += line_width;
    }
    return grid;
}


void fill(image_t grid, char* old, char* result, int size, int solved)
{
    int32_t line_width = 2 * size;
    int32_t case_width = 42 * size;
    int32_t x;
    int32_t y;
    size_t i = 0;

    color_t gray = RGB_TO_COLOR(0x31, 0x31, 0x31);

    //font_init();
    font_t f = font_load_from_file("assets/fonts/Poppins-Medium.ttf");

    for (size_t a = 0; a < 9; a++)
    {
        y = line_width * (a + a/3 + 2) + case_width * a;
        for (size_t b = 0; b < 9; b++)
        {
            x = line_width * (b + b/3 + 2) + case_width * b;
            if (old[i]!= '.')
            {
                draw_rect(grid, x, y, x+case_width, y+case_width, gray);
                img_draw_number(grid, old[i], x, y, f, case_width, sizes[size - 1]);
            }
            else if (solved)
                img_draw_number(grid, result[i], x, y, f, case_width, sizes[size - 1]);
            i++;
        }
    }
    font_free(f);
    //font_destroy();
}


image_t draw_grid(char* filename_old, char* filename_result, int size, int solved)
{
    image_t grid = create_grid(size);
    char o[81] = {};
    char* old = o;
    read_file(filename_old, old);
    char r[81] = {};
    char* result = r;
    if (solved) read_file(filename_result, result);

    fill(grid, old, result, size, solved);

    return grid;
}