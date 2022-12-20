#pragma once

#include "common.h"

#include "graphic/image.h"
#include "graphic/font.h"
#include "utils/string.h"
#include "utils/vector.h"
#include "graphic/renderer.h"
#include "system/input.h"
#include "graphic/atlas.h"



typedef enum gui_elem_type_id_e {
    GUI_TYPEID_FRAME,
    GUI_TYPEID_BUTTON,
    GUI_TYPEID_PICTURE,
    GUI_TYPEID_LABEL,
    GUI_TYPEID_UNDEFINED,
    GUI_TYPEID_SLIDER,
    GUI_TYPEID_SWITCH
} gui_elem_type_id_t;



#define GUI_SET_VAL(elem, a, r, s, aref, rref) \
    (elem).val.abs  = (a); \
    (elem).val.rel  = (r); \
    (elem).val.self = (s); \
    (elem).val.refs = (aref) | (rref);


#define GUI_SET_X1(elem, a, r, s, aref, rref) \
    GUI_SET_VAL((elem)->base.pos.x1, a, r, s, aref, rref)

#define GUI_SET_X2(elem, a, r, s, aref, rref) \
    GUI_SET_VAL((elem)->base.pos.x2, a, r, s, aref, rref)

#define GUI_SET_Y1(elem, a, r, s, aref, rref) \
    GUI_SET_VAL((elem)->base.pos.y1, a, r, s, aref, rref)

#define GUI_SET_Y2(elem, a, r, s, aref, rref) \
    GUI_SET_VAL((elem)->base.pos.y2, a, r, s, aref, rref)



#define GUI_DECL_TYPE(name) \
    extern gui_type_t _GUI_TYPE_##name; \
    static const gui_type_t* const GUI_TYPE_##name = &_GUI_TYPE_##name;


#define GUI_DECL_ELEM(name, data) \
    typedef struct {gui_base_elem_t; } name;


#define GUI_CAST(e) ((gui_elem_t)e)


#define GUI_ABS_UNSET    (0 << 0)
#define GUI_ABS_PARENT   (1 << 0)
#define GUI_ABS_PREV     (2 << 0)
#define GUI_ABS_SELF     (3 << 0)
#define GUI_ABS_BASELINE (4 << 0)
#define GUI_ABS_INLINE   (5 << 0)

#define GUI_REL_UNSET    (0 << 4)
#define GUI_REL_PARENT   (1 << 4)
#define GUI_REL_PREV     (2 << 4)
#define GUI_REL_SELF     (3 << 4) // Self optimal/min/max size
#define GUI_REL_RATIO    (4 << 4)


#define GUI_SELF_MIN (0)
#define GUI_SELF_MAX (1)
#define GUI_SELF_OPT (2)


#define GUI_DEFAULT_WIDTH (60)
#define GUI_DEFAULT_HEIGHT (60)


/*
#define GUI_REF_PARENT_END    1 // The size of the parent and start from top left corner of the parent
#define GUI_REF_PARENT_START  2 // The top of the end of the last element
#define GUI_REF_PARENT_TOP    3 // The text baseline of the last element (top if no baseline)
#define GUI_REF_PARENT_BOTTOM 4

#define GUI_REF_LAST_END
#define GUI_REF_LAST_START
#define GUI_REF_LAST_TOP
#define GUI_REF_LAST_BOTTOM

#define GUI_REF_NEXT_END
#define GUI_REF_NEXT_START
#define GUI_REF_NEXT_TOP
#define GUI_REF_NEXT_BOTTOM
*/


typedef struct gui_posval_s {
    float rel;
    float self;
    int16_t abs;
    uint16_t refs;
} gui_posval_t;


typedef struct gui_posdim_s {
    gui_posval_t val;
    gui_posval_t min;
    gui_posval_t max;
} gui_posdim_t;


typedef struct gui_pos_s {
    gui_posdim_t x1;
    gui_posdim_t y1;
    gui_posdim_t x2;
    gui_posdim_t y2;
} gui_pos_t;


typedef struct gui_rect_s {
    int32_t x1;
    int32_t y1;
    int32_t x2;
    int32_t y2;
} gui_rect_t;


typedef struct gui_relval_s {
    float pwcoef; // Proportion of the parent width
    float phcoef; // Proportion of the parent height
    int32_t abs; // Absolute offset
} gui_relval_t;


typedef struct gui_relrect_s {
    gui_relval_t x1;
    gui_relval_t y1;
    gui_relval_t x2;
    gui_relval_t y2;
} gui_relrect_t;


struct gui_s;
struct gui_elem_s;
struct gui_type_s;


typedef struct gui_base_elem_s {
    gui_pos_t pos;
    gui_rect_t rect;
    gui_relrect_t relrect;

    gui_rect_t* viewport;

    struct gui_elem_s* parent;
    struct gui_elem_s* firstchild;
    struct gui_elem_s* lastchild;
    struct gui_elem_s* sibling;
    int16_t depth; // Distance to the root

    const struct gui_type_s* type;
} *gui_base_elem_t;


typedef struct gui_elem_s {
    struct gui_base_elem_s base;
} *gui_elem_t;


typedef vect2i32_t (*gui_get_size_cb_t)(gui_elem_t elem, uint8_t type);

typedef int32_t (*gui_get_baseline_cb_t)(gui_elem_t elem);

typedef int32_t (*gui_get_inline_cb_t)(gui_elem_t elem);

typedef bool (*gui_onevent_cb_t)(struct gui_s* gui, gui_elem_t elem, event_t* event);

typedef void (*gui_render_cb_t)(
    struct gui_s* gui,
    gui_elem_t elem,
    gui_rect_t viewport
);


typedef struct gui_type_s {
    gui_elem_type_id_t id;

    gui_get_size_cb_t get_size;

    gui_get_baseline_cb_t get_baseline_x;
    gui_get_baseline_cb_t get_baseline_y;

    gui_get_inline_cb_t get_inline_x;
    gui_get_inline_cb_t get_inline_y;

    gui_onevent_cb_t onevent;

    gui_render_cb_t render;
} gui_type_t;


typedef struct gui_s {
    gui_elem_t root;
    gui_elem_t update;
    gui_elem_t focused;
    renderer_t renderer;
    vect2i32_t cursor;
    atlas_t atlas;
    uint16_t dpi;
} *gui_t;


gui_t gui_new(renderer_t renderer, atlas_t atlas, uint16_t dpi);

void gui_free(gui_t gui);

gui_elem_t gui_new_elem(const gui_type_t* type, size_t size);

void gui_free_elem(gui_elem_t elem);

void gui_append_elem(gui_t gui, void* _r, void* _e);
void gui_prepend_elem(gui_t gui, gui_elem_t r, gui_elem_t e);

void gui_render(gui_t gui);

// Mark an element as updated (recompute position at next gui_render)
void gui_invalidate(gui_t gui, gui_elem_t e);

void gui_process_event(gui_t gui, event_t* ev);



// Elements


/*

// Button

typedef void (*gui_btn_cb_t)(uint8_t type);

typedef struct gui_btn_s {
    gui_btn_cb_t callback;
} gui_btn_t;



// Progress Bar

typedef struct {
    float min;
    float max;
    float val;
    color_t fg;
    color_t bg;
} gui_prgbar_t;



// Text

typedef enum gui_txt_align_e {
    GUI_TXT_ALIGN_LEFT,
    GUI_TXT_ALIGN_CENTER,
    GUI_TXT_ALIGN_RIGHT
} gui_txt_align_t;

#define GUI_TXT_STYLE_BOLD      (1 << 0)
#define GUI_TXT_STYLE_UNDERLINE (1 << 1)
#define GUI_TXT_STYLE_ITALIC    (1 << 2)

typedef struct gui_txt_style_s {
    float size;
    uint8_t flags;
    color_t color;
} gui_txt_style_t;

typedef struct gui_txt_s {
    string_t text;
    gui_txt_align_t align;
    gui_txt_style_t style;
    font_t font;
} gui_txt_t;



// Image

typedef enum gui_img_align_e {
    GUI_IMG_ALIGN_BEGIN,
    GUI_IMG_ALIGN_CENTER,
    GUI_IMG_ALIGN_END
} gui_img_align_t;

typedef enum gui_img_fill_e {
    GUI_IMG_FILL_CONTAIN,
    GUI_IMG_FILL_COVER,
    GUI_IMG_FILL_EXPAND
} gui_img_fill_t;

typedef struct {
    image_t image;
    gui_img_align_t alignx;
    gui_img_align_t aligny;
    gui_img_fill_t fill;
} gui_img_t;

*/

#include "gui/elems/frame.h"
#include "gui/elems/button.h"
#include "gui/elems/label.h"
#include "gui/elems/picture.h"
#include "gui/elems/slider.h"
#include "gui/elems/switch.h"
