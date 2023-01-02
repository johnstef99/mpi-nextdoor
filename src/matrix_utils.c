#include "matrix_utils.h"
#include "debug.h"

void print_matrix(void *A, int m, int n, enum MatrixType type) {
  for (int i = 0; i < m; i++) {
    for (int j = 0; j < n; j++) {
      switch (type) {
      case int_matrix:
        dd("%8d ", (int)((int *)A)[i * n + j]);
        break;
      case double_matrix:
        dd("%8.2f ", (double)((double *)A)[i * n + j]);
        break;
      }
    }
    dd("\n");
  }
}

