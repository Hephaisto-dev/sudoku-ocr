#include "switch.h"



void _gui_render_switch(gui_t gui, gui_elem_t elem, gui_rect_t viewport) {
    gui_switch_t data = (gui_switch_t) elem;

    int32_t width = elem->base.rect.x2 - elem->base.rect.x1;
    int32_t height = elem->base.rect.y2 - elem->base.rect.y1;

    int32_t dx = (data->stats) ? (width - 5) : (5);
    int32_t dy = height / 2;

    // Draw bar
    if (data->stats) {
        rndr_draw_rect(
            gui->renderer,
            elem->base.rect.x1, elem->base.rect.y1 + dy - 4,
            elem->base.rect.x1 + dx, elem->base.rect.y1 + dy + 4,
            data->enabled
        );
    } else {
        rndr_draw_rect(
            gui->renderer,
            elem->base.rect.x1 + dx, elem->base.rect.y1 + dy - 4,
            elem->base.rect.x2, elem->base.rect.y1 + dy + 4,
            data->disabled
        );
    }

    // Draw handle
    rndr_draw_circle(gui->renderer, elem->base.rect.x1 + dx, elem->base.rect.y1 + dy, 10, data->handle);
}



bool _gui_onevent_switch(gui_t gui, gui_elem_t elem, event_t* ev) {
    gui_switch_t data = (gui_switch_t) elem;
    if (ev->type == EVENT_MOUSE_RELEASE) {
        if (data->onclick) {
            data->onclick(data->userdata);
        }
        data->stats = !data->stats;
    }
    return false;
}



gui_switch_t gui_new_switch() {
    gui_switch_t e = gui_new_elem(GUI_TYPE_SWITCH, sizeof(struct gui_switch_s));
    e->onclick = NULL;
    e->disabled = RGB_TO_COLOR(120,120,120);
    e->enabled  = RGB_TO_COLOR(200,200,200);
    e->handle   = RGB_TO_COLOR(180,180,180);
    e->userdata = NULL;
    e->stats = false;
    return e;
}



void gui_free_switch(gui_switch_t btn) {
    gui_free_elem(btn);
}



gui_type_t _GUI_TYPE_SWITCH = {
    .id = GUI_TYPEID_SWITCH,
    .render = _gui_render_switch,
    .onevent = _gui_onevent_switch
};
