#include <stdio.h>
#include <stdlib.h>
/*
Floyd’s Cycle Detection Algorithm (O(n) time, O(1) space)

Original list:   1 → 2 → 3 → 4 → (back to 2)
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

// Function to detect loop/cycle using Floyd's algorithm
int detectLoop(struct Node* head) {
    struct Node *slow = head;
    struct Node *fast = head;

    while (slow && fast && fast->next) {
        slow = slow->next;           // move 1 step
        fast = fast->next->next;     // move 2 steps

        if (slow == fast) {
            return 1;  // Loop detected => → List is circular / has cycle
        }
    }
    return 0;  // No loop
}

/*
Find the start of the loop

a loop is detected (slow == fast), you can find the start of the loop:

Reset one pointer (say slow) to head.
Move both pointers one step at a time. They will meet at the start of the loop.
*/
struct Node* findLoopStart(struct Node* head) {
    struct Node *slow = head, *fast = head;
    
    // First detect loop
    while (slow && fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) break;
    }
    
    if (slow != fast) return NULL;  // no loop
    
    // Find start of loop
    slow = head;
    while (slow != fast) {
        slow = slow->next;
        fast = fast->next;
    }
    return slow;  // start of the loop
}

/*
Removing the loop
*/
void removeLoop(struct Node* head) {
    struct Node *slow = head, *fast = head;
    // ... (detect loop as above)

    if (slow != fast) return;  // no loop

    slow = head;
    while (slow->next != fast->next) {
        slow = slow->next;
        fast = fast->next;
    }
    fast->next = NULL;  // break the loop
}

/*
How to Distinguish a Proper Circular Linked List
A circular linked list is a special case where the last node points back to the head.
To check if a list is fully circular (every node is part of the cycle starting from head):
*/
int isCircular(struct Node* head) {
    if (head == NULL) return 1;  // empty list can be considered circular

    struct Node* temp = head->next;

    while (temp != NULL && temp != head) { //checking if loop has reached end i.e. NULL or back to head=> circular
        temp = temp->next;
    }

    if (temp == head) {
        return 1;        // Properly circular (last node points to head)
    }
    
    // Optional: If you also want to reject lists that have a loop but are not fully circular
    // You can add Floyd’s check here to detect any loop
    return 0;
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
    // Create sample list: 1 → 2 → 3 → 4 → (back to 2)
    struct Node* head = newNode(1);
    head->next = newNode(2);
    head->next->next = newNode(3);
    head->next->next->next = newNode(4);

    // Create a loop: 4 -> 2
    head->next->next->next->next = head->next;

    printf("Original list:   ");
    printList(head);

    // Choose one method at a time (comment/uncomment as needed)

    // Method 1 - Iterative (recommended for interviews)
    if (detectLoop(head)) {
        printf("Loop detected in the linked list\n");
    } else {
        printf("No loop in the linked list\n");
    }

    struct Node* loopStart = findLoopStart(head);
    if (loopStart != NULL) {
        printf("Start of the loop: %d\n", loopStart->data);
    }
    removeLoop(head);
    printf("List after removing loop: ");
    printList(head);

    // Clean up
    freeList(head);
    
    return 0;
}

