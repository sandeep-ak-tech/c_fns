#include <stdio.h>

/*
Goal: Maximize locality so data stays in cache as long as possible (Cache Hotness)

Cache hotness refers to how frequently and recently a piece of data is being accessed by the CPU.
The "hotter" the data, the more likely it is to stay in the fast CPU cache (L1 or L2). Hot data gives you maximum performance. Cold data causes cache misses, which are very slow.

- Why This Improves Cache Behavior
   Ans: In the inner loop (now j loop):

    1. a[i][k] is fixed for the entire inner loop → stays in a register or L1 cache. Extremely high temporal locality. 
        stored in a local variable a_local => CPU register — the fastest storage. The inner loop accumulates into that register.
    2. b[k][j] is accessed sequentially across columns in one row of B → excellent spatial locality (consecutive elements fit nicely in cache lines).
    3. c[i][j] is updated repeatedly for one row of C → high temporal locality.

Cache Hotness:
a. One element of A is extremely hot.
b. One row of C stays hot in L1.
c. B is streamed sequentially.

Result: Dramatically fewer cache misses compared to original, where B was accessed with large stride.
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
    
    for(int i=0;i<ROW_A;i++)
    {
        for(int k=0; k<COLUMN_A;k++)
        {
            int a_local = a[i][k];
            for(int j=0;j<COLUMN_B;j++)
            
            {
                c[i][j]+=a_local*b[k][j];
            }
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