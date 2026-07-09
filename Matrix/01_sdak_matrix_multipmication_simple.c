#include <stdio.h>
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
            for(int k=0;k<COLUMN_A;k++)
            {
                c[row][col]+=a[row][k]*b[k][col];
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