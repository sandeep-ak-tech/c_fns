/*
 * Optimized matrix multiplication using STATIC 2D arrays (no malloc).
 * A is ROW_A x COL_A, B is COL_A x COL_B, C is ROW_A x COL_B.
 *
 * Same math, reordered for cache. Inner loop walks a row of B left-to-right
 * (stride 1), which is fast and SIMD-friendly. C must start at 0 (we use +=).
 *
 * Build: gcc -O3 -march=native matmul_opt_static.c -o matmul_opt_static
 */
#include <stdio.h>

#define ROW_A 3   /* rows of A and C               */
#define COL_A 3   /* cols of A, rows of B (shared) */
#define COL_B 3   /* cols of B and C               */

int main(void) {
    int A[ROW_A][COL_A] = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };
    int B[COL_A][COL_B] = {
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1}
    };
    int C[ROW_A][COL_B] = {0};   /* must be zero: we accumulate with += */

    /* row-then-k order: one A value updates a whole row of C at a time. */
    for (int row = 0; row < ROW_A; row++) {
        for (int k = 0; k < COL_A; k++) {
            int aValue = A[row][k];            /* reused across the row */
            for (int col = 0; col < COL_B; col++) {
                C[row][col] += aValue * B[k][col];  /* B row read left-to-right */
            }
        }
    }

    printf("Result C:\n");
    for (int row = 0; row < ROW_A; row++) {
        for (int col = 0; col < COL_B; col++) {
            printf("%d ", C[row][col]);
        }
        printf("\n");
    }
    return 0;
}
