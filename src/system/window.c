#include "window.h"


window_t win_create(
    const char* title,
    int32_t x, int32_t y,
    int32_t width, int32_t height,
    uint32_t flags
) {
    window_t win = (window_t) malloc(sizeof(struct window_s));
    renderer_t rndr = (renderer_t) malloc(sizeof(struct renderer_s));
    win->renderer = rndr;
    win->_visible = true;
    SDL_CreateWindowAndRenderer(width, height, flags, &win->_win, &rndr->_rndr);
    SDL_SetWindowPosition(win->_win, x, y);
    win->id = SDL_GetWindowID(win->_win);
    return win;
}



void win_set_size(window_t win, int32_t width, int32_t height) {
    SDL_SetWindowSize(win->_win, width, height);
}

vect2i32_t win_get_size(window_t win) {
    vect2i32_t r;
    SDL_GetWindowSize(win->_win, &r.x, &r.y);
    return r;
}



void win_set_min_size(window_t win, int32_t width, int32_t height) {
    SDL_SetWindowMinimumSize(win->_win, width, height);
}

vect2i32_t win_get_min_size(window_t win) {
    vect2i32_t r;
    SDL_GetWindowMinimumSize(win->_win, &r.x, &r.y);
    return r;
}



void win_set_max_size(window_t win, int32_t width, int32_t height) {
    SDL_SetWindowMaximumSize(win->_win, width, height);
}

vect2i32_t win_get_max_size(window_t win) {
    vect2i32_t r;
    SDL_GetWindowMaximumSize(win->_win, &r.x, &r.y);
    return r;
}



void win_set_pos(window_t win, int32_t x, int32_t y) {
    SDL_SetWindowPosition(win->_win, x, y);
}

vect2i32_t win_get_pos(window_t win) {
    vect2i32_t r;
    SDL_GetWindowPosition(win->_win, &r.x, &r.y);
    return r;
}



void win_set_title(window_t win, const char* title) {
    SDL_SetWindowTitle(win->_win, title);
}
const char* win_get_title(window_t win) {
    return SDL_GetWindowTitle(win->_win);
}



void win_set_visible(window_t win, bool visible) {
    if (!visible) SDL_HideWindow(win->_win);
    else SDL_ShowWindow(win->_win);
    win->_visible = visible;
}

bool win_is_visible(window_t win) {
    return win->_visible;
}



void win_update(window_t win) {
    //SDL_UpdateWindowSurface(win->_win);
    SDL_RenderPresent(win->renderer->_rndr);
}



void win_free(window_t win) {
    SDL_DestroyWindow(win->_win);
    free(win->renderer);
    free(win);
}
