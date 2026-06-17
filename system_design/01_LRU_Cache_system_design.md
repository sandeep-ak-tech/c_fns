# LRU Cache - System Design Explanation

## What is LRU Cache?

**LRU (Least Recently Used) Cache** is a data structure that stores a limited number of key-value pairs and automatically removes the **least recently used** item when the cache reaches its maximum capacity.

### Core Principle:
- When an item is accessed (via `get` or `put`), it becomes the **Most Recently Used (MRU)**.
- When the cache is full and a new item needs to be added, the **Least Recently Used (LRU)** item is evicted.

### Real-world Use Cases:
- Web browsers (caching web pages/images)
- Database buffer pools
- Operating Systems (page replacement)
- Content Delivery Networks (CDNs)
- CPU caches

---

## Design Requirements

- **Capacity**: Fixed maximum size
- **`get(key)`**: Return value if key exists, else -1. Move accessed item to MRU.
- **`put(key, value)`**: Insert or update key-value pair. If full, evict LRU first.
- Both operations should be **O(1)** time complexity.

---

## Optimal Data Structure Choice

| Data Structure          | Purpose                              | Why? |
|-------------------------|--------------------------------------|------|
| **Hash Table**          | Fast key lookup                      | O(1) average access |
| **Doubly Linked List**  | Maintain recency order (MRU ↔ LRU)   | O(1) node removal & insertion |

**Why Doubly Linked List?**
- Allows O(1) removal of any node if we have direct reference.
- Easy to move nodes to the head (MRU position).

---

## Architecture

```
Most Recently Used (MRU)                    Least Recently Used (LRU)
         ↓                                           ↓
   +------+     +------+     +------+     +------+     +------+
   | Head | <-> | Node | <-> | Node | <-> | Node | <-> | Tail |
   +------+     +------+     +------+     +------+     +------+
```

- **Head** → Most Recently Used
- **Tail** → Least Recently Used
- On every `get/put`, move the node to Head.

---

## Key Operations

1. **get(key)**:
   - Lookup in HashMap
   - If found: Move to MRU position → Return value
   - Else: Return -1

2. **put(key, value)**:
   - If key exists: Update value + Move to MRU
   - Else:
     - If cache full: Remove LRU (Tail) + Remove from HashMap
     - Create new node
     - Add to MRU (Head)
     - Add to HashMap

---

## Time & Space Complexity

- **Time**: `get()` and `put()` → **O(1)**
- **Space**: **O(capacity)**

---

## Trade-offs & Variations

- **LFU (Least Frequently Used)**: Evicts based on usage count
- **Thread Safety**: Add mutex locks for concurrent access
- **Serialization**: Support saving/loading cache state
- **Fixed vs Dynamic Capacity**

This design is widely used in system design interviews and real-world applications.
