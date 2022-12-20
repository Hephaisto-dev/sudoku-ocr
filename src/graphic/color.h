#pragma once

#include "common.h"


typedef union color_u {
    struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };
    uint32_t v;
} color_t;


#define RGB_TO_COLOR(vr, vg, vb) \
    ((color_t){.r=(vr), .g=(vg), .b=(vb), .a=0xFF})

#define RGBA_TO_COLOR(vr, vg, vb, va) \
    ((color_t){.r=(vr), .g=(vg), .b=(vb), .a=(va)})

#define HEX_TO_COLOR(hex) \
    ((color_t){.r=((hex)>>16)&0xFF, .g=((hex)>>8)&0xFF, .b=(hex)&0xFF, .a=0xFF})
