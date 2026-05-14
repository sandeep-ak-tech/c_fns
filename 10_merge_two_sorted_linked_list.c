#include <stdio.h>
#include <stdlib.h>
/*
L3 - 1-2-5-6-7-8-9-10
List L1:   2 → 5 → 8 → 9 → NULL
List L2: 1 → 6 → 7 → 10 → NULL

Merged Sorted List L3:  1->2->5->6->7->8->9->10->NULL

Just tell the interviewer the simple concept first and then tell the time complexity and then how to optimise the code.
1. Simplest solution :

+------------------------------+           +----------------------+         +-----------------------------------+
|  store L1 and L2 in an array |     ->    |  Sort the full array |   ->    | Convert the array to a new ist L3 |
+------------------------------+           +----------------------+         + ----------------------------------z+
   
*/
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

// -----------------------------
// Method 1: Iterative (Recommended for interviews)
// -----------------------------
// Merge two sorted linked lists
struct Node* mergeSortedLists(struct Node* L1, struct Node* L2) {
    if (L1 == NULL) return L2;
    if (L2 == NULL) return L1;

    struct Node* dummy = newNode(0);   // Dummy node to simplify code
    struct Node* tail = dummy;

    while (L1 != NULL && L2 != NULL) {
        if (L1->data <= L2->data) {
            tail->next = L1;
            L1 = L1->next;
        } else {
            tail->next = L2;
            L2 = L2->next;
        }
        tail = tail->next;
    }

    // Attach remaining nodes
    if (L1 != NULL)
        tail->next = L1;
    else
        tail->next = L2;

    struct Node* head = dummy->next;
    free(dummy);        // Free dummy node
    return head;
}
int main() {
// List 1: 2 → 5 → 8 → 9
    struct Node* L1 = newNode(2);
    L1->next = newNode(5);
    L1->next->next = newNode(8);
    L1->next->next->next = newNode(9);

    // List 2: 1 → 6 → 7 → 10
    struct Node* L2 = newNode(1);
    L2->next = newNode(6);
    L2->next->next = newNode(7);
    L2->next->next->next = newNode(10);

    printf("List L1: ");
    printList(L1);
    printf("List L2: ");
    printList(L2);

    struct Node* merged = mergeSortedLists(L1, L2);

    printf("Merged Sorted List: ");
    printList(merged);

    // Free the allocated memory
    freeList(L1);
    freeList(L2);
    freeList(merged);

    return 0;
}

// -----------------------------
// Method 2: Recursive
// -----------------------------
struct Node* reverseRecursive(struct Node* head) {
    // Base cases
    if (head == NULL || head->next == NULL) {
        return head;
    }
    
    // Recursively reverse the rest of the list
    struct Node* newHead = reverseRecursive(head->next);
    
    // Now head->next is the last node of reversed list
    // Make head the last node
    head->next->next = head;
    head->next = NULL;
    
    return newHead;
}