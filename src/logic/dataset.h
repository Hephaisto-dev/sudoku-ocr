#pragma once

#include "common.h"

#include "graphic/image.h"


typedef struct dataset_img_s {
    uint8_t* data;
    uint8_t w;
    uint8_t h;
    char character;
} dataset_img_t;

typedef struct dataset_s {
    dataset_img_t* imgs;
    size_t nb_imgs;
} *dataset_t;


dataset_t dataset_load_mnist(const char* imgsfile, const char* labelsfile);

void dataset_free(dataset_t dataset);

void dataset_write_on_img(dataset_img_t* src, image_t img);

dataset_img_t dataset_from_img(image_t img);
