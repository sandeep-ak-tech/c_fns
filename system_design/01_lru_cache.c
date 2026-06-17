#include <stdio.h>
#include <stdlib.h>

// Node for Doubly Linked List
typedef struct Node {
    int key;
    int value;
    struct Node* prev;
    struct Node* next;
} Node;

// LRU Cache Structure
typedef struct {
    int capacity;
    int count;
    Node* head;      // Most Recently Used (MRU)
    Node* tail;      // Least Recently Used (LRU)
    Node** hashTable; // HashMap for O(1) lookup
    int hashSize;
} LRUCache;

// Create a new node
Node* createNode(int key, int value) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->key = key;
    node->value = value;
    node->prev = NULL;
    node->next = NULL;
    return node;
}

// Hash function
int hash(int key, int hashSize) {
    return abs(key) % hashSize;
}

// Initialize LRU Cache
LRUCache* lRUCacheCreate(int capacity) {
    if (capacity <= 0) return NULL;
    
    LRUCache* cache = (LRUCache*)malloc(sizeof(LRUCache));
    cache->capacity = capacity;
    cache->count = 0;
    cache->hashSize = capacity * 2 + 1; // Good size to reduce collisions
    
    // Create dummy head and tail
    cache->head = createNode(0, 0);
    cache->tail = createNode(0, 0);
    cache->head->next = cache->tail;
    cache->tail->prev = cache->head;
    
    // Initialize hash table
    cache->hashTable = (Node**)calloc(cache->hashSize, sizeof(Node*));
    
    return cache;
}

// Remove a node from linked list
void removeNode(LRUCache* cache, Node* node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

// Add node to the front (MRU position)
void addToHead(LRUCache* cache, Node* node) {
    node->next = cache->head->next;
    node->prev = cache->head;
    cache->head->next->prev = node;
    cache->head->next = node;
}

// Move node to head (Most Recently Used)
void moveToHead(LRUCache* cache, Node* node) {
    removeNode(cache, node);
    addToHead(cache, node);
}

// Get value for key
int lRUCacheGet(LRUCache* cache, int key) {
    if (!cache) return -1;
    
    int idx = hash(key, cache->hashSize);
    Node* node = cache->hashTable[idx];
    
    while (node) {
        if (node->key == key) {
            moveToHead(cache, node);
            return node->value;
        }
        node = node->next;  // In case of collision (chaining)
    }
    return -1;
}

// Put key-value pair
void lRUCachePut(LRUCache* cache, int key, int value) {
    if (!cache) return;
    
    int idx = hash(key, cache->hashSize);
    Node* node = cache->hashTable[idx];
    
    // Check if key already exists
    while (node) {
        if (node->key == key) {
            node->value = value;
            moveToHead(cache, node);
            return;
        }
        node = node->next;
    }
    
    // Create new node
    Node* newNode = createNode(key, value);
    
    // If cache is full, remove LRU
    if (cache->count >= cache->capacity) {
        Node* lru = cache->tail->prev;
        removeNode(cache, lru);
        
        // Remove from hash table (simplified)
        int lruIdx = hash(lru->key, cache->hashSize);
        if (cache->hashTable[lruIdx] == lru) {
            cache->hashTable[lruIdx] = lru->next;
        }
        free(lru);
        cache->count--;
    }
    
    // Add to head
    addToHead(cache, newNode);
    
    // Add to hash table (simple chaining)
    newNode->next = cache->hashTable[idx];
    cache->hashTable[idx] = newNode;
    
    cache->count++;
}

// Free LRU Cache
void lRUCacheFree(LRUCache* cache) {
    if (!cache) return;
    
    Node* current = cache->head;
    while (current) {
        Node* temp = current;
        current = current->next;
        free(temp);
    }
    
    free(cache->hashTable);
    free(cache);
}

// ==================== TEST CODE ====================
int main() {
    LRUCache* cache = lRUCacheCreate(2);
    
    lRUCachePut(cache, 1, 10);
    lRUCachePut(cache, 2, 20);
    printf("Get 1: %d\n", lRUCacheGet(cache, 1));    // 10
    
    lRUCachePut(cache, 3, 30);  // Evicts key 2
    printf("Get 2: %d\n", lRUCacheGet(cache, 2));    // -1
    
    lRUCachePut(cache, 4, 40);  // Evicts key 1
    printf("Get 1: %d\n", lRUCacheGet(cache, 1));    // -1
    printf("Get 3: %d\n", lRUCacheGet(cache, 3));    // 30
    printf("Get 4: %d\n", lRUCacheGet(cache, 4));    // 40
    
    lRUCacheFree(cache);
    return 0;
}
