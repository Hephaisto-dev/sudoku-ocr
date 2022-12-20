#include "button.h"



void _gui_render_btn(gui_t gui, gui_elem_t elem, gui_rect_t viewport) {
    gui_btn_t data = (gui_btn_t) elem;
    rndr_draw_rect(
        gui->renderer,
        elem->base.rect.x1, elem->base.rect.y1,
        elem->base.rect.x2, elem->base.rect.y2,
        data->normal
    );
}



bool _gui_onevent_btn(gui_t gui, gui_elem_t elem, event_t* ev) {
    gui_btn_t data = (gui_btn_t) elem;
    if (ev->type == EVENT_MOUSE_RELEASE) {
        if (data->onclick) {
            data->onclick(data->userdata);
        }
    }
    return false;
}



gui_btn_t gui_new_btn() {
    gui_btn_t e = gui_new_elem(GUI_TYPE_BUTTON, sizeof(struct gui_btn_s));
    e->onclick = NULL;
    e->disabled = RGBA_TO_COLOR(120,120,120,0xFF);
    e->normal   = RGBA_TO_COLOR(200,200,200,0xFF);
    e->hovered  = RGBA_TO_COLOR(180,180,180,0xFF);
    e->pressed  = RGBA_TO_COLOR(150,150,150,0xFF);
    e->userdata = NULL;
    return e;
}



void gui_free_btn(gui_btn_t btn) {
    gui_free_elem(btn);
}



gui_type_t _GUI_TYPE_BUTTON = {
    .id = GUI_TYPEID_BUTTON,
    .render = _gui_render_btn,
    .onevent = _gui_onevent_btn
};
