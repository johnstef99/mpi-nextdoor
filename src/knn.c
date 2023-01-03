#include "knn.h"

#define max(T, a, b) ((a) > (b) ? (a) : (b))

void distance_matrix(double *X, double *Y, int m, int n, int d, double *D) {
  double alpha = -2;
  double beta = 0;
  // D = alpha*X*Y + beta*D
  cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasTrans, m, n, d, alpha, X, d, Y,
              d, beta, D, n);

  cilk_for(int i = 0; i < m; i++) {
    cilk_for(int j = 0; j < n; j++) {
      for (int dim = 0; dim < d; dim++) {
        D[i * n + j] +=
            X[i * d + dim] * X[i * d + dim] + Y[j * d + dim] * Y[j * d + dim];
      }
    }
  }
}

knnresult kNN(double *X, double *Y, int m, int n, int d, int k) {
  double *D = calloc(m * n, sizeof(double));
  int *D_ind = malloc(m * n * sizeof(int));
  for (int i = 0; i < m * n; i++)
    D_ind[i] = i;

  if (!D || !D_ind) {
    fprintf(stderr, "Memory allocation failed!\n");
    exit(1);
  }

  distance_matrix(X, Y, m, n, d, D);

  dd("D before sort\n");
  print_matrix(D, m, n, double_matrix, 1);
  dd("\n");

  cilk_for(int i = 0; i < m; i++) {
    quickselect(D, D_ind, i * n, i * n + n - 1, k);
  }

  dd("D after sort\n");
  print_matrix(D, m, n, double_matrix, 1);
  dd("\n");
  print_matrix(D_ind, m, n, int_matrix, 1);
  dd("\n");

  knnresult res;
  res.k = k;
  res.m = m;
  res.ndist = malloc(m * k * sizeof(double));
  res.nidx = malloc(m * k * sizeof(int));
  cilk_for(int i = 0; i < m; i++) {
    cilk_for(int j = 0; j < k; j++) {
      res.ndist[i * k + j] = D[i * n + j];
      res.nidx[i * k + j] = D_ind[i * n + j] - i * n;
    }
  }

  free(D);
  free(D_ind);
  return res;
}
