#include "graphic/renderer.h"
#include "graphic/texture.h"
#include "graphic/image.h"
#include "graphic/color.h"
#include "graphic/draw.h"
#include "graphic/atlas.h"
#include "utils/vector.h"
#include "system/system.h"
#include "system/window.h"
#include "gui/gui.h"
#include "gui/elems/frame.h"
#include "gui/elems/button.h"
#include "gui/elems/label.h"


#define WINDOW_WIDTH  700
#define WINDOW_HEIGHT 460



int main(int argc, char** args) {
    // Inits

    SDL_Init(SDL_INIT_VIDEO);
    input_init();
    font_init();


    // Window

    vect2i32_t ssize = sys_get_screen_size();
    window_t win = win_create(
        "GUI Test",
        (ssize.x - WINDOW_WIDTH) / 2, (ssize.y - WINDOW_HEIGHT) / 2,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        WIN_FLAG_RESIZEABLE
    );
    renderer_t rndr = win->renderer;


    // Texture

    /*
    image_t img = img_load_from_file("samples/img_03.jpeg");

    texture_t tex = tex_new_from_img(rndr, img);

    image_t editable = tex_edit(tex);
    draw_rect(editable, 0, 0, 100, 60, RGB_TO_COLOR(0, 150, 0));
    draw_circle(editable, 10, 10, 15, RGB_TO_COLOR(255, 100, 255));
    draw_line(img, 0, 0, 50, 50, 1, RGB_TO_COLOR(0,0,0));
    tex_update(tex);

    rndr_clear(rndr, RGB_TO_COLOR(40, 40, 40));
    rndr_clip(rndr, tex, 20, 10);
    win_update(win);

    sys_delay(1000);

    tex_free(tex);
    img_free(img);
    */


    // Load font

    font_t font = font_load_from_file("assets/fonts/consolas.ttf");
    if (!font) {
        mprintf("Failed to load font file\n");
        return 1;
    }
    mprintf("Font loaded\n");

    font_glyph_t glyph = font_render_glyph(font, L'S', 200);

    /*
    texture_t glyphtex = tex_new_from_img(rndr, glyph.img);

    rndr_clip(rndr, glyphtex, 0, 0);
    win_update(win);
    sys_delay(1000);
    tex_free(glyphtex);
    */


    // Atlas

    atlas_t atlas = atlas_new(rndr);

    /*
    atlas_add(atlas, L'S', 200, font->id, &glyph);
    atlas_entry_t* entry = atlas_get(atlas, L'S', 200, font->id);
    rndr_clear(rndr, RGB_TO_COLOR(0,0,0));
    rndr_set_mul_color(rndr, entry->texture, RGB_TO_COLOR(255,0,255));
    rndr_clip_rect(rndr, entry->texture, entry->x, entry->y, entry->width, entry->height, 0, 0);
    win_update(win);
    sys_delay(2000);
    */


    // Setup GUI

    gui_t gui = gui_new(rndr, atlas, 120);

    gui_frame_t e1 = gui_new_frame();
    e1->bg = RGB_TO_COLOR(255,0,0);
    GUI_SET_X1(e1, -30, 0.5, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_X2(e1,  30, 0.5, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y1(e1, -20, 0.5, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y2(e1,  20, 0.5, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    gui_append_elem(gui, gui->root, GUI_CAST(e1));


    gui_frame_t e2 = gui_new_frame();
    e2->bg = RGB_TO_COLOR(0,0,255);
    GUI_SET_X1(e2,  10,   0, 0, GUI_ABS_PREV, GUI_REL_PREV);
    GUI_SET_X2(e2, 100,   0, 0, GUI_ABS_SELF, GUI_REL_PREV);
    GUI_SET_Y1(e2,   0,  -1, 0, GUI_ABS_PREV, GUI_REL_PREV);
    GUI_SET_Y2(e2,   0,   1, 0, GUI_ABS_SELF, GUI_REL_PREV);
    gui_append_elem(gui, gui->root, GUI_CAST(e2));

    gui_label_t e3 = gui_new_label();
    e3->text = str_from_wchars_cp(L"Hello, World!\nTest.");
    e3->style.color = HEX_TO_COLOR(0x00FFFF);
    e3->style.font = font;
    e3->style.size = 16;
    e3->style.words_warp = false;
    GUI_SET_X1(e3, 0, 0, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y1(e3, 0, 0, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_X2(e3, 0, 1, 0, GUI_ABS_SELF, GUI_REL_SELF);
    GUI_SET_Y2(e3, 0, 1, 0, GUI_ABS_SELF, GUI_REL_SELF);
    gui_append_elem(gui, GUI_CAST(e2), GUI_CAST(e3));


    // Main loop

    bool quit = false;
    while (!quit) {
        rndr_clear(rndr, RGB_TO_COLOR(20,20,20));
        gui_render(gui);
        rndr_clip(rndr, atlas->current, 0, 0);
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


    // Frees

    gui_free(gui);
    font_free(font);
    win_free(win);


    // Destroys

    font_destroy();
    input_destroy();
    SDL_Quit();


    return 0;
}
