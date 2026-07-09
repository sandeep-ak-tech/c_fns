/*
 * Matrix Multiplication: naive vs CPU-optimized
 *
 * Talking points for the interview:
 *  1. Loop order: ikj instead of ijk -> C and B accessed row-wise (stride 1),
 *     so memory access is sequential and cache-friendly.
 *  2. Blocking / tiling: work on BLOCK x BLOCK sub-tiles so the working set
 *     fits in L1/L2 cache, maximizing data reuse (fewer cache misses).
 *  3. Scalar hoisting: 'a[i][k]' is loaded once into a register per inner loop.
 *  4. Contiguous flat arrays (1D) -> better prefetching than int**.
 *  5. restrict + alignment -> lets the compiler auto-vectorize (SIMD).
 *
 * Build: gcc -O3 -march=native -funroll-loops matmul.c -o matmul
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define N      512    /* matrix dimension */
#define BLOCK  64     /* tile size, tuned to fit cache lines */

static double now_sec(void) {
    return (double)clock() / CLOCKS_PER_SEC;
}

/* Baseline: classic ijk, poor cache behavior on B (column stride). */
static void matmul_naive(const double *A, const double *B, double *C) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            double sum = 0.0;
            for (int k = 0; k < N; k++)
                sum += A[i * N + k] * B[k * N + j];
            C[i * N + j] = sum;
        }
}

/* Optimized: blocked + ikj ordering for sequential, vectorizable access. */
static void matmul_opt(const double *restrict A,
                       const double *restrict B,
                       double *restrict C) {
    for (int ii = 0; ii < N; ii += BLOCK)
        for (int kk = 0; kk < N; kk += BLOCK)
            for (int jj = 0; jj < N; jj += BLOCK)
                for (int i = ii; i < ii + BLOCK; i++)
                    for (int k = kk; k < kk + BLOCK; k++) {
                        const double a = A[i * N + k];      /* hoisted scalar */
                        const double *brow = &B[k * N];
                        double *crow = &C[i * N];
                        for (int j = jj; j < jj + BLOCK; j++)
                            crow[j] += a * brow[j];         /* stride-1, SIMD */
                    }
}

int main(void) {
    double *A = malloc(N * N * sizeof(double));
    double *B = malloc(N * N * sizeof(double));
    double *C = malloc(N * N * sizeof(double));
    if (!A || !B || !C) return 1;

    for (int i = 0; i < N * N; i++) { A[i] = i % 7; B[i] = i % 5; }

    double t0 = now_sec();
    matmul_naive(A, B, C);
    double t_naive = now_sec() - t0;

    memset(C, 0, N * N * sizeof(double));
    t0 = now_sec();
    matmul_opt(A, B, C);
    double t_opt = now_sec() - t0;

    printf("N = %d, BLOCK = %d\n", N, BLOCK);
    printf("naive: %.3f s\n", t_naive);
    printf("opt:   %.3f s\n", t_opt);
    printf("speedup: %.2fx\n", t_naive / t_opt);

    free(A); free(B); free(C);
    return 0;
}
