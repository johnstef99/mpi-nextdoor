#include <stdio.h>
#include <stdlib.h>

#include "knn.h"
#include "matrix_utils.h"

int main(int argc, char *argv[]) {
  int m = 2;
  int n = 7;
  int d = 2;

  double X[2][2] = {
      {20, 0},
      {70, 0},
  };

  double Y[7][2] = {
      {0, 0}, {40, 0}, {50, 0}, {60, 0}, {70, 0}, {10, 0}, {30, 0},
  };

  int k = atoi(argv[1]);
  knnresult res = kNN((double *)X, (double *)Y, m, n, d, k);

  print_matrix(res.ndist, m, k, double_matrix);
  print_matrix(res.nidx, m, k, int_matrix);

  return 0;
}
