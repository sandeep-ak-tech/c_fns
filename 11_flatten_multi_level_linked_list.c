#include <stdio.h>
#include <stdlib.h>
/*
// Create a sample multilevel linked list:
    // 1 -> 2 -> 3 -> 4
    //      |         |
    //      5 -> 6    7
    //           |
    //           8   
Now Flatten it and the output = Flattened List: 1 -> 2 -> 3 -> 4 -> 5 -> 6 -> 7 -> 8 -> NULL


Time and Space Complexity Summary
=================================

| Complexity Type       | Complexity | Remarks                              |
|-----------------------|------------|--------------------------------------|
| Time Complexity       | O(N)       | Linear in total number of nodes      |
| Space Complexity      | O(1)       | Constant extra space                 |
| Recursion Depth       | None       | Fully Iterative version              |
| Overall Efficiency    | Excellent  | Optimal for this problem             |

Where N = Total number of nodes in the multilevel linked list.
*/
// Node structure
typedef struct Node {
    int data;
    struct Node* right;
    struct Node* down;
} Node;

// Create new node
Node* createNode(int data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    newNode->right = NULL;
    newNode->down = NULL;
    return newNode;
}

// Print list
void printList(Node* head) {
    Node* curr = head;
    while (curr != NULL) {
        printf("%d", curr->data);
        if (curr->right) printf(" -> ");
        curr = curr->right;
    }
    printf(" -> NULL\n");
}

// -----------------------------
// Iterative Flatten - Row Major Order (Right first, then Down)
Node* flattenIterative(Node* head) {
    if (head == NULL) return NULL;

    Node* curr = head;
    Node* tail = head;

    // First, move tail to the end of the current list
    while (tail->right != NULL) {
        tail = tail->right;
    }

    // Process each node
    while (curr != NULL) {
        if (curr->down != NULL) {
            // Attach the down list at the end
            tail->right = curr->down;
            
            // Move tail to the new end
            while (tail->right != NULL) {
                tail = tail->right;
            }
            
            // Clear the down pointer
            curr->down = NULL;
        }
        curr = curr->right;
    }

    return head;
}

int main() {
    // Create multilevel linked list
    Node* head = createNode(1);
    head->right = createNode(2);
    head->right->right = createNode(3);
    head->right->right->right = createNode(4);

    // Down branches
    head->right->down = createNode(5);
    head->right->down->right = createNode(6);
    head->right->down->right->down = createNode(8);

    head->right->right->right->down = createNode(7);

    printf("Flattening using Iterative Method...\n\n");
    
    Node* flattened = flattenIterative(head);
    
    printf("Flattened List: ");
    printList(flattened);

    return 0;
}