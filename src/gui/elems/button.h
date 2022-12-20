#pragma once

#include "common.h"

#include "gui/gui.h"


typedef void (*gui_btn_click_cb)(void* userdata);


typedef struct gui_btn_s {
    struct gui_base_elem_s base;
    color_t normal;
    color_t hovered;
    color_t pressed;
    color_t disabled;
    gui_btn_click_cb onclick;
    color_t bg;
    void* userdata;
} *gui_btn_t;


GUI_DECL_TYPE(BUTTON)


gui_btn_t gui_new_btn();

void gui_free_btn(gui_btn_t btn);
