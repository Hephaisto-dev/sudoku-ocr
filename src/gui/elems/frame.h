#pragma once

#include "common.h"

#include "gui/gui.h"


typedef struct gui_frame_s {
    struct gui_base_elem_s base;
    color_t bg;
} *gui_frame_t;

GUI_DECL_TYPE(FRAME)


gui_frame_t gui_new_frame();

void gui_free_frame(gui_frame_t frame);
