#include "logic/dataset.h"
#include "image/postprocess.h"

#include "zlib.h"



// Little endian-byte order to host byteorder
static inline uint32_t be_to_h_u32(uint32_t v) {
    uint8_t* x = (uint8_t*) &v;
    return (*(x) << 24) | (*(x+1) << 16) | (*(x+2) << 8) | (*(x+3) << 0);
}



void dataset_free(dataset_t dataset) {
    for (size_t i = 0; i < dataset->nb_imgs; i++) {
        if (dataset->imgs[i].data)
            free(dataset->imgs[i].data);
    }
    if (dataset->imgs)
        free(dataset->imgs);
    free(dataset);
}



void dataset_write_on_img(dataset_img_t* src, image_t dst) {
    img_lock(dst);
    uint8_t* srcdata = src->data;
    color_t* dstdata = dst->data;
    int32_t w = min_i32(src->w, dst->width);
    int32_t h = min_i32(src->h, dst->height);
    int32_t sw = src->w;
    int32_t dw = dst->width;
    for (int32_t y = 0; y < h; y++) {
        for (int32_t x = 0; x < w; x++) {
            uint8_t v = srcdata[y * sw + x];
            color_t c = {.r = v, .g = v, .b = v, .a = 0xFF};
            dstdata[y * dw + x] = c;
        }
    }
    img_unlock(dst);
}


dataset_img_t dataset_from_img(image_t img) {
    dataset_img_t r;
    r.character = '\0';
    r.data = malloc(28*28);
    r.w = 28;
    r.h = 28;
    image_t resized = img_resize(img, 28, 28);
    for (size_t i = 0; i < (28*28); i++) {
        r.data[i] =
        (resized->data[i].r + resized->data[i].g + resized->data[i].b) / 3;
    }
    img_free(resized);
    return r;
}


static bool _dataset_load_mnist_imgs(dataset_t ds, const char* filename) {
    gzFile file = gzopen(filename, "rb");
    if (file == NULL) {
        // TODO : Error
        gzclose(file);
        return false;
    }

    // Read and check magic number
    uint32_t magic;
    if (gzread(file, &magic, 4) != 4 || be_to_h_u32(magic) != 0x00000803) {
        // TODO : Error
        gzclose(file);
        return false;
    }

    // Read other header informations

    uint32_t nb_images, nb_rows, nb_columns;

    if (gzread(file, &nb_images,  4) != 4) {
        // TODO : Error
        gzclose(file);
        return false;
    }

    if (gzread(file, &nb_rows,    4) != 4) {
        // TODO : Error
        gzclose(file);
        return false;
    }

    if (gzread(file, &nb_columns, 4) != 4) {
        // TODO : Error
        gzclose(file);
        return false;
    }

    nb_images  = be_to_h_u32(nb_images);
    nb_rows    = be_to_h_u32(nb_rows);
    nb_columns = be_to_h_u32(nb_columns);

    // Read images

    ds->imgs = (dataset_img_t*) malloc(nb_images * sizeof(dataset_img_t));
    ds->nb_imgs = nb_images;

    const int img_data_len = nb_rows * nb_columns;

    for (uint32_t i = 0; i < nb_images; i++) {
        dataset_img_t* img = &ds->imgs[i];
        img->w = nb_columns;
        img->h = nb_rows;
        img->data = (uint8_t*) malloc(img_data_len);
        if (gzread(file, img->data, img_data_len) != img_data_len) {
            // TODO : Error
            free(img->data);
            img->data = NULL;
            img->w = 0;
            img->h = 0;
            continue;
        }
        // Inverse colors (in the database file 0 is white and 255 is black)
        for (int j = img_data_len; j--;)
            img->data[j] = 0xFF - img->data[j];
    }

    gzclose(file);

    return true;
}



static bool _dataset_load_mnist_labels(dataset_t ds, const char* filename) {
    gzFile file = gzopen(filename, "rb");
    if (file == NULL) {
        // TODO : Error
        gzclose(file);
        return false;
    }

    // Read and check magic number
    uint32_t magic;
    if (gzread(file, &magic, 4) != 4 || be_to_h_u32(magic) != 0x00000801) {
        // TODO : Error
        gzclose(file);
        return false;
    }

    // Read other header informations

    uint32_t nb_labels;

    if (gzread(file, &nb_labels,  4) != 4) {
        // TODO : Error
        gzclose(file);
        return false;
    }

    nb_labels = be_to_h_u32(nb_labels);

    if (nb_labels != ds->nb_imgs) {
        gzclose(file);
        return false;
    }

    // Read labels

    for (uint32_t i = 0; i < nb_labels; i++) {
        if (gzread(file, &ds->imgs[i].character, 1) != 1) {
            // TODO : Error
            ds->imgs[i].character = '\0';
            continue;
        }
        ds->imgs[i].character += '0';
    }

    gzclose(file);

    return true;
}



dataset_t dataset_load_mnist(const char* imgsfile, const char* labelsfile) {
    dataset_t dataset = (dataset_t) malloc(sizeof(struct dataset_s));
    bool success =
        _dataset_load_mnist_imgs(dataset, imgsfile) &&
        _dataset_load_mnist_labels(dataset, labelsfile);

    if (!success) {
        dataset_free(dataset);
        return NULL;
    }

    return dataset;
}
