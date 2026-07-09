/*
 * CPU-optimized matrix multiplication (C = A x B), made easy to follow.
 *
 * Same math as the naive version, but arranged so the CPU can keep data in
 * cache and reuse it. Four ideas:
 *   1. ROW-WISE access: we read along rows (stride 1) so memory comes in
 *      sequential, cache-friendly chunks instead of jumping down columns.
 *   2. TILING: we work on a small block of columns at a time so the data
 *      we reuse stays hot in cache.
 *   3. HOISTING: A[row][k] is constant in the inner loop, so we keep it in
 *      one variable instead of re-reading it.
 *   4. restrict: tells the compiler the matrices don't overlap, so it can use
 *      SIMD (do several multiplications per instruction).
 *
 * Build: gcc -O3 -march=native matmul_opt.c -o matmul_opt
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROW_SIZE_A 512   /* rows of A and C               */
#define COL_SIZE_A 512   /* cols of A, rows of B (shared) */
#define COL_SIZE_B 512   /* cols of B and C               */
#define COL_BLOCK  64    /* columns handled per tile      */

static double now_sec(void) {
    return (double)clock() / CLOCKS_PER_SEC;
}

static void matmul_opt(const int *restrict A,
                       const int *restrict B,
                       int *restrict C) {
    /* Handle a block of columns at a time so reused data stays in cache. */
    for (int colStart = 0; colStart < COL_SIZE_B; colStart += COL_BLOCK) {
        int colEnd = colStart + COL_BLOCK;
        if (colEnd > COL_SIZE_B) {
            colEnd = COL_SIZE_B;                       /* clamp last (partial) block */
        }

        for (int row = 0; row < ROW_SIZE_A; row++) {
            int *cRow = &C[row * COL_SIZE_B];          /* current row of result */

            for (int k = 0; k < COL_SIZE_A; k++) {
                int aValue = A[row * COL_SIZE_A + k];  /* one A element, reused below */
                const int *bRow = &B[k * COL_SIZE_B];  /* k-th row of B               */

                /* Walk this block of C left-to-right (stride 1 -> fast, SIMD). */
                for (int col = colStart; col < colEnd; col++) {
                    cRow[col] += aValue * bRow[col];
                }
            }
        }
    }
}

int main(void) {
    int *A = malloc(ROW_SIZE_A * COL_SIZE_A * sizeof(int));
    int *B = malloc(COL_SIZE_A * COL_SIZE_B * sizeof(int));
    int *C = calloc(ROW_SIZE_A * COL_SIZE_B, sizeof(int));   /* start at 0: we use += */
    if (!A || !B || !C) return 1;

    for (int i = 0; i < ROW_SIZE_A * COL_SIZE_A; i++) { A[i] = i % 7; }
    for (int i = 0; i < COL_SIZE_A * COL_SIZE_B; i++) { B[i] = i % 5; }

    double t0 = now_sec();
    matmul_opt(A, B, C);
    double t = now_sec() - t0;

    printf("A=%dx%d  B=%dx%d  C=%dx%d  col block=%d\n", ROW_SIZE_A, COL_SIZE_A, COL_SIZE_A, COL_SIZE_B, ROW_SIZE_A, COL_SIZE_B, COL_BLOCK);
    printf("opt: %.3f s\n", t);

    free(A); free(B); free(C);
    return 0;
}
