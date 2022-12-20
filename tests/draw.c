#include "graphic/image.h"
#include "graphic/draw.h"

#include "working_SDL2/SDL.h"
#include "working_SDL2/SDL_image.h"


int randint(int min, int max) {
    return (int) (((float) rand() / (float) RAND_MAX) * (max - min) + 0.5f) + min;
}

color_t randcolor() {
    color_t c = {
        .r = randint(0,256),
        .g = randint(0,256),
        .b = randint(0,256),
        .a = 0xFF
    };
    return c;
}


int main(int argc, char** args) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow(
        "Draw Test",
        50, 50, 900, 500,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );


    // Prepare draw target
    image_t target =
        img_new(900, 500, RGB_TO_COLOR(255, 255, 255));

    SDL_Event event;
    
    mprintf("Start drawing lines and points...\n");

    // Draw some randome lines
    for (size_t i = 0; i < 20; i++) {
        draw_line(
            target,
            randint(0,900),  randint(0,500),
            randint(0,900),  randint(0,500),
            randint(1,8), randcolor()
        );
    }
    
    // Draw some randome points
    for (size_t i = 0; i < 50; i++) {
        draw_circle(
            target,
            randint(0,900),  randint(0,500),
            randint(1,8), randcolor()
        );
    }
    
    // Draw some non random lines
    for (size_t i = 0; i < 30; i++) {
        draw_line(
            target,
            1800 - 80*i,  50*i,
            0,  0,
            randint(1,8), randcolor()
        );
    }

    mprintf("Drawing finished!\n");


    // Prepare renderer and texture
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, target->_surf);
    SDL_SetTextureScaleMode(texture, SDL_ScaleModeBest);

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

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
