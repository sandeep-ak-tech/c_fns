/******************************************************************************

Welcome to GDB Online.
  GDB online is an online compiler and debugger tool for C, C++, Python, PHP, Ruby, 
  C#, OCaml, VB, Perl, Swift, Prolog, Javascript, Pascal, COBOL, HTML, CSS, JS
  Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <locale.h>
#include <ctype.h>


/*
Write the code for the function countPeaks, the function has to count the nuymber of peaks in the array.
A top peak = if the value is greater than previous element and next elemt by 5.0 in the array
A bottom peak = if the value is less than previous element and next elemt by 5.0 in the array

eg:- double arr[10] = {4.1,10.1,4.6,7.9,8.2,1.2,6.2,10.6,4.3,12.8};

here 
top peak = 10.1
bottom peak = 1.2,4.3

Total number of peaks = 3

The first and last elements are not treated as peaks i.e. 4.1 and 12.8 are not peaks.
*/
typedef struct doubleArray {
  int length;
  double* items;
} doubleArray;

/**
 * @param values The radioactivity values measured by the sensor
 * @return The total number of top peaks and bottom peaks found in the radioactivity values.
 */
int countPeaks(doubleArray values) {
    int peak_count = 0;
    double temp1=0,temp2=0;
  // Write your code here
  for(int i=1;i<(values.length-1);i++){
     temp1 = values.items[i]-values.items[i-1];
     temp2 = values.items[i]-values.items[i+1];
     
     if((abs(temp1)>=5) && (abs(temp2)>=5)){
         printf("%f\r\n",values.items[i]);
         peak_count++;
     }
  }
  
  // To debug: fprintf(stderr, "Debug messages...\n");

  return peak_count;
}

int main()
{
    double arr[10] = {4.1,10.1,4.6,7.9,8.2,1.2,6.2,10.6,4.3,12.8};
    doubleArray array = {sizeof(arr)/sizeof(double), malloc(sizeof(double) * array.length)};
    doubleArray values;
    
  for (int i = 0 ; i < array.length ; i++) {
    array.items[i] = arr[i];
  }
  
  values = array;
  int output = countPeaks(values);
    printf("The number of peaks : %d\r\n",output);

    return 0;
}