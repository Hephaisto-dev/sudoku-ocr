#pragma once

#include "common.h"

#include "gui/gui.h"
#include "utils/string.h"
#include "utils/writer.h"


typedef struct gui_label_s {
    struct gui_base_elem_s base;
    string_t text;
    writer_params_t style;
    writer_data_t _wdata;
    color_t bg;
} *gui_label_t;


GUI_DECL_TYPE(LABEL)


gui_label_t gui_new_label();

void gui_free_label(gui_label_t label);
