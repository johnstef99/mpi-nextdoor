#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void generate_grid(int d, const int *dim_sizes) {
  // Allocate an array to store the current indices of the grid cell
  int *indices = malloc(d * sizeof(int));

  // Initialize the indices to 0
  for (int i = 0; i < d; i++) {
    indices[i] = 0;
  }

  // Loop through each cell of the grid
  int i = 0;
  while (i >= 0) {
    // Print the current indices of the grid cell
    for (int j = 0; j < d; j++) {
      printf("%d ", indices[j]);
    }
    printf("\n");

    // Increment the indices
    i = d - 1;
    while (i >= 0 && ++indices[i] == dim_sizes[i]) {
      indices[i] = 0;
      i--;
    }
  }
}

int main(int argc, char *argv[]) {
  int d = atoi(argv[1]);
  if (argc - 2 != d) {
    return 1;
  }
  int *dim_sizes = malloc(d * sizeof(int));
  for (int i = 0; i < d; i++) {
    dim_sizes[i] = atoi(argv[i + 2]);
  }
  generate_grid(d, dim_sizes);
  return 0;
}
