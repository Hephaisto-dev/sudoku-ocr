#pragma once

#include "common.h"

#include "gui/gui.h"


typedef void (*gui_slider_change_cb)(void* userdata, float value);


typedef struct gui_slider_s {
    struct gui_base_elem_s base;
    gui_slider_change_cb onchange;
    void* userdata;
    color_t handle;
    color_t left;
    color_t right;
    float value;
} *gui_slider_t;


GUI_DECL_TYPE(SLIDER)


gui_slider_t gui_new_slider();

void gui_free_slider(gui_slider_t btn);
