#include <float.h>
#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "knn.h"
#include "matrix_utils.h"
#include "quickselect.h"

int m = 20;
int d = 2;
double file[20][2] = {
    {1, 0},  {3, 0},  {3, 0},  {4, 0},  {5, 0},  {6, 0},  {7, 0},
    {8, 0},  {9, 0},  {10, 0}, {11, 0}, {12, 0}, {13, 0}, {14, 0},
    {15, 0}, {16, 0}, {17, 0}, {18, 0}, {19, 0}, {0, 0},
};

void work(int rank, int n_proc, char *proc_name, int k) {

  // init process

  int proc_size = m / n_proc; // this process size
  int extra = m % n_proc;     // extra items that are going to the last process

  double *X = malloc((proc_size + extra) * d * sizeof(double));
  double *Y = malloc((proc_size + extra) * d * sizeof(double));
  double *Z = malloc((proc_size + extra) * d * sizeof(double));

  if (!X || !Y || !Z) {
    fprintf(stderr, "Memory allocation failed!\n");
    exit(1);
  }

  cilk_for(int i = 0; i < proc_size + extra; i++) {
    cilk_for(int dim = 0; dim < d; dim++) {
      X[i * d + dim] = ((double *)file)[i * d + dim + rank * proc_size * d];
    }
  }

  memcpy(Y, X, (proc_size + extra) * d * sizeof(double));

  int x_size = rank != n_proc - 1 ? proc_size : proc_size + extra;
  int y_size, z_size;
  int rank_next = (rank + 1) % n_proc;
  int rank_prev = rank == 0 ? n_proc - 1 : rank - 1;
  MPI_Request send_req, recv_req;
  double *temp;
  knnresult res_old = {.m = -1};
  knnresult res_new = {.m = -1};
  double *result = malloc(k * sizeof(double));
  int *result_idx = malloc(k * sizeof(int));
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
      cilk_for(int row = 0; row < x_size; row++) {
        int i = 0, j = 0, w = 0;
        while (w < k) {
          if (i < k && (j >= k || res_old.ndist[row * k + i] <
                                      res_new.ndist[row * k + j])) {
            result[w] = res_old.ndist[row * k + i];
            result_idx[w] = res_old.nidx[row * k + i];
            w++;
            i++;
          } else {
            result[w] = res_new.ndist[row * k + j];
            result_idx[w] = res_new.nidx[row * k + j];
            w++;
            j++;
          }
        }
        for (int l = 0; l < k; l++) {
          res_new.ndist[row * k + l] = result[l];
          res_new.nidx[row * k + l] = result_idx[l];
        }
      }
    }

    if (rank == r) {
      printf("r=%d %d/%d@%s ===============================\n", r, rank, n_proc,
             proc_name);
      print_matrix(res_new.nidx, res_new.m, k, int_matrix, 0);
      print_matrix(res_new.ndist, res_new.m, k, double_matrix, 0);
      printf("\n");
    }

    MPI_Wait(&send_req, MPI_STATUS_IGNORE);
    MPI_Wait(&recv_req, MPI_STATUS_IGNORE);
    temp = Y;
    Y = Z;
    Z = temp;
    free(res_old.nidx);
    free(res_old.ndist);
    res_old = res_new;
  }

  free(result);
  free(result_idx);
  free(X);
}

int main(int argc, char *argv[]) {
  // Initialize the MPI environment
  MPI_Init(&argc, &argv);

  int k = atoi(argv[1]);

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

  printf("%d/%d@%s is alive\n", rank, n_proc, processor_name);

  work(rank, n_proc, processor_name, k);

  MPI_Finalize();

  return 0;
}
