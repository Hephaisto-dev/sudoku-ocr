#include "edges.h"

#include <math.h>

#define LINK_SEARCH_RADIUS (8)

#define MIN_COLOR_DIFF (12)

// Don't affect as much the quality but have a huge impact on optimization
#define MAX_EDGE_WIDTH (10)

// The minimum power of 2 greater or equal to MAX_EDGE_WIDTH
// this must be computed manually for optimization reasons
#define MAX_EDGE_WIDTH_MIN_GREATER_POWER_OF_2 (16)



void _edge_detect_points(image_t img, edge_lines_t* ret);
void _edge_simplify_points(edge_lines_t* data, float min_strength);
void _edge_link_points(edge_lines_t* data);
void _edge_create_lines(edge_lines_t* data);
void _edge_simplify_lines(
    edge_lines_t* data,
    int32_t min_length,
    float min_angle
);



// The main function
edge_lines_t edge_detect(image_t img) {
    edge_lines_t data = {0};
    _edge_detect_points(img, &data); // Working
    _edge_simplify_points(&data, 0.1); // Working
    _edge_link_points(&data); // Sound like it work
    _edge_create_lines(&data); // Sound like it work
    //_edge_simplify_lines(&data, 60, PI / 12.0); // Working
    return data;
}


static inline int16_t _edge_grayscale(color_t c) {
    return ((int16_t) c.r + (int16_t) c.g + (int16_t) c.b) / 3;
}


// Do a - b while converting RGB to grayscale
static inline int16_t _edge_color_diff(color_t a, color_t b) {
    return _edge_grayscale(a) - _edge_grayscale(b);
}


typedef struct _edge_adj_result_s {
    float score;
    int8_t dir;
    vect2i32_t pos;
    float avg_diff;
    int16_t total_diff;
} _edge_adj_result_t;


typedef struct _edge_point_s {
    vect2f_t position;
    vect2f_t normal;
    float strength;
    float width;
} _edge_point_t;


#define _EDGE_DIR_UP         0
#define _EDGE_DIR_UP_RIGHT   1
#define _EDGE_DIR_RIGHT      2
#define _EDGE_DIR_DOWN_RIGHT 3
#define _EDGE_DIR_DOWN       4
#define _EDGE_DIR_DOWN_LEFT  5
#define _EDGE_DIR_LEFT       6
#define _EDGE_DIR_UP_LEFT    7


// Constant array of adjancent pixels for faster computing
const vect2i32_t adj_positions[8] = {
    [_EDGE_DIR_UP]         = { 0, -1},
    [_EDGE_DIR_UP_RIGHT]   = { 1, -1},
    [_EDGE_DIR_RIGHT]      = { 1,  0},
    [_EDGE_DIR_DOWN_RIGHT] = { 1,  1},
    [_EDGE_DIR_DOWN]       = { 0,  1},
    [_EDGE_DIR_DOWN_LEFT]  = {-1,  1},
    [_EDGE_DIR_LEFT]       = {-1,  0},
    [_EDGE_DIR_UP_LEFT]    = {-1, -1}
};

// The list of next pixel positions to check for each of 8 directions
// The 3 direction are index for an offset of the array "adj_positions"
const int8_t nxt_positions[8][3] = {
    [_EDGE_DIR_UP]         = {_EDGE_DIR_UP_LEFT, _EDGE_DIR_UP, _EDGE_DIR_UP_RIGHT},
    [_EDGE_DIR_UP_RIGHT]   = {_EDGE_DIR_UP, _EDGE_DIR_UP_RIGHT, _EDGE_DIR_RIGHT},
    [_EDGE_DIR_RIGHT]      = {_EDGE_DIR_UP_RIGHT, _EDGE_DIR_RIGHT, _EDGE_DIR_DOWN_RIGHT},
    [_EDGE_DIR_DOWN_RIGHT] = {_EDGE_DIR_RIGHT, _EDGE_DIR_DOWN_RIGHT, _EDGE_DIR_DOWN},
    [_EDGE_DIR_DOWN]       = {_EDGE_DIR_DOWN_RIGHT, _EDGE_DIR_DOWN, _EDGE_DIR_DOWN_LEFT},
    [_EDGE_DIR_DOWN_LEFT]  = {_EDGE_DIR_DOWN, _EDGE_DIR_DOWN_LEFT, _EDGE_DIR_LEFT},
    [_EDGE_DIR_LEFT]       = {_EDGE_DIR_DOWN_LEFT, _EDGE_DIR_LEFT, _EDGE_DIR_UP_LEFT},
    [_EDGE_DIR_UP_LEFT]    = {_EDGE_DIR_LEFT, _EDGE_DIR_UP_LEFT, _EDGE_DIR_UP}
};

// The opposite direction for each directions
const int8_t opposite_dirs[8] = {
    [_EDGE_DIR_UP]         = _EDGE_DIR_DOWN,
    [_EDGE_DIR_UP_RIGHT]   = _EDGE_DIR_DOWN_LEFT,
    [_EDGE_DIR_RIGHT]      = _EDGE_DIR_LEFT,
    [_EDGE_DIR_DOWN_RIGHT] = _EDGE_DIR_UP_LEFT,
    [_EDGE_DIR_DOWN]       = _EDGE_DIR_UP,
    [_EDGE_DIR_DOWN_LEFT]  = _EDGE_DIR_UP_RIGHT,
    [_EDGE_DIR_LEFT]       = _EDGE_DIR_RIGHT,
    [_EDGE_DIR_UP_LEFT]    = _EDGE_DIR_DOWN_RIGHT
};


int16_t _edge_get_avg_color(image_t img, int32_t x, int32_t y, int8_t dir) {
    vect2i32_t o1 = adj_positions[(dir == 0) ? 7 : (dir - 1)];
    vect2i32_t o2 = adj_positions[dir];
    vect2i32_t o3 = adj_positions[(dir + 1) % 8];

    int16_t sum = 0;
    int16_t n = 1;

    int32_t tx = x + o2.x; int32_t ty = y + o2.y;
    if (tx >= 0 && tx < img->width && ty >= 0 && ty < img->height) {
        sum += _edge_grayscale(img_get_pixel(img, tx, ty));
    } else {
        return -1;
    }

    tx = x + o1.x; ty = y + o1.y;
    if (tx >= 0 && tx < img->width && ty >= 0 && ty < img->height) {
        sum += _edge_grayscale(img_get_pixel(img, tx, ty));
        n++;
    }

    tx = x + o3.x; ty = y + o3.y;
    if (tx >= 0 && tx < img->width && ty >= 0 && ty < img->height) {
        sum += _edge_grayscale(img_get_pixel(img, tx, ty));
        n++;
    }

    return sum / n;
}


_edge_adj_result_t _edge_add_adj(
        image_t img,
        int32_t x, int32_t y,
        const int8_t* adjs, size_t nb_adjs,
        int8_t length, float old_avg_diff, int16_t old_total_diff,
        // 1 or -1 depending on if we want a positive or negative diff
        int16_t sign
) {
    // The result (the one with the best score)
    _edge_adj_result_t result = {
        .score = -INFINITY,
        .avg_diff = old_avg_diff,
        .total_diff = old_total_diff,
        .dir = -1,
        .pos = {0}
    };

    color_t cur_color = img->data[y * img->width + x];

    // For each adjencent pixel
    for (size_t adj_dir = nb_adjs; adj_dir--;) {
        int8_t dir = adjs[adj_dir];

        int16_t adj_color = _edge_get_avg_color(img, x, y, dir);

        if (adj_color != -1) {
            // Compute the difference with the current pixel color
            int16_t diff = (adj_color - _edge_grayscale(cur_color)) * sign;

            if (diff <= 0) continue;

            // Compute the new average of differences
            float new_avg_diff =
                (((float) length) / (float) (length+1)) * old_avg_diff +
                    (((float) diff) / (float) (length+1));

            // And the new total difference
            int16_t new_total_diff = old_total_diff + diff;

            // And finally the score with this adjancent pixel
            float score = new_avg_diff * new_total_diff;
            if (score > result.score) {
                result.score = score;
                result.dir = adj_dir;
                result.avg_diff = new_avg_diff;
                result.total_diff = new_total_diff;
                result.pos.x = x + adj_positions[dir].x;
                result.pos.y = y + adj_positions[dir].y;
            }
        }
    }

    return result;
}


_edge_adj_result_t _edge_remove_adj(
        color_t prev_color, color_t last_color,
        int8_t length, float old_avg_diff, int16_t old_total_diff,
        // 1 or -1 depending on if we want a positive or negative diff
        int16_t sign
) {
    // The result (the one with the best score)
    _edge_adj_result_t result = {
        .score = -INFINITY,
        .avg_diff = old_avg_diff,
        .total_diff = old_total_diff,
        .dir = -1,
        .pos = {0}
    };

    // Compute the difference with the current pixel color
    int16_t diff = _edge_color_diff(last_color, prev_color) * sign;

    // Compute the new average of differences
    result.avg_diff =
        (old_avg_diff - (((float)1)/length) * diff) *
            (((float)length) / (length - 1));

    // And the new total difference
    result.total_diff = old_total_diff - diff;

    // And finally the score with this adjancent pixel
    result.score = result.avg_diff * result.total_diff;

    return result;
}


void _edge_detect_points(image_t img, edge_lines_t* ret) {
/*
    Principe (oui c'est ecrit en francais parceque flemme):

    Une ligne est une bordure entre deux couleurs different et est constitue
    d'une suite de point, mais la difficulte reside dans le faite que la largeur
    de cette bordure peut s'etendre sur plusieur pixels de long.
    Donc l'on doit verifier pour chaque pixel s'il en le comparent avec
    un autre pixel poche de lui, sa difference de couleur est elever. Sachant
    que cette autre pixel avec lequelle on compare, n'est pas forcement
    adjacant. En effet comme dit precedament une ligne peut faire plusieur
    pixels de large. Le bute de cette algorithme est donc de placer plein de
    points sur sur l'image, et chacun de ces points doit decrire une seperation
    de deux couleurs avec plusieur characteristique tel que : la difference
    de couleur, la largeur de la separation, la visibilite (ou force) de cette
    separation. En generale sur la separation entre deux couleurs, plus l'on
    eloigne les deux pixels a comparer et plus on a de chance d'avoir une
    difference elever. Ceci est un problem car la largeur des separation aurat
    tendance a être tres elever. Donc pour eviter cela j'ai trouver trois
    methods applicable : definir une largeur maximum, verifier si chaque pixels
    separent les deux pixels a comparer sont continue (pas de variation a sens
    opposer d'un pixel a l'autre) et chercher a recuperer le segment avec la
    plus grande variation moyen en considerant egalement la variation total.
    Pour ce faire j'ai trouver une fonction mathematique permetant de determiner
    la meilleur longueur separent les deux pixels:
    score = moy_of_diff_between(pixels) * diff(pixel_1, pixel_2)
    ou le bute est d'avoir le score le plus haut possible.
    On peut aussi le definir comme les suites:

    An+1 = An * (n/(n+1)) + diff(Pixel(n-1), Pixel(n))
    Dn+1 = Dn + diff(Pixel(n-1), Pixel(n))
    Un+1 = An+1 * Dn+1

    avec Un la suite du score, An la suite de la moyen des differences et
    Dn la suite de la difference entre les deux pixels.

    Un flous peut être appliquer comme pre-traitement de l'image pour ameliorer
    la detections des lignes car chaque pixel contiendra alors les influences
    des pixel voisins.
*/

    // Array of the same size than the image and indicate for each pixels
    // if it has been used in a segment calculation
    bool* computeds = (bool*)
            malloc(img->width * img->height * sizeof(bool));
    for (size_t i = img->width * img->height; i--;)
        computeds[i] = false;

    // The list of points found
    _edge_point_t* final_points = (_edge_point_t*)
            malloc(img->width * img->height * sizeof(_edge_point_t));
    size_t nb_final_points = 0;

    // Store some image info in locals variables for a faser access
    int32_t w =  img->width;
    int32_t h =  img->height;
    color_t* pixels = img->data;

    // For each pixels on the image
    for (int32_t y = 0; y < h; y++) {
        for (int32_t x = 0; x < w; x++) {
            // Skip because this point has already been used in computation
            if (computeds[y * w + x]) continue;

            // Get the score with each adjancente pixels
            // (in a 9x9 square arround the current pixel)
            // because we need to find on which direction we must begin the path

            int32_t best_diff = MIN_COLOR_DIFF;
            int8_t best_dir = -1;
            vect2i32_t best_pos = {0};
            int16_t color = _edge_grayscale(pixels[y * w + x]);

            for (int8_t dir = 8; dir--;) {
                int16_t diff = _edge_get_avg_color(img, x, y, dir) - color;
                if (diff > best_diff) {
                    best_diff = diff;
                    best_dir = dir;
                    best_pos.x = x + adj_positions[dir].x;
                    best_pos.y = y + adj_positions[dir].y;
                }
            }

            // If we found a direction to start from
            if (best_dir != -1 && !computeds[best_pos.y * w + best_pos.x]) {
                // Then compute the rest of the path by trying to maximize
                // the path's score

                // The path dir (constant)
                const int8_t dir = best_dir;
                const int8_t odir = opposite_dirs[dir];

                // The path has two ends, one that is rising (positive diff)
                // and the other that is falling (negative diff)
                _edge_adj_result_t rising_end;
                _edge_adj_result_t falling_end;

                // The current path score
                _edge_adj_result_t current = {
                    .avg_diff = (float) best_diff,
                    .total_diff = (int16_t) best_diff,
                    .pos = best_pos,
                    .score = (float) (best_diff * best_diff)
                };

                // The current rising and falling end positions
                vect2i32_t rising_pos = best_pos;
                vect2i32_t falling_pos = {.x = x, .y = y};

                // The list of points of the path. Usefull because we may need
                // to remove ends to incress the score and we need at the
                // end to mark each pixel of this path as computed in the
                // "computeds" array.
                // A circular list in perfect in this case because the number
                // of pixel of this list can't go over MAX_LINE_WIDTH and we
                // may remove and append at the begin and the end.
                const size_t points_max = MAX_EDGE_WIDTH_MIN_GREATER_POWER_OF_2;
                vect2i32_t points[points_max];
                size_t points_begin = 0;
                size_t points_length = 2;

                // Add the point at (x, y)
                points[0] = falling_pos;

                // Add the first point that we have already found
                points[1] = rising_pos;

                while (points_length < MAX_EDGE_WIDTH) {
                    bool modified = false;

                    // Try to add one end and see if the score is incressing

                    rising_end = _edge_add_adj(
                        img, rising_pos.x, rising_pos.y,
                        nxt_positions[dir], 3,
                        points_length, current.avg_diff, current.total_diff,
                        1 // Want a positive diff (we are on the rising end)
                    );

                    falling_end = _edge_add_adj(
                        img, falling_pos.x, falling_pos.y,
                        nxt_positions[odir], 3,
                        points_length, current.avg_diff, current.total_diff,
                        -1 // Want a negative diff (we are on the falling end)
                    );

                    if (rising_end.score > falling_end.score) {
                        if (rising_end.score > current.score) {
                            // Yes! the score is incressing so keep this one
                            current = rising_end;
                            // Add the new rising end at the end of "points"
                            rising_pos = rising_end.pos;
                            points[(points_begin + points_length) % points_max]
                                = rising_pos;
                            points_length++;
                            modified = true;
                        }
                    } else {
                        if (falling_end.score > current.score) {
                            // Yes! the score is incressing so keep this one
                            current = falling_end;
                            // Add the new falling end at the start of "points"
                            if (points_begin == 0)
                                points_begin = points_max - 1;
                            else
                                points_begin--;
                            falling_pos = falling_end.pos;
                            points[points_begin]
                                = falling_pos;
                            points_length++;
                            modified = true;
                        }
                    }

                    // Now try to remove one end see if the score is incressing

                    if (points_length > 1) {
                        vect2i32_t last_pos = points[
                            (points_begin + points_length - 1) % points_max
                        ];
                        vect2i32_t prev_pos = points[
                            (points_begin + points_length - 2) % points_max
                        ];

                        rising_end = _edge_remove_adj(
                            pixels[prev_pos.y * w + prev_pos.x],
                            pixels[last_pos.y * w + last_pos.x],
                            points_length, current.avg_diff, current.total_diff,
                            1
                        );

                        last_pos = points[points_begin];
                        prev_pos = points[(points_begin + 1) % points_max];

                        falling_end = _edge_remove_adj(
                            pixels[prev_pos.y * w + prev_pos.x],
                            pixels[last_pos.y * w + last_pos.x],
                            points_length, current.avg_diff, current.total_diff,
                            -1
                        );

                        if (rising_end.score > falling_end.score) {
                            if (rising_end.score > current.score) {
                                // The score is incressing so remove this one
                                current = rising_end;
                                // Remove this rising end from "points"
                                points_length--;
                                rising_pos = points[
                                    (points_begin + points_length) % points_max
                                ];
                                modified = true;
                            }
                        } else {
                            if (falling_end.score > current.score) {
                                // The score is incressing so remove this one
                                current = falling_end;
                                // Remove this falling end from "points"
                                points_length--;
                                points_begin = (points_begin + 1) % points_max;
                                falling_pos = points[points_begin];
                                modified = true;
                            }
                        }
                    }

                    // If there is nothing to improve in the current path then
                    // we are done
                    if (!modified)
                        break;
                }

                // Fill the "computeds" array
                size_t points_end = (points_begin + points_length) % points_max;
                for (
                    size_t i = points_begin;
                    i != points_end;
                    i = (i + 1) % points_max
                ) {
                    computeds[points[i].y * w + points[i].x] = true;
                }

                // Compute point informations if needed
                if (points_length > 1) {
                    vect2i32_t a = points[points_begin];

                    if (points_end == 0)
                        points_end = points_max - 1;
                    else
                        points_end--;
                    vect2i32_t b = points[points_end];

                    int32_t dx = b.x - a.x;
                    int32_t dy = b.y - a.y;
                    float width = sqrt(dx*dx + dy*dy);

                    _edge_point_t* point = &final_points[nb_final_points++];

                    point->normal.x = -((float) dy) / width;
                    point->normal.y = ((float) dx) / width;
                    point->position.x = ((float) (a.x + b.x)) / 2.0f;
                    point->position.y = ((float) (a.y + b.y)) / 2.0f;
                    point->width = width;
                    point->strength = ((float) current.total_diff) / 255.0f;
                }
            }
        }
    }

    // Finally !
    // We have the list of points of edges of the image

    // So get ready to return
    edge_point_t* return_points = (edge_point_t*)
        malloc(nb_final_points * sizeof(edge_point_t));
    for (size_t i = 0; i < nb_final_points; i++) {
        edge_point_t* a = &return_points[i];
        _edge_point_t* b = &final_points[i];
        a->index = i;
        a->line = NULL;
        a->link1 = NULL;
        a->link2 = NULL;
        a->normal = b->normal;
        a->position = b->position;
        a->score1 = 0;
        a->score2 = 0;
        a->strength = b->strength;
        a->width = b->width;
    }

    // Free dynamic allocations
    free(computeds);
    free(final_points);

    // And return
    ret->points = return_points;
    ret->nb_points = nb_final_points;
}



// Compute the angle in radians between two normalized vectors
static inline float _edge_get_angle(vect2f_t a, vect2f_t b) {
    return (float) acos(a.x * b.x + a.y * b.y);
}


#define PI_DIV_2 (PI / 2.0)


// Compute the score between two points (score is between 0 and PI)
static inline float _edge_score_link(
    edge_point_t* point,
    edge_point_t* other,
    edge_point_t* prev
) {
    vect2f_t otherlink = {
        .x = other->position.x - point->position.x,
        .y = other->position.y - point->position.y
    };
    float length = sqrt(otherlink.x * otherlink.x + otherlink.y * otherlink.y);

    if (prev == NULL) return length;

    otherlink.x /= length;
    otherlink.y /= length;

    vect2f_t prevlink = {
        .x = prev->position.x - point->position.x,
        .y = prev->position.y - point->position.y
    };
    prevlink = vect_normalize_2f(prevlink);

    return _edge_get_angle(otherlink, prevlink) / length;
}


void _edge_create_link(
    edge_point_t* a, edge_point_t* b,
    edge_point_t** alink, edge_point_t** blink,
    float* ascore, float* bscore,
    float score
) {
    if (*alink != NULL) {
        if ((*alink)->link1 == a) {
            (*alink)->link1 = NULL;
            (*alink)->score1 = 0;
        } else {
            (*alink)->link2 = NULL;
            (*alink)->score2 = 0;
        }
    }
    if (*blink != NULL) {
        if ((*blink)->link1 == b) {
            (*blink)->link1 = NULL;
            (*blink)->score1 = 0;
        } else {
            (*blink)->link2 = NULL;
            (*blink)->score2 = 0;
        }
    }
    *alink = b;
    *blink = a;
    *ascore = score;
    *bscore = score;
}


void _edge_link_points(edge_lines_t* data) {
/*
    Abstract (not as lazy here):

    To create line we must link points together. For simplicity reason,
    a point can be linked to a maximum of 2 other points.
    Each link can't exced a certain amount in length. So we have a maximum
    search radius arround each point to choose to which other point we will
    be linked. But when there is many points arround a points, we must have a
    way to choose the one we want to be linked with.
    So to choose I suggest to compute a score for each points arround the
    current point.
    This score must based on the length of the link and the angle with the
    point's normals.

    So with a score system, the objective is to have the highest value if we
    sum all link's score for all lines.

    So for each point we will try to link them to the highest scored points
    in a specific radius.

    If the point we want to link with is already linked then we have 3
    possibilities:
        - Don't link with the already linked point
        - Replace the link on the first end of the already linked point
        - Replace the link on the second end of the already linked point

    To decide can try to replace a link and see if the score removed from the
    destroyed link plus the score added by the newly created link is positive.
    If this is the case then go ahead and replace the link becausee the total
    score is better that previously.
*/

    // Store some information to local variables because it's faster
    edge_point_t* points = data->points;
    size_t nb_points = data->nb_points;

    const float sq_radius = LINK_SEARCH_RADIUS * LINK_SEARCH_RADIUS;

    // For each points
    for (size_t i = 0; i < nb_points; i++) {
        edge_point_t* point = &points[i];

        // Repeat everything below 2 times because there is 2 links to build
        for (size_t k = 2; k--;) {
            // Find near other points and get the best scored one
            float best_score = -INFINITY;
            float best_gain = -INFINITY;
            edge_point_t* best_other = NULL;
            edge_point_t** best_point_link = NULL;
            edge_point_t** best_other_link = NULL;
            float* best_point_score = NULL;
            float* best_other_score = NULL;

            edge_point_t* other;
            for (size_t j = 0; j < nb_points; j++) {
                other = &points[j];
                if (other == point) continue;

                // Check if the other point is in radius
                float dx = other->position.x - point->position.x;
                float dy = other->position.y - point->position.y;
                if ((dx*dx + dy*dy) > sq_radius) continue;

                // Check if we are already linked to this point
                if (point->link1 == other || point->link2 == other) continue;

                // Get the score gain if we link, link1
                float score1 = _edge_score_link(point, other, point->link2);
                float gain1 = score1 - point->score1;
                edge_point_t** other1link;
                float* other1score;
                if (other->link1 == NULL || other->score1 < other->score2) {
                    gain1 -= other->score1;
                    other1link = &other->link1;
                    other1score = &other->score1;
                } else {
                    gain1 -= other->score2;
                    other1link = &other->link2;
                    other1score = &other->score2;
                }

                if (gain1 > best_gain) {
                    best_gain = gain1;
                    best_score = score1;
                    best_other = other;
                    best_other_link = other1link;
                    best_point_link = &point->link1;
                    best_point_score = &point->score1;
                    best_other_score = other1score;
                }

                // Get the score gain if we link, link2
                float score2 = _edge_score_link(point, other, point->link1);
                float gain2 = score2 - point->score2;
                edge_point_t** other2link;
                float* other2score;
                if (other->link1 == NULL || other->score1 < other->score2) {
                    gain2 -= other->score1;
                    other2link = &other->link1;
                    other2score = &other->score1;
                } else {
                    gain2 -= other->score2;
                    other2link = &other->link2;
                    other2score = &other->score2;
                }

                if (gain2 > best_gain) {
                    best_gain = gain2;
                    best_score = score2;
                    best_other = other;
                    best_other_link = other2link;
                    best_point_link = &point->link2;
                    best_point_score = &point->score2;
                    best_other_score = other2score;
                }
            }

            if (best_other == NULL || best_gain <= 0) continue;

            _edge_create_link(
                point, best_other,
                best_point_link, best_other_link,
                best_point_score, best_other_score,
                best_score
            );
        }
    }
}



void _edge_create_lines(edge_lines_t* data) {
    edge_line_t* lines = (edge_line_t*)
        malloc(data->nb_points * sizeof(edge_line_t));
    size_t nb_lines = 0;

    // Store some information to local variables because it's faster
    edge_point_t* points = data->points;
    size_t nb_points = data->nb_points;

    bool* computeds = (bool*)
        malloc(nb_points * sizeof(bool));
    for (size_t i = nb_points; i--;)
        computeds[i] = false;

    // For each points
    for (size_t i = 0; i < nb_points; i++) {
        if (computeds[i]) continue;

        edge_point_t* initial = &points[i];

        // Create the line
        edge_line_t* line = &lines[nb_lines++];
        line->length = 0;
        line->first = initial;
        line->last = initial;
        line->loop = false;
        initial->line = line;

        edge_point_t* prev;
        edge_point_t* curr;

        // Extend the line on link2/next (to the last and travel using next)
        prev = initial;
        curr = initial->next;
        while (curr != NULL && curr != initial) {
            computeds[curr->index] = true;
            curr->line = line;

            float dx = prev->position.x - curr->position.x;
            float dy = prev->position.y - curr->position.y;
            line->length += (float) sqrt(dx*dx + dy*dy);

            if (curr->prev == prev) {
                // link1 is in union with prev so it's ok
                prev = curr;
                curr = curr->next;
            } else {
                // link1 is not in union with prev so switch link1 and link2
                edge_point_t* tmplink = curr->link1;
                float tmpscore = curr->score1;
                curr->link1 = curr->link2;
                curr->score1 = curr->score2;
                curr->link2 = tmplink;
                curr->score2 = tmpscore;
                // Now we can get the next
                prev = curr;
                curr = curr->next;
            }
        }
        line->last = prev;

        // If this line is a loop
        if (curr == initial) {
            line->loop = true;
            continue;
        }

        // Extend the line on link2/prev (to the first and travel using prev)
        prev = initial;
        curr = initial->prev;
        while (curr != NULL && curr != line->last) {
            computeds[curr->index] = true;
            curr->line = line;

            float dx = prev->position.x - curr->position.x;
            float dy = prev->position.y - curr->position.y;
            line->length += (float) sqrt(dx*dx + dy*dy);

            if (curr->next == prev) {
                // link2 is in union with prev so it's ok
                prev = curr;
                curr = curr->prev;
            } else {
                // link1 is not in union with prev so switch link1 and link2
                edge_point_t* tmplink = curr->link1;
                float tmpscore = curr->score1;
                curr->link1 = curr->link2;
                curr->score1 = curr->score2;
                curr->link2 = tmplink;
                curr->score2 = tmpscore;
                // Now we can get the next
                prev = curr;
                curr = curr->prev;
            }
        }
        line->first = prev;
    }

    // Prepare to return
    edge_line_t* final_lines = (edge_line_t*)
        malloc(nb_lines * sizeof(edge_line_t));
    for (size_t i = 0; i < nb_lines; i++) {
        final_lines[i] = lines[i];
    }

    // Free some malloc
    free(computeds);
    free(lines);

    // Return
    data->lines = final_lines;
    data->nb_lines = nb_lines;
}



void _edge_simplify_lines(
    // Lines
    edge_lines_t* data,
    // The minimum segment length
    int32_t min_length,
    // The minimum segment angle to avoid simplifying by removing line's points
    float min_angle
) {
/*
    Here are what is done to clean/simplify lines:
        - Remove too small lines
        - Reduce the number of points of a line
*/

    for (size_t i = 0; i < data->nb_lines;) {
        edge_line_t* line = &data->lines[i];

        // Check for too small line
        if (line->length < min_length) {
            // Line too small, so remove them
            data->nb_lines--;
            data->lines[i] = data->lines[data->nb_lines];
            continue;
        }

        // We didn't removed any line so we can increment i
        i++;

        // Simplify a line by removing points when possible
        /*edge_point_t* prev = line->first;
        edge_point_t* point = prev->next;
        float angle_sum = 0;
        while (point != NULL && point != line->last) {
            edge_point_t* next = point->next;

            // Compute the angle between last and next point

            vect2f_t seg1 = {
                .x = point->position.x - prev->position.x,
                .y = point->position.y - prev->position.y
            };
            seg1 = vect_normalize_2f(seg1);

            vect2f_t seg2 = {
                .x = next->position.x - point->position.x,
                .y = next->position.y - point->position.y
            };
            seg2 = vect_normalize_2f(seg2);

            float angle = _edge_get_angle(seg1, seg2);
            angle =  PI_DIV_2 - abs_f(angle - PI_DIV_2);

            // Compute the rotation direction (angle sign)

            float weird_scalar =
                prev->normal.x * point->normal.y -
                prev->normal.y * point->normal.x;
            if (weird_scalar > 0)
                angle = -angle;

            // Check if we can remove this point

            angle_sum += angle;
            if (abs_f(angle_sum) < min_angle) {
                // We can remove this point
                point->prev->next = point->next;
                point->next->prev = point->prev;
            } else {
                angle_sum = 0;
                prev = point;
            }

            point = point->next;
        }*/
    }
}



void _edge_simplify_points(edge_lines_t* data, float min_strengh) {
    for (size_t i = 0; i < data->nb_points;) {
        if (data->points[i].strength < min_strengh) {
            data->nb_points--;
            data->points[i] = data->points[data->nb_points];
            data->points[i].index = i;
        } else {
            i++;
        }
    }
}
