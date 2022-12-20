//
// Created by Mathias FILLIOL on 06/12/2022.
//

#pragma once

#include "common.h"

#include "dataset.h"
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <stdlib.h>

#define NUM_INPUTS (28*28)
#define NUM_HIDDEN_NODES 128
#define NUM_OUTPUTS 10


// Activation function and its derivative

static inline double sigmoid(double x) { return 1. / (1. + exp(-x)); }

static inline double derivative_sigmoid(double x) { return x * (1. - x); }

static inline double softmax(double x, double sum) {
    if (sum == 0) return 0;
    return exp(x) / sum;
}

static inline double derivative_softmax(double x, double sum) {
    double exp_x = exp(x);
    return (sum * exp_x - exp_x * exp_x) / (sum * sum);
}

static inline double init_weight() { return 1 - (double) (rand() % 2) * 2; }



// Shuffle the dataset
void shuffle(size_t *array, size_t n);

void train(int epochs, const double learning_rate, const char* filename);

void test_accuracy(
        double hiddenWeights[NUM_INPUTS][NUM_HIDDEN_NODES],
        double outputWeights[NUM_HIDDEN_NODES][NUM_OUTPUTS],
        double hiddenLayerBias[NUM_HIDDEN_NODES],
        double outputLayerBias[NUM_OUTPUTS]
);

int save(
    double hiddenWeights[NUM_INPUTS][NUM_HIDDEN_NODES],
    double outputWeights[NUM_HIDDEN_NODES][NUM_OUTPUTS],
    double hiddenLayerBias[NUM_HIDDEN_NODES],
    double outputLayerBias[NUM_OUTPUTS],
    const char* filename
);

int load(
    double hiddenWeights[NUM_INPUTS][NUM_HIDDEN_NODES],
    double outputWeights[NUM_HIDDEN_NODES][NUM_OUTPUTS],
    double hiddenLayerBias[NUM_HIDDEN_NODES],
    double outputLayerBias[NUM_OUTPUTS],
    const char* filename
);

void test_accuracy(
        double hiddenWeights[NUM_INPUTS][NUM_HIDDEN_NODES],
        double outputWeights[NUM_HIDDEN_NODES][NUM_OUTPUTS],
        double hiddenLayerBias[NUM_HIDDEN_NODES],
        double outputLayerBias[NUM_OUTPUTS]
);

// Predict which number a image represents
char predict_load(dataset_img_t image, const char* filename);

char predict(
        double hiddenWeights[NUM_INPUTS][NUM_HIDDEN_NODES],
        double outputWeights[NUM_HIDDEN_NODES][NUM_OUTPUTS],
        double hiddenLayerBias[NUM_HIDDEN_NODES],
        double outputLayerBias[NUM_OUTPUTS],
        double *img_data
);
