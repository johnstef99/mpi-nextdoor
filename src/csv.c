#include "csv.h"

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

    for (int skip = 0; skip < columns_to_skip; skip++) {
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

int write_to_csv(char *filename, int append, void *A, int m, int n,
                 size_t size) {
  FILE *fp = fopen(filename, append ? "a" : "w");
  if (fp == NULL) {
    printf("Error opening file\n");
    return 1;
  }

  for (int i = 0; i < m; i++) {
    for (int j = 0; j < n; j++) {
      if (size == sizeof(int)) {
        fprintf(fp, "%d", ((int *)A)[i * n + j]);
      } else {
        fprintf(fp, "%f", ((double *)A)[i * n + j]);
      }
      if (j != n - 1) {
        fprintf(fp, ",");
      } else {
        fprintf(fp, "\n");
      }
    }
  }

  fclose(fp);
  return 0;
}
