#include "graphic/image.h"
#include "image/detection/edges.h"
#include "graphic/draw.h"

#include "working_SDL2/SDL.h"
#include "working_SDL2/SDL_image.h"


int main(int argc, char** args) {
    char* filename = "samples/img_01.jpeg";

    if (argc == 2) {
        filename = args[1];
    }

    image_t img = img_load_from_file(filename);

    if (img == NULL) {
        fprintf(stderr, "ERROR: Can't read image file\n");
        return 1;
    }

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow(
        "Edges Detection Test",
        50, 50, img->width + 20, img->height + 20,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );


    // Detect lines
    mprintf("Start detecting lines...\n");
    edge_lines_t lines = edge_detect(img);
    mprintf("Line detection finished!\n");


    // Prepare draw target
    image_t target =
        img_new(img->width + 20, img->height + 20, RGB_TO_COLOR(255, 255, 255));

    SDL_Event event;

    mprintf("Start drawing lines and points...\n");
    // Draw points
    /*
    for (size_t i = 0; i < lines.nb_points; i++) {
        edge_point_t* p = &lines.points[i];
        uint8_t s = 255 - (uint8_t) (p->strength * 255.0f);
        img_set_pixel(
            target,
            (int32_t) p->position.x + 10, (int32_t)  p->position.y + 10,
            RGB_TO_COLOR(s, s, s)
        );
    }
    */

    // Draw lines (be carefull, a line can be a loop!)
    for (size_t i = 0; i < lines.nb_lines; i++) {
        edge_line_t* line = &lines.lines[i];
        for (
            edge_point_t* p = line->first;
            p != NULL && p != line->last;
            p = p->next
        ) {
            int x1 = (int)(p->position.x) + 10;
            int y1 = (int)(p->position.y) + 10;
            int x2 = (int)(p->next->position.x) + 10;
            int y2 = (int)(p->next->position.y) + 10;
            if (x1 < 10 || x1 >= target->width - 10 || x2 < 10 || x2 >= target->width - 10 || y1 < 10 || y1 >= target->height - 10 || y2 < 10 || y2 >= target->height - 10) {
                mprintf("Error\n");
            }
            draw_rect(target, x1, y1, x1, y1, HEX_TO_COLOR(0xFF0000));
            draw_rect(target, x2, y2, x2, y2, HEX_TO_COLOR(0xFF0000));
            /*draw_line(
                target,
                x1, y1,
                x2, y2,
                1, RGB_TO_COLOR(255, 0, 0)
            );*/
        }
    }

    // Draw lines (method 2)
    /*
    for (size_t i = 0; i < lines.nb_points; i++) {
        edge_point_t* p = &lines.points[i];
        if (p->next != NULL) {
            draw_line(
                target,
                p->position.x + 10, p->position.y + 10,
                p->next->position.x + 10, p->next->position.y + 10,
                1, RGB_TO_COLOR(255, 0, 0)
            );
        }
        if (p->prev != NULL) {
            draw_line(
                target,
                p->position.x + 10, p->position.y + 10,
                p->prev->position.x + 10, p->prev->position.y + 10,
                1, RGB_TO_COLOR(255, 0, 0)
            );
        }
    }
    */

    // Draw right most and left most
    /*
    edge_point_t* upright  = lines.lines[0].first;
    edge_point_t* downleft = lines.lines[0].first;
    for (size_t i = 0; i < lines.nb_lines; i++) {
        edge_line_t* line = &lines.lines[i];
        for (
            edge_point_t* p = line->first;
            p != NULL && p != line->last;
            p = p->next
        ) {
            if (p->position.y - p->position.x < upright->position.y - upright->position.x) {
                upright = p;
            }
            if (p->position.x - p->position.y < downleft->position.x - downleft->position.y) {
                downleft = p;
            }
        }
    }
    draw_circle(target,  upright->position.x + 10,  upright->position.y + 10, 3, RGB_TO_COLOR(0, 0, 255));
    draw_circle(target, downleft->position.x + 10, downleft->position.y + 10, 3, RGB_TO_COLOR(0, 0, 255));
    */


    mprintf("Drawing finished!\n");


    // Prepare renderer and texture
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, target->_surf);
    SDL_SetTextureScaleMode(texture, SDL_ScaleModeBest);

    //SDL_UpdateTexture(texture, NULL, target->data, 4 * target->width);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    uint8_t quit = 0;
    while (!quit) {
        SDL_WaitEvent(&event);
        switch (event.type) {
            case SDL_QUIT:
                quit = 1;
                break;
            case SDL_WINDOWEVENT:
	            if(event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    SDL_RenderCopy(renderer, texture, NULL, NULL);
                    SDL_RenderPresent(renderer);
                }
                break;
        }
    }

    img_free(target);
    img_free(img);

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
