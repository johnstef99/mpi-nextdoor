#include "knn.h"

#define min(a, b) ((a < b ? a : b))

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
  cilk_for(int i = 0; i < m * n; i++) D_ind[i] = i;

  if (!D || !D_ind) {
    fprintf(stderr, "Memory allocation failed!\n");
    exit(1);
  }

  distance_matrix(X, Y, m, n, d, D);

  cilk_for(int i = 0; i < m; i++) {
    if (n > k) {
      quickselect(D, D_ind, i * n, i * n + n - 1, k);
    }
    quick_sort(D, D_ind, i * n, i * n + min(k, n) - 1);
  }

  knnresult res;
  res.k = k;
  res.m = m;
  res.ndist = malloc(m * k * sizeof(double));
  res.nidx = malloc(m * k * sizeof(int));
  cilk_for(int i = 0; i < m; i++) {
    for (int j = 0; j < k; j++) {
      res.ndist[i * k + j] = D[i * n + j];
      res.nidx[i * k + j] = D_ind[i * n + j] - i * n;
      if (j >= n) {
        res.nidx[i * k + j] = -1;
        res.ndist[i * k + j] = DBL_MAX;
      }
    }
  }

  free(D);
  free(D_ind);
  return res;
}
