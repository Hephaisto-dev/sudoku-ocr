#include "button.h"



void _gui_render_slider(gui_t gui, gui_elem_t elem, gui_rect_t viewport) {
    gui_slider_t data = (gui_slider_t) elem;

    int32_t width = elem->base.rect.x2 - elem->base.rect.x1;
    int32_t height = elem->base.rect.y2 - elem->base.rect.y1;

    int32_t dx = (int32_t) (width * data->value);
    int32_t dy = height / 2;

    // Draw left bar
    rndr_draw_rect(
        gui->renderer,
        elem->base.rect.x1, elem->base.rect.y1 + dy - 4,
        elem->base.rect.x1 + dx, elem->base.rect.y1 + dy + 4,
        data->left
    );

    // Draw right bar
    rndr_draw_rect(
        gui->renderer,
        elem->base.rect.x1 + dx, elem->base.rect.y1 + dy - 4,
        elem->base.rect.x2, elem->base.rect.y1 + dy + 4,
        data->right
    );

    // Draw handle
    rndr_draw_circle(gui->renderer, elem->base.rect.x1 + dx, elem->base.rect.y1 + dy, 10, data->handle);
}



bool _gui_onevent_slider(gui_t gui, gui_elem_t elem, event_t* ev) {
    gui_slider_t data = (gui_slider_t) elem;
    if (ev->type == EVENT_MOUSE_MOVE && input_is_mouse_pressed()) {
        int32_t dx = ev->motion.x - elem->base.rect.x1;
        int32_t width = elem->base.rect.x2 - elem->base.rect.x1;
        data->value = min_f(1, max_f(0, (float) dx / (float) width));
        //mprintf("Update value to %f", data->value);
        if (data->onchange) {
            data->onchange(data->userdata, data->value);
        }
    }
    return false;
}



gui_slider_t gui_new_slider() {
    gui_slider_t e = gui_new_elem(GUI_TYPE_SLIDER, sizeof(struct gui_slider_s));
    e->onchange = NULL;
    e->userdata = NULL;
    e->handle = HEX_TO_COLOR(0xFFFFFF);
    e->left   = HEX_TO_COLOR(0xAAAAAA);
    e->right  = HEX_TO_COLOR(0x777777);
    e->value = 0.5;
    return e;
}



void gui_free_slider(gui_slider_t btn) {
    gui_free_elem(btn);
}



gui_type_t _GUI_TYPE_SLIDER = {
    .id = GUI_TYPEID_SLIDER,
    .render = _gui_render_slider,
    .onevent = _gui_onevent_slider
};

