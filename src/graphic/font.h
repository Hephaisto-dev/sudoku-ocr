#pragma once

#include "common.h"

#include "graphic/image.h"
#include "graphic/texture.h"

#include <ft2build.h>
#include FT_FREETYPE_H



typedef struct font_s {
    uint16_t id;
    int16_t ascender;
    FT_Face _face;
} *font_t;



typedef struct font_glyph_s {
    image_t img;
    int16_t advance;
    int16_t xoffset;
    int16_t yoffset;
} font_glyph_t;



void font_init();

void font_destroy();

font_t font_load_from_file(const char* filename);

font_glyph_t font_render_glyph(
    font_t font,
    wchar_t chr,
    uint16_t size
);

void font_free(font_t font);
