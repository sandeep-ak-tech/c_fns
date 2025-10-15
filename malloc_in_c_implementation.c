#include <unistd.h>
#include <stdio.h>
#include <stddef.h>

/*
metadata = size and a pointer to the next block

*/
// Structure to represent a memory block (metadata + payload)
typedef struct Block {
    size_t size;          // Size of the block (excluding metadata)
    struct Block *next;   // Pointer to the next free block
    int is_free;          // Flag to indicate if the block is free (1) or allocated (0)
} Block;

// Global pointer to the head of the free list
static Block *free_list = NULL;

// Function to request memory from the OS using sbrk
void *request_memory(size_t size) {
    void *ptr = sbrk(size);
    if (ptr == (void *)-1) { // sbrk returns -1 on failure
        return NULL;
    }
    return ptr;
}

void *my_malloc(int size)
{
    if(size<0)
    {
        return NULL;
    }

    size_t total_size = size + sizeof(Block); 

    if(free_list == NULL)// checking if there are no free blocks available
    {
        //since no free blocks are available request for a new one
        Block *newblock = request_memory(total_size);
        if(newblock == NULL)
        {
            return NULL;
        }

    
    /*add metadata to the newly allocated block*/
        newblock->size = size;
        newblock->next = NULL;
        newblock->is_free = 0;//the new block is allocated 

    /*Since the newly allocated block is already allocated, 
    the available freelist is zero
    */
    free_list = NULL;

    /*Return pointer to newblock without metadata*/
    /*
    [ Block Metadata | Payload ]
    ^                 ^
    |                 |
    newblock           newblock + 1
    */
      if(newblock != NULL)
      {
          return (void *)(newblock+1);
      }
   }

    /* If free blocks are available i.e. free_list is not empty,
    then lets search for it from the available blocks whose head list is free_list */
    Block* current = free_list;
    Block* previous = NULL;

    while(current != NULL)
    {
        /*So lets check the first block is free i.e. is_free == 1*/
        if(current->is_free ==1 && current->size > size)
        {
            current->is_free = 0;//lets mark it as allocated first

            /*Check if the available memory is too big*/
            if(current->size > size+sizeof(Block)+1)
            {
                /*Since its too big lets split into small chunks again*/
                /*Before Splitting - memory layout
                [current: metadata | payload (current->size bytes)]
                */
                /*Current block is given for current memory allocation with size size
                Now lets Create a newblock that will point to the remaining memory, 
                i.e. points to where the new free block’s metadata will start.*/
                Block *new_block = (Block *)((char*)current + sizeof(Block) + size);
                /*After splitting - memory layout
                [current: metadata | payload (size bytes)] [new_block: metadata | payload (remaining bytes)]*/
                new_block->size = current->size - sizeof(Block) - size; 
                /*= original block’s payload size - metadata size for the new block - payload allocated for the user*/
                new_block->is_free = 1;
                new_block->next = current->next; /*new block points to the next free block in the list i.e. free_list(if any)*/
                current->next = new_block;
                current->size = size;
            }
            /*Return pointer to the payload*/
            return(void *)(current+1);
        }
        /*So now since the first block is not free i.e. is_free != 1 or the size id not
        enough current->size < size, lets traverse to the next block */
        previous = current;
        current = current->next;

    }
    /*Since current == NULL, ie no suitable free block is found, lets request a new memory*/
    /*This condition is needed to address a scenario where we have free blocks with its size < size passed*/
    Block *newblock = request_memory(total_size);
    if(newblock == NULL)
    {
        return NULL;
    }
    newblock->size = size;
    newblock->is_free = 0;
    /*since we have traversed through the whole list and the last list always 
    has its next value as NULL and this newblock is the last block now */
    newblock->next = NULL;
    /*after traversing the previous block holds the address for the last block refer line no 107*/
    if(previous!=NULL)
    {
        previous->next = newblock;
    }
    
    return (void*)(newblock+1);

}
/*
Function name : main
parameters - void
Return type - int
*/
int main(){
    int num = 16;
    int* ptr = (int*)my_malloc(sizeof(int));
    if(ptr)
    {
        *ptr = 42;
        printf("The memory is allocated at %u\r\n",ptr);
        printf("Allocated integer: %d\n", *ptr);
    }
    
}
