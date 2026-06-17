#include <stdio.h>
#include <stdlib.h>

// Node for SJF Cache
typedef struct Node {
    int key;
    int value;
    int size;
    struct Node* next;  // For hash chaining
} Node;

// Min-Heap Node for Priority Queue (based on size)
typedef struct HeapNode {
    int key;
    int size;
    Node* dataNode;
} HeapNode;

// SJF Cache Structure
typedef struct {
    int capacity;
    int count;
    Node** hashTable;
    int hashSize;
    HeapNode* heap;
    int heapSize;
    int heapCapacity;
} SJFCache;

// Create new node
Node* createNode(int key, int value, int size) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->key = key;
    node->value = value;
    node->size = size;
    node->next = NULL;
    return node;
}

// Hash function
int hash(int key, int hashSize) {
    return abs(key) % hashSize;
}

// Min-Heap helper functions
void swap(HeapNode* a, HeapNode* b) {
    HeapNode temp = *a;
    *a = *b;
    *b = temp;
}

void heapifyDown(SJFCache* cache, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < cache->heapSize && cache->heap[left].size < cache->heap[smallest].size)
        smallest = left;
    if (right < cache->heapSize && cache->heap[right].size < cache->heap[smallest].size)
        smallest = right;

    if (smallest != idx) {
        swap(&cache->heap[idx], &cache->heap[smallest]);
        heapifyDown(cache, smallest);
    }
}

void heapifyUp(SJFCache* cache, int idx) {
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        if (cache->heap[idx].size >= cache->heap[parent].size)
            break;
        swap(&cache->heap[idx], &cache->heap[parent]);
        idx = parent;
    }
}

// Insert into min-heap
void heapPush(SJFCache* cache, int key, int size, Node* dataNode) {
    if (cache->heapSize >= cache->heapCapacity) return;

    cache->heap[cache->heapSize].key = key;
    cache->heap[cache->heapSize].size = size;
    cache->heap[cache->heapSize].dataNode = dataNode;
    heapifyUp(cache, cache->heapSize);
    cache->heapSize++;
}

// Remove smallest from heap
void heapPop(SJFCache* cache) {
    if (cache->heapSize == 0) return;

    cache->heap[0] = cache->heap[cache->heapSize - 1];
    cache->heapSize--;
    heapifyDown(cache, 0);
}

// Initialize SJF Cache
SJFCache* sjfCacheCreate(int capacity) {
    if (capacity <= 0) return NULL;

    SJFCache* cache = (SJFCache*)malloc(sizeof(SJFCache));
    cache->capacity = capacity;
    cache->count = 0;
    cache->hashSize = capacity * 2 + 1;
    cache->heapCapacity = capacity;
    cache->heapSize = 0;

    cache->hashTable = (Node**)calloc(cache->hashSize, sizeof(Node*));
    cache->heap = (HeapNode*)malloc(capacity * sizeof(HeapNode));

    return cache;
}

// Get value
int sjfCacheGet(SJFCache* cache, int key) {
    if (!cache) return -1;

    int idx = hash(key, cache->hashSize);
    Node* node = cache->hashTable[idx];

    while (node) {
        if (node->key == key) {
            return node->value;
        }
        node = node->next;
    }
    return -1;
}

// Put key-value with size
void sjfCachePut(SJFCache* cache, int key, int value, int size) {
    if (!cache) return;

    int idx = hash(key, cache->hashSize);
    Node* node = cache->hashTable[idx];

    // Update if exists
    while (node) {
        if (node->key == key) {
            node->value = value;
            node->size = size;
            return;
        }
        node = node->next;
    }

    // If full, evict smallest
    while (cache->count >= cache->capacity) {
        if (cache->heapSize == 0) break;

        HeapNode smallest = cache->heap[0];
        heapPop(cache);

        // Remove from hash table
        int lruIdx = hash(smallest.key, cache->hashSize);
        Node* curr = cache->hashTable[lruIdx];
        Node* prev = NULL;

        while (curr) {
            if (curr->key == smallest.key) {
                if (prev) prev->next = curr->next;
                else cache->hashTable[lruIdx] = curr->next;
                free(curr);
                break;
            }
            prev = curr;
            curr = curr->next;
        }
        cache->count--;
    }

    // Add new node
    Node* newNode = createNode(key, value, size);
    newNode->next = cache->hashTable[idx];
    cache->hashTable[idx] = newNode;

    heapPush(cache, key, size, newNode);
    cache->count++;
}

// Free cache
void sjfCacheFree(SJFCache* cache) {
    if (!cache) return;

    for (int i = 0; i < cache->hashSize; i++) {
        Node* curr = cache->hashTable[i];
        while (curr) {
            Node* temp = curr;
            curr = curr->next;
            free(temp);
        }
    }

    free(cache->hashTable);
    free(cache->heap);
    free(cache);
}

// Test
int main() {
    SJFCache* cache = sjfCacheCreate(3);

    sjfCachePut(cache, 1, 10, 50);   // size 50
    sjfCachePut(cache, 2, 20, 100);  // size 100
    sjfCachePut(cache, 3, 30, 30);   // size 30

    printf("Get 1: %d\n", sjfCacheGet(cache, 1));

    sjfCachePut(cache, 4, 40, 20);   // size 20 -> should evict key 3 (smallest)

    printf("Get 3: %d\n", sjfCacheGet(cache, 3));  // -1
    printf("Get 4: %d\n", sjfCacheGet(cache, 4));

    sjfCacheFree(cache);
    return 0;
}
