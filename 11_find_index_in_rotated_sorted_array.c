/*
This is modified binary search. 

How the Algorithm Works:

	1. Use modified binary search.
	2. At every step, check which half is sorted:
	   If arr[low] <= arr[mid] → left half is sorted.
	   Else → right half is sorted.
	3. Decide which half the target can possibly be in based on sorted property.
	4. Continue narrowing down the search space.

Time Complexity: O(log n)
Space Complexity: O(1)

Input - Output Sample :
Array:   [7, 8, 9, 1, 2, 3, 4, 5, 6]
Target:  3
Output:  5  (index of 3)

Array:   [4, 5, 6, 7, 0, 1, 2]
Target:  0
Output:  4

Explanation:
Here lets fix low= first element = 4
high = last element = 2
mid = (low + high) / 2 = (0 + 6) / 2 = 3 (index of 7) => element 7

now, lets consider input =0, a[0]<a[mid] =>4<7 => left half is sorted
a[mid]>a[last] => 7>2 => second half is rotated 
now, target 0 is not in left half, so we will search in right half
low = mid + 1 = 4
*/

#include <stdio.h>

// Function to find index of target in rotated sorted array
int searchRotatedArray(int arr[], int n, int target)
{
    int low = 0;
    int high = n - 1;

    while (low <= high)
    {
        int mid = low + (high - low) / 2;

        // If target found at mid
        if (arr[mid] == target)
            return mid;

        // Check if left half is sorted
        if (arr[low] <= arr[mid])
        {
            // Target lies in left sorted half
            if (target >= arr[low] && target < arr[mid])
                high = mid - 1;
            else
                low = mid + 1;      // Target must be in right half
        }
        // Right half is sorted
        else
        {
            // Target lies in right sorted half
            if (target > arr[mid] && target <= arr[high])
                low = mid + 1;
            else
                high = mid - 1;     // Target must be in left half
        }
    }

    return -1;  // Element not found
}

int main()
{
    int arr[] = {7, 8, 9, 1, 2, 3, 4, 5, 6};
    int n = sizeof(arr) / sizeof(arr[0]);
    int target;

    printf("Enter the element to search: ");
    scanf("%d", &target);

    int index = searchRotatedArray(arr, n, target);

    if (index != -1)
        printf("Element %d found at index %d\n", target, index);
    else
        printf("Element %d not found in the array\n", target);

    // Test cases
    printf("\n--- Test Cases ---\n");
    printf("Array: ");
    for(int i = 0; i < n; i++)
        printf("%d ", arr[i]);
    printf("\n");

    int tests[] = {3, 7, 6, 10};
    for(int i = 0; i < 4; i++) {
        int idx = searchRotatedArray(arr, n, tests[i]);
        printf("Search %d → Index: %d\n", tests[i], idx);
    }

    return 0;
}