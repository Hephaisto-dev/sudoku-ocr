#include "image/postprocess.h"
#include "graphic/image.h"

#include "working_SDL2/SDL.h"
#include "working_SDL2/SDL_image.h"

int main(int argc, char** args) {
    char* filename = "samples/img_01.jpeg";

    if (argc == 2) {
        filename = args[1];
    }

    image_t img = img_load_from_file(filename);
    image_t rotated = img_rotate(img, 30);
    img_free(img);

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow(
        "Image Rotation Test",
        50, 50, rotated->width / 2, rotated->height / 2,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, rotated->_surf);

    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    SDL_Event event;
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

    img_free(rotated);

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
