#pragma once

#include "gui.h"


#define _GUI_EDGE_LEFT   0
#define _GUI_EDGE_RIGHT  1
#define _GUI_EDGE_TOP    2
#define _GUI_EDGE_BOTTOM 3

#define _GUI_AXE_X 0
#define _GUI_AXE_Y 1


static inline gui_relval_t _gui_relval_diff(gui_relval_t a, gui_relval_t b) {
    gui_relval_t r = {
        .abs = abs_i32(a.abs - b.abs),
        .phcoef = abs_f(a.phcoef - b.phcoef),
        .pwcoef = abs_f(a.pwcoef - b.pwcoef)
    };
    return r;
}


static inline gui_relval_t _gui_relval_scale(gui_relval_t a, float k) {
    a.abs = (int32_t) (k * a.abs);
    a.phcoef *= k;
    a.pwcoef *= k;
    return a;
}


static inline gui_relval_t _gui_relval_sum(gui_relval_t a, gui_relval_t b) {
    a.abs += b.abs;
    a.phcoef += b.phcoef;
    a.pwcoef += b.pwcoef;
    return a;
}


gui_relval_t _gui_abs_prev(gui_base_elem_t prev, uint8_t edge);
gui_relval_t _gui_abs_self(gui_base_elem_t elem, uint8_t edge);
int32_t _gui_abs_inline(gui_base_elem_t elem, gui_base_elem_t prev, uint8_t edge);
int32_t _gui_abs_baseline(gui_base_elem_t elem, gui_base_elem_t prev, uint8_t edge);


gui_relval_t _gui_rel_parent(gui_base_elem_t elem, uint8_t axe, float k);
gui_relval_t _gui_rel_prev(gui_base_elem_t prev, uint8_t axe, float k);
gui_relval_t _gui_rel_ratio(gui_base_elem_t elem, uint8_t axe, float k);
gui_relval_t _gui_rel_self(gui_base_elem_t elem, uint8_t axe, float k, uint8_t type);


void _gui_calc_relrect(
    gui_base_elem_t elem,
    gui_base_elem_t prev,
    gui_relrect_t* relrect
);
