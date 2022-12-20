# Reynet (OCR)

Reynet is a simple OCR (Optical Character Recognition) made in C and intended to solve a Sodoku from an image.

## Installation

To build both the Sodoku solver and Reynet, run the following command:
```bash
$ make
```

## Usage

To run the Reynet OCR gui, run the following command:
```bash
$ ./build/reynet
```

To run the command-line solver, run the following command:
```bash
$ ./build/solver <path_to_the_sudoku>
```

## Tests

To build the test and run it, run the following commands:
```bash
$ make test/<name_of_the_test>
```
To build the test, run:
```bash
$ make build/<name_of_the_test>
```
To run the test, run:
```bash
$ make run/<name_of_the_test>
```
