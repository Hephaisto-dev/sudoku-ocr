#pragma once

#include "common.h"

#include "graphic/renderer.h"
#include "utils/vector.h"

#include "working_SDL2/SDL.h"


#define WIN_FLAG_RESIZEABLE SDL_WINDOW_RESIZABLE


typedef struct window_s {
    renderer_t renderer;
    SDL_Window* _win;
    bool _visible;
    uint32_t id;
}* window_t;


window_t win_create(
    const char* title,
    int32_t x, int32_t y,
    int32_t width, int32_t height,
    uint32_t flags
);

void win_set_size(window_t win, int32_t width, int32_t height);
vect2i32_t win_get_size(window_t win);

void win_set_min_size(window_t win, int32_t width, int32_t height);
vect2i32_t win_get_min_size(window_t win);

void win_set_max_size(window_t win, int32_t width, int32_t height);
vect2i32_t win_get_max_size(window_t win);

void win_set_pos(window_t win, int32_t x, int32_t y);
vect2i32_t win_get_pos(window_t win);

void win_set_title(window_t win, const char* title);
const char* win_get_title(window_t win);

void win_set_visible(window_t win, bool visible);
bool win_is_visible(window_t win);

void win_update(window_t win);

void win_free(window_t win);
