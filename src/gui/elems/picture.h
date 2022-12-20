#pragma once

#include "common.h"

#include "gui/gui.h"


typedef struct gui_picture_s {
    struct gui_base_elem_s base;
    color_t bg;
    image_t img;
} *gui_picture_t;


GUI_DECL_TYPE(PICTURE)


gui_picture_t gui_new_picture();

void gui_free_picture(gui_picture_t label);
