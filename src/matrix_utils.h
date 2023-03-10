#ifndef MATRIX_UTILS
#define MATRIX_UTILS

#include <stdio.h>

#include "debug.h"

enum MatrixType { int_matrix, double_matrix };

void print_matrix(void *A, int m, int n, enum MatrixType type, int debug);

#endif /* !MATRIX_UTILS */
