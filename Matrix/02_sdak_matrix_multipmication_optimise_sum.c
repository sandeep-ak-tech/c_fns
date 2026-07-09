#include <stdio.h>
/*
Introducing multiiple memory access for every update for mtrix c

- `sum` is a local variable, so the compiler keeps it in a **CPU register** — the fastest storage. 
    The inner loop accumulates into that register.
- If we instead wrote `c[row][col]+=a[row][k]*b[k][col];`, every iteration would 
    1. read C from memory, 
    2. add, 
    3. write C back — a memory round-trip each step. Much slower, 
    and without `restrict` the compiler can't prove `C` doesn't overlap with `A`/`B`, so it cannot optimize.
- So `sum` is updated `COLUMN_A` times in a register and `C` is written **once** after the inner `k` loop finishes.
*/
#define ROW_A           3
#define COLUMN_A        2
#define ROW_B           2
#define COLUMN_B        3

int a[ROW_A][COLUMN_A] = {{1,2},{3,4},{5,6}};
int b[ROW_B][COLUMN_B] = {{7,8,9},{10,11,12}};
int c[ROW_A][COLUMN_B] = {0};

int main()
{
    if (COLUMN_A != ROW_B) printf("The matrix cannot  be multiplied\r\n");
    
    for(int row=0;row<ROW_A;row++)
    {
        for(int col=0; col<COLUMN_B;col++)
        {
            int sum = 0;
            for(int k=0;k<COLUMN_A;k++)
            {
                sum+=a[row][k]*b[k][col];
            }
            c[row][col] = sum;
        }
    }
    
    for(int row=0;row<ROW_A;row++)
    {
        for(int col=0; col<COLUMN_B;col++)
        {
            printf("%d\t",c[row][col]);
        }
        printf("\r\n");
    }
    
    printf("Hello World");

    return 0;
}