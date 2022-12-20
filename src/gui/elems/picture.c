#include "picture.h"

#include "image/postprocess.h"


void _gui_render_picture(gui_t gui, gui_elem_t elem, gui_rect_t viewport) {
    gui_picture_t data = (gui_picture_t) elem;
    int32_t width = elem->base.rect.x2 - elem->base.rect.x1;
    int32_t height = elem->base.rect.y2 - elem->base.rect.y1;
    if (data->bg.a > 0) {
        rndr_draw_rect(
            gui->renderer,
            elem->base.rect.x1, elem->base.rect.y1,
            elem->base.rect.x2, elem->base.rect.y2,
            data->bg
        );
    }
    if (data->img != NULL) {
        float ratio = data->img->width / data->img->height;
        int newimgh = (ratio * width) / 2;
        image_t resized = img_resize(data->img, width / 2, newimgh);
        texture_t tex = tex_new_from_img(gui->renderer, resized);
        rndr_clip(gui->renderer, tex, elem->base.rect.x1, elem->base.rect.y1 + (height - newimgh) / 2);
        img_free(resized);
    }
}



bool _gui_onevent_picture(gui_t gui, gui_elem_t elem, event_t* ev) {
    return true;
}



gui_picture_t gui_new_picture() {
    gui_picture_t e = gui_new_elem(GUI_TYPE_PICTURE, sizeof(struct gui_picture_s));
    e->img = NULL;
    e->bg = RGBA_TO_COLOR(0,0,0,0);
    return e;
}



void gui_free_picture(gui_picture_t btn) {
    gui_free_elem(btn);
}



gui_type_t _GUI_TYPE_PICTURE = {
    .id = GUI_TYPEID_PICTURE,
    .render = _gui_render_picture,
    .onevent = _gui_onevent_picture
};

