#include <stdio.h>
#include <stdlib.h>
/*
List A: 1 -> 2 -> 3 -> 6 -> 7 -> 8 -> NULL
List B: 4 -> 5 -> 6 -> 7 -> 8 -> NULL

Intersection Point: 6

Time Complexity: O(M + N) — where M and N are lengths of two lists.
Space Complexity: O(1) — No extra space used.

understand  the concept : https://youtube.com/shorts/WjK-_KN0_Ck?si=-q4jH47HzPmTE2Oc

AB+BC+DB= DB+BC+AB

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

// Elegant Two-Pointer Method to find intersection
Node* getIntersectionNode(Node* headA, Node* headB) {
    if (headA == NULL || headB == NULL) 
        return NULL;

    Node* ptrA = headA;
    Node* ptrB = headB;

    while (ptrA != ptrB) {
        // Switch to the other list when reaching end
        ptrA = (ptrA == NULL) ? headB : ptrA->next;
        ptrB = (ptrB == NULL) ? headA : ptrB->next;
    }

    // If they meet, that's the intersection point
    return ptrA;   // ptrA == ptrB
}

int main() {
    // Create common intersection part
    Node* common = newNode(6);
    common->next = newNode(7);
    common->next->next = newNode(8);

    // List A: 1 -> 2 -> 3 -> 6 -> 7 -> 8
    Node* headA = newNode(1);
    headA->next = newNode(2);
    headA->next->next = newNode(3);
    headA->next->next->next = common;   // intersection starts

    // List B: 4 -> 5 -> 6 -> 7 -> 8
    Node* headB = newNode(4);
    headB->next = newNode(5);
    headB->next->next = common;         // intersection starts

    printf("List A: ");
    printList(headA);
    
    printf("List B: ");
    printList(headB);

    Node* intersection = getIntersectionNode(headA, headB);

    if (intersection) {
        printf("\nIntersection Point: %d\n", intersection->data);
    } else {
        printf("\nNo Intersection\n");
    }

    return 0;
}
