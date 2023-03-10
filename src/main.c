#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "csv.h"
#include "debug.h"
#include "knn.h"

#define MAX_FILENAME 100

double get_elapsed_ms(struct timespec start, struct timespec end) {
  return (double)(end.tv_sec - start.tv_sec) * 1000 +
         (double)(end.tv_nsec - start.tv_nsec) / 1000000;
}

void merge_res(int row, int k, knnresult res_old, knnresult res_new,
               double *merged, int *merged_idx) {
  int i = 0, j = 0, w = 0;
  int r = row * k;
  while (w < k) {
    if (i < k && (j >= k || res_old.ndist[r + i] < res_new.ndist[r + j])) {
      merged[w] = res_old.ndist[r + i];
      merged_idx[w] = res_old.nidx[r + i];
      w++;
      i++;
    } else {
      merged[w] = res_new.ndist[r + j];
      merged_idx[w] = res_new.nidx[r + j];
      w++;
      j++;
    }
  }
}

void work(int rank, int n_proc, char *proc_name, char *filename,
          int max_line_size, int columns_to_skip, int m, int d, int k,
          char *output_file) {
  // init process
  int proc_size = m / n_proc; // this process size
  int extra = m % n_proc;     // extra items that are going to the last process

  // if you are the last process you take the extra items
  int x_size = rank != n_proc - 1 ? proc_size : proc_size + extra;
  int y_size, z_size;
  double *X = malloc(x_size * d * sizeof(double));
  double *Y = malloc((proc_size + extra) * d * sizeof(double));
  double *Z = malloc((proc_size + extra) * d * sizeof(double));

  if (!X || !Y || !Z) {
    fprintf(stderr, "Memory allocation failed!\n");
    exit(1);
  }

  if (read_csv(filename, max_line_size, rank * proc_size, x_size, d, X,
               columns_to_skip) != 0) {
    fprintf(stderr, "Couldn't read file %s", filename);
    exit(1);
  }

  memcpy(Y, X, x_size * d * sizeof(double));

  int rank_next = (rank + 1) % n_proc;               // process after to me
  int rank_prev = rank == 0 ? n_proc - 1 : rank - 1; // process before to me
  MPI_Request send_req, recv_req;                    // flags needed for wait

  double *temp;                  // temp pointer to swap Y and Z
  knnresult res_old = {.m = -1}; // results from previous run
  knnresult res_new = {.m = -1}; // results from this run

  // arrays to merge and sort old and new results
  double *merged = malloc(k * sizeof(double));
  int *merged_idx = malloc(k * sizeof(int));
  MPI_Barrier(MPI_COMM_WORLD);
  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);
  // start ring
  for (int r = 0; r < n_proc; r++) {
    y_size = (n_proc - 1 + r) % n_proc == rank ? proc_size + extra : proc_size;
    z_size = r == rank ? proc_size + extra : proc_size;

    MPI_Isend(Y, y_size * d, MPI_DOUBLE, rank_next, 0, MPI_COMM_WORLD,
              &send_req);
    MPI_Irecv(Z, z_size * d, MPI_DOUBLE, rank_prev, 0, MPI_COMM_WORLD,
              &recv_req);

    res_new = kNN((double *)X, (double *)Y, x_size, y_size, d, k);

    int fix_index = rank - r < 0 ? n_proc + rank - r : rank - r;
    cilk_for(int i = 0; i < res_new.m * k; i++) {
      res_new.nidx[i] += proc_size * fix_index;
    }

    if (res_old.m != -1) {
      for (int row = 0; row < x_size; row++) {
        merge_res(row, k, res_old, res_new, merged, merged_idx);
        for (int l = 0; l < k; l++) {
          res_new.ndist[row * k + l] = merged[l];
          res_new.nidx[row * k + l] = merged_idx[l];
        }
      }
    }

    dd("r=%d %d/%d@%s ========================\n", r, rank, n_proc, proc_name);
    print_matrix(res_new.nidx, res_new.m, k, int_matrix, 1);
    print_matrix(res_new.ndist, res_new.m, k, double_matrix, 1);
    dd("\n");

    MPI_Wait(&send_req, MPI_STATUS_IGNORE);
    MPI_Wait(&recv_req, MPI_STATUS_IGNORE);
    temp = Y;
    Y = Z;
    Z = temp;
    free(res_old.nidx);
    free(res_old.ndist);
    res_old = res_new;
  }
  MPI_Barrier(MPI_COMM_WORLD);
  clock_gettime(CLOCK_MONOTONIC, &end);

  // free memory
  free(merged);
  free(merged_idx);
  free(X);
  free(Y);
  free(Z);

  // receive and write results
  if (rank == 0) {
    char res_dist_filename[MAX_FILENAME];
    char res_idx_filename[MAX_FILENAME];
    char res_time_filename[MAX_FILENAME];
    sprintf(res_idx_filename, "results/%s_idx.csv", output_file);
    sprintf(res_dist_filename, "results/%s_dist.csv", output_file);
    sprintf(res_time_filename, "results/%s_time.txt", output_file);

    FILE *fp = fopen(res_time_filename, "w");
    if (fp == NULL) {
      fprintf(stderr, "Error opening file to write the time result!\n");
    }
    fprintf(fp, "%f ms", get_elapsed_ms(start, end));
    fclose(fp);

    printf("[%d/%d@%s]: Writting my results\n", rank, n_proc - 1, proc_name);
    write_to_csv(res_idx_filename, 0, res_old.nidx, res_old.m, res_old.k,
                 sizeof(int));
    write_to_csv(res_dist_filename, 0, res_old.ndist, res_old.m, res_old.k,
                 sizeof(double));

    double *dist = malloc((proc_size + extra) * k * sizeof(double));
    int *idx = malloc((proc_size + extra) * k * sizeof(int));
    if (!idx || !dist) {
      fprintf(stderr, "Memory allocation failed!\n");
      exit(1);
    }

    for (int r = 1; r < n_proc; r++) {
      int x_size = r != n_proc - 1 ? proc_size : proc_size + extra;
      printf("[%d/%d@%s]: Waiting results from %d\n", rank, n_proc - 1,
             proc_name, r);
      MPI_Recv(idx, x_size * k, MPI_INT, r, 0, MPI_COMM_WORLD, NULL);
      MPI_Recv(dist, x_size * k, MPI_DOUBLE, r, 0, MPI_COMM_WORLD, NULL);
      printf("[%d/%d@%s]: Writting results from %d\n", rank, n_proc - 1,
             proc_name, r);
      write_to_csv(res_idx_filename, 1, idx, x_size, k, sizeof(int));
      write_to_csv(res_dist_filename, 1, dist, x_size, k, sizeof(double));
    }

    free(idx);
    free(dist);
  } else {
    MPI_Send(res_old.nidx, x_size * k, MPI_INT, 0, 0, MPI_COMM_WORLD);
    MPI_Send(res_old.ndist, x_size * k, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
  }

  free(res_old.nidx);
  free(res_old.ndist);
}

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);

  if (argc != 8) {
    fprintf(stderr,
            "Usage: %s [filename] [max_line_size] [columns_to_skip] [m] [d] "
            "[k] [output_file]\n",
            argv[0]);
    return 1;
  }

  char *filename = argv[1];
  int max_line_size = atoi(argv[2]);
  int columns_to_skip = atoi(argv[3]);
  int m = atoi(argv[4]);
  int d = atoi(argv[5]);
  int k = atoi(argv[6]);
  char *output = argv[7];

  // number of processes
  int n_proc;
  MPI_Comm_size(MPI_COMM_WORLD, &n_proc);

  // who am i (my rank)
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // my hostname
  int name_len;
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  MPI_Get_processor_name(processor_name, &name_len);

  if (m < n_proc) {
    fprintf(stderr,
            "You assigend too many hosts for this amount of data (%d > %d)\n",
            n_proc, m);
    return 1;
  }

  work(rank, n_proc, processor_name, filename, max_line_size, columns_to_skip,
       m, d, k, output);

  MPI_Finalize();

  return 0;
}
