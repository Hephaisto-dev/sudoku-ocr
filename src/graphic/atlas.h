#pragma once

#include "common.h"

#include "graphic/renderer.h"
#include "graphic/texture.h"
#include "graphic/font.h"


#define ATLAS_TABLE_SIZE   65536
#define ATLAS_MAX_CURSOR   1024
#define ATLAS_MAX_ENTRIES  2048
#define ATLAS_MIN_TEX_SIZE 512
#define ATLAS_MAX_TEX_SIZE 2048
#define ATLAS_MAX_COLUMNS (2 * ATLAS_MAX_TEX_SIZE / ATLAS_MIN_TEX_SIZE)


typedef struct atlas_entry_s {
    int16_t x, y;
    int16_t width, height;
    int16_t advance;
    int16_t xoffset;
    int16_t yoffset;
    texture_t texture;
} atlas_entry_t;


typedef struct atlas_node_s {
    atlas_entry_t data;
    wchar_t chr;
    int16_t size;
    uint16_t next;
    uint16_t fontid;
} atlas_node_t;


typedef struct atlas_cursor_s {
    int16_t x;
    int16_t y;
    int16_t height;
    int16_t max_width;
    texture_t texture;
    uint16_t next;
    uint16_t column;
    uint16_t facing;
} atlas_cursor_t;


typedef struct atlas_s {
    renderer_t rndr;
    texture_t current;
    uint16_t table[ATLAS_TABLE_SIZE];
    atlas_node_t entries[ATLAS_MAX_ENTRIES];
    atlas_cursor_t cursors[ATLAS_MAX_CURSOR];
    uint16_t columns[ATLAS_MAX_COLUMNS];
    size_t nb_entries;
    size_t nb_cursors;
    size_t nb_columns;
} *atlas_t;


atlas_entry_t* atlas_add(
    atlas_t atlas,
    wchar_t chr,
    int16_t size,
    uint16_t fontid,
    font_glyph_t* glyph
);

atlas_entry_t* atlas_get(
    atlas_t atlas,
    wchar_t chr,
    int16_t size,
    uint16_t fontid
);

atlas_t atlas_new(renderer_t rndr);

void atlas_free(atlas_t atlas);
