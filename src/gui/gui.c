#include "gui_private.h"

#include <stdlib.h>


struct gui_type_s _gui_root_type = {};



gui_t gui_new(renderer_t renderer, atlas_t atlas, uint16_t dpi) {
    gui_t e = (gui_t) malloc(sizeof(struct gui_s));
    e->renderer = renderer;
    e->root = gui_new_elem(&_gui_root_type, sizeof(struct gui_elem_s));
    e->update = NULL;
    e->focused = e->root;
    e->dpi = dpi;
    e->atlas = atlas;
    return e;
}



void _gui_free_rec(gui_elem_t e) {
    for (gui_elem_t c = e->base.firstchild; c;) {
        gui_elem_t n = c->base.sibling;
        _gui_free_rec(c);
        c = n;
    }
    gui_free_elem(e);
}


void gui_free(gui_t gui) {
    _gui_free_rec(gui->root);
    free(gui);
}



gui_elem_t gui_new_elem(const gui_type_t* type, size_t size) {
    gui_elem_t e = (gui_elem_t) calloc(1, size);
    e->base.depth = -1;
    e->base.type = type;
    e->base.pos.x1.max.abs = INT16_MAX;
    e->base.pos.x1.min.abs = INT16_MIN;
    e->base.pos.x2.max.abs = INT16_MAX;
    e->base.pos.x2.min.abs = INT16_MIN;
    e->base.pos.y1.max.abs = INT16_MAX;
    e->base.pos.y1.min.abs = INT16_MIN;
    e->base.pos.y2.max.abs = INT16_MAX;
    e->base.pos.y2.min.abs = INT16_MIN;
    return e;
}



void gui_free_elem(gui_elem_t elem) {
    free(elem);
}



void gui_append_elem(gui_t gui, void* _r, void* _e) {
    gui_elem_t r = (gui_elem_t) _r;
    gui_elem_t e = (gui_elem_t) _e;
    if (r->base.lastchild) {
        r->base.lastchild->base.sibling = e;
    } else {
        r->base.firstchild = e;
    }
    r->base.lastchild = e;
    e->base.parent = r;
    e->base.depth = r->base.depth + 1;
    gui_invalidate(gui, e);
}



void gui_prepend_elem(gui_t gui, gui_elem_t r, gui_elem_t e) {
    if (r->base.firstchild) {
        e->base.sibling = r->base.firstchild;
    } else {
        r->base.lastchild = e;
    }
    r->base.firstchild = e;
    e->base.parent = r;
    e->base.depth = r->base.depth + 1;
    gui_invalidate(gui, e);
}



/*
Step to compute an element rect:
    - Compute optimal, maximum and minimum size (if needed)
    - Compute posvals with GUI_REL_RATIO, GUI_ABS_SELF and .self to 0
    - Compute posvals GUI_ABS_SELF
    - Compute posvals GUI_REL_RATIO
    - Compute posvals .self
    - Compute min & max by using reference from the already computed rect
    - Apply min & max

Compute optimal size:
    - Compute element rect normaly except for:
        - GUI_REL_PARENT
        - GUI_ABS_SELF
        - GUI_ABS_PREV
*/



static gui_rect_t _gui_normalize_rect(gui_rect_t rect) {
    if (rect.x1 > rect.x2) {
        int32_t x2 = rect.x2;
        rect.x2 = rect.x1;
        rect.x1 = x2;
    }
    if (rect.y1 > rect.y2) {
        int32_t y2 = rect.y2;
        rect.y2 = rect.y1;
        rect.y1 = y2;
    }
    return rect;
}



static inline uint8_t _gui_edge_to_axe(uint8_t edge) {
    return edge >> 1;
}



// Compute the posval with GUI_ABS_SELF and GUI_REL_RATIO to 0
static void _gui_calc_pos_step1(
        gui_base_elem_t elem,
        gui_base_elem_t prev,
        gui_posval_t* pos,
        gui_relval_t* rel,
        uint8_t edge,
        uint8_t self // The self size (optimal, mininum, maximum)
) {
    const uint8_t axe = _gui_edge_to_axe(edge);

    gui_relval_t ret = {
        .abs = pos->abs,
        .phcoef = 0,
        .pwcoef = 0
    };

    switch (pos->refs & 0x0F)
    {
        case GUI_ABS_UNSET:
        case GUI_ABS_PARENT:
            break;

        case GUI_ABS_SELF:
            // Special case because we need to compute values while ignoring
            // self offset
            break;

        case GUI_ABS_PREV:
            ret = _gui_relval_sum(ret,
                _gui_abs_prev(prev, edge)
            );
            break;

        case GUI_ABS_INLINE:
            ret.abs += _gui_abs_inline(elem, prev, _gui_edge_to_axe(edge));
            break;

        case GUI_ABS_BASELINE:
            ret.abs += _gui_abs_baseline(elem, prev, _gui_edge_to_axe(edge));
            break;

        default:
            // TODO : Error
            break;
    }

    switch (pos->refs & 0xF0)
    {
        case GUI_REL_PARENT:
            ret = _gui_relval_sum(ret,
                _gui_rel_parent(elem, axe, pos->rel)
            );
            break;

        case GUI_REL_SELF:
            ret = _gui_relval_sum(ret,
                _gui_rel_self(elem, axe, pos->rel, self)
            );
            break;

        case GUI_REL_RATIO:
            // Special case because we need to compute values while ignoring
            // ratios first
            break;

        case GUI_REL_PREV:
            ret = _gui_relval_sum(ret,
                _gui_rel_prev(prev, axe, pos->rel)
            );
            break;

        case GUI_REL_UNSET:
            break;

        default:
            // TODO : Error
            break;
    }

    *rel = ret;
}



// Add GUI_ABS_SELF
static void _gui_calc_pos_step2(
    gui_base_elem_t elem,
    gui_posval_t* pos,
    gui_relval_t* rel,
    uint8_t axe
) {
    if ((pos->refs & 0x0F) == GUI_ABS_SELF) {
        if (axe == _GUI_AXE_X) {
            *rel = _gui_relval_sum(
                elem->relrect.x1,
                elem->relrect.x2
            );
        } else {
            *rel = _gui_relval_sum(
                elem->relrect.y1,
                elem->relrect.y2
            );
        }
    }
}



// Add GUI_REL_RATIO
static void _gui_calc_pos_step3(
    gui_base_elem_t elem,
    gui_posval_t* pos,
    gui_relval_t* rel,
    uint8_t axe
) {
    if ((pos->refs & 0xF0) == GUI_REL_RATIO) {
        *rel = _gui_relval_sum(*rel,
            _gui_rel_ratio(elem, axe, pos->rel)
        );
    }
}



// Add .self
static void _gui_calc_pos_step4(
    gui_base_elem_t elem,
    gui_posval_t* pos,
    gui_relval_t* rel,
    uint8_t axe
) {
    if (axe == _GUI_AXE_X) {
        *rel = _gui_relval_sum(*rel,
            _gui_relval_scale(
                _gui_relval_diff(elem->relrect.x1, elem->relrect.x2),
                pos->self
            )
        );
    } else {
        *rel = _gui_relval_sum(*rel,
            _gui_relval_scale(
                _gui_relval_diff(elem->relrect.y1, elem->relrect.y2),
                pos->self
            )
        );
    }
}



static inline void _gui_calc_pos(
    gui_base_elem_t elem,
    gui_base_elem_t prev,
    gui_posval_t* pos,
    gui_relval_t* rel,
    uint8_t edge,
    uint8_t self
) {
    _gui_calc_pos_step1(elem, prev, pos, rel, edge, self);
    _gui_calc_pos_step2(elem, pos, rel, edge);
    _gui_calc_pos_step3(elem, pos, rel, _gui_edge_to_axe(edge));
    _gui_calc_pos_step4(elem, pos, rel, _gui_edge_to_axe(edge));
}



static inline int32_t _gui_relval_to_val(gui_relval_t v, float pw, float ph) {
    return v.abs + (int32_t) (v.phcoef * ph) + (int32_t) (v.pwcoef * pw);
}



static gui_rect_t _gui_relrect_to_rect(
    gui_relrect_t* relrect,
    int32_t pw,
    int32_t ph
) {
    gui_rect_t rect = {
        .x1 = _gui_relval_to_val(relrect->x1, pw, ph),
        .x2 = _gui_relval_to_val(relrect->x2, pw, ph),
        .y1 = _gui_relval_to_val(relrect->y1, pw, ph),
        .y2 = _gui_relval_to_val(relrect->y2, pw, ph)
    };
    return rect;
}



void _gui_calc_relrect(
    gui_base_elem_t elem,
    gui_base_elem_t prev,
    gui_relrect_t* relrect
) {
    // Compute base position step 1
    _gui_calc_pos_step1(
        elem, prev,
        &elem->pos.x1.val, &relrect->x1,
        _GUI_EDGE_LEFT,   GUI_SELF_OPT
    );
    _gui_calc_pos_step1(
        elem, prev,
        &elem->pos.x2.val, &relrect->x2,
        _GUI_EDGE_RIGHT,  GUI_SELF_OPT
    );
    _gui_calc_pos_step1(
        elem, prev,
        &elem->pos.y1.val, &relrect->y1,
        _GUI_EDGE_TOP,    GUI_SELF_OPT
    );
    _gui_calc_pos_step1(
        elem, prev,
        &elem->pos.y2.val, &relrect->y2,
        _GUI_EDGE_BOTTOM, GUI_SELF_OPT
    );

    // Compute base position step 2
    _gui_calc_pos_step2(
        elem,
        &elem->pos.x2.val, &relrect->x2,
        _GUI_AXE_X
    );
    _gui_calc_pos_step2(
        elem,
        &elem->pos.y1.val, &relrect->y1,
        _GUI_AXE_Y
    );
    _gui_calc_pos_step2(
        elem,
        &elem->pos.y2.val, &relrect->y2,
        _GUI_AXE_Y
    );
    _gui_calc_pos_step2(
        elem,
        &elem->pos.x1.val, &relrect->x1,
        _GUI_AXE_X
    );

    // Compute base position step 3
    _gui_calc_pos_step3(elem, &elem->pos.x1.val, &relrect->x1, _GUI_AXE_X);
    _gui_calc_pos_step3(elem, &elem->pos.x2.val, &relrect->x2, _GUI_AXE_X);
    _gui_calc_pos_step3(elem, &elem->pos.y1.val, &relrect->y1, _GUI_AXE_Y);
    _gui_calc_pos_step3(elem, &elem->pos.y2.val, &relrect->y2, _GUI_AXE_Y);

    // Compute base position step 4
    _gui_calc_pos_step4(elem, &elem->pos.x1.val, &relrect->x1, _GUI_AXE_X);
    _gui_calc_pos_step4(elem, &elem->pos.x2.val, &relrect->x2, _GUI_AXE_X);
    _gui_calc_pos_step4(elem, &elem->pos.y1.val, &relrect->y1, _GUI_AXE_Y);
    _gui_calc_pos_step4(elem, &elem->pos.y2.val, &relrect->y2, _GUI_AXE_Y);
}



void _gui_calc_rect(gui_base_elem_t elem, gui_base_elem_t prev) {
    _gui_calc_relrect(elem, prev, &elem->relrect);


    // Compute minimums

    gui_relrect_t relminrect = {};

    _gui_calc_pos(
        elem, prev,
        &elem->pos.x1.min, &relminrect.x1,
        _GUI_EDGE_LEFT, GUI_SELF_MIN
    );

    _gui_calc_pos(
        elem, prev,
        &elem->pos.x2.min, &relminrect.x2,
        _GUI_EDGE_LEFT, GUI_SELF_MIN
    );

    _gui_calc_pos(
        elem, prev,
        &elem->pos.y1.min, &relminrect.y1,
        _GUI_EDGE_LEFT, GUI_SELF_MIN
    );

    _gui_calc_pos(
        elem, prev,
        &elem->pos.y2.min, &relminrect.y2,
        _GUI_EDGE_LEFT, GUI_SELF_MIN
    );


    // Compute maximums

    gui_relrect_t relmaxrect = {};

    _gui_calc_pos(
        elem, prev,
        &elem->pos.x1.max, &relmaxrect.x1,
        _GUI_EDGE_LEFT, GUI_SELF_MAX
    );

    _gui_calc_pos(
        elem, prev,
        &elem->pos.x2.max, &relmaxrect.x2,
        _GUI_EDGE_LEFT, GUI_SELF_MAX
    );

    _gui_calc_pos(
        elem, prev,
        &elem->pos.y1.max, &relmaxrect.y1,
        _GUI_EDGE_LEFT, GUI_SELF_MAX
    );

    _gui_calc_pos(
        elem, prev,
        &elem->pos.y2.max, &relmaxrect.y2,
        _GUI_EDGE_LEFT, GUI_SELF_MAX
    );


    // Compute rects

    float pw = (float)
        abs_i32(elem->parent->base.rect.x1 - elem->parent->base.rect.x2);
    float ph = (float)
        abs_i32(elem->parent->base.rect.y1 - elem->parent->base.rect.y2);
    elem->rect = _gui_relrect_to_rect(&elem->relrect, pw, ph);
    gui_rect_t minrect = _gui_relrect_to_rect(&relminrect, pw, ph);
    gui_rect_t maxrect = _gui_relrect_to_rect(&relmaxrect, pw, ph);


    // Apply maximums

    elem->rect.x1 = max_i32(minrect.x1, elem->rect.x1);
    elem->rect.x2 = max_i32(minrect.x2, elem->rect.x2);
    elem->rect.y1 = max_i32(minrect.y1, elem->rect.y1);
    elem->rect.y2 = max_i32(minrect.y2, elem->rect.y2);


    // Apply maximums

    elem->rect.x1 = min_i32(maxrect.x1, elem->rect.x1);
    elem->rect.x2 = min_i32(maxrect.x2, elem->rect.x2);
    elem->rect.y1 = min_i32(maxrect.y1, elem->rect.y1);
    elem->rect.y2 = min_i32(maxrect.y2, elem->rect.y2);

    // Normalize
    elem->rect = _gui_normalize_rect(elem->rect);
}



void _gui_calc_rects(gui_base_elem_t parent) {
    gui_base_elem_t prev = NULL;
    for (
        gui_base_elem_t e = &parent->firstchild->base;
        e;
        e = &e->sibling->base
    ) {
        _gui_calc_rect(e, prev);
        e->rect.x1 += parent->rect.x1;
        e->rect.y1 += parent->rect.y1;
        e->rect.x2 += parent->rect.x1;
        e->rect.y2 += parent->rect.y1;
        _gui_calc_rects(e);
        prev = e;
    }
}



void _gui_render_elem(gui_t gui, gui_elem_t e, gui_rect_t viewport) {
    if (e->base.viewport != NULL)
        viewport = *e->base.viewport;
    if (e->base.type->render)
        e->base.type->render(gui, e, viewport);
    for (e = e->base.firstchild; e; e = e->base.sibling)
        _gui_render_elem(gui, e, viewport);
}



void gui_render(gui_t gui) {
    // Update root element size
    vect2i32_t rootsize = rndr_get_size(gui->renderer);
    gui->root->base.rect.x1 = 0;
    gui->root->base.rect.y1 = 0;
    gui->root->base.rect.x2 = rootsize.x;
    gui->root->base.rect.y2 = rootsize.y;

    if (gui->update != NULL) {
        // Update rects that need to recalculated
        gui_base_elem_t e =
            (gui->update != gui->root) ?
                &gui->update->base.parent->base :
                &gui->update->base;
        if (e) {
            _gui_calc_rects(e);
        } else { // If gui->update is the root
            for (e = &e->firstchild->base; e; e = &e->sibling->base)
                _gui_calc_rects(e);
        }
        gui->update = NULL;
    }

    gui_rect_t viewport = {
        .x1 = 0,
        .y1 = 0,
        .x2 = rootsize.x,
        .y2 = rootsize.y
    };

    _gui_render_elem(gui, gui->root, viewport);
}



void gui_invalidate(gui_t gui, gui_elem_t e) {
    if (!gui->update || e->base.depth < gui->update->base.depth) {
        gui->update = e;
    }
}



static inline bool _gui_rect_collid(gui_rect_t* rect, int32_t x, int32_t y) {
    return (x >= rect->x1 && x <= rect->x2 && y >= rect->y1 && y <= rect->y2);
}



static size_t _gui_get_hovered_elems(
    gui_elem_t elem,
    int32_t x, int32_t y,
    gui_elem_t* r, size_t max
) {
    size_t founds = 0;
    if (_gui_rect_collid(&elem->base.rect, x, y)) {
        *(r++) = elem;
        founds++;
    }
    for (
        elem = elem->base.firstchild;
        elem && max > founds;
        elem = elem->base.sibling
    ) {
        founds += _gui_get_hovered_elems(elem, x, y, r, max);
    }
    return founds;
}



static void _gui_dispatch_events(gui_t gui, gui_elem_t* elems, size_t count, event_t* ev) {
    while (count--) {
        gui_elem_t e = elems[count];
        //mprintf("Dispatch event %i to %p\n", ev->type, e);
        // Stop propagation if onevent return false or onevent is NULL
        if (!e->base.type->onevent)
            return;
        if (!elems[count]->base.type->onevent(gui, elems[count], ev))
            return;
    }
}



void gui_process_event(gui_t gui, event_t* ev) {
    const size_t max = 128;
    gui_elem_t elems[max];
    size_t count = 1;
    elems[0] = gui->root;

    switch (ev->type) {
        case EVENT_KEY_PRESS:
        case EVENT_KEY_RELEASE:
            elems[count++] = gui->focused;
            break;

        case EVENT_MOUSE_PRESS:
        case EVENT_MOUSE_RELEASE:
            gui->cursor.x = ev->motion.x;
            gui->cursor.y = ev->motion.y;
            count = _gui_get_hovered_elems(
                gui->root,
                gui->cursor.x, gui->cursor.y,
                elems, max
            );
            break;

        case EVENT_MOUSE_SCROLL:
            count = _gui_get_hovered_elems(
                gui->root,
                gui->cursor.x, gui->cursor.y,
                elems, max
            );
            break;

        case EVENT_MOUSE_MOVE:
            gui->cursor.x = ev->motion.x;
            gui->cursor.y = ev->motion.y;
            if (gui->focused) {
                elems[count++] = gui->focused;
            }
            break;

        default:
            return;
    }

    if (ev->type == EVENT_MOUSE_PRESS && count > 0) {
        gui->focused = elems[count - 1];
        mprintf("Give focus to %p\n", elems[count - 1]);
    }

    _gui_dispatch_events(gui, elems, count, ev);
}
