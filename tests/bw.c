#include "graphic/image.h"
#include "image/postprocess.h"

#include "working_SDL2/SDL.h"
#include "working_SDL2/SDL_image.h"


int main(int argc, char** args) {
    char* filename = "samples/img_06.jpeg";

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
        "To black and white Test",
        50, 50, img->width, img->height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );

    
    // To black and white
    mprintf("Start converting to black and white...\n");
    img_blackandwhite(img);
    mprintf("Convertion to black and white finished\n");

    // Prepare renderer and texture
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, img->_surf);
    SDL_SetTextureScaleMode(texture, SDL_ScaleModeBest);

    SDL_Event event;

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

    img_free(img);

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
