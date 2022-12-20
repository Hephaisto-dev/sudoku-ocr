#include "logic/dataset.h"
#include "graphic/image.h"

#include "working_SDL2/SDL.h"
#include "working_SDL2/SDL_image.h"

int main(int argc, char** args) {
    char* imgs_filename   = "dataset/train-images-idx3-ubyte.gz";
    char* labels_filename = "dataset/train-labels-idx1-ubyte.gz";

    dataset_t dataset = dataset_load_mnist(imgs_filename, labels_filename);

    if (!dataset) {
        mprintf("Error: Can't load dataset files\n");
        return 1;
    }

    image_t img = img_new(dataset->imgs[0].w, dataset->imgs[0].h, RGB_TO_COLOR(0,0,0));

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow(
        "Data Set visualisation Test",
        50, 50, img->width, img->height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );

    size_t img_index = 0;
    dataset_write_on_img(&dataset->imgs[img_index++], img);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, img->_surf);

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
	            if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    SDL_RenderCopy(renderer, texture, NULL, NULL);
                    SDL_RenderPresent(renderer);
                }
                break;

            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_RETURN) {
                    // Render next image
                    dataset_img_t* dimg = &dataset->imgs[img_index++];
                    mprintf("Current character : %c\n", dimg->character);
                    dataset_write_on_img(dimg, img);
                    img_lock(img);
                    SDL_UpdateTexture(texture, NULL, img->data, img->width * 4);
                    img_unlock(img);
                    SDL_RenderCopy(renderer, texture, NULL, NULL);
                    SDL_RenderPresent(renderer);
                }
                break;
        }
    }

    dataset_free(dataset);

    img_free(img);

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
