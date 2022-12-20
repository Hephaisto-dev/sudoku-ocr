#include "atlas.h"



static uint16_t _atlas_hash(wchar_t chr, int16_t size, uint16_t ftid) {
    if (sizeof(wchar_t) == 4)
        chr ^= (chr >> 16);
    size ^= ((size >> 8) & 0x00FF) ^ ((size << 8) & 0xFF00);
    chr  ^= ((chr  >> 8) & 0x00FF) ^ ((chr  << 8) & 0xFF00);
    ftid ^= ((ftid >> 8) & 0x00FF) ^ ((ftid << 8) & 0xFF00);
    size = (size * 51479) & 0xFFFF;
    chr  = (chr  * 39119) & 0xFFFF;
    ftid = (ftid * 45979) & 0xFFFF;
    return (size ^ chr ^ ftid) % ATLAS_TABLE_SIZE;
}


static void _atlas_update_max_width(atlas_t atlas, atlas_cursor_t* c) {
    int16_t maxy = c->y + c->height;
    int16_t endx = c->x;
    if (c->facing != 0) {
        for (atlas_cursor_t* e = &atlas->cursors[c->facing]; e->y < maxy;) {
            int16_t diff = abs_i16(endx - e->x);
            if (diff < c->max_width)
                c->max_width = diff;
            if (e->next == 0) break;
            e = &atlas->cursors[e->next];
        }
    }
}


static int16_t _atlas_forward_cursor(atlas_t atlas, atlas_cursor_t* c, int16_t d) {
    int32_t r;
    if (c->column & 0x1) {
        c->x -= d;
        r = c->x;
    } else {
        r = c->x;
        c->x += d;
    }
    c->max_width -= d;
    _atlas_update_max_width(atlas, c);
    return r;
}


static inline atlas_cursor_t* _atlas_add_cursor(atlas_t atlas) {
    return &atlas->cursors[atlas->nb_cursors++];
}


static void _atlas_add_column(atlas_t atlas) {
    int16_t xoffset = (atlas->nb_columns / 2) * ATLAS_MIN_TEX_SIZE;

    atlas->nb_columns += 2;
    atlas_cursor_t* c1 = _atlas_add_cursor(atlas);
    atlas_cursor_t* c2 = _atlas_add_cursor(atlas);

    c1->height = 0;
    c1->x = xoffset;
    c1->y = 0;
    c1->column = atlas->nb_columns - 2;
    c1->next = 0;
    c1->facing = atlas->nb_cursors - 1;
    c1->max_width = 0;
    c1->texture = atlas->current;

    c2->height = 0;
    c2->x = xoffset + ATLAS_MIN_TEX_SIZE;
    c2->y = 0;
    c2->column = atlas->nb_columns - 1;
    c2->next = 0;
    c2->facing = atlas->nb_cursors - 2;
    c2->max_width = 0;
    c2->texture = atlas->current;

    atlas->columns[atlas->nb_columns - 2] = atlas->nb_cursors - 2;
    atlas->columns[atlas->nb_columns - 1] = atlas->nb_cursors - 1;
}


static uint8_t _atlas_extend_texture(atlas_t atlas) {
    texture_t tex = atlas->current;

    if (tex != NULL) {
        if (tex->height <= tex->width) {
            if (tex->height * 2 <= ATLAS_MAX_TEX_SIZE) {
                // Extend texture vertically
                texture_t newtex = tex_new(
                    atlas->rndr,
                    tex->width, tex->height * 2,
                    HEX_TO_COLOR(0)
                );

                image_t src = tex_edit(atlas->current);
                tex_clip_image(newtex, src, 0, 0);
                tex_free(tex);
                atlas->current = newtex;

                return 1;
            }
        } else {
            if (tex->width * 2 <= ATLAS_MAX_TEX_SIZE) {
                // Extend texture horizontally
                texture_t newtex = tex_new(
                    atlas->rndr,
                    tex->width * 2, tex->height,
                    HEX_TO_COLOR(0)
                );

                image_t src = tex_edit(atlas->current);
                tex_clip_image(newtex, src, 0, 0);
                tex_free(tex);
                atlas->current = newtex;

                // Add columns
                uint16_t nb_new_cols = tex->width / ATLAS_MAX_TEX_SIZE;
                for (uint16_t col = 0; col < nb_new_cols; col++) {
                    _atlas_add_column(atlas);
                }

                return 2;
            }
        }
    }

    // Can't extend current texture so create a new one

    atlas->current = tex_new(
        atlas->rndr,
        ATLAS_MIN_TEX_SIZE, ATLAS_MIN_TEX_SIZE,
        HEX_TO_COLOR(0)
    );

    atlas->nb_columns = 0;
    _atlas_add_column(atlas);

    return 3;
}


static bool _atlas_get_best_pos(atlas_t atlas, atlas_entry_t* entry) {
    int16_t height = entry->height;
    int16_t width = entry->width;

    if (width > (ATLAS_MIN_TEX_SIZE / 2) || height > (ATLAS_MIN_TEX_SIZE / 2)) {
        return NULL;
    }

    if (atlas->current != NULL) {
        atlas_cursor_t* best = NULL;
        int16_t best_diff = INT16_MAX;

        atlas_cursor_t* best_split = NULL;
        int16_t best_split_diff = INT16_MAX;

        for (size_t cursor = 0; cursor < atlas->nb_cursors; cursor++) {
            atlas_cursor_t* c = &atlas->cursors[cursor];
            if (c->height >= height && c->max_width >= width) {
                int16_t diff = c->height - height;
                if (diff < best_diff) {
                    best_diff = diff;
                    best = c;
                }

                diff = c->height - height * 2;
                if (diff > 0 && diff < best_split_diff) {
                    best_split_diff = diff;
                    best_split = c;
                }
            }
        }

        // Check if we need to add or split a cursor
        if (best /*&& best_diff * 4 > best->height*/) {
            // We are good
            mprintf("Add char on best cursor on column %d\n", best->column);
            entry->x = _atlas_forward_cursor(atlas, best, width);
            entry->y = best->y;
            entry->texture = best->texture;
            return true;
        }

        // If we can split then split
        if (best_split) {
            mprintf("Split cursor on column %d\n", best_split->column);
            atlas_cursor_t* c = _atlas_add_cursor(atlas);
            int16_t newh = best_split->height / (best_split->height / height);
            c->column = best_split->column;
            c->texture = best_split->texture;
            c->facing = 0;
            c->next = 0;
            c->height = best_split->height - newh;
            c->max_width = width;
            c->y = best_split->y + newh;
            c->x = best_split->x;

            entry->x = _atlas_forward_cursor(atlas, best_split, width);
            entry->y = best_split->y;
            entry->texture = best_split->texture;

            return true;
        }

        // Add a new cursor at the end of a column
        for (size_t col = 0; col < atlas->nb_columns; col += 2) {
            uint16_t self_col = col;
            uint16_t opp_col = col + 1;
            atlas_cursor_t* self_cursor = &atlas->cursors[atlas->columns[self_col]];
            atlas_cursor_t* opp_cursor  = &atlas->cursors[atlas->columns[opp_col]];

            // Choose if it's better to add the odd or even column
            if (self_cursor->y + self_cursor->height > opp_cursor->y + opp_cursor->height) {
                atlas_cursor_t* tmp_cursor = self_cursor;
                self_cursor = opp_cursor;
                opp_cursor = tmp_cursor;

                uint16_t tmp_col = opp_col;
                opp_col = self_col;
                self_col = tmp_col;
            }

            if (self_cursor->texture->height - (self_cursor->y + self_cursor->height) >= height) {
                // Add cursor
                mprintf("Add cursor on column %d\n", self_col);

                atlas_cursor_t* c = _atlas_add_cursor(atlas);
                c->column = self_col;
                c->texture = self_cursor->texture;
                c->facing = atlas->columns[opp_col];
                c->height = height;
                c->y = self_cursor->y + self_cursor->height;
                c->x = ((self_col+1) / 2) * ATLAS_MIN_TEX_SIZE;
                c->max_width = abs_i16(opp_cursor->x - c->x);
                c->next = 0;

                self_cursor->next = atlas->nb_cursors - 1;
                atlas->columns[self_col] = atlas->nb_cursors - 1;

                entry->texture = c->texture;
                entry->x = _atlas_forward_cursor(atlas, c, width);
                entry->y = c->y;

                return true;
            }
        }
    }

    // Extend texture
    uint8_t extend_type = _atlas_extend_texture(atlas);
    if (extend_type) {
        atlas_cursor_t* c = _atlas_add_cursor(atlas);
        c->texture = atlas->current;
        c->facing = 0;
        c->next = 0;
        c->max_width = ATLAS_MIN_TEX_SIZE;
        c->height = height;

        if (extend_type == 1) {
            // If extend in height
            mprintf("Extend Height\n");
            atlas_cursor_t* cursor1 = &atlas->cursors[atlas->columns[0]];
            atlas_cursor_t* cursor2  = &atlas->cursors[atlas->columns[1]];
            if (cursor1->y + cursor1->height > cursor2->y + cursor2->height) {
                c->column = 1;
                c->y = cursor2->y + cursor2->height;
                c->x = ATLAS_MIN_TEX_SIZE;
                c->facing = atlas->columns[0];
            } else {
                c->column = 0;
                c->y = cursor1->y + cursor1->height;
                c->x = 0;
                c->facing = atlas->columns[1];
            }
        } else if(extend_type == 2) {
            // If extend in width
            mprintf("Extend Width\n");
            c->column = atlas->nb_columns - 2;
            c->y = 0;
            c->x = (c->column / 2) * ATLAS_MIN_TEX_SIZE;
            c->facing = atlas->columns[c->column + 1];
        } else {
            mprintf("New Texture\n");
            // If extend with a new texture
            c->column = 0;
            c->y = 0;
            c->x = 0;
            c->facing = atlas->columns[1];
        }

        atlas->cursors[atlas->columns[c->column]].next = atlas->nb_cursors - 1;
        atlas->columns[c->column] = atlas->nb_cursors - 1;

        entry->texture = atlas->current;
        entry->x = _atlas_forward_cursor(atlas, c, width);
        entry->y = 0;

        return true;
    }

    return NULL;
}


atlas_entry_t* atlas_add(
    atlas_t atlas,
    wchar_t chr,
    int16_t size,
    uint16_t fontid,
    font_glyph_t* glyph
) {
    uint16_t hash = _atlas_hash(chr, size, fontid);

    // Add glyph to hash table
    atlas_node_t* node = &atlas->entries[atlas->nb_entries++];
    node->next = atlas->table[hash];
    node->chr = chr;
    node->size = size;
    node->fontid = fontid;
    atlas->table[hash] = atlas->nb_entries;

    atlas_entry_t* entry = &node->data;
    entry->advance = glyph->advance;
    entry->xoffset = glyph->xoffset;
    entry->yoffset = glyph->yoffset;
    entry->width   = glyph->img->width;
    entry->height  = glyph->img->height;

    if (entry->width > 0 && entry->height > 0) {
        if (!_atlas_get_best_pos(atlas, entry)) {
            // Failed to find a place so create a texture just for this glyph
            texture_t tex = tex_new_from_img_cpy(atlas->rndr, glyph->img);
            entry->texture = tex;
            entry->x = 0;
            entry->y = 0;
            return entry;
        }

        // Copy glyph image to atlas texture
        tex_clip_image(entry->texture, glyph->img, entry->x, entry->y);
    }

    return entry;
}



atlas_entry_t* atlas_get(
    atlas_t atlas,
    wchar_t chr,
    int16_t size,
    uint16_t fontid
) {
    uint16_t hash = _atlas_hash(chr, size, fontid);
    size_t next = atlas->table[hash];
    while (next) {
        atlas_node_t* v = &atlas->entries[next - 1];
        if (v->chr == chr && v->size == size && v->fontid == fontid) {
            return &v->data;
        }
        next = v->next;
    }
    return NULL;
}



atlas_t atlas_new(renderer_t rndr) {
    atlas_t a = (atlas_t) malloc(sizeof(struct atlas_s));
    a->nb_cursors = 0;
    a->nb_entries = 0;
    a->rndr = rndr;
    a->nb_columns = 0;
    a->current = NULL;
    for (size_t i = ATLAS_TABLE_SIZE; i--;) {
        a->table[i] = 0;
    }
    return a;
}



void atlas_free(atlas_t atlas) {
    free(atlas);
}
