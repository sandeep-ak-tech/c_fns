#include <stdio.h>
#include <stdlib.h>

// Min-heap implementation for kth largest
typedef struct {
    int* heap;
    int capacity;
    int size;
} MinHeap;

MinHeap* createMinHeap(int capacity) {
    MinHeap* h = (MinHeap*)malloc(sizeof(MinHeap));
    h->heap = (int*)malloc(capacity * sizeof(int));
    h->capacity = capacity;
    h->size = 0;
    return h;
}

void heapify(MinHeap* h, int i) {
    int smallest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < h->size && h->heap[left] < h->heap[smallest])
        smallest = left;
    if (right < h->size && h->heap[right] < h->heap[smallest])
        smallest = right;

    if (smallest != i) {
        int temp = h->heap[i];
        h->heap[i] = h->heap[smallest];
        h->heap[smallest] = temp;
        heapify(h, smallest);
    }
}

void insert(MinHeap* h, int val) {
    if (h->size == h->capacity) {
        if (val <= h->heap[0]) return; // Don't need larger than current smallest
        h->heap[0] = val;
        heapify(h, 0);
        return;
    }

    h->heap[h->size] = val;
    int i = h->size++;
    
    while (i > 0 && h->heap[(i-1)/2] > h->heap[i]) {
        int temp = h->heap[i];
        h->heap[i] = h->heap[(i-1)/2];
        h->heap[(i-1)/2] = temp;
        i = (i-1)/2;
    }
}

int findKthLargestHeap(int arr[], int n, int k) {
    MinHeap* heap = createMinHeap(k);
    
    for (int i = 0; i < k && i < n; i++)
        insert(heap, arr[i]);
    
    for (int i = k; i < n; i++) {
        if (arr[i] > heap->heap[0])
            insert(heap, arr[i]);
    }
    
    int result = heap->heap[0];
    free(heap->heap);
    free(heap);
    return result;
}

int main() {
    int arr[] = {3, 2, 3, 1, 2, 4, 5, 5, 6};
    int n = sizeof(arr)/sizeof(arr[0]);
    int k = 4;

    printf("4th largest element is %d\n", findKthLargestHeap(arr, n, k));
    return 0;
}