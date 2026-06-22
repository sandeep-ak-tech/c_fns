#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define CAPACITY 5

/*
 * ================================================================
 * THREAD-SAFE CIRCULAR BUFFER (Mutex-Protected)
 * ================================================================
 *
 * This version uses pthread_mutex_t to protect shared data
 * and allows multiple producers and consumers to safely access
 * the buffer concurrently.
 *
 * Key differences from basic version:
 * 1. Mutex lock around enqueue/dequeue operations
 * 2. No race conditions on count, head, tail
 * 3. Safe for multi-threaded environments (Linux, embedded OS)
 *
 * PICTORIAL:
 * Indices :   0    1    2    3    4
 * Data    : [11] [22] [33] [__] [__]
 *             ^               ^
 *            head            tail
 *
 * Mutex protects all concurrent access to: head, tail, count, data
 */

typedef struct {
    int data[CAPACITY];
    int head;
    int tail;
    int count;
    pthread_mutex_t lock;  /* Protects all above fields */
} ThreadSafeRingBuffer;

/* Initialize buffer with mutex */
void tsrb_init(ThreadSafeRingBuffer *rb) {
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
    pthread_mutex_init(&rb->lock, NULL);
}

/* Cleanup mutex */
void tsrb_destroy(ThreadSafeRingBuffer *rb) {
    pthread_mutex_destroy(&rb->lock);
}

bool tsrb_is_empty(ThreadSafeRingBuffer *rb) {
    pthread_mutex_lock(&rb->lock);
    bool empty = (rb->count == 0);
    pthread_mutex_unlock(&rb->lock);
    return empty;
}

bool tsrb_is_full(ThreadSafeRingBuffer *rb) {
    pthread_mutex_lock(&rb->lock);
    bool full = (rb->count == CAPACITY);
    pthread_mutex_unlock(&rb->lock);
    return full;
}

/*
 * Thread-safe enqueue
 * ==================
 * CRITICAL SECTION:
 *   1. Lock mutex
 *   2. Check if full
 *   3. Write to buffer
 *   4. Update tail and count
 *   5. Unlock mutex
 *
 * Multiple producers can queue up here, each gets exclusive
 * access to modify the buffer during its turn.
 */
bool tsrb_enqueue(ThreadSafeRingBuffer *rb, int value) {
    pthread_mutex_lock(&rb->lock);

    if (rb->count == CAPACITY) {
        pthread_mutex_unlock(&rb->lock);
        return false;  /* Buffer full, enqueue fails */
    }

    /* Safe to write: no other thread can modify data at this moment */
    rb->data[rb->tail] = value;
    rb->tail = (rb->tail + 1) % CAPACITY;
    rb->count++;

    pthread_mutex_unlock(&rb->lock);
    return true;
}

/*
 * Thread-safe dequeue
 * ===================
 * CRITICAL SECTION:
 *   1. Lock mutex
 *   2. Check if empty
 *   3. Read from buffer
 *   4. Update head and count
 *   5. Unlock mutex
 */
bool tsrb_dequeue(ThreadSafeRingBuffer *rb, int *out_value) {
    pthread_mutex_lock(&rb->lock);

    if (rb->count == 0) {
        pthread_mutex_unlock(&rb->lock);
        return false;  /* Buffer empty, dequeue fails */
    }

    /* Safe to read: no producer can change while we hold lock */
    *out_value = rb->data[rb->head];
    rb->head = (rb->head + 1) % CAPACITY;
    rb->count--;

    pthread_mutex_unlock(&rb->lock);
    return true;
}

int tsrb_count(ThreadSafeRingBuffer *rb) {
    pthread_mutex_lock(&rb->lock);
    int cnt = rb->count;
    pthread_mutex_unlock(&rb->lock);
    return cnt;
}

void tsrb_print(ThreadSafeRingBuffer *rb) {
    pthread_mutex_lock(&rb->lock);
    printf("[head=%d tail=%d count=%d] data: ", rb->head, rb->tail, rb->count);
    for (int i = 0; i < CAPACITY; i++) {
        printf("%d ", rb->data[i]);
    }
    printf("\n");
    pthread_mutex_unlock(&rb->lock);
}

/* Global buffer for threads to use */
ThreadSafeRingBuffer global_buffer;

/*
 * Producer thread function
 * ========================
 * Continuously enqueues values with delays.
 * Demonstrates multiple producers competing for lock.
 */
void *producer_thread(void *arg) {
    int producer_id = *(int *)arg;
    int value = producer_id * 100;

    for (int i = 0; i < 5; i++) {
        if (tsrb_enqueue(&global_buffer, value + i)) {
            printf("[Producer %d] Enqueued: %d (count now: %d)\n",
                   producer_id, value + i, tsrb_count(&global_buffer));
        } else {
            printf("[Producer %d] Buffer FULL, skipped %d\n", producer_id, value + i);
        }
        sleep(1);  /* Simulate work */
    }

    printf("[Producer %d] Done\n", producer_id);
    return NULL;
}

/*
 * Consumer thread function
 * ========================
 * Continuously dequeues values with delays.
 * Demonstrates multiple consumers competing for lock.
 */
void *consumer_thread(void *arg) {
    int consumer_id = *(int *)arg;
    int read_val;

    for (int i = 0; i < 5; i++) {
        if (tsrb_dequeue(&global_buffer, &read_val)) {
            printf("[Consumer %d] Dequeued: %d (count now: %d)\n",
                   consumer_id, read_val, tsrb_count(&global_buffer));
        } else {
            printf("[Consumer %d] Buffer EMPTY, nothing to read\n", consumer_id);
        }
        sleep(1);  /* Simulate work */
    }

    printf("[Consumer %d] Done\n", consumer_id);
    return NULL;
}

int main(void) {
    tsrb_init(&global_buffer);

    printf("Thread-Safe Ring Buffer (Mutex Protected)\n");
    printf("=========================================\n\n");

    printf("Initial state:\n");
    tsrb_print(&global_buffer);
    printf("\n");

    /* Create threads */
    pthread_t prod1, prod2, cons1, cons2;
    int p1_id = 1, p2_id = 2;
    int c1_id = 1, c2_id = 2;

    printf("Starting 2 producers and 2 consumers...\n\n");

    pthread_create(&prod1, NULL, producer_thread, &p1_id);
    pthread_create(&prod2, NULL, producer_thread, &p2_id);
    pthread_create(&cons1, NULL, consumer_thread, &c1_id);
    pthread_create(&cons2, NULL, consumer_thread, &c2_id);

    /* Wait for all threads */
    pthread_join(prod1, NULL);
    pthread_join(prod2, NULL);
    pthread_join(cons1, NULL);
    pthread_join(cons2, NULL);

    printf("\nAll threads done.\n");
    printf("Final state:\n");
    tsrb_print(&global_buffer);

    tsrb_destroy(&global_buffer);
    return 0;
}

/*
 * ================================================================
 * EXPLANATION: Why Mutex is Needed
 * ================================================================
 *
 * Without Mutex (UNSAFE):
 * =======================
 * Thread A: read count = 2
 * Thread B: read count = 2
 * Thread A: count = 3 (enqueued)
 * Thread B: count = 3 (overwrites A's increment)  ← BUG! Lost update
 *
 * With Mutex (SAFE):
 * ==================
 * Thread A: LOCK
 * Thread A: read count = 2
 * Thread A: count = 3
 * Thread A: UNLOCK
 *
 * Thread B: LOCK (had to wait for A)
 * Thread B: read count = 3
 * Thread B: count = 4
 * Thread B: UNLOCK
 *
 * Result: count correctly incremented both times.
 *
 * ================================================================
 * Compilation (Linux/Unix with pthreads)
 * ================================================================
 *
 *   gcc -pthread -o ringbuffer_mt 07_ring_buffer_mutex.c
 *   ./ringbuffer_mt
 *
 * Or with explicit linking:
 *   gcc -o ringbuffer_mt 07_ring_buffer_mutex.c -lpthread
 *
 * ================================================================
 * Performance Note
 * ================================================================
 *
 * Mutex is safe but not the fastest for ultra-high-frequency
 * single-producer-single-consumer scenarios. For that, consider
 * lock-free algorithms or atomic operations (separate file).
 */

/*
 * ================================================================
 * IS IT NON-BLOCKING NOW?
 * ================================================================
 *
 * MISCONCEPTION: "Mutex makes it non-blocking"  ← WRONG
 *
 * REALITY: It's still mostly non-blocking FOR THE CALLER, but:
 *
 * What "Non-Blocking" means:
 * ==========================
 * A function call returns IMMEDIATELY without waiting for a condition.
 *
 * Original simple ring buffer (without mutex):
 *   - enqueue() returns false IMMEDIATELY if buffer full
 *   - dequeue() returns false IMMEDIATELY if buffer empty
 *   - Never waits for space or data
 *   - ✓ Non-blocking
 *   - ✗ NOT thread-safe (race conditions)
 *
 * Mutex-protected ring buffer:
 *   - enqueue() tries to acquire lock (might wait briefly if lock held)
 *   - If lock acquired: returns false IMMEDIATELY if full, true if inserted
 *   - If buffer full or empty: returns IMMEDIATELY (doesn't wait for space/data)
 *   - ✓ Mostly non-blocking (returns immediately to caller)
 *   - ✓ THREAD-SAFE (mutex prevents corruption)
 *   - ✗ Brief blocking on lock contention (other thread holding lock)
 *
 * Contrast with BLOCKING operations:
 *   - Blocking enqueue: would WAIT until space is available
 *   - Blocking dequeue: would WAIT until data is available
 *   - This version never waits for data/space, just returns false
 *
 * So the mutex version adds:
 *   + Thread-safety (solves race conditions)
 *   - Minimal overhead (lock acquire/release)
 *   ~ Still non-blocking in the sense that caller gets response immediately
 *
 * ================================================================
 * WHAT IS THREAD-SAFE?
 * ================================================================
 *
 * Thread-safe means: the code works correctly even when multiple
 * threads access it concurrently without additional synchronization.
 *
 * Original simple buffer (NOT thread-safe):
 * ==========================================
 *
 * Code:
 *   bool enqueue(buf, val) {
 *       if (buf->count == CAPACITY) return false;
 *       buf->data[buf->tail] = val;     ← Thread A writes here
 *       buf->tail = (buf->tail + 1) % CAPACITY;
 *       buf->count++;                   ← Thread B increments here
 *       return true;
 *   }
 *
 * Race Condition Example:
 *   Time 1: Thread A checks: count==4 (not full, OK to write)
 *   Time 2: Thread B checks: count==4 (not full, OK to write)
 *   Time 3: Thread A writes data, increments count to 5
 *   Time 4: Thread B writes data, increments count to 5  ← WRONG! Should be 6
 *           One write is lost because both threads read count==4
 *
 * Corruption scenarios without mutex:
 *   - Lost updates: multiple threads increment count, some increments lost
 *   - Torn reads: read incomplete/partial value (multi-byte on some CPUs)
 *   - Memory ordering issues: operations reordered by CPU/compiler
 *   - Stale data: thread reads old cached value instead of latest
 *
 * Mutex-protected buffer (IS thread-safe):
 * ========================================
 *
 * Code:
 *   bool enqueue(buf, val) {
 *       pthread_mutex_lock(&buf->lock);     ← CRITICAL SECTION START
 *       if (buf->count == CAPACITY) {
 *           pthread_mutex_unlock(&buf->lock);
 *           return false;
 *       }
 *       buf->data[buf->tail] = val;
 *       buf->tail = (buf->tail + 1) % CAPACITY;
 *       buf->count++;
 *       pthread_mutex_unlock(&buf->lock);   ← CRITICAL SECTION END
 *       return true;
 *   }
 *
 * Why it's safe:
 *   - Mutex ensures ONLY ONE thread can execute critical section at a time
 *   - All operations inside mutex block appear atomic to other threads
 *   - No two threads can read count==4 and both increment
 *
 * Thread A Timeline:          Thread B Timeline:
 *   1. LOCK acquired            1. Tries LOCK (BLOCKED - A has it)
 *   2. Read count==4            2. Waits...
 *   3. Write data               3. Waits...
 *   4. Increment count=5        4. Waits...
 *   5. UNLOCK released          5. LOCK acquired (finally)
 *                               6. Read count==5 (A's write visible!)
 *                               7. Write data
 *                               8. Increment count=6 (correct!)
 *                               9. UNLOCK released
 *
 * Result: count==6 (both writes visible, correct behavior)
 *
 * ================================================================
 * HOW MUTEX HELPS WITH NON-BLOCKING + THREAD-SAFE
 * ================================================================
 *
 * Mutex is the bridge that makes this possible:
 *
 * 1. THREAD-SAFETY benefit of Mutex:
 *    Prevents race conditions by ensuring sequential access
 *    to shared data. All 5 scenarios above are eliminated.
 *
 * 2. MINIMAL IMPACT ON NON-BLOCKING:
 *    - Lock acquisition is very fast (microseconds)
 *    - Lock is held only during critical section (milliseconds max)
 *    - Caller still gets response immediately (true/false)
 *    - Not blocking for space/data (original non-blocking behavior preserved)
 *
 * 3. Trade-off:
 *    + Gains: thread-safe, correct under concurrency
 *    - Loses: slight performance hit from lock overhead
 *    ~ Still non-blocking semantically (no wait for data/space)
 *
 * When mutex is NOT the best choice:
 *    - Ultra-high frequency operations (millions/sec)
 *    - Single producer, single consumer (lock-free preferred)
 *    - Real-time systems with strict latency (use lock-free atomic ops)
 *
 * When mutex IS the best choice:
 *    - General multi-threaded apps
 *    - Mixed producer/consumer threads
 *    - Simplicity and correctness matter more than raw speed
 *    - Kernel drivers and system software
 *
 * ================================================================
 * SUMMARY TABLE
 * ================================================================
 *
 * | Aspect           | Original Simple | Mutex-Protected |
 * |------------------|-----------------|-----------------|
 * | Non-blocking     | ✓ YES           | ✓ YES (mostly)  |
 * | Thread-safe      | ✗ NO            | ✓ YES           |
 * | Speed            | ⚡ Fast         | 🟡 Slower       |
 * | Race conditions  | ✗ YES (unsafe)  | ✓ NO (safe)     |
 * | Multi-thread OK? | ✗ NO (corrupts) | ✓ YES (correct) |
 * | Easy to use?     | ✓ Simple        | ✓ Simple        |
 *
 * ================================================================
 */
