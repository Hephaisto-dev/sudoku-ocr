//
// Created by Mathias FILLIOL on 25/10/2022.
//

#ifndef OCR_CELLDETECTION_H
#define OCR_CELLDETECTION_H

#include "common.h"

#include "graphic/image.h"
#include "image/detection/grid.h"


typedef struct cells_s {
	image_t* cells;
	size_t nb_cells;
	grid_t grid;
} cells_t;


cells_t cells_detect(image_t img);


#endif //OCR_CELLDETECTION_H
