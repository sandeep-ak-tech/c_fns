/*
Do it after allocating 2d array with malloc and 2d array using pointers.
*/

#include <stdio.h>
#include <stdlib.h>

#define COLUMN              3
#define ROW                 3

int transp_matrix(int (*matrix)[COLUMN], int (*transpose)[ROW])
{

    for(int i=0;i<ROW;i++)
    {
        for(int j=0; j<COLUMN; j++)
        {
            transpose[j][i] = matrix[i][j];
        }
    }
    
    return 0;

}

int main()
{
    printf("Start\r\n");
    int (*trans_matr)[ROW] = malloc(COLUMN * sizeof(trans_matr));
    if(!trans_matr) return -1;
    
    int (*arr)[COLUMN] = malloc(ROW * sizeof(arr));
    if(!arr) return -1;
    
    arr[0][0] = 10;
    int rand = 10;
    
      // store number
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLUMN; j++) {
            arr[i][j] = rand++; 
        }
        printf("\n");
    }  

    
    // Print all
    printf("\r\nInitial matrix\r\n");
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLUMN; j++) {
            printf("%4d  ", arr[i][j]);
        }
        printf("\n");
    }

    int result = transp_matrix(arr,trans_matr);
    if(result) return -1;
    
    // Print all
    printf("\r\nTranspose matrix\r\n");
    for (int i = 0; i < COLUMN; i++) {
        for (int j = 0; j < ROW; j++) {
            printf("%4d  ", trans_matr[i][j]);
        }
        printf("\n");
    }
    free(arr);
    free(trans_matr);
    printf("\r\nEnd");

    return 0;
}