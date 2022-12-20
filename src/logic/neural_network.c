//
// Created by Mathias FILLIOL on 06/12/2022.
//
#include "neural_network.h"

int save(
        double hiddenWeights[NUM_INPUTS][NUM_HIDDEN_NODES],
        double outputWeights[NUM_HIDDEN_NODES][NUM_OUTPUTS],
        double hiddenLayerBias[NUM_HIDDEN_NODES],
        double outputLayerBias[NUM_OUTPUTS],
        const char* filename
) {
    FILE *f = fopen(filename, "wb");

    for (int i = 0; i < NUM_HIDDEN_NODES; i++) {
        for (int j = 0; j < NUM_INPUTS; j++) {
            if (fwrite(&hiddenWeights[j][i], sizeof(double),
                       1, f) != 1) {
                fclose(f);
                return 0;
            }
        }
    }

    for (int i = 0; i < NUM_OUTPUTS; i++) {
        for (int j = 0; j < NUM_HIDDEN_NODES; j++) {
            if (fwrite(&outputWeights[j][i], sizeof(double),
                       1, f) != 1) {
                fclose(f);
                return 0;
            }
        }
    }

    if (fwrite(hiddenLayerBias, sizeof(double),
               NUM_HIDDEN_NODES, f) != NUM_HIDDEN_NODES) {
        fclose(f);
        return 0;
    }

    if (fwrite(outputLayerBias, sizeof(double),
               NUM_OUTPUTS, f) != NUM_OUTPUTS) {
        fclose(f);
        return 0;
    }

    fclose(f);
    return 1;
}


int load(
        double hiddenWeights[NUM_INPUTS][NUM_HIDDEN_NODES],
        double outputWeights[NUM_HIDDEN_NODES][NUM_OUTPUTS],
        double hiddenLayerBias[NUM_HIDDEN_NODES],
        double outputLayerBias[NUM_OUTPUTS],
        const char* filename
) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        printf("Error opening file\n");
        return 0;
    }

    for (int i = 0; i < NUM_HIDDEN_NODES; i++) {
        for (int j = 0; j < NUM_INPUTS; j++) {
            if (fread(&hiddenWeights[j][i], sizeof(double),
                      1, f) != 1) {
                fclose(f);
                return 0;
            }
        }
    }

    for (int i = 0; i < NUM_OUTPUTS; i++) {
        for (int j = 0; j < NUM_HIDDEN_NODES; j++) {
            if (fread(&outputWeights[j][i], sizeof(double),
                      1, f) != 1) {
                fclose(f);
                return 0;
            }
        }
    }

    if (fread(hiddenLayerBias, sizeof(double),
              NUM_HIDDEN_NODES, f) != NUM_HIDDEN_NODES) {
        fclose(f);
        return 0;
    }

    if (fread(outputLayerBias, sizeof(double),
              NUM_OUTPUTS, f) != NUM_OUTPUTS) {
        fclose(f);
        return 0;
    }

    fclose(f);

    return 1;
}

void shuffle(size_t *array, size_t n) {
    if (n > 1) {
        for (size_t i = 0, m = n - 1; i < m; i++) {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            size_t tmp = array[j];
            array[j] = array[i];
            array[i] = tmp;
        }
    }
}

void train(int epochs, const double lr, const char* ai_data_file) {

    // Initialize layers
    double hiddenLayer[NUM_HIDDEN_NODES];
    double outputLayer[NUM_OUTPUTS];

    // Biases
    double hiddenLayerBias[NUM_HIDDEN_NODES];
    double outputLayerBias[NUM_OUTPUTS];

    // Weights
    double hiddenWeights[NUM_INPUTS][NUM_HIDDEN_NODES];
    double outputWeights[NUM_HIDDEN_NODES][NUM_OUTPUTS];

    const char *imgs_filename = "dataset/train-images-idx3-ubyte.gz";
    const char *labels_filename = "dataset/train-labels-idx1-ubyte.gz";

    dataset_t dataset = dataset_load_mnist(imgs_filename, labels_filename);

    if (!dataset) {
        mprintf("Error: Can't load dataset files\n");
        return;
    }

    if (!load(hiddenWeights, outputWeights, hiddenLayerBias, outputLayerBias, ai_data_file)) {
        printf("Can't load nn_data.bin, initializing weights and biases\n");
        for (int i = 0; i < NUM_INPUTS; i++)
            for (int j = 0; j < NUM_HIDDEN_NODES; j++)
                hiddenWeights[i][j] = init_weight();

        for (int i = 0; i < NUM_HIDDEN_NODES; i++) {
            hiddenLayerBias[i] = init_weight();
            for (int j = 0; j < NUM_OUTPUTS; j++)
                outputWeights[i][j] = init_weight();
        }

        // Initialize output layer bias
        for (int i = 0; i < NUM_OUTPUTS; i++)
            outputLayerBias[i] = init_weight();
    }
    size_t trainingSetOrder[dataset->nb_imgs];
    for (size_t i = 0; i < dataset->nb_imgs; i++) {
        trainingSetOrder[i] = i;
    }

    double *imgs_data = malloc(sizeof(double) * dataset->nb_imgs * NUM_INPUTS);
    for (size_t i = 0; i < dataset->nb_imgs; i++) {
        for (size_t j = 0; j < NUM_INPUTS; j++) {
            imgs_data[NUM_INPUTS * i + j] =
                    (dataset->imgs[i].data[j] > 127) ? 0 : 1;
        }
    }

    size_t totalSuccess = 0;
    size_t totalIteration = dataset->nb_imgs;

    // Train the neural network for a number of epochs
    for (int epoch = 0; epoch < epochs; epoch++) {
        printf("====================================================\n");
        printf("Epoch %d\n", epoch);
        printf("====================================================\n");

        // As per Stochastic Gradient Descent, shuffle the order of the training set
        shuffle(trainingSetOrder, dataset->nb_imgs);
        int success = 0;

        // Cycle through each of the training set elements
        for (size_t x = 0; x < totalIteration; x++) {
            int i = trainingSetOrder[x];
            double *img = &imgs_data[i * NUM_INPUTS];
            uint8_t number = dataset->imgs[i].character - '0';
            if (number > 9) continue;

            // Forward pass

            // Compute hidden layer activation
            for (int j = 0; j < NUM_HIDDEN_NODES; j++) {
                double activation = hiddenLayerBias[j];
                for (int k = 0; k < NUM_INPUTS; k++) {
                    activation += img[k] * hiddenWeights[k][j];
                    //printf("Image data : %f\n", ((double ) image.data[k]));
                }
                //printf("Activation : %f\n", activation);
                hiddenLayer[j] = sigmoid(activation);
                //printf("Hidden layer: %f\n", hiddenLayer[j]);
            }

            // Compute output layer activation
            double sum = 0;
            for (int j = 0; j < NUM_OUTPUTS; j++) {
                double activation = outputLayerBias[j];
                for (int k = 0; k < NUM_HIDDEN_NODES; k++) {
                    activation += hiddenLayer[k] * outputWeights[k][j];
                }
                outputLayer[j] = activation;
                sum += exp(activation);
            }

            // Apply the softmax function to the output layer
            for (int j = 0; j < NUM_OUTPUTS; j++) {
                outputLayer[j] = softmax(outputLayer[j], sum);
            }
            /*for (int j = 0; j < NUM_OUTPUTS; j++) {
                double activation = outputLayerBias[j];
                for (int k = 0; k < NUM_HIDDEN_NODES; k++){
                    activation += hiddenLayer[k] * outputWeights[k][j];
                    //printf("Output weight: %f\n", hiddenLayer[k]);
                }
                outputLayer[j] = sigmoid(activation);
                //printf("Activation : %f\n", activation);
            }*/


            uint8_t max = 0;
            double maxv = outputLayer[0];
            for (uint8_t j = 1; j < NUM_OUTPUTS; j++) {
                if (maxv < outputLayer[j]) {
                    max = j;
                    maxv = outputLayer[j];
                }
            }

            // Print the results from forward pass
            /*printf("Percentages : %f %f %f %f %f %f %f %f %f %f\n",
                   outputLayer[0], outputLayer[1], outputLayer[2], outputLayer[3],
                   outputLayer[4], outputLayer[5], outputLayer[6], outputLayer[7],
                   outputLayer[8], outputLayer[9]);*/
            /*printf("Entrée: %i  Sortie: %i ", number, max);
            */if (number == max) /*
                printf("\033[0;32m");
                printf("SUCCES ");
                printf("\033[0m");*/
                success++;/*
            } else {
                printf("\033[0;31m");
                printf("ECHEC ");
                printf("\033[0m");
            }
            printf("Accuracy: %d/%zu (%.2f%%)\n", success, x + 1, (double) success / (x + 1) * 100);*/


            // Back Propagation

            // Compute change in output weights
            //Best way to do it
            double deltaOutput[NUM_OUTPUTS];
            sum = 0;
            double exps[NUM_OUTPUTS];
            for (size_t i = 0; i < NUM_OUTPUTS; i++) {
                double v = exp(outputLayer[i]);
                exps[i] = v;
                sum += v;
            }

            double sum_2 = sum * sum;
            for (size_t i = 0; i < NUM_OUTPUTS; i++) {
                double v = exps[i];
                if (sum_2 != 0) deltaOutput[i] = v * (sum - v) / sum_2;
                else deltaOutput[i] = 0;
            }

            for (int j = 0; j < NUM_OUTPUTS; j++) {
                deltaOutput[j] *= -outputLayer[j];
            }
            if (outputLayer[number] != 0)
                deltaOutput[number] *= 1 - (1 / outputLayer[number]);

            /*
            double deltaOutput[NUM_OUTPUTS];
            for (int j = 0; j < NUM_OUTPUTS; j++) {
                double errorOutput = (number == j) - outputLayer[j];
                deltaOutput[j] = errorOutput;
            }
            */

            /*
            double loss = 0;
            for (int j = 0; j < NUM_OUTPUTS; j++) {
                if (number == j) {
                    loss += -log(outputLayer[j]);
                }
            }

            double deltaOutput[NUM_OUTPUTS];
            for (int j = 0; j < NUM_OUTPUTS; j++) {
                if (number == j) {
                    deltaOutput[j] = -1.0 / outputLayer[j];
                } else {
                    deltaOutput[j] = 0;
                }
            }*/

            /*
            //Classical way
            double deltaOutput[NUM_OUTPUTS];
            for (int j = 0; j < NUM_OUTPUTS; j++) {
                double errorOutput = (number == j) - outputLayer[j];
                deltaOutput[j] = errorOutput * derivative_sigmoid(outputLayer[j]);
                //printf("Error output %f\n", errorOutput);
            }*/


            // Compute change in hidden weights
            double deltaHidden[NUM_HIDDEN_NODES];
            for (int j = 0; j < NUM_HIDDEN_NODES; j++) {
                double errorHidden = 0;
                for (int k = 0; k < NUM_OUTPUTS; k++)
                    errorHidden += deltaOutput[k] * outputWeights[j][k];
                deltaHidden[j] =
                        errorHidden * derivative_sigmoid(hiddenLayer[j]);
            }

            // Update output weights
            for (int j = 0; j < NUM_OUTPUTS; j++) {
                double delta = deltaOutput[j] * lr;
                outputLayerBias[j] += delta;
                for (int k = 0; k < NUM_HIDDEN_NODES; k++)
                    outputWeights[k][j] += delta * hiddenLayer[k];
            }

            // Update hidden weights
            for (int j = 0; j < NUM_HIDDEN_NODES; j++) {
                hiddenLayerBias[j] += deltaHidden[j] * lr;
            }
            for (int k = 0; k < NUM_INPUTS; k++) {
                double d = img[k] * lr;
                for (int j = 0; j < NUM_HIDDEN_NODES; j++)
                    hiddenWeights[k][j] += d * deltaHidden[j];
            }
        }
        totalSuccess += success;
        printf("End of epoch %i; Accuracy: (%.2f%%)\n", epoch,
               (double) success / totalIteration * 100);
        fflush(stdout);
    }

    // After training, print the final weights and biases
    /*printf("Poids finaux de la couche cachée\n[ ");
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
    printf("]\n");*/

    printf("Total Accuracy: %zu/%zu (%.2f%%)\n", totalSuccess,
           totalIteration * epochs,
           (double) totalSuccess / (totalIteration * epochs) * 100);
    test_accuracy(hiddenWeights, outputWeights, hiddenLayerBias,
                  outputLayerBias);
    fflush(stdout);

    if (save(hiddenWeights, outputWeights, hiddenLayerBias, outputLayerBias, ai_data_file))
        printf("Saved\n");
    else
        printf("Error while saving\n");
    dataset_free(dataset);
    free(imgs_data);
}

void test_accuracy(
        double hiddenWeights[NUM_INPUTS][NUM_HIDDEN_NODES],
        double outputWeights[NUM_HIDDEN_NODES][NUM_OUTPUTS],
        double hiddenLayerBias[NUM_HIDDEN_NODES],
        double outputLayerBias[NUM_OUTPUTS]
) {
    const char *imgs_filename = "dataset/t10k-images-idx3-ubyte.gz";
    const char *labels_filename = "dataset/t10k-labels-idx1-ubyte.gz";

    dataset_t dataset = dataset_load_mnist(imgs_filename, labels_filename);
    size_t totalSuccess = 0;

    if (!dataset) {
        mprintf("Error: Can't load test images\n");
        return;
    }
    double *imgs_data = malloc(sizeof(double) * dataset->nb_imgs * NUM_INPUTS);
    for (size_t i = 0; i < dataset->nb_imgs; i++) {
        for (size_t j = 0; j < NUM_INPUTS; j++) {
            imgs_data[NUM_INPUTS * i + j] =
                    (dataset->imgs[i].data[j] > 127) ? 0 : 1;
        }
    }
    for (size_t i = 0; i < dataset->nb_imgs; ++i) {
        char prediction = predict(hiddenWeights, outputWeights, hiddenLayerBias,
                                  outputLayerBias,
                                  &imgs_data[i * NUM_INPUTS]);
        if (prediction == dataset->imgs[i].character)
            totalSuccess++;
    }
    printf("Real Accuracy: %zu/%zu (%.2f%%)\n", totalSuccess, dataset->nb_imgs,
           (double) totalSuccess / dataset->nb_imgs * 100);
    dataset_free(dataset);
    free(imgs_data);
}

char predict(
        double hiddenWeights[NUM_INPUTS][NUM_HIDDEN_NODES],
        double outputWeights[NUM_HIDDEN_NODES][NUM_OUTPUTS],
        double hiddenLayerBias[NUM_HIDDEN_NODES],
        double outputLayerBias[NUM_OUTPUTS],
        double *img_data
) {
    // Forward pass

    // Compute hidden layer activation
    double hiddenLayer[NUM_HIDDEN_NODES];
    for (int j = 0; j < NUM_HIDDEN_NODES; j++) {
        double activation = hiddenLayerBias[j];
        for (int k = 0; k < NUM_INPUTS; k++) {
            activation += img_data[k] * hiddenWeights[k][j];
        }
        hiddenLayer[j] = sigmoid(activation);
    }

    // Compute output layer activation
    double outputLayer[NUM_OUTPUTS];
    double sum = 0;
    for (int j = 0; j < NUM_OUTPUTS; j++) {
        double activation = outputLayerBias[j];
        for (int k = 0; k < NUM_HIDDEN_NODES; k++) {
            activation += hiddenLayer[k] * outputWeights[k][j];
        }
        outputLayer[j] = activation;
        sum += exp(activation);
    }

    // Apply the softmax function to the output layer
    for (int j = 0; j < NUM_OUTPUTS; j++) {
        outputLayer[j] = softmax(outputLayer[j], sum);
    }
    uint8_t max = 0;
    double maxv = outputLayer[0];
    for (uint8_t j = 1; j < NUM_OUTPUTS; j++) {
        if (maxv < outputLayer[j]) {
            max = j;
            maxv = outputLayer[j];
        }
    }

    return max + '0';
}

char predict_load(dataset_img_t image, const char* ai_data_file) {
    // Biases
    double hiddenLayerBias[NUM_HIDDEN_NODES];
    double outputLayerBias[NUM_OUTPUTS];

    // Weights
    double hiddenWeights[NUM_INPUTS][NUM_HIDDEN_NODES];
    double outputWeights[NUM_HIDDEN_NODES][NUM_OUTPUTS];

    if (!load(hiddenWeights, outputWeights, hiddenLayerBias, outputLayerBias,
        ai_data_file)
    ) {
        printf("Impossible de charger les poids et les biais\n");
        exit(1);
    }

    double *img_data = malloc(sizeof(double) * NUM_INPUTS);
    for (size_t j = 0; j < NUM_INPUTS; j++) {
        img_data[j] = (image.data[j] > 127) ? 0 : 1;
    }
    return predict(hiddenWeights, outputWeights, hiddenLayerBias,
                   outputLayerBias, img_data);
}
