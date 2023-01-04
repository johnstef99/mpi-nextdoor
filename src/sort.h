#ifndef SORT_C
#define SORT_C

#include <limits.h>
#include <stdio.h>

#define swap(T, a, b)                                                          \
  {                                                                            \
    T temp = *a;                                                               \
    *a = *b;                                                                   \
    *b = temp;                                                                 \
  }

int quickselect(double *arr, int *ind, int low, int high, int k);

void quick_sort(double *arr, int *arr_ind, int left, int right);

#endif /* SORT_C */
