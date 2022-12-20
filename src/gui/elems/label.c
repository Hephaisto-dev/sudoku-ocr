#include "label.h"



void _gui_render_label(gui_t gui, gui_elem_t elem, gui_rect_t viewport) {
    gui_label_t data = (gui_label_t) elem;
    if (!data->text) return;
    if (!data->style.font) return;

    if (data->_wdata.chars)
        free(data->_wdata.chars);

    writer_calc_data(
        data->text,
        &data->_wdata,
        gui->atlas,
        gui->dpi,
        &data->style,
        elem->base.rect.x2 - elem->base.rect.x1,
        elem->base.rect.y2 - elem->base.rect.y1
    );

    if (data->bg.a > 0) {
        rndr_draw_rect(
            gui->renderer,
            elem->base.rect.x1, elem->base.rect.y1,
            elem->base.rect.x2, elem->base.rect.y2,
            RGB_TO_COLOR(255,0,0)
        );
    }

    writer_rndr_text(
        gui->renderer,
        &data->_wdata,
        &data->style,
        (elem->base.rect.x1 + elem->base.rect.x2) / 2 - data->_wdata.width / 2,
        (elem->base.rect.y1 + elem->base.rect.y2) / 2 - data->_wdata.height / 2
    );
}


vect2i32_t _gui_size_label(gui_elem_t elem, uint8_t type) {
    gui_label_t data = (gui_label_t) elem;
    return (vect2i32_t) {.x = data->_wdata.width, .y = data->_wdata.height};
}


bool _gui_onevent_label(gui_t gui, gui_elem_t elem, event_t* ev) {
    return true;
}



gui_label_t gui_new_label() {
    gui_label_t e = (gui_label_t)
        gui_new_elem(GUI_TYPE_LABEL, sizeof(struct gui_label_s));
    e->style.color = HEX_TO_COLOR(0xFFFFFF);
    e->style.font = NULL;
    e->style.size = 12;
    e->style.halign = WRITER_ALIGN_LEFT;
    e->style.valign = WRITER_ALIGN_TOP;
    e->style.tab_size = 4;
    e->style.line_height = 1;
    e->style.words_warp = false;
    e->text = NULL;
    e->bg = RGBA_TO_COLOR(0,0,0,0);
    e->_wdata.chars = NULL;
    return e;
}



void gui_free_label(gui_label_t label) {
    // TODO : Free ._wdata
    gui_free_elem(label);
}



gui_type_t _GUI_TYPE_LABEL = {
    .id = GUI_TYPEID_LABEL,
    .render = _gui_render_label,
    .onevent = _gui_onevent_label,
    .get_size = _gui_size_label
};
