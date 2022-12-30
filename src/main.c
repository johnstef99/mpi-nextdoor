#include <cilk/cilk.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <vecLib/vecLib.h>

void distance_matrix(double *X, double *Y, int m, int n, int d, double *C) {
  double alpha = -2;
  double beta = 0;
  // C = alpha*X*Y + beta*C
  cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasTrans, m, n, d, alpha, X, d, Y,
              d, beta, C, m);

  cilk_for(int i = 0; i < m; i++) {
    cilk_for(int j = 0; j < n; j++) {
      for (int dim = 0; dim < d; dim++) {
        C[i * n + j] += X[i + dim] * X[i + dim] + Y[j + dim] * Y[j + dim];
      }
    }
  }
}

void print_matrix(double *A, int m, int n) {
  for (int i = 0; i < m; i++) {
    for (int j = 0; j < n; j++) {
      printf("%f ", A[i * n + j]);
    }
    printf("\n");
  }
}

int main(int argc, char *argv[]) {
  int m = 2;
  int n = 2;
  int d = 2;

  double X[2][2] = {
      {20, 0},
      {50, 0},
  };

  double Y[2][2] = {
      {0, 0},
      {10, 0},
  };

  double *C = calloc(m * n, sizeof(double));

  /* distance((double *)X, (double *)Y, m, n, d, C); */
  distance_matrix((double *)X, (double *)Y, m, n, d, C);

  print_matrix(C, m, n);

  free(C);
  return 0;
}
