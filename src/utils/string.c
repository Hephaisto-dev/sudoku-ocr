#include "string.h"

#include <stdlib.h>


size_t wstrlen(const wchar_t* s) {
    size_t i = 0;
    while (s[i++]);
    return i - 1;
}



string_t str_new() {
    string_t s = (string_t) malloc(sizeof(*s));
    s->allocated = 0;
    s->data = NULL;
    s->len = 0;
    return s;
}


string_t str_alloc(size_t len) {
    string_t s = (string_t) malloc(sizeof(struct string_s));
    s->allocated = len;
    s->data = (wchar_t*) malloc(len * sizeof(wchar_t));
    s->len = 0;
    return s;
}



string_t str_from_wchars_cp(const wchar_t* s) {
    size_t len = wstrlen(s);
    string_t r = str_alloc(len + 1);
    r->len = len;
    fastmemcpy(r->data, s, len * sizeof(wchar_t));
    return r;
}

string_t str_from_wchars_nc(wchar_t* s) {
    string_t r = str_new();
    size_t len = wstrlen(s);
    r->data = s;
    r->len = len;
    r->allocated = len + 1;
    return r;
}


wchar_t* str_to_wchars_cp(string_t s) {
    wchar_t* r = (wchar_t*) malloc(sizeof(wchar_t) * (s->len + 1));
    fastmemcpy(r, s->data, s->len * sizeof(wchar_t));
    r[s->len] = 0;
    return r;
}

wchar_t* str_to_wchars_nc(string_t s) {
    s->data[s->len] = 0;
    return s->data;
}



void str_free(string_t s) {
    if (s) {
        free(s->data);
        free(s);
        //s->allocated = 0;
        //s->len = 0;
    }
}



void str_clear(string_t s) {
    s->len = 0;
}



string_t str_sub_cp(string_t s, size_t begin, size_t len) {
    string_t r = str_alloc(len + 1);
    r->len = len;
    fastmemcpy(r->data, &s->data[begin], len);
    return r;
}

void str_sub_nc(string_t s, size_t begin, size_t len) {
    wchar_t* src = &s->data[begin];
    for (size_t i = 0; i < len; i++)
        s->data[i] = src[i];
    s->len = len;
}


string_t str_rep_char_cp(string_t s, wchar_t rep, wchar_t by) {
    string_t r = str_alloc(s->len + 1);
    wchar_t* src = s->data;
    wchar_t* dst = r->data;
    for (size_t i = s->len; i--;) {
        if (src[i] == rep) dst[i] = by;
        else dst[i] = src[i];
    }
    return r;
}

void str_rep_char_nc(string_t s, wchar_t rep, wchar_t by) {
    wchar_t* d = s->data;
    for (size_t i = s->len; i--;) {
        if (d[i] == rep) d[i] = by;
    }
}


string_t str_rep_str_cp(string_t s, string_t rep, string_t by);
void str_rep_str_nc(string_t s, string_t rep, string_t by);


string_t str_merge_cp(string_t s, string_t a);
void str_merge_nc(string_t s, string_t a);



string_t str_sub(string_t s, size_t begin, size_t len, string_t out);

string_t str_rep_char(string_t s, wchar_t rep, wchar_t by, string_t out);

string_t str_rep_str(string_t s, string_t rep, string_t by, string_t out);

string_t str_merge(string_t s, string_t a, string_t out);
