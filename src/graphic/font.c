#include "font.h"



static FT_Library ftlib;
static int fterr;



void font_init() {
    fterr = FT_Init_FreeType(&ftlib);
    if (fterr) {
        // TODO
    }
}



void font_destroy() {
    fterr = FT_Done_FreeType(ftlib);
    if (fterr) {
        // TODO
    }
}


static uint16_t _font_current_id = 0;

font_t font_load_from_file(const char* filename) {
    FT_Face face;
    fterr = FT_New_Face(ftlib, filename, 0, &face);
    if (fterr) {
        // TODO
        mprintf("FT Load error : %d\n", fterr);
        return NULL;
    }
    font_t font = (font_t) malloc(sizeof(struct font_s));
    font->_face = face;
    font->id = ++_font_current_id;
    font->ascender = face->ascender / 64;
    return font;
}



font_glyph_t font_render_glyph(
    font_t font,
    wchar_t chr,
    uint16_t size
) {
    FT_Set_Pixel_Sizes(font->_face, 0, size);

    FT_UInt index = FT_Get_Char_Index(font->_face, chr);

    fterr = FT_Load_Glyph(font->_face, index, 0);
    FT_GlyphSlot slot = font->_face->glyph;
    if (slot->format != FT_GLYPH_FORMAT_BITMAP) {
        FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);
    }

    font_glyph_t r;

    r.advance = slot->advance.x / 64;
    r.xoffset = slot->metrics.horiBearingX / 64;
    r.yoffset = -(slot->metrics.horiBearingY / 64);

    image_t img = img_new(
        slot->bitmap.width, slot->bitmap.rows,
        HEX_TO_COLOR(0xFFFFFF)
    );
    r.img = img;

    // Copy pixels
    img_lock(img);
    uint8_t* srcdata = slot->bitmap.buffer;
    size_t i = 0;
    for (size_t y = 0, my = img->height; y < my; y++) {
        for (size_t x = 0, mx = img->width; x < mx; x++) {
            color_t color = {
                .r = 0xFF,
                .g = 0xFF,
                .b = 0xFF,
                .a = srcdata[i++]
            };
            img_set_pixel(img, x, y, color);
        }
    }
    img_unlock(img);

    return r;
}



void font_free(font_t font) {
    FT_Done_Face(font->_face);
    free(font);
}
