#ifndef KNN_H
#define KNN_H

#include <cilk/cilk.h>
#include <cblas.h>
#include <float.h>

#include "sort.h"
#include "matrix_utils.h"
#include "debug.h"


// Definition of the kNN result struct
typedef struct knnresult {
  int *nidx;     // Indices (0-based) of nearest neighbors [m-by-k]
  double *ndist; // Distance of nearest neighbors          [m-by-k]
  int m;         // Number of query points                 [scalar]
  int k;         // Number of nearest neighbors            [scalar]
} knnresult;

//! Compute k nearest neighbors of each point in X [m-by-d]
/*!

  \param  X      Query data points               [m-by-d]
  \param  Y      Corpus data points              [n-by-d]
  \param  m      Number of query points          [scalar]
  \param  n      Number of corpus points         [scalar]
  \param  d      Number of dimensions            [scalar]
  \param  k      Number of neighbors             [scalar]

  \return  The kNN result
*/
knnresult kNN(double *X, double *Y, int m, int n, int d, int k);

#endif // !KNN_H
