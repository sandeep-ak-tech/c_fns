/*
+------------------------------------+
|  You want     | Write this instead | 
+------------------------+-----------+
          
  arr[i][j]           *(*(arr + i) + j)
  arr[0][0]           **arr or *arr[0]
  arr[i][0]           **(arr + i)
  arr[0][j]           *(arr[0] + j)

  Any element         *((int*)arr + i*4 + j)  (if 4 columns)
                      *((int*)arr + i*columns + j)
*/

#include <stdio.h>
int a[3][4] = {
        {10, 11, 12, 13},
        {20, 21, 22, 23},
        {30, 31, 32, 33}
    };
int b[3] = {1,2,3};
int main()
{
    int *ptr = &a[0][0];
    
    printf("Start\r\n");
    
    /*Print single array*/
    printf("Print single array\r\n");
    for(int i=0; i<3; i++)
    {
        printf("%ld \r\n",*(b+i));
    }
    
    /*Print 2D array Using pointers*/
    printf("Print 2D array Using pointers\r\n");
    for(int i=0; i<12; i++)
    {
      printf("%ld\t",*(ptr+i));  
    }
    printf("\r\nPrint 2D array \r\n");
    /*Print 2D array */
    for(int i=0; i<3; i++)
    {
        for(int j=0; j<4; j++)
        {
            printf("%ld\t",*(*(a+i)+j));
        }
        
        
    }
    printf("\r\nEnd");

    return 0;
}