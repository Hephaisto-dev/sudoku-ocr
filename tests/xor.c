//
// Created by Mathias FILLIOL on 27/10/2022.
//
#include "logic/xor.h"

#include <stdlib.h>


int main(int argc, char** args) {
    if (argc != 2)
        xtrain(10000);
    else {
        int epochs = atoi(args[1]);
        xtrain(epochs);
    }
    return 0;
}
