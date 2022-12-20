#include "writer.h"



void writer_calc_data(
    string_t txt,
    writer_data_t* data,
    atlas_t atlas,
    uint16_t dpi,
    writer_params_t* params,
    int32_t max_width, int32_t max_height
) {
    uint16_t pixelsize = (uint16_t) (params->size * dpi / 72);
    uint16_t fontid = params->font->id;

    size_t nb_chars_at_last_space = 0;
    int32_t cursor_x = 0;
    int32_t cursor_y = params->font->ascender;
    int32_t line_height = pixelsize * params->line_height;

    int32_t width = 0;
    int32_t height = line_height;

    writer_char_t* chars =
        (writer_char_t*) malloc(sizeof(writer_char_t) * str_len(txt));
    size_t nb_chars = 0;

    // For each text line
    for (size_t i = 0; i < txt->len; i++) {
        wchar_t c = str_get(txt, i);
        if(c != '\r') {
            if (c == ' '  && max_width > 0 && cursor_x > max_width && params->words_warp) {
                // New line
                nb_chars--;
                int32_t x_offset = -chars[nb_chars_at_last_space+1].dst_x;
                int32_t y_offset = line_height;
                for (size_t i = nb_chars_at_last_space; i < nb_chars; i++) {
                    chars[i] = chars[i + 1];
                    chars[i].dst_y += y_offset;
                    chars[i].dst_x += x_offset;
                }
                if (-x_offset > width)
                    width = -x_offset;
                height += line_height;
                cursor_y += y_offset;
                cursor_x += x_offset;
                if (cursor_x > width)
                    width = cursor_x;
                continue;
            }

            if (c == '\n') {
                cursor_y += line_height;
                if (cursor_x > width)
                    width = cursor_x;
                cursor_x = 0;
                height += line_height;
                continue;
            }

            if (c == ' ') {
                nb_chars_at_last_space = nb_chars;
            }

            atlas_entry_t* entry = atlas_get(atlas, c, pixelsize, fontid);
            if (entry == NULL) {
                //mprintf("Add to atlas %c\n", (char) c);
                font_glyph_t glyph = font_render_glyph(params->font, c, pixelsize);
                entry = atlas_add(atlas, c, pixelsize, params->font->id, &glyph);
            }

            writer_char_t* chr = &chars[nb_chars++];
            chr->dst_x  = cursor_x + entry->xoffset;
            chr->dst_y  = cursor_y + entry->yoffset;
            chr->src_x  = entry->x;
            chr->src_y  = entry->y;
            chr->width  = entry->width;
            chr->height = entry->height;
            chr->texture = entry->texture;

            cursor_x += entry->advance;
        }
    }

    if (cursor_x > width)
        width = cursor_x;

    data->height = height;
    data->width = width;
    data->nb_chars = nb_chars;
    data->chars = chars;
}



void writer_rndr_text(
    renderer_t rndr,
    writer_data_t* data,
    writer_params_t* params,
    int32_t x_offset, int32_t y_offset
) {
    for (size_t i = data->nb_chars; i--;) {
        writer_char_t* chr = &data->chars[i];
        if (chr->height > 0 && chr->width > 0) {
            rndr_set_mul_color(rndr, chr->texture, params->color);
            rndr_clip_rect(
                rndr, chr->texture,
                chr->src_x, chr->src_y,
                chr->width, chr->height,
                chr->dst_x + x_offset, chr->dst_y + y_offset
            );
        }
    }
}



void writer_draw_char(
    wchar_t chr,
    image_t dst,
    atlas_t atlas,
    uint16_t dpi,
    writer_params_t* params,
    int32_t x_offset, int32_t y_offset
) {
    uint16_t pixelsize = (uint16_t) (params->size * 72 * dpi);
    atlas_entry_t* entry = atlas_get(atlas, chr, pixelsize, params->font->id);
    if (entry == NULL) {
        font_glyph_t glyph = font_render_glyph(params->font, chr, pixelsize);
        entry = atlas_add(atlas, chr, pixelsize, params->font->id, &glyph);
    }
    // TODO
}
