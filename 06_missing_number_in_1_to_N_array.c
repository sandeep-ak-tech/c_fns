/*
Why XOR Trick?

	1. XOR of all numbers from 1 to N
	2. XOR of all elements in the array
	3. Final XOR of both results gives the missing number

Time Complexity: O(N)
Space Complexity: O(1) (No extra space)

XOR from 1 to N    ^    XOR of all array elements   =   Missing Number

Key Properties of XOR:

x ^ x = 0
x ^ 0 = x
XOR is associative and commutative

So when you XOR everything (1 to N + array), the numbers that appear twice cancel out (x^x=0), leaving only the missing number.
*/

#include <stdio.h>

// Function to find missing number using XOR
int findMissingNumber(int arr[], int n)
{
    int xor_all = 0;
    
    // Step 1: XOR all numbers from 1 to N
    for(int i = 1; i <= n; i++) {
        xor_all ^= i;
    }
    
    // Step 2: XOR all elements in the array
    for(int i = 0; i < n-1; i++) {      // array has N-1 elements
        xor_all ^= arr[i];
    }
    
    // The remaining value is the missing number
    return xor_all;
}

int main()
{
    // Example 1
    int arr1[] = {1, 2, 4, 5, 6, 7, 8};     // N=8, missing 3
    int n1 = 8;
    
    printf("Array: ");
    for(int i = 0; i < n1-1; i++)
        printf("%d ", arr1[i]);
    
    int missing = findMissingNumber(arr1, n1);
    printf("\nMissing number (1 to %d) is: %d\n\n", n1, missing);
    
    // Example 2
    int arr2[] = {3, 7, 1, 2, 8, 4, 5, 9, 10};  // N=10, missing 6
    int n2 = 10;
    
    printf("Array: ");
    for(int i = 0; i < n2-1; i++)
        printf("%d ", arr2[i]);
    
    missing = findMissingNumber(arr2, n2);
    printf("\nMissing number (1 to %d) is: %d\n", n2, missing);
    
    return 0;
}

/*Optimized Version (Slightly Cleaner)*/
int findMissingNumber(int arr[], int n)
{
    int xor_result = 0;
    
    // XOR all array elements + numbers from 1 to N in one loop
    for(int i = 0; i < n-1; i++) {
        xor_result ^= arr[i];           // array element
        xor_result ^= (i + 1);          // number from 1 to N-1
    }
    
    xor_result ^= n;    // XOR the last number N
    
    return xor_result;
}