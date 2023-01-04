#ifndef CSV_H
#define CSV_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int read_csv(char *filename, int max_line_size, int from, int n, int d,
             double *A, int columns_to_skip);

#endif // !CSV_H
