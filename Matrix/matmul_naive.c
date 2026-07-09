/*
 * Naive matrix multiplication (baseline).
 * A is ROW_SIZE_A x COL_SIZE_A, B is COL_SIZE_A x COL_SIZE_B, C is ROW_SIZE_A x COL_SIZE_B.
 * Classic order: B is walked column-wise, causing frequent cache misses.
 *
 * Build: gcc -O3 matmul_naive.c -o matmul_naive
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROW_SIZE_A 512   /* rows of A and C               */
#define COL_SIZE_A 512   /* cols of A, rows of B (shared) */
#define COL_SIZE_B 512   /* cols of B and C               */

static double now_sec(void) {
    return (double)clock() / CLOCKS_PER_SEC;
}

static void matmul_naive(const int *A, const int *B, int *C) {
    for (int row = 0; row < ROW_SIZE_A; row++) {
        for (int col = 0; col < COL_SIZE_B; col++) {
            int sum = 0;
            for (int k = 0; k < COL_SIZE_A; k++) {
                sum += A[row * COL_SIZE_A + k] * B[k * COL_SIZE_B + col];  /* B walked down a column */
            }
            C[row * COL_SIZE_B + col] = sum;
        }
    }
}

int main(void) {
    int *A = malloc(ROW_SIZE_A * COL_SIZE_A * sizeof(int));
    int *B = malloc(COL_SIZE_A * COL_SIZE_B * sizeof(int));
    int *C = malloc(ROW_SIZE_A * COL_SIZE_B * sizeof(int));
    if (!A || !B || !C) return 1;

    for (int i = 0; i < ROW_SIZE_A * COL_SIZE_A; i++) { A[i] = i % 7; }
    for (int i = 0; i < COL_SIZE_A * COL_SIZE_B; i++) { B[i] = i % 5; }

    double t0 = now_sec();
    matmul_naive(A, B, C);
    double t = now_sec() - t0;

    printf("A=%dx%d  B=%dx%d  C=%dx%d\n", ROW_SIZE_A, COL_SIZE_A, COL_SIZE_A, COL_SIZE_B, ROW_SIZE_A, COL_SIZE_B);
    printf("naive: %.3f s\n", t);

    free(A); free(B); free(C);
    return 0;
}
