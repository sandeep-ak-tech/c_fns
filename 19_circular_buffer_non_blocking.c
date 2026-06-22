#include <stdio.h>
#include <stdbool.h>

#define CAPACITY 8

/*
 * ================================================================
 * PICTORIAL REPRESENTATION OF RingBuffer (Queue on circular array)
 * ================================================================
 *
 * CAPACITY = 8
 *
 * Indices :   0    1    2    3    4    5    6    7
 * Data    : [11] [22] [33] [__] [__] [__] [__] [__]
 *             ^               ^
 *            head            tail
 *
 * head  = index of next element to dequeue (read/remove)
 * tail  = index of next free slot to enqueue (write/insert)
 * count = number of valid elements currently stored
 *
 * Example state shown above:
 * head = 0
 * tail = 3
 * count = 3
 * valid elements (FIFO order): 11, 22, 33
 *
 * Why head moves forward after dequeue:
 * - dequeue removes oldest element at head.
 * - next oldest element is at head+1.
 * - so we do: head = (head + 1) % CAPACITY
 * - decrementing head would move backward and break FIFO order.
 */

typedef struct {
    int data[CAPACITY];
    int head;   /* next read position */
    int tail;   /* next write position */
    int count;  /* how many elements currently stored */
} RingBuffer;

/* Initialize to empty state */
void rb_init(RingBuffer *rb) {
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
}

bool rb_is_empty(const RingBuffer *rb) {
    return rb->count == 0;
}

bool rb_is_full(const RingBuffer *rb) {
    return rb->count == CAPACITY;
}

/* Insert one element at tail (enqueue) */
bool rb_enqueue(RingBuffer *rb, int value) {
    if (rb_is_full(rb)) {
        return false;
    }

    rb->data[rb->tail] = value;
    rb->tail = (rb->tail + 1) % CAPACITY;  /* wrap around */
    rb->count++;
    return true;
}

/* Remove one element from head (dequeue) */
bool rb_dequeue(RingBuffer *rb, int *out_value) {
    if (rb_is_empty(rb)) {
        return false;
    }

    *out_value = rb->data[rb->head];
    rb->head = (rb->head + 1) % CAPACITY;  /* move to next oldest item */
    rb->count--;
    return true;
}

void rb_print_state(const RingBuffer *rb) {
    printf("head=%d tail=%d count=%d | data: ", rb->head, rb->tail, rb->count);
    for (int i = 0; i < CAPACITY; i++) {
        printf("%d ", rb->data[i]);
    }
    printf("\n");
}

int main(void) {
    RingBuffer rb;
    rb_init(&rb);

    printf("Initial state:\n");
    rb_print_state(&rb);

    printf("\nEnqueue 11, 22, 33, 44, 55\n");
    rb_enqueue(&rb, 11);
    rb_enqueue(&rb, 22);
    rb_enqueue(&rb, 33);
    rb_enqueue(&rb, 44);
    rb_enqueue(&rb, 55);
    rb_print_state(&rb);

    int x;
    printf("\nDequeue two elements:\n");
    if (rb_dequeue(&rb, &x)) printf("Dequeued: %d\n", x);
    if (rb_dequeue(&rb, &x)) printf("Dequeued: %d\n", x);
    rb_print_state(&rb);

    printf("\nEnqueue 66, 77, 88, 99 (tail wraps if needed)\n");
    rb_enqueue(&rb, 66);
    rb_enqueue(&rb, 77);
    rb_enqueue(&rb, 88);
    rb_enqueue(&rb, 99);
    rb_print_state(&rb);

    printf("\nDrain all:\n");
    while (rb_dequeue(&rb, &x)) {
        printf("Dequeued: %d\n", x);
    }

    printf("\nFinal empty? %s\n", rb_is_empty(&rb) ? "YES" : "NO");
    return 0;
}
