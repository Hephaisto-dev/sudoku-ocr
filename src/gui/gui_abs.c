#include "gui_private.h"



gui_relval_t _gui_abs_prev(gui_base_elem_t prev, uint8_t edge) {
    gui_relval_t r = {};
    if (!prev) return r;
    switch (edge) {
        case _GUI_EDGE_LEFT:
            return prev->relrect.x2;
        case _GUI_EDGE_RIGHT:
            return prev->relrect.x1;
        case _GUI_EDGE_TOP:
            return prev->relrect.y2;
        case _GUI_EDGE_BOTTOM:
            return prev->relrect.y1;
    }
    // TODO : Error
    return r;
}



gui_relval_t _gui_abs_self(gui_base_elem_t elem, uint8_t edge) {
    switch (edge) {
        case _GUI_EDGE_LEFT:
            return elem->relrect.x2;
        case _GUI_EDGE_RIGHT:
            return elem->relrect.x1;
        case _GUI_EDGE_TOP:
            return elem->relrect.y2;
        case _GUI_EDGE_BOTTOM:
            return elem->relrect.y1;
    }
    // TODO : Error
    gui_relval_t r = {};
    return r;
}



// Be carefull, prev can be NULL
int32_t _gui_abs_inline(gui_base_elem_t elem, gui_base_elem_t prev, uint8_t edge) {
    return 0; // TODO : abs inline
}



// Be carefull, prev can be NULL
int32_t _gui_abs_baseline(gui_base_elem_t elem, gui_base_elem_t prev, uint8_t edge) {
    return 0; // TODO : abs baseline
}
