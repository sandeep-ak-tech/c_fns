#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TABLE_SIZE 100

typedef struct Node {
    int data;
    struct Node* next;
} Node;

typedef struct {
    Node* table[TABLE_SIZE];
} Set;

unsigned int hash(int key) {
    return (unsigned int)key % TABLE_SIZE;
}

void set_init(Set* set) {
    for (int i = 0; i < TABLE_SIZE; i++)
        set->table[i] = NULL;
}

bool set_contains(Set* set, int key) {
    unsigned int index = hash(key);
    Node* current = set->table[index];
    
    while (current != NULL) {
        if (current->data == key)
            return true;
        current = current->next;
    }
    return false;
}

void set_insert(Set* set, int key) {
    if (set_contains(set, key))
        return;  // already exists → ignore

    unsigned int index = hash(key);
    
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->data = key;
    new_node->next = set->table[index];
    
    set->table[index] = new_node;
}

void set_free(Set* set) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        Node* current = set->table[i];
        while (current) {
            Node* temp = current;
            current = current->next;
            free(temp);
        }
    }
}

// Usage
int main() {
    Set s;
    set_init(&s);
    
    set_insert(&s, 10);
    set_insert(&s, 20);
    set_insert(&s, 10);   // duplicate → ignored
    
    printf("Contains 10? %d\n", set_contains(&s, 10));  // 1
    printf("Contains 15? %d\n", set_contains(&s, 15));  // 0
    
    set_free(&s);
    return 0;
}

/*
(Most common follow-up: improve the hash function, add resizing)
Improvements and Follow-ups
Improved Hash Function: The current hash function uses a simple modulo operation, which may lead to collisions. Consider using a more robust hash function, such as the FNV-1a hash or the murmurhash.
Resizing: To handle dynamic insertion and removal of keys, implement a resizing mechanism that grows or shrinks the hash table as needed, maintaining a reasonable load factor.
Collision Resolution: In addition to chaining, consider implementing other collision resolution techniques, such as open addressing or linear probing, to improve performance in certain scenarios.

*/