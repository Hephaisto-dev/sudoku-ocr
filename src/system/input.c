#include "input.h"

#include "working_SDL2/SDL.h"



static uint32_t _input_win_resize = 0;
static uint32_t _input_win_resize_ts = 0;
static bool _mouse_pressed = false;


static int _input_events_watcher(void* data, SDL_Event* event) {
    if (event->type == SDL_WINDOWEVENT &&
        event->window.event == SDL_WINDOWEVENT_RESIZED
    ) {
        //mprintf("Window of id %i resized\n", event->window.windowID);
        _input_win_resize = event->window.windowID;
        _input_win_resize_ts = event->window.timestamp;
    }
    return 0;
}



void input_init() {
    SDL_AddEventWatch(_input_events_watcher, NULL);
}



void input_destroy() {
    ;
}


void input_wait() {
    SDL_WaitEvent(NULL);
}



bool input_availables() {
    return (_input_win_resize != 0) || SDL_PollEvent(NULL);
}


bool input_is_mouse_pressed() {
    return _mouse_pressed;
}


bool input_get(event_t* ev) {
    if (_input_win_resize) {
        ev->type = EVENT_WIN_RESIZED;
        ev->window = _input_win_resize;
        ev->timestamp = _input_win_resize_ts;
        _input_win_resize = 0;
        return true;
    }

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_QUIT:
                ev->type = EVENT_QUIT;
                ev->timestamp = e.quit.timestamp;
                ev->window = 0;
                break;

            case SDL_MOUSEBUTTONDOWN:
                ev->type = EVENT_MOUSE_PRESS;
                ev->window = e.button.windowID;
                ev->timestamp = e.button.timestamp;
                ev->button.x = e.button.x;
                ev->button.y = e.button.y;
                ev->button.button = e.button.button;
                _mouse_pressed = true;
                break;

            case SDL_MOUSEBUTTONUP:
                ev->type = EVENT_MOUSE_RELEASE;
                ev->window = e.button.windowID;
                ev->timestamp = e.button.timestamp;
                ev->button.x = e.button.x;
                ev->button.y = e.button.y;
                ev->button.button = e.button.button;
                _mouse_pressed = false;
                break;

            case SDL_MOUSEMOTION:
                ev->type = EVENT_MOUSE_MOVE;
                ev->window = e.motion.windowID;
                ev->timestamp = e.motion.timestamp;
                ev->motion.x = e.motion.x;
                ev->motion.y = e.motion.y;
                break;

            case SDL_MOUSEWHEEL:
                ev->type = EVENT_MOUSE_SCROLL;
                ev->window = e.wheel.windowID;
                ev->timestamp = e.wheel.timestamp;
                ev->scroll.x = e.wheel.preciseX;
                ev->scroll.y = e.wheel.preciseY;
                break;

            case SDL_KEYDOWN:
                ev->type = EVENT_KEY_PRESS;
                ev->window = e.key.windowID;
                ev->timestamp = e.key.timestamp;
                ev->key.keycode = e.key.keysym.sym;
                break;

            case SDL_KEYUP:
                ev->type = EVENT_KEY_RELEASE;
                ev->window = e.key.windowID;
                ev->timestamp = e.key.timestamp;
                ev->key.keycode = e.key.keysym.sym;
                break;

            case SDL_WINDOWEVENT:
                {
                    ev->window = e.window.windowID;
                    ev->timestamp = e.window.timestamp;
                    switch (e.window.event) {
                        case SDL_WINDOWEVENT_EXPOSED:
                            ev->type = EVENT_WIN_PAINT;
                            break;

                        case SDL_WINDOWEVENT_MOVED:
                            ev->type = EVENT_WIN_MOVED;
                            break;

                        // The following event is triggered after SIZE_CHANGED
                        //case SDL_WINDOWEVENT_RESIZED:
                        //    ev->type = EVENT_WIN_RESIZE;
                        //    break;

                        case SDL_WINDOWEVENT_SIZE_CHANGED:
                            _input_win_resize = e.window.windowID;
                            _input_win_resize_ts = e.window.timestamp;
                            continue;

                        case SDL_WINDOWEVENT_MINIMIZED:
                            ev->type = EVENT_WIN_MINIMIZED;
                            break;

                        case SDL_WINDOWEVENT_MAXIMIZED:
                            ev->type = EVENT_WIN_MAXIMIZED;
                            break;

                        case SDL_WINDOWEVENT_RESTORED:
                            ev->type = EVENT_WIN_RESTORED;
                            break;

                        case SDL_WINDOWEVENT_ENTER:
                            ev->type = EVENT_WIN_ENTER;
                            break;

                        case SDL_WINDOWEVENT_LEAVE:
                            ev->type = EVENT_WIN_LEAVE;
                            break;

                        case SDL_WINDOWEVENT_FOCUS_GAINED:
                            ev->type = EVENT_WIN_FOCUSED;
                            break;

                        case SDL_WINDOWEVENT_FOCUS_LOST:
                            ev->type = EVENT_WIN_UNFOCUSED;
                            break;

                        case SDL_WINDOWEVENT_CLOSE:
                            ev->type = EVENT_WIN_CLOSE;
                            break;

                        default:
                            continue;
                    }
                }
                break;

            default:
                continue;
        }
        return true;
    }

    return false;
}
