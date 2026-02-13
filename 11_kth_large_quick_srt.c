/*
This is code for find the kth largest element using quick sort
The QuickSelect method is generally preferred because:

Average time complexity: O(n)
Worst case: O(n²) → can be improved with random pivot
Space: O(1) auxiliary (besides recursion stack)

Steps:
	1. Choose a pivot
	2. Partition the array
	3. Recursively sort the sub arrays untill the given subarray cannot be further divided.
*/

#include <stdio.h>
#include <stdlib.h>

/*Function for swapping*/
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
		if(array[j] > pivot)
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
int quick_select(int array[], int start, int end, int k)
{
    if (end < start) return -1;

	int pivot = partition(array, start, end);

    // If pivot is at (k-1)th position (0-indexed)
    if (pivot == k - 1)
        return array[pivot];
    else if (pivot > k - 1)
        return quick_select(array, start, pivot - 1, k);
    else
        return quick_select(array, pivot + 1, end, k);
}

/*Function to find kth largest*/
int find_kth_largest(int arr[], int n, int k) {
    if (k < 1 || k > n) {
        printf("Invalid value of k\n");
        return -1;
    }
    
    // Make a copy since QuickSelect modifies the array
    int* temp = (int*)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++)
        temp[i] = arr[i];

    int result = quick_select(temp, 0, n - 1, k);
    free(temp);
    return result;
}

int main()
{
	int array[] = {8,4,9,3,7,2,6,5,10};
	int array_len = sizeof(array) / sizeof(array[0]); 
	int k = 4;

	printf("Unsorted array %d\r\n",array_len);
	for(int i = 0; i < array_len; i++)
	{
		printf("%d\t", array[i]);
	}
	
    int kth = find_kth_largest(array, array_len, k);
    if (kth != -1)
        printf("\r\n%dth largest element is %d\n", k, kth);

    return 0;
}
