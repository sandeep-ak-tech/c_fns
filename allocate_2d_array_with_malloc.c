#include <stdio.h>
#include <stdlib.h>

#define COLUMNS 4 
#define ROWS    3 

int main()
{
    
    
    printf("Start\r\n");
    
    int (*arr)[COLUMNS] = malloc(ROWS * sizeof(arr)); //No need to write sizeof(int[COLS]) — *arr is the type of one row.
    if(!arr) return -1;
    
    arr[0][0] = 10;
    
      // store number
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            arr[i][j] = 10 + j + i; 
        }
        printf("\n");
    }  
    /*
    **arr = 26;
    int *ptr =  (int *)arr;
    ptr++;
    *ptr = 55;
    printf("\r\nptr value = %d\r\n",*ptr);
    */
    /*Play around by removing the removing the comment below and build*/
    //*arr++;
 
    
    // Print all
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            printf("%4d", arr[i][j]);
        }
        printf("\n");
    }

    free(arr);
    return 0;
    printf("\r\nEnd");

    return 0;
}
