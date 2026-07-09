/*
 * Naive matrix multiplication using STATIC 2D arrays (no malloc).
 * A is ROW_A x COL_A, B is COL_A x COL_B, C is ROW_A x COL_B.
 * Easy to read: indexing is just A[row][k], B[k][col], C[row][col].
 *
 * Build: gcc -O3 matmul_naive_static.c -o matmul_naive_static
 */
#include <stdio.h>

#define ROW_A 3   /* rows of A and C               */
#define COL_A 3   /* cols of A, rows of B (shared) */
#define COL_B 3   /* cols of B and C               */

int main(void) {
    /* Matrices live on the stack, sizes fixed at compile time. */
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
    int C[ROW_A][COL_B] = {0};   /* result, all zeros */

    /* For each output cell, take the dot product of A's row and B's column. */
    for (int row = 0; row < ROW_A; row++) {
        for (int col = 0; col < COL_B; col++) {
            int sum = 0;
            for (int k = 0; k < COL_A; k++) {
                sum += A[row][k] * B[k][col];  /* B[k][col] walks DOWN a column */
            }
            C[row][col] = sum;                 /* write the cell once */
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
