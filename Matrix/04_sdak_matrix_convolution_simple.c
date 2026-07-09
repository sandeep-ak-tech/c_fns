#include <stdio.h>

/*
 * 2D "valid" convolution of an input matrix with a kernel.
 * The kernel only slides where it fully overlaps A, so no bounds
 * check is needed. Output size is
 * (rowsA - rowsB + 1) x (colsA - colsB + 1).
 */
 
#define ROW_A           3
#define COLUMN_A        3
#define ROW_k           2
#define COLUMN_k        2
#define ROW_OUT         ROW_A - ROW_k + 1
#define COLUMN_OUT      COLUMN_A - COLUMN_k + 1

void convolve_2D(int A[][COLUMN_A], int kern[][COLUMN_k], int krows, int kcols, int output[][COLUMN_OUT]);
int a[ROW_A][COLUMN_A] = {{1,2,3},{3,4,5},{5,6,7}};
int k[ROW_k][COLUMN_k] = {{7,8},{10,11}};
int c[ROW_OUT][COLUMN_OUT] = {0};

int main()
{
    if (ROW_k > ROW_A || COLUMN_k > COLUMN_A) {
        printf("Kernel B must not be larger than matrix A.\n");
        return 1;
    }

    convolve_2D(a, k, ROW_k, COLUMN_k, c);
  
    for(int i=0;i<ROW_OUT;i++)
    {
        for(int j=0; j<COLUMN_OUT;j++)
        {
            printf("%d\t",c[i][j]);
        }
        printf("\r\n");
    }
    
    printf("Hello World");

    return 0;
}

/*
 * 2D "valid" convolution of an input matrix with a kernel.
 * The kernel only slides where it fully overlaps A, so no bounds
 * check is needed. Output size is
 * (rowsA - rowsB + 1) x (colsA - colsB + 1).
 */
void convolve_2D(int A[][COLUMN_A], int kern[][COLUMN_k], int krows, int kcols, int output[][COLUMN_OUT])
{

    for(int i=0;i<ROW_OUT;i++)
    {
        for(int j=0; j<COLUMN_OUT;j++)
        {
            int sum = 0;

            for(int m=0;m<krows;m++)
            {
                for(int n=0; n<kcols;n++)
                {
         
                  sum += A[i + m][j + n] * kern[m][n];
                }
            }
            output[i][j] = sum;
        }
    }
}