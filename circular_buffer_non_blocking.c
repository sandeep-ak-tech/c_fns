#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
/*
write_idx = “where I will put the next new byte”
read_idx  = “where I will take the next old byte from”
*/

// Size must be power of 2! (e.g., 64, 128, 256, 512, 1024, 4096)
#define BUFFER_SIZE 256

typedef struct {
    uint8_t  data[BUFFER_SIZE];
    uint32_t write_idx;   // Producer writes here
    uint32_t read_idx;    // Consumer reads here
} CircularBuffer;

// Initialize buffer
void cb_init(CircularBuffer *cb) {
    cb->write_idx = 0;
    cb->read_idx  = 0;
}

// Returns number of bytes available to read
static inline uint32_t cb_available_to_read(const CircularBuffer *cb) {
    return cb->write_idx - cb->read_idx;
}

// Returns number of bytes free to write
static inline uint32_t cb_available_to_write(const CircularBuffer *cb) {
    return BUFFER_SIZE - (cb->write_idx - cb->read_idx);
}

// Write data (non-blocking)
bool cb_write(CircularBuffer *cb, const uint8_t *data, uint32_t len) {
    uint32_t space = cb_available_to_write(cb);
    if (len > space) {
        return false;  // Not enough space
    }

    uint32_t mask = BUFFER_SIZE - 1;
    uint32_t idx = cb->write_idx & mask;

    if (idx + len > BUFFER_SIZE) {
        // Wrap around
        uint32_t part1 = BUFFER_SIZE - idx;
        memcpy(&cb->data[idx], data, part1);
        memcpy(&cb->data[0], data + part1, len - part1);
    } else {
        memcpy(&cb->data[idx], data, len);
    }

    // Use memory barrier or volatile if compiler is too smart
    __sync_synchronize();  // Full barrier (optional on x86)
    cb->write_idx += len;
    return true;
}

// Read data (non-blocking)
bool cb_read(CircularBuffer *cb, uint8_t *dest, uint32_t len) {
    uint32_t avail = cb_available_to_read(cb);
    if (len > avail) {
        return false;  // Not enough data
    }

    uint32_t mask = BUFFER_SIZE - 1;
    uint32_t idx = cb->read_idx & mask;

    if (idx + len > BUFFER_SIZE) {
        uint32_t part1 = BUFFER_SIZE - idx;
        memcpy(dest, &cb->data[idx], part1);
        memcpy(dest + part1, &cb->data[0], len - part1);
    } else {
        memcpy(dest, &cb->data[idx], len);
    }

    __sync_synchronize();
    cb->read_idx += len;
    return true;
}

// Example usage
int main() {
    CircularBuffer cb;
    cb_init(&cb);

    const char *msg = "Hello Circular Buffer!";
    cb_write(&cb, (uint8_t*)msg, strlen(msg) + 1);

    uint8_t buffer[100] = {0};
    if (cb_read(&cb, buffer, strlen(msg) + 1)) {
        printf("Read: %s\n", buffer);
    }

    printf("Space free: %u bytes\n", cb_available_to_write(&cb));
    return 0;
}



Even Faster Version (Using volatile + no memcpy)
For ultra-high performance (e.g. audio, DMA):
Ctypedef struct {
    volatile uint32_t write_idx;
    volatile uint32_t read_idx;
    uint8_t data[BUFFER_SIZE];
} CircularBufferFast;

// Write single byte (used in ISR)
static inline bool cb_write_byte(CircularBufferFast *cb, uint8_t byte) {
    uint32_t next = (cb->write_idx + 1) & (BUFFER_SIZE - 1);
    if (next == cb->read_idx) return false;  // Full
    cb->data[cb->write_idx] = byte;
    cb->write_idx = next;
    return true;
}

// Read single byte
static inline bool cb_read_byte(CircularBufferFast *cb, uint8_t *byte) {
    if (cb->read_idx == cb->write_idx) return false;  // Empty
    *byte = cb->data[cb->read_idx];
    cb->read_idx = (cb->read_idx + 1) & (BUFFER_SIZE - 1);
    return true;
}