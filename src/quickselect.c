#include "quickselect.h"
#include <limits.h>

int partition(double *arr, int *ind, int low, int high) {
  int pivot = arr[high];

  int i = low - 1;

  for (int j = low; j <= high - 1; j++) {
    if (arr[j] < pivot) {
      i++;
      swap(double, &arr[i], &arr[j]);
      swap(int, &ind[i], &ind[j]);
    }
  }

  swap(double, &arr[i + 1], &arr[high]);
  swap(int, &ind[i + 1], &ind[high]);
  return (i + 1);
}

int quickselect(double *arr, int *ind, int low, int high, int k) {
  if (k > 0 && k <= high - low + 1) {
    int pivotIndex = partition(arr, ind, low, high);

    if (pivotIndex - low == k - 1)
      return arr[pivotIndex];

    if (pivotIndex - low > k - 1)
      return quickselect(arr, ind, low, pivotIndex - 1, k);

    return quickselect(arr, ind, pivotIndex + 1, high,
                       k - pivotIndex + low - 1);
  }

  return INT_MAX;
}
