/*
Binary Search is an efficient algorithm to find an element in a sorted array.
It works by repeatedly dividing the search interval in half.

Time Complexity: O(log n)
Space Complexity: O(1) for iterative version
*/

/*Function for swapping*/
#include <stdio.h>

// Iterative Binary Search Function
// Returns index of target if found (not the value), else -1
int binarySearch(int arr[], int n, int target)
{
    int low = 0;
    int high = n - 1;

    while (low <= high)
    {
        int mid = low + (high - low) / 2;   // low + (high - low)/2 is preferred over (low + high)/2 to prevent integer overflow.

        if (arr[mid] == target)
            return mid;                     // Element found

        else if (arr[mid] < target)
            low = mid + 1;                  // Search in right half

        else
            high = mid - 1;                 // Search in left half
    }

    return -1;  // Element not found
}

int main()
{
    int arr[] = {2, 5, 8, 12, 16, 23, 38, 45, 56, 67, 78};
    int n = sizeof(arr) / sizeof(arr[0]);
    int target;

    printf("Array: ");
    for(int i = 0; i < n; i++)
        printf("%d ", arr[i]);
    printf("\n\n");

    printf("Enter element to search: ");
    scanf("%d", &target);

    int result = binarySearch(arr, n, target);

    if (result != -1)
        printf("Element %d found at index %d\n", target, result);
    else
        printf("Element %d not found in the array\n", target);

    return 0;
}

/*
Recursive Binary Search Version
*/
int binarySearchRecursive(int arr[], int low, int high, int target)
{
    if (low > high)
        return -1;

    int mid = low + (high - low) / 2;

    if (arr[mid] == target)
        return mid;

    if (arr[mid] < target)
        return binarySearchRecursive(arr, mid + 1, high, target);
    else
        return binarySearchRecursive(arr, low, mid - 1, target);
}

// Wrapper function
int binarySearchRec(int arr[], int n, int target)
{
    return binarySearchRecursive(arr, 0, n-1, target);
}