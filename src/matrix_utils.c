#include "matrix_utils.h"

void print_matrix(void *A, int m, int n, enum MatrixType type, int debug) {
  for (int i = 0; i < m; i++) {
    for (int j = 0; j < n; j++) {
      switch (type) {
      case int_matrix:
        if (debug)
          dd("%8d ", (int)((int *)A)[i * n + j]);
        else
          printf("%8d ", (int)((int *)A)[i * n + j]);
        break;
      case double_matrix:
        if (debug)
          dd("%8.2f ", (double)((double *)A)[i * n + j]);
        else
          printf("%8.2f ", (double)((double *)A)[i * n + j]);
        break;
      }
    }
    if (debug)
      dd("\n");
    else
      printf("\n");
  }
}
