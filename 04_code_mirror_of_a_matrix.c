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

/*
Use two pointers (left and right) starting from both ends of the current row:

left starts at index 0
right starts at index cols-1
We move them toward the center, exchange elements at left and right positions.
Stop when pointers meet or cross — middle element (if odd length) stays unchanged.
*/

int horizontal_mirror_matrix(int (*matrix)[COLUMN])
{
    int tmp = 0;
    for(int i=0;i<ROW;i++)
    {
        for(int left=0, right = COLUMN -1; left<right; left++,right--)
        {
            tmp = matrix[i][left];
            matrix[i][left] = matrix[i][right];
            matrix[i][right] = tmp;
        }
    }
    
    return 0;

}

int vertical_mirror_matrix(int (*matrix)[COLUMN])
{
    int tmp = 0;
    for(int j=0; j<COLUMN; j++)
    {
        for(int top=0, bottom = ROW -1; top<bottom; top++,bottom--)
        {
            tmp = matrix[top][j];
            matrix[top][j] = matrix[bottom][j];
            matrix[bottom][j] = tmp;
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