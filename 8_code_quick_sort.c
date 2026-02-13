/*
This is code for quick sort
for more details on how it is implemented refer : https://www.youtube.com/watch?v=Vtckgz38QHs&t=279s

Steps:
	1. Choose a pivot
	2. Partition the array
	3. Recursively sort the sub arrays untill the given subarray cannot be further divided.
*/

/*Function for swapping*/
#include <stdio.h>

void swap(int *num1, int* num2)
{
	if(!num1 | !num2) return;
	
	int temp = *num1;
	*num1 = *num2;
	*num2 = temp;
}

/*Function for partitioning*/
int partition(int array[], int start, int end)
{
	int i = start-1;
	int j = start;
	int pivot = array[end];
	
	for (j; j < end; j++)
	{
		if(array[j] < pivot)
		{
			i++;
			swap(&array[j],&array[i]);
		}
	}
	i++;
	swap(&array[end],&array[i]);
	return i;
}

/*Function for quicksort*/
void quick_sort(int array[], int start, int end)
{
    if (end < start) return;

	int pivot = partition(array, start, end);

	quick_sort(array, start, pivot-1);
	quick_sort(array, pivot+1, end);
}


int main()
{
	int array[] = {8,4,9,3,7,2,6,5};
	int array_len = sizeof(array) / sizeof(array[0]); 
	printf("Unsorted array %d\r\n",array_len);
	for(int i = 0; i < array_len; i++)
	{
		printf("%d\t", array[i]);
	}
	
 	quick_sort(array, 0, array_len-1);
	
	printf("\r\nSorted array\r\n");
	for(int i = 0; i < array_len; i++)
	{
		printf("%d\t", array[i]);
	}
}
