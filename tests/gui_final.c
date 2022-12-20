#include "graphic/renderer.h"
#include "graphic/texture.h"
#include "graphic/image.h"
#include "graphic/color.h"
#include "graphic/draw.h"
#include "utils/vector.h"
#include "system/system.h"
#include "system/window.h"
#include "gui/gui.h"
#include "gui/elems/button.h"
#include "gui/elems/frame.h"


#define WINDOW_WIDTH  700
#define WINDOW_HEIGHT 460


#define CLEAR_COLOR RGB_TO_COLOR(68, 88, 88)


int main(int argc, char** args) {
    SDL_Init(SDL_INIT_VIDEO);

    input_init();

    // Window

    vect2i32_t ssize = sys_get_screen_size();
    window_t win = win_create(
        "OCR Sudoku",
        (ssize.x - WINDOW_WIDTH) / 2, (ssize.y - WINDOW_HEIGHT) / 2,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        WIN_FLAG_RESIZEABLE
    );
    renderer_t rndr = win->renderer;


    // GUI

    atlas_t atlas = atlas_new(rndr);

    gui_t gui = gui_new(rndr, atlas, 120);

    // Bottom bar full
    gui_frame_t bottom_bar = gui_new_frame();
    bottom_bar->bg = RGB_TO_COLOR(33,46,46);
    GUI_SET_X1(bottom_bar, 0,   0, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_X2(bottom_bar, 0,   1, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y1(bottom_bar, 0, 0.9, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y2(bottom_bar, 0,   1, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    gui_append_elem(gui, gui->root, bottom_bar);

    // Bottom Bar Right
    gui_frame_t bottom_bar_right = gui_new_frame();
    bottom_bar_right->bg = RGB_TO_COLOR(63,77,77);
    GUI_SET_X1(bottom_bar_right, 0, -3, 0, GUI_ABS_SELF,   GUI_REL_RATIO);
    GUI_SET_X2(bottom_bar_right, 0,  1, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y1(bottom_bar_right, 0,  0, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y2(bottom_bar_right, 0,  1, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    gui_append_elem(gui, bottom_bar, bottom_bar_right);

    // Main Right
    gui_frame_t main_right = gui_new_frame();
    main_right->bg = RGB_TO_COLOR(68, 88, 88);
    GUI_SET_X1(main_right, 0, 0.9, 0, GUI_ABS_PARENT,   GUI_REL_RATIO);
    GUI_SET_X2(main_right, 0,  1, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y1(main_right, 0,  0, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y2(main_right, 0,  0.9, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    gui_append_elem(gui, gui->root, main_right);

    // Image Preview
    gui_frame_t image_preview = gui_new_frame();
    image_preview->bg = RGB_TO_COLOR(255,0,0);
    GUI_SET_X1(image_preview, 10, 0, 0, GUI_ABS_PARENT,   GUI_REL_RATIO);
    GUI_SET_X2(image_preview, -10,  1, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y1(image_preview, 10,  0, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y2(image_preview, -10,  0.8, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    gui_append_elem(gui, main_right, image_preview);

    // Solve Sudoku
    gui_frame_t solve_sudoku = gui_new_frame();
    solve_sudoku->bg = RGB_TO_COLOR(135, 166, 166);
    GUI_SET_X1(solve_sudoku, 10, 0, 0, GUI_ABS_PARENT,   GUI_REL_RATIO);
    GUI_SET_X2(solve_sudoku, -10,  0.5, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y1(solve_sudoku, 10,  0.8, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y2(solve_sudoku, -10,  1, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    gui_append_elem(gui, main_right, solve_sudoku);

    // Save Image
    gui_frame_t save_image = gui_new_frame();
    save_image->bg = RGB_TO_COLOR(85, 45, 150);
    GUI_SET_X1(save_image, 10, 0.5, 0, GUI_ABS_PARENT,   GUI_REL_PARENT);
    GUI_SET_X2(save_image, -10,  1, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y1(save_image, 10,  0.8, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y2(save_image, -10,  1, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    gui_append_elem(gui, main_right, save_image);

    // Main Left
    gui_frame_t main_left = gui_new_frame();
    main_left->bg = RGB_TO_COLOR(68, 88, 88);
    GUI_SET_X1(main_left, 0, 0, 0, GUI_ABS_PARENT,   GUI_REL_RATIO);
    GUI_SET_X2(main_left, 0,  0.9, 0, GUI_ABS_PARENT, GUI_REL_RATIO);
    GUI_SET_Y1(main_left, 0,  0, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y2(main_left, 0,  0.9, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    gui_append_elem(gui, gui->root, main_left);

    // Train AI
    gui_frame_t train_ai = gui_new_frame();
    train_ai->bg = RGB_TO_COLOR(85, 45, 150);
    GUI_SET_X1(train_ai, 10, 0, 0, GUI_ABS_PARENT,   GUI_REL_RATIO);
    GUI_SET_X2(train_ai, -10,  0.5, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y1(train_ai, 10,  0.8, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y2(train_ai, -10,  1, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    gui_append_elem(gui, main_left, train_ai);

    // Crop
    gui_frame_t crop = gui_new_frame();
    crop->bg = RGB_TO_COLOR(85, 45, 150);
    GUI_SET_X1(crop, 10, 0, 0, GUI_ABS_PARENT,   GUI_REL_RATIO);
    GUI_SET_X2(crop, -10,  1, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y1(crop, 10,  0.65, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y2(crop, -10,  0.8, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    gui_append_elem(gui, main_left, crop);

    // Load Settings
    gui_frame_t load_settings = gui_new_frame();
    load_settings->bg = RGB_TO_COLOR(135, 166, 166);
    GUI_SET_X1(load_settings, 10, 0, 0, GUI_ABS_PARENT,   GUI_REL_RATIO);
    GUI_SET_X2(load_settings, -10,  0.5, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y1(load_settings, 10,  0, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y2(load_settings, -10,  0.15, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    gui_append_elem(gui, main_left, load_settings);

    // Logs
    gui_frame_t logs = gui_new_frame();
    logs->bg = RGB_TO_COLOR(85, 45, 150);
    GUI_SET_X1(logs, 10, 0.5, 0, GUI_ABS_PARENT,   GUI_REL_PARENT);
    GUI_SET_X2(logs, -10,  1, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y1(logs, 10,  0, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y2(logs, -10,  0.15, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    gui_append_elem(gui, main_left, logs);


    bool quit = false;
    while (!quit) {
        rndr_clear(rndr, CLEAR_COLOR);
        gui_render(gui);
        win_update(win);
        event_t ev;
        while (input_get(&ev) && !quit) {
            switch (ev.type)
            {
                case EVENT_QUIT:
                    quit = true;
                    break;

                case EVENT_WIN_RESIZED:
                    gui_invalidate(gui, gui->root);
                    break;

                default:
                    gui_process_event(gui, &ev);
                    break;
            }
        }
        sys_delay(1000 / 30);
    }

    // End

    input_destroy();

    win_free(win);

    SDL_Quit();

    return 0;
}
