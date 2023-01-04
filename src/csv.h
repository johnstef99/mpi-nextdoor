#ifndef CSV_H
#define CSV_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "knn.h"

int read_csv(char *filename, int max_line_size, int from, int n, int d,
             double *A, int columns_to_skip);

int write_to_csv(char *filename, int append, void *A, int m, int n,
                 size_t size);

#endif // !CSV_H
