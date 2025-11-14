#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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
        *head = new_node;
    }
    else
    {
        new_node->next = *head;
        *head = new_node;
    }
}

void print_nodes(struct Node* head)
{
    while(head)
    {
        printf("%d -> ",head->data);
        head = head->next;
    }
    printf("NULL");
}

struct Node* get_middle_node(struct Node * head)
{
    struct Node* slow = head, *fast = head;
    while(fast != NULL && fast->next != NULL)
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
