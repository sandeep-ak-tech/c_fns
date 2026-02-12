#include <stdio.h>
#include <stdlib.h>

// Node structure
struct Node {
    int data;
    struct Node* next;
};

// Helper function to create a new node
struct Node* newNode(int data) {
    struct Node* temp = (struct Node*)malloc(sizeof(struct Node));
    temp->data = data;
    temp->next = NULL;
    return temp;
}

// -----------------------------
// Method 1: Iterative (Most commonly used in interviews)
// NCPC = CPCN
// -----------------------------
struct Node* reverseIterative(struct Node* head) {
    struct Node *prev = NULL;
    struct Node *current = head;
    struct Node *next = NULL;

    while (current != NULL) {
        // Store next
        next = current->next;
        
        // Reverse current node's pointer
        current->next = prev;
        
        // Move prev and current one step forward
        prev = current;
        current = next;
    }
    
    return prev;    // prev becomes new head
}

// -----------------------------
// Utility function to print list
// -----------------------------
void printList(struct Node* head) {
    struct Node* temp = head;
    while (temp != NULL) {
        printf("%d → ", temp->data);
        temp = temp->next;
    }
    printf("NULL\n");
}

// -----------------------------
// Utility function to free list
// -----------------------------
void freeList(struct Node* head) {
    struct Node* temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

int main() {
    // Create sample list: 1 → 2 → 3 → 4 → 5
    struct Node* head = newNode(1);
    head->next = newNode(2);
    head->next->next = newNode(3);
    head->next->next->next = newNode(4);
    head->next->next->next->next = newNode(5);

    printf("Original list:   ");
    printList(head);

    // Choose one method at a time (comment/uncomment as needed)

    // Method 1 - Iterative (recommended for interviews)
    head = reverseIterative(head);
    printf("Reversed (iter): ");
    printList(head);

    // Clean up
    freeList(head);
    
    return 0;
}