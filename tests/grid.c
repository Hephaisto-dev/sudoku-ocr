#include "graphic/image.h"
#include "image/detection/grid.h"
#include "graphic/texture.h"
#include "system/window.h"
#include "graphic/renderer.h"
#include "system/input.h"
#include "image/postprocess.h"

#include "working_SDL2/SDL.h"
#include "working_SDL2/SDL_image.h"


#include <stdio.h>
#include <string.h>




int main(int argc, char** args) {
    char* filename = "samples/img_03.jpeg";

    if (argc == 2) {
        filename = args[1];
    }

    image_t img = img_load_from_file(filename);

    if (img == NULL) {
        fprintf(stderr, "ERROR: Can't read image file '%s'\n", filename);
        return 1;
    }

    // Inits

    SDL_Init(SDL_INIT_VIDEO);
    input_init();

    window_t win = win_create("Grid Test", 0, 10, 700, 700, 0);
    renderer_t rndr = win->renderer;

    // Grid detection

    mprintf("Start extracting grid...\n");
    image_t grid = grid_extract_v2(img, rndr, win);

    // Test deformation
    /*image_t grid = img_new(200, 200, HEX_TO_COLOR(0));
    grid_t origin = {
        .upleft = {100,0},
        .upright = {img->width, 100},
        .downleft = {0, img->height-100},
        .downright = {img->width-100, img->height}
    };
    rect_t target = {0, 0, 200, 200};
    img_deform_square(img, grid, &origin, target, HEX_TO_COLOR(0xFF0000));*/


    mprintf("Finished extracting grid\n\n");

    texture_t tex = tex_new_from_img(rndr, grid);

    rndr_clip(rndr, tex, 0, 0);
    win_update(win);

    bool quit = false;
    while (!quit) {
        event_t ev;
        while (input_get(&ev) && !quit) {
            switch (ev.type)
            {
                case EVENT_QUIT:
                    quit = true;
                    break;

                case EVENT_WIN_PAINT:
                case EVENT_WIN_RESIZED:
                    rndr_clip(rndr, tex, 0, 0);
                    win_update(win);
                    break;

                default:
                    break;
            }
        }
    }


    // Frees

    win_free(win);
    tex_free(tex);
    img_free(img);
    img_free(grid);

    // Destroys

    input_destroy();
    SDL_Quit();

    return 0;
}
