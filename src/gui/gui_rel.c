#include "gui_private.h"


gui_relval_t _gui_rel_parent(gui_base_elem_t elem, uint8_t axe, float k) {
    gui_relval_t r = {};
    if (axe == _GUI_AXE_X)
        r.pwcoef = k;
    else
        r.phcoef = k;
    return r;
}



gui_relval_t _gui_rel_prev(gui_base_elem_t prev, uint8_t axe, float k) {
    gui_relval_t r = {};
    if (!prev) return r;
    if (axe == _GUI_AXE_X)
        r = _gui_relval_diff(prev->relrect.x2, prev->relrect.x1);
    else
        r = _gui_relval_diff(prev->relrect.y2, prev->relrect.y1);
    return _gui_relval_scale(r, k);
}



gui_relval_t _gui_rel_ratio(gui_base_elem_t elem, uint8_t axe, float k) {
    gui_relval_t r;
    if (axe == _GUI_AXE_Y)
        r = _gui_relval_diff(elem->relrect.x2, elem->relrect.x1);
    else
        r = _gui_relval_diff(elem->relrect.y2, elem->relrect.y1);
    return _gui_relval_scale(r, k);
}



static vect2i32_t _gui_calc_size(gui_base_elem_t elem, uint8_t type) {
    int32_t max_width  = -1;
    int32_t max_height = -1;

    if (elem->firstchild) {
        gui_base_elem_t prev = NULL;
        for (
            gui_base_elem_t child = &elem->firstchild->base;
            child;
            child = &child->sibling->base
        ) {
            gui_relrect_t relrect = {};
            _gui_calc_relrect(child, prev, &relrect);

            int32_t width  = abs_i32(relrect.x1.abs - relrect.x2.abs);
            int32_t height = abs_i32(relrect.y1.abs - relrect.y2.abs);

            float a = abs_f(relrect.x1.pwcoef - relrect.x2.pwcoef);
            float b = abs_f(relrect.x1.phcoef - relrect.x2.phcoef);
            float c = abs_f(relrect.y1.pwcoef - relrect.y2.pwcoef);
            float d = abs_f(relrect.y1.phcoef - relrect.y2.phcoef);

            if (a > 0) {
                int32_t s = (int32_t) (width / a);
                if (s > max_width) max_width = s;
            }

            if (b > 0) {
                int32_t s = (int32_t) (width / b);
                if (s > max_height) max_height = s;
            }

            if (c > 0) {
                int32_t s = (int32_t) (height / c);
                if (s > max_width) max_width = s;
            }

            if (d > 0) {
                int32_t s = (int32_t) (height / d);
                if (s > max_height) max_height = s;
            }

            prev = child;
        }
    }

    vect2i32_t ret = {.x = max_width, .y = max_height};
    return ret;
}



gui_relval_t _gui_rel_self(gui_base_elem_t elem, uint8_t axe, float k, uint8_t type){
    vect2i32_t size = _gui_calc_size(elem, type);

    if (elem->type->get_size) {
        vect2i32_t other = elem->type->get_size((gui_elem_t)elem, type);
        size.x = max_i32(size.x, other.x);
        size.y = max_i32(size.y, other.y);
    }

    if (size.x <= 0) size.x = GUI_DEFAULT_WIDTH;
    if (size.y <= 0) size.y = GUI_DEFAULT_HEIGHT;

    gui_relval_t ret = {
        .abs = (axe == _GUI_AXE_X) ? size.x : size.y,
        .phcoef = 0,
        .pwcoef = 0
    };

    return _gui_relval_scale(ret, k);
}
