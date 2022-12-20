//
// Created by Mathias FILLIOL on 27/10/2022.
//

#pragma once

#include "common.h"


#define NUM_INPUTS 2
#define NUM_HIDDEN_NODES 2
#define NUM_TRAINING_SETS 4
#define NUM_OUTPUTS 1


// Activation function and its derivative
double xsigmoid(double x);

double xderivative_sigmoid(double x);

double xinit_weight();

// Shuffle the dataset
void xshuffle(int *array, size_t n);

void xtrain(int epochs);

void xsave(double hiddenWeights[NUM_INPUTS][NUM_HIDDEN_NODES],
          double outputWeights[NUM_HIDDEN_NODES][NUM_OUTPUTS],
          double hiddenLayerBias[NUM_HIDDEN_NODES], double outputLayerBias[NUM_OUTPUTS]);

int xload(double hiddenWeights[NUM_INPUTS][NUM_HIDDEN_NODES],
          double outputWeights[NUM_HIDDEN_NODES][NUM_OUTPUTS],
          double hiddenLayerBias[NUM_HIDDEN_NODES], double outputLayerBias[NUM_OUTPUTS]);