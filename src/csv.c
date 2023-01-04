#include "csv.h"
#include <stdio.h>

int read_csv(char *filename, int max_line_size, int from, int n, int d,
             double *A, int columns_to_skip) {
  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("Error opening file\n");
    return 1;
  }

  char buffer[max_line_size];
  for (int line = 0; line < n + from; line++) {
    fgets(buffer, max_line_size, fp);

    if (line < from)
      continue;

    char *field = strtok(buffer, ",");

    for (; columns_to_skip != 0; columns_to_skip--) {
      field = strtok(NULL, ","); // skip the char
    }

    for (int col = 0; col < d; col++) {
      A[(line - from) * d + col] = atof(field);
      field = strtok(NULL, ",");
    }
  }
  fclose(fp);
  return 0;
}
