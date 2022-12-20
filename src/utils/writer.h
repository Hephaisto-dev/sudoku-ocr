#pragma once

#include "common.h"

#include "utils/string.h"
#include "graphic/atlas.h"
#include "graphic/font.h"



#define WRITER_ALIGN_LEFT   0
#define WRITER_ALIGN_RIGHT  1
#define WRITER_ALIGN_CENTER 2
#define WRITER_ALIGN_TOP    0
#define WRITER_ALIGN_BOTTOM 1



typedef struct writer_params_s {
    // Multiplied by the font size
    float line_height;

    // The font size in point (1/72 of an inch)
    float size;

    // The font to use
    font_t font;

    // If allowed to put word on new line if there is not enough space remaining
    bool words_warp;

    // Horizontal text align
    uint8_t halign;

    // Vertical text align
    uint8_t valign;

    // Tab size (in number of spaces)
    uint8_t tab_size;

    // Text color
    color_t color;
} writer_params_t;


typedef struct writer_char_s {
    int32_t dst_x;
    int32_t dst_y;
    int32_t src_x;
    int32_t src_y;
    int32_t width;
    int32_t height;
    texture_t texture;
} writer_char_t;


typedef struct writer_data_s {
    writer_char_t* chars;
    size_t nb_chars;
    int32_t width;
    int32_t height;
} writer_data_t;



void writer_calc_data(
    string_t txt,
    writer_data_t* data,
    atlas_t atlas,
    uint16_t dpi,
    writer_params_t* params,
    int32_t max_width, int32_t max_height
);



void writer_rndr_text(
    renderer_t rndr,
    writer_data_t* data,
    writer_params_t* params,
    int32_t x_offset, int32_t y_offset
);



void writer_draw_char(
    wchar_t chr,
    image_t dst,
    atlas_t atlas,
    uint16_t dpi,
    writer_params_t* params,
    int32_t x_offset, int32_t y_offset
);
