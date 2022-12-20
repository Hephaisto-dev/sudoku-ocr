#pragma once

#include "common.h"

#include "gui/gui.h"


typedef void (*gui_switch_click_cb)(void* userdata);


typedef struct gui_switch_s {
    struct gui_base_elem_s base;
    color_t disabled;
    color_t enabled;
    color_t handle;
    gui_switch_click_cb onclick;
    color_t bg;
    bool stats;
    void* userdata;
} *gui_switch_t;


GUI_DECL_TYPE(SWITCH)


gui_switch_t gui_new_switch();

void gui_free_switch(gui_switch_t btn);
