#include "frame.h"



void _gui_render_frame(gui_t gui, gui_elem_t elem, gui_rect_t viewport) {
    gui_frame_t frame = (gui_frame_t) elem;
    rndr_draw_rect(
        gui->renderer,
        frame->base.rect.x1, frame->base.rect.y1,
        frame->base.rect.x2, frame->base.rect.y2,
        frame->bg
    );
}



bool _gui_onevent_frame(gui_t gui, gui_elem_t elem, event_t* ev) {
    return true;
}



gui_frame_t gui_new_frame() {
    gui_frame_t e = (gui_frame_t*)
        gui_new_elem(GUI_TYPE_FRAME, sizeof(struct gui_frame_s));
    e->bg = RGBA_TO_COLOR(0,0,0,0);
    return e;
}



void gui_free_frame(gui_frame_t frame) {
    gui_free_elem(frame);
}



gui_type_t _GUI_TYPE_FRAME = {
    .id = GUI_TYPEID_FRAME,
    .render = _gui_render_frame,
    .onevent = _gui_onevent_frame
};
