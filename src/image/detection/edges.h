#pragma once

#include "common.h"

#include "utils/vector.h"
#include "graphic/image.h"


struct edge_line_s;
struct edge_point_s;


typedef struct edge_point_s {
    vect2f_t position;
    vect2f_t normal;
    float strength;
    float width;
    struct edge_line_s* line;
    union {
        struct edge_point_s* link1;
        struct edge_point_s* prev;
    };
    union {
        struct edge_point_s* link2;
        struct edge_point_s* next;
    };
    float score1;
    float score2;
    size_t index;
} edge_point_t;


typedef struct edge_line_s {
    struct edge_point_s* first;
    struct edge_point_s* last;
    float length;
    bool loop;
} edge_line_t;


typedef struct edge_lines_s {
    edge_point_t* points;
    size_t nb_points;
    edge_line_t* lines;
    size_t nb_lines;
} edge_lines_t;


edge_lines_t edge_detect(image_t img);
