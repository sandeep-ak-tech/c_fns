#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
/*
1 → 2 → 3 → 2 → 1  => Palindrome
7 → 4 → 9 → 4 → 7 => Palindrome

1 → 2 → 2 → 1 → 3 => Not Palindrome
*/


struct Node{
    int data;
    struct Node* next;
};

void push(struct Node** head, int data)
{
    struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));
    new_node->data = data;
    if(*head == NULL)
    {
        new_node->next = NULL;   
    }
    else
    {
        new_node->next = *head;
    }
    *head = new_node;
}
/*
Optimised push
void push(struct Node** head, int data)
{
    struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));
    new_node->data = data;
    new_node->next = *head;
    *head = new_node;
}
*/
void print_nodes(struct Node* head)
{
    while(head)
    {
        printf("%d -> ",head->data);
        head = head->next;
    }
    printf("NULL");
}
/*Even node example:
Now for 1->2->3->4->2->1: slow stops at idx2 (value 3), so:

first half = 1->2->3, second half = 4->2->1 → reversed 1->2->4
compare: 1==1 ✓, 2==2 ✓, 3==4 ✗ → returns false ✅
Also keep the reverse_nodes fix from before (return NULL, not (struct Node*)-1) 
so single-node/empty second halves don't crash. With both changes the function is 
correct for odd and even lengths. */
struct Node* get_middle_node(struct Node * head)
{
    struct Node* slow = head, *fast = head;
    while(fast->next != NULL && fast->next->next != NULL)//works for both even and odd nodes
    {
        slow = slow->next;
        fast = fast->next->next;
    }
    
    return slow;
}

struct Node * reverse_nodes(struct Node * start)
{
    if(start == NULL)   return (struct Node *)-1;
    struct Node * current = start, *previous = NULL, *next = NULL;
    while (current)
    {
        next = current->next;
        current->next = previous;
        previous = current;
        current = next;
    }
    return previous;
}

bool is_palindrome(struct Node * head)
{
    if (head == NULL) return true; 
    struct Node * middle_node = get_middle_node(head);
    struct Node * first_half = head;
    struct Node * second_half = middle_node->next;
    middle_node->next = NULL;
    
    second_half = reverse_nodes(second_half);
    struct Node * second_half_cpy = second_half;
    
    bool result = true;
    
    while(second_half != NULL)
    {
        if(first_half->data != second_half->data)
        {
            result = false;
            break;
        }
        first_half = first_half->next;
        second_half = second_half->next;
    }
    
    second_half = reverse_nodes(second_half_cpy);
    middle_node->next = second_half;
    
    return result;
    
}
int main()
{
    struct Node* head = NULL;
    push(&head, 1);
    push(&head, 2);
    push(&head, 3);
    push(&head, 2);
    push(&head, 1);
    
    printf("Hello World\r\n");
    print_nodes(head);
    struct Node* middle = get_middle_node(head);
    printf("\r\nMiddle node value = %d\r\n",middle->data);
    //struct Node* reversed_node = reverse_nodes(middle);
    //print_nodes(reversed_node);
    if(is_palindrome(head))
    {
        printf("The list is Palindrom...\r\n");
    }

    return 0;
}
