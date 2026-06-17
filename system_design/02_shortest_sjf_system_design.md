# Shortest Job First (SJF) Cache - System Design

## What is Shortest Job First (SJF) Cache?

**Shortest Job First (SJF)** is a cache replacement policy that evicts the item with the **smallest size** (or shortest estimated processing time) when the cache reaches its capacity.

It is inspired by the Shortest Job First scheduling algorithm in Operating Systems, adapted for caching.

### Core Principle
> When the cache is full and a new item needs to be added, **remove the smallest item(s)** first to free up space.

---

## Why Use SJF Cache?

- Maximizes **space efficiency** by preferring to keep larger items if they are important.
- Good for scenarios where item sizes vary significantly.
- Useful in web caching, CDNs, database buffers, and memory-constrained embedded systems.

### Advantages
- High **byte-hit ratio** (better utilization of cache space).
- Prevents small items from occupying too much space unnecessarily.
- Predictable eviction behavior when sizes are known.

### Disadvantages
- Ignores **recency** and **frequency** of access (unlike LRU/LFU).
- Can cause starvation of large items if small items keep coming.
- Requires knowing the size of each item upfront.

---

## Comparison with Other Policies

| Policy       | Eviction Criteria              | Strength                     | Weakness                     |
|--------------|--------------------------------|------------------------------|------------------------------|
| **LRU**      | Least Recently Used            | Temporal locality            | Large items pollute cache    |
| **LFU**      | Least Frequently Used          | Frequency matters            | Complex aging mechanism      |
| **FIFO**     | First In First Out             | Simple                       | No access pattern awareness  |
| **SJF**      | Smallest Size / Shortest Job   | Space efficiency             | Ignores usage patterns       |

---

## Design Architecture

### Key Components:
1. **Hash Table** - O(1) lookup by key
2. **Min-Heap (Priority Queue)** - To quickly find and remove the smallest item
3. **Node Structure** - Stores key, value, size

### Operations:
- **`get(key)`**: Return value if exists (O(1) or O(log N))
- **`put(key, value, size)`**: Insert/Update. If full, evict smallest item(s) first.

### Time Complexity:
- Average: **O(log N)** per operation due to heap
- Space: **O(capacity)**

---

## Use Cases
- Web servers caching files of varying sizes
- Embedded systems with strict memory limits
- Content Delivery Networks (CDNs)
- Database page caches

This policy is particularly effective when **item size** is a strong indicator of value or cost.
