#include <stdio.h>
#include <stdlib.h>


/*
Operation,Matrix result,Description
Original :
1 2 3
4 5 6
7 8 9

Horizontal mirror - upside-down flip
7 8 9
4 5 6
1 2 3

Vertical mirror - left-right flip
3 2 1
6 5 4
9 8 7

Both (180° rotation) - horizontal + vertical = rotate 180°
9 8 7
6 5 4
3 2 1
*/
#define COLUMN              3
#define ROW                 3

/* Horizontal mirror: flip top <-> bottom rows */
int horizontal_mirror_matrix(int (*matrix)[COLUMN])
{
    for (int i = 0; i < ROW / 2; i++) {
        for (int j = 0; j < COLUMN; j++) {
            int temp = matrix[i][j];
            matrix[i][j] = matrix[ROW - 1 - i][j];
            matrix[ROW - 1 - i][j] = temp;
        }
    }
    
    return 0;

}

/* Vertical mirror: flip left <-> right columns */
int vertical_mirror_matrix(int (*matrix)[COLUMN])
{
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLUMN / 2; j++) {
            int temp = matrix[i][j];
            matrix[i][j] = matrix[i][COLUMN - 1 - j];
            matrix[i][COLUMN - 1 - j] = temp;
        }
    }
    
    return 0;

}

int main()
{
    printf("Start\r\n");

    int (*arr)[COLUMN] = malloc(ROW * sizeof(arr));
    if(!arr) return -1;
    
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

    int result = vertical_mirror_matrix(arr);
    if(result) return -1;
    
    // Print all
    printf("\r\nMirror matrix\r\n");
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLUMN; j++) {
            printf("%4d  ", arr[i][j]);
        }
        printf("\n");
    }
    free(arr);
    printf("\r\nEnd");

    return 0;
}