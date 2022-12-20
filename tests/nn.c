//
// Created by Mathias FILLIOL on 08/12/2022.
//
#include "logic/neural_network.h"

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** args) {
    if (argc == 1)
        train(1, 0.1);
    else if (argc == 2){
        int epochs = atoi(args[1]);
        train(epochs, 0.1);
    }
    else if (argc == 3) {
        int epochs = atoi(args[1]);
        double learning_rate = atof(args[2]);
        train(epochs, learning_rate);
    }
    else
        printf("Usage: %s [epochs] [learning_rate]", args[0]);
    return 0;
}
