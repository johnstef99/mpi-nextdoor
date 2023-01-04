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

void quick_sort(double *arr, int *arr_ind, int left, int right) {
  if (left < right) {
    int pivot = arr[right];
    int i = left - 1;
    for (int j = left; j <= right - 1; j++) {
      if (arr[j] < pivot) {
        i++;
        swap(double, &arr[i], &arr[j]);
        swap(int, &arr_ind[i], &arr_ind[j]);
      }
    }
    swap(double, &arr[i + 1], &arr[right]);
    swap(int, &arr_ind[i + 1], &arr_ind[right]);
    int partition_index = i + 1;
    quick_sort(arr, arr_ind, left, partition_index - 1);
    quick_sort(arr, arr_ind, partition_index + 1, right);
  }
}
