//
// Created by Mathias FILLIOL on 27/10/2022.
//

#include "xor.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


double xsigmoid(double x) { return 1 / (1 + exp(-x)); }

double xderivative_sigmoid(double x) { return x * (1 - x); }

double xinit_weight() { return ((double) rand()) / ((double) RAND_MAX); }

void xsave(double hiddenWeights[NUM_INPUTS][NUM_HIDDEN_NODES],
          double outputWeights[NUM_HIDDEN_NODES][NUM_OUTPUTS],
          double hiddenLayerBias[NUM_HIDDEN_NODES], double outputLayerBias[NUM_OUTPUTS]) {
    FILE *f = fopen("tests/out/xor_data.bin", "wb");
    for (int i = 0; i < NUM_HIDDEN_NODES; i++) {
        for (int j = 0; j < NUM_INPUTS; j++) {
            fwrite(&hiddenWeights[i][j], sizeof(double), 1, f);
        }
    }
    for (int i = 0; i < NUM_OUTPUTS; i++) {
        for (int j = 0; j < NUM_HIDDEN_NODES; j++) {
            fwrite(&outputWeights[i][j], sizeof(double), 1, f);
        }
    }
    for (int i = 0; i < NUM_HIDDEN_NODES; i++) {
        fwrite(&hiddenLayerBias[i], sizeof(double), 1, f);
    }
    for (int i = 0; i < NUM_OUTPUTS; i++) {
        fwrite(&outputLayerBias[i], sizeof(double), 1, f);
    }
    fclose(f);
}

int xload(double hiddenWeights[NUM_INPUTS][NUM_HIDDEN_NODES],
          double outputWeights[NUM_HIDDEN_NODES][NUM_OUTPUTS],
          double hiddenLayerBias[NUM_HIDDEN_NODES], double outputLayerBias[NUM_OUTPUTS]) {
    FILE *f = fopen("tests/out/xor_data.bin", "rb");
    if (f == NULL) {
        return 0;
    }
    for (int i = 0; i < NUM_HIDDEN_NODES; i++) {
        for (int j = 0; j < NUM_INPUTS; j++) {
            if (!fread(&hiddenWeights[i][j], sizeof(double), 1, f))
            {
                fclose(f);
                return 0;
            }
        }
    }
    for (int i = 0; i < NUM_OUTPUTS; i++) {
        for (int j = 0; j < NUM_HIDDEN_NODES; j++) {
            if(!fread(&outputWeights[i][j], sizeof(double), 1, f))
            {
                fclose(f);
                return 0;
            }
        }
    }
    for (int i = 0; i < NUM_HIDDEN_NODES; i++) {
        if (!fread(&hiddenLayerBias[i], sizeof(double), 1, f))
        {
            fclose(f);
            return 0;
        }
    }
    for (int i = 0; i < NUM_OUTPUTS; i++) {
        if (!fread(&outputLayerBias[i], sizeof(double), 1, f))
        {
            fclose(f);
            return 0;
        }
    }
    fclose(f);
    return 1;
}

void xshuffle(int *array, size_t n) {
    if (n > 1) {
        size_t i;
        for (i = 0; i < n - 1; i++) {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            int buffer = array[j];
            array[j] = array[i];
            array[i] = buffer;
        }
    }
}

void xtrain(int epochs) {

    // Learning rate
    const double lr = 0.2f;

    // Initialize layers
    double hiddenLayer[NUM_HIDDEN_NODES];
    double outputLayer[NUM_OUTPUTS];

    // Biases
    double hiddenLayerBias[NUM_HIDDEN_NODES];
    double outputLayerBias[NUM_OUTPUTS];

    // Weights
    double hiddenWeights[NUM_INPUTS][NUM_HIDDEN_NODES];
    double outputWeights[NUM_HIDDEN_NODES][NUM_OUTPUTS];

    // Training set
    double training_inputs[NUM_TRAINING_SETS][NUM_INPUTS] = {{0.0f, 0.0f},
                                                             {1.0f, 0.0f},
                                                             {0.0f, 1.0f},
                                                             {1.0f, 1.0f}};
    double training_outputs[NUM_TRAINING_SETS][NUM_OUTPUTS] = {{0.0f},
                                                               {1.0f},
                                                               {1.0f},
                                                               {0.0f}};

    if (!xload(hiddenWeights, outputWeights, hiddenLayerBias, outputLayerBias)) {
        for (int i = 0; i < NUM_INPUTS; i++)
            for (int j = 0; j < NUM_HIDDEN_NODES; j++)
                hiddenWeights[i][j] = xinit_weight();

        for (int i = 0; i < NUM_HIDDEN_NODES; i++) {
            hiddenLayerBias[i] = xinit_weight();
            for (int j = 0; j < NUM_OUTPUTS; j++)
                outputWeights[i][j] = xinit_weight();
        }

        // Initialize output layer bias
        for (int i = 0; i < NUM_OUTPUTS; i++)
            outputLayerBias[i] = xinit_weight();
    }


    int trainingSetOrder[] = {0, 1, 2, 3};

    // Train the neural network for a number of epochs
    for (int epoch = 0; epoch < epochs; epoch++) {
        printf("====================================================\n");
        printf("Epoque %d\n", epoch);
        printf("====================================================\n");
        // As per Stochastic Gradient Descent, shuffle the order of the training set
        xshuffle(trainingSetOrder, NUM_TRAINING_SETS);

        // Cycle through each of the training set elements
        for (int x = 0; x < NUM_TRAINING_SETS; x++) {

            int i = trainingSetOrder[x];

            // Forward pass

            // Compute hidden layer activation
            for (int j = 0; j < NUM_HIDDEN_NODES; j++) {
                double activation = hiddenLayerBias[j];
                for (int k = 0; k < NUM_INPUTS; k++)
                    activation += training_inputs[i][k] * hiddenWeights[k][j];
                hiddenLayer[j] = xsigmoid(activation);
            }

            // Compute output layer activation
            for (int j = 0; j < NUM_OUTPUTS; j++) {
                double activation = outputLayerBias[j];
                for (int k = 0; k < NUM_HIDDEN_NODES; k++)
                    activation += hiddenLayer[k] * outputWeights[k][j];
                outputLayer[j] = xsigmoid(activation);
            }

            double rounded = round(outputLayer[0]);

            // Print the results from forward pass
            printf("Entrée: %g %g  Sortie: %g  Sortie arrondie: %g  Sortie attendue: %g  ",
                   training_inputs[i][0], training_inputs[i][1], outputLayer[0],
                   rounded, training_outputs[i][0]);
            if (rounded == training_outputs[i][0]) {
                printf("\033[0;32m");
                printf("SUCCES\n");
                printf("\033[0m");
            } else {
                printf("\033[0;31m");
                printf("ECHEC\n");
                printf("\033[0m");
            }


            // Back Propagation

            // Compute change in output weights
            double deltaOutput[NUM_OUTPUTS];
            for (int j = 0; j < NUM_OUTPUTS; j++) {
                double errorOutput = (training_outputs[i][j] - outputLayer[j]);
                deltaOutput[j] = errorOutput * xderivative_sigmoid(outputLayer[j]);
            }

            // Compute change in hidden weights
            double deltaHidden[NUM_HIDDEN_NODES];
            for (int j = 0; j < NUM_HIDDEN_NODES; j++) {
                double errorHidden = 0.0f;
                for (int k = 0; k < NUM_OUTPUTS; k++)
                    errorHidden += deltaOutput[k] * outputWeights[j][k];
                deltaHidden[j] = errorHidden * xderivative_sigmoid(hiddenLayer[j]);
            }

            // Update output weights
            for (int j = 0; j < NUM_OUTPUTS; j++) {
                outputLayerBias[j] += deltaOutput[j] * lr;
                for (int k = 0; k < NUM_HIDDEN_NODES; k++)
                    outputWeights[k][j] += hiddenLayer[k] * deltaOutput[j] * lr;
            }

            // Update hidden weights
            for (int j = 0; j < NUM_HIDDEN_NODES; j++) {
                hiddenLayerBias[j] += deltaHidden[j] * lr;
                for (int k = 0; k < NUM_INPUTS; k++)
                    hiddenWeights[k][j] += training_inputs[i][k] * deltaHidden[j] * lr;
            }
        }
    }

    // After training, print the final weights and biases
    printf("Poids finaux de la couche cachée\n[ ");
    for (int j = 0; j < NUM_HIDDEN_NODES; j++) {
        printf("[ ");
        for (int k = 0; k < NUM_INPUTS; k++)
            printf("%f ", hiddenWeights[k][j]);
        printf("] ");
    }

    printf("]\nBiais finaux de la couche cachée\n[ ");
    for (int j = 0; j < NUM_HIDDEN_NODES; j++) {
        printf("%f ", hiddenLayerBias[j]);
    }

    printf("]\nPoids finaux de la couche de sortie\n[ ");
    for (int j = 0; j < NUM_OUTPUTS; j++) {
        printf("[ ");
        for (int k = 0; k < NUM_HIDDEN_NODES; k++)
            printf("%f ", outputWeights[k][j]);
        printf("]\n");
    }

    printf("Biais finaux de la couche de sortie\n[ ");
    for (int j = 0; j < NUM_OUTPUTS; j++)
        printf("%f ", outputLayerBias[j]);

    printf("]\n");
    fflush(stdout);

    xsave(hiddenWeights, outputWeights, hiddenLayerBias, outputLayerBias);
}
