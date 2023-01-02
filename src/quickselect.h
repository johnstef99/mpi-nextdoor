#ifndef QUICKSELECT_C
#define QUICKSELECT_C

#include <stdio.h>

#define swap(T, a, b)                                                          \
  {                                                                            \
    T temp = *a;                                                               \
    *a = *b;                                                                   \
    *b = temp;                                                                 \
  }

int partition(double *arr, int *ind, int low, int high);

int quickselect(double *arr, int *ind, int low, int high, int k);

#endif /* QUICKSELECT_C */
