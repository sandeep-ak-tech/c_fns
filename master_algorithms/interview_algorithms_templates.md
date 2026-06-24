# C Interview Algorithm Templates — Complete Reference

---

## How to learn and master (step-by-step)

1. **Build C foundations first (1 week)**
   - Arrays, strings, pointers, structs, dynamic memory
   - Recursion basics
   - Big-O analysis basics
   - Write clean helper functions and test on small cases

2. **Learn one pattern at a time (6-8 weeks)**
   - For each pattern:
     1. Learn the trigger words ("sorted array", "subarray", "shortest path", etc.)
     2. Memorize the template
     3. Solve only 8-12 problems from that single pattern
     4. Re-solve 2 days later from memory (no peeking)

3. **Weekly revision cycle**
   - Day 1-4: new patterns
   - Day 5: mixed set of previously learned patterns
   - Day 6: timed mock interview (2-3 questions)
   - Day 7: error-log review and template revision

4. **Keep an interview notebook**
   - Problem trigger / pattern chosen
   - Mistake made
   - Final template used

5. **Follow this progression order**
   - Arrays/Strings → Two Pointers → Sliding Window → Hashing/Prefix Sum → Binary Search
   - Stack/Queue → Linked List → Recursion/Backtracking
   - Trees → Graphs → Greedy → DP

---

## Pattern recognition cheat triggers

| If the problem says... | Reach for... |
|------------------------|--------------|
| "contiguous subarray / substring" | Sliding Window / Prefix Sum |
| "sorted array, find pair / triplet" | Two Pointers |
| "find first / last in sorted data" | Binary Search |
| "minimize the maximum / maximize the minimum" | Binary Search on Answer |
| "all combinations / permutations / subsets" | Backtracking |
| "next greater / smaller element" | Monotonic Stack |
| "shortest path in unweighted graph / grid" | BFS |
| "shortest path with weights (non-negative)" | Dijkstra |
| "connectivity / are X and Y connected" | Union-Find (DSU) |
| "prerequisites / ordering / DAG" | Topological Sort |
| "cycle / middle of linked list" | Fast & Slow Pointers |
| "overlapping choices, count ways / min / max" | Dynamic Programming |
| "two strings / grid / knapsack" | DP 2D |
| "top K / repeatedly get min or max" | Heap |
| "intervals / scheduling / fewest resources" | Greedy |
| "XOR tricks / power of two / bitmask subsets" | Bit Manipulation |
| "find pattern inside text fast" | KMP |

---

## 1. Two Pointers

**Pattern Name:** Two Pointers

**When to use:**
- Sorted array — find pair/triplet with given sum
- In-place partitioning, reversals, removing duplicates
- Comparing from both ends (palindrome check)

**Time Complexity:** O(n)

**Space Complexity:** O(1)

**Template Code:**
```c
// Two Pointers: generic search on sorted array
int twoPointerSearch(int *a, int n, int target) {
    int left = 0, right = n - 1;
    while (left < right) {
        int sum = a[left] + a[right];
        if (sum == target) return 1;      // found
        else if (sum < target) left++;    // need bigger
        else right--;                     // need smaller
    }
    return 0; // not found
}
```

**5 Practice Problems:**
1. Two Sum II — Input Array is Sorted (LeetCode 167)
2. Remove Duplicates from Sorted Array (LeetCode 26)
3. Container With Most Water (LeetCode 11)
4. 3Sum (LeetCode 15)
5. Trapping Rain Water (LeetCode 42)

---

## 2. Sliding Window (Fixed Size)

**Pattern Name:** Sliding Window — Fixed Size

**When to use:**
- Find max/min/average of contiguous subarray of size K
- Exactly K-length window computations

**Time Complexity:** O(n)

**Space Complexity:** O(1)

**Template Code:**
```c
// Sliding Window Fixed: max sum of subarray of size k
int maxSumFixed(int *a, int n, int k) {
    int windowSum = 0;
    for (int i = 0; i < k; i++) windowSum += a[i];
    int best = windowSum;
    for (int r = k; r < n; r++) {
        windowSum += a[r] - a[r - k]; // slide: add right, remove left
        if (windowSum > best) best = windowSum;
    }
    return best;
}
```

**5 Practice Problems:**
1. Maximum Average Subarray I (LeetCode 643)
2. Maximum Sum Subarray of Size K
3. Find All Anagrams in a String (LeetCode 438)
4. Permutation in String (LeetCode 567)
5. Grumpy Bookstore Owner (LeetCode 1052)

---

## 3. Sliding Window (Variable Size)

**Pattern Name:** Sliding Window — Variable Size

**When to use:**
- Smallest/longest subarray meeting a condition
- Substring with at most K distinct characters
- Any "minimum window" problem

**Time Complexity:** O(n)

**Space Complexity:** O(1) to O(k) depending on auxiliary data

**Template Code:**
```c
// Sliding Window Variable: shortest subarray with sum >= target
int minSubArrayLen(int target, int *a, int n) {
    int left = 0, sum = 0, minLen = n + 1;
    for (int right = 0; right < n; right++) {
        sum += a[right];
        while (sum >= target) {
            int len = right - left + 1;
            if (len < minLen) minLen = len;
            sum -= a[left++];
        }
    }
    return (minLen == n + 1) ? 0 : minLen;
}
```

**5 Practice Problems:**
1. Minimum Size Subarray Sum (LeetCode 209)
2. Longest Substring Without Repeating Characters (LeetCode 3)
3. Longest Repeating Character Replacement (LeetCode 424)
4. Minimum Window Substring (LeetCode 76)
5. Fruit Into Baskets (LeetCode 904)

---

## 4. Prefix Sum

**Pattern Name:** Prefix Sum

**When to use:**
- Multiple range sum queries on static array
- Subarray sum equals K (combine with hash map)
- Difference array for range updates

**Time Complexity:** O(n) build, O(1) per query

**Space Complexity:** O(n)

**Template Code:**
```c
// Prefix Sum: build and query range [l, r] inclusive
void buildPrefix(int *a, int n, int *pref) {
    pref[0] = 0;
    for (int i = 1; i <= n; i++)
        pref[i] = pref[i - 1] + a[i - 1];
}
int rangeSum(int *pref, int l, int r) {
    return pref[r + 1] - pref[l]; // sum a[l..r]
}
```

**5 Practice Problems:**
1. Range Sum Query — Immutable (LeetCode 303)
2. Subarray Sum Equals K (LeetCode 560)
3. Contiguous Array (LeetCode 525)
4. Product of Array Except Self (LeetCode 238)
5. Find Pivot Index (LeetCode 724)

---

## 5. Hashing / Frequency Counting

**Pattern Name:** Hashing / Frequency Map

**When to use:**
- O(1) average-time lookup needed
- Counting occurrences, finding duplicates, complement search
- Group elements by some key

**Time Complexity:** O(n)

**Space Complexity:** O(n)

**Template Code:**
```c
// Hashing: Two Sum using frequency array (small range)
// For large ranges, use a hash table library or manual hash
int twoSum(int *a, int n, int target) {
    int seen[200001] = {0}; // offset for negative values
    int OFFSET = 100000;
    for (int i = 0; i < n; i++) {
        int complement = target - a[i];
        if (complement + OFFSET >= 0 && complement + OFFSET <= 200000
            && seen[complement + OFFSET])
            return 1;
        seen[a[i] + OFFSET] = 1;
    }
    return 0;
}
```

**5 Practice Problems:**
1. Two Sum (LeetCode 1)
2. First Unique Character in a String (LeetCode 387)
3. Contains Duplicate (LeetCode 217)
4. Group Anagrams (LeetCode 49)
5. Longest Consecutive Sequence (LeetCode 128)

---

## 6. Binary Search (Standard)

**Pattern Name:** Binary Search

**When to use:**
- Sorted array lookup
- Find first/last occurrence (lower/upper bound)
- Search insert position

**Time Complexity:** O(log n)

**Space Complexity:** O(1)

**Template Code:**
```c
// Binary Search: lower bound (first index where a[i] >= x)
int lowerBound(int *a, int n, int x) {
    int lo = 0, hi = n; // search in [lo, hi)
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (a[mid] < x) lo = mid + 1;
        else hi = mid;
    }
    return lo;
}

// Upper bound (first index where a[i] > x)
int upperBound(int *a, int n, int x) {
    int lo = 0, hi = n;
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (a[mid] <= x) lo = mid + 1;
        else hi = mid;
    }
    return lo;
}
```

**5 Practice Problems:**
1. Binary Search (LeetCode 704)
2. Search Insert Position (LeetCode 35)
3. Find First and Last Position (LeetCode 34)
4. Find Peak Element (LeetCode 162)
5. Search in Rotated Sorted Array (LeetCode 33)

---

## 7. Binary Search on Answer

**Pattern Name:** Binary Search on Answer

**When to use:**
- Minimize the maximum or maximize the minimum
- A monotone feasibility function exists (if X works, all > X also work, or vice versa)
- "What is the minimum capacity / maximum distance such that..."

**Time Complexity:** O(n × log(search_space))

**Space Complexity:** O(1)

**Template Code:**
```c
// Binary Search on Answer: minimum capacity to ship in 'days' days
int feasible(int *weights, int n, int cap, int days) {
    int trips = 1, curLoad = 0;
    for (int i = 0; i < n; i++) {
        if (weights[i] > cap) return 0;
        if (curLoad + weights[i] > cap) {
            trips++;
            curLoad = 0;
        }
        curLoad += weights[i];
    }
    return trips <= days;
}
int shipWithinDays(int *weights, int n, int days) {
    int lo = 0, hi = 0;
    for (int i = 0; i < n; i++) {
        if (weights[i] > lo) lo = weights[i];
        hi += weights[i];
    }
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (feasible(weights, n, mid, days)) hi = mid;
        else lo = mid + 1;
    }
    return lo;
}
```

**5 Practice Problems:**
1. Capacity to Ship Packages Within D Days (LeetCode 1011)
2. Koko Eating Bananas (LeetCode 875)
3. Split Array Largest Sum (LeetCode 410)
4. Minimize Max Distance to Gas Station (LeetCode 774)
5. Magnetic Force Between Two Balls (LeetCode 1552)

---

## 8. Fast & Slow Pointers (Floyd's)

**Pattern Name:** Fast & Slow Pointers

**When to use:**
- Cycle detection in linked list or sequence
- Find middle of linked list
- Find duplicate in array (pigeonhole)

**Time Complexity:** O(n)

**Space Complexity:** O(1)

**Template Code:**
```c
typedef struct ListNode {
    int val;
    struct ListNode *next;
} ListNode;

// Detect cycle
int hasCycle(ListNode *head) {
    ListNode *slow = head, *fast = head;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) return 1;
    }
    return 0;
}

// Find middle node
ListNode* middleNode(ListNode *head) {
    ListNode *slow = head, *fast = head;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    return slow;
}
```

**5 Practice Problems:**
1. Linked List Cycle (LeetCode 141)
2. Linked List Cycle II — find start (LeetCode 142)
3. Middle of the Linked List (LeetCode 876)
4. Happy Number (LeetCode 202)
5. Find the Duplicate Number (LeetCode 287)

---

## 9. Monotonic Stack

**Pattern Name:** Monotonic Stack

**When to use:**
- Next greater / next smaller element
- Previous greater / previous smaller
- Histogram area, stock span, temperatures

**Time Complexity:** O(n)

**Space Complexity:** O(n)

**Template Code:**
```c
// Monotonic Stack: next greater element for each index
void nextGreaterElement(int *a, int n, int *result) {
    int stack[100000], top = -1;
    for (int i = n - 1; i >= 0; i--) {
        // Pop elements smaller or equal to current
        while (top >= 0 && stack[top] <= a[i])
            top--;
        result[i] = (top >= 0) ? stack[top] : -1;
        stack[++top] = a[i]; // push current
    }
}
```

**5 Practice Problems:**
1. Next Greater Element I (LeetCode 496)
2. Daily Temperatures (LeetCode 739)
3. Largest Rectangle in Histogram (LeetCode 84)
4. Stock Span Problem (LeetCode 901)
5. Asteroid Collision (LeetCode 735)

---

## 10. BFS (Breadth-First Search)

**Pattern Name:** BFS

**When to use:**
- Shortest path in unweighted graph / grid
- Level-order traversal of tree
- Multi-source expansion (rotting oranges)

**Time Complexity:** O(V + E)

**Space Complexity:** O(V)

**Template Code:**
```c
// BFS: shortest path in unweighted graph (adjacency matrix)
void bfs(int n, int adj[][100], int src, int *dist) {
    int q[10000], front = 0, back = 0;
    for (int i = 0; i < n; i++) dist[i] = -1;
    dist[src] = 0;
    q[back++] = src;
    while (front < back) {
        int u = q[front++];
        for (int v = 0; v < n; v++) {
            if (adj[u][v] && dist[v] == -1) {
                dist[v] = dist[u] + 1;
                q[back++] = v;
            }
        }
    }
}
```

**5 Practice Problems:**
1. Number of Islands (LeetCode 200) — grid BFS
2. Rotting Oranges (LeetCode 994)
3. Binary Tree Level Order Traversal (LeetCode 102)
4. Word Ladder (LeetCode 127)
5. Shortest Path in Binary Matrix (LeetCode 1091)

---

## 11. DFS (Depth-First Search)

**Pattern Name:** DFS

**When to use:**
- Graph/tree traversal, connectivity
- Detecting cycles, topological sort
- Flood fill, island counting
- Path existence queries

**Time Complexity:** O(V + E)

**Space Complexity:** O(V) (recursion stack)

**Template Code:**
```c
// DFS: recursive on adjacency matrix
void dfs(int u, int n, int adj[][100], int *visited) {
    visited[u] = 1;
    for (int v = 0; v < n; v++) {
        if (adj[u][v] && !visited[v])
            dfs(v, n, adj, visited);
    }
}
// Count connected components
int countComponents(int n, int adj[][100]) {
    int visited[100] = {0}, count = 0;
    for (int i = 0; i < n; i++) {
        if (!visited[i]) {
            dfs(i, n, adj, visited);
            count++;
        }
    }
    return count;
}
```

**5 Practice Problems:**
1. Number of Connected Components (LeetCode 323)
2. Flood Fill (LeetCode 733)
3. Clone Graph (LeetCode 133)
4. Course Schedule — cycle detection (LeetCode 207)
5. Surrounded Regions (LeetCode 130)

---

## 12. Backtracking

**Pattern Name:** Backtracking

**When to use:**
- Generate all valid combinations/permutations/subsets
- Constraint satisfaction (N-Queens, Sudoku)
- Decision tree exploration with pruning

**Time Complexity:** O(2^n) or O(n!) depending on problem

**Space Complexity:** O(n) recursion depth

**Template Code:**
```c
// Backtracking: generate all subsets
void backtrack(int *nums, int n, int idx, int *path, int pathLen) {
    // Process current subset (path[0..pathLen-1])
    // e.g., print it
    for (int i = idx; i < n; i++) {
        path[pathLen] = nums[i];          // choose
        backtrack(nums, n, i + 1, path, pathLen + 1); // explore
        // automatic undo (no explicit action needed here)
    }
}
// Usage: int path[100]; backtrack(nums, n, 0, path, 0);
```

**5 Practice Problems:**
1. Subsets (LeetCode 78)
2. Permutations (LeetCode 46)
3. Combination Sum (LeetCode 39)
4. N-Queens (LeetCode 51)
5. Letter Combinations of a Phone Number (LeetCode 17)

---

## 13. Merge Sort / Divide and Conquer

**Pattern Name:** Merge Sort / Divide and Conquer

**When to use:**
- Stable O(n log n) sorting
- Count inversions, reverse pairs
- Problems where merging sorted halves reveals answer

**Time Complexity:** O(n log n)

**Space Complexity:** O(n)

**Template Code:**
```c
// Merge Sort
void merge(int *a, int l, int m, int r, int *tmp) {
    int i = l, j = m + 1, k = l;
    while (i <= m && j <= r)
        tmp[k++] = (a[i] <= a[j]) ? a[i++] : a[j++];
    while (i <= m) tmp[k++] = a[i++];
    while (j <= r) tmp[k++] = a[j++];
    for (int x = l; x <= r; x++) a[x] = tmp[x];
}
void mergeSort(int *a, int l, int r, int *tmp) {
    if (l >= r) return;
    int m = l + (r - l) / 2;
    mergeSort(a, l, m, tmp);
    mergeSort(a, m + 1, r, tmp);
    merge(a, l, m, r, tmp);
}
// Call: int tmp[N]; mergeSort(arr, 0, n-1, tmp);
```

**5 Practice Problems:**
1. Sort an Array (LeetCode 912)
2. Count of Smaller Numbers After Self (LeetCode 315)
3. Reverse Pairs (LeetCode 493)
4. Merge Intervals (LeetCode 56) — uses sort
5. Maximum Subarray (LeetCode 53) — divide & conquer approach

---

## 14. Heap / Priority Queue

**Pattern Name:** Heap (Min-Heap / Max-Heap)

**When to use:**
- Repeatedly extract min or max
- Top-K elements
- Merge K sorted streams
- Dijkstra's shortest path

**Time Complexity:** O(n log n) for n insertions; O(log n) per push/pop

**Space Complexity:** O(n)

**Template Code:**
```c
// Min-Heap (array-based)
int heap[100001];
int heapSize = 0;

void heapPush(int x) {
    int i = heapSize++;
    heap[i] = x;
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (heap[parent] <= heap[i]) break;
        int tmp = heap[parent]; heap[parent] = heap[i]; heap[i] = tmp;
        i = parent;
    }
}
int heapPop() {
    int top = heap[0];
    heap[0] = heap[--heapSize];
    int i = 0;
    while (1) {
        int l = 2*i+1, r = 2*i+2, smallest = i;
        if (l < heapSize && heap[l] < heap[smallest]) smallest = l;
        if (r < heapSize && heap[r] < heap[smallest]) smallest = r;
        if (smallest == i) break;
        int tmp = heap[i]; heap[i] = heap[smallest]; heap[smallest] = tmp;
        i = smallest;
    }
    return top;
}
```

**5 Practice Problems:**
1. Kth Largest Element in an Array (LeetCode 215)
2. Top K Frequent Elements (LeetCode 347)
3. Merge K Sorted Lists (LeetCode 23)
4. Find Median from Data Stream (LeetCode 295)
5. Last Stone Weight (LeetCode 1046)

---

## 15. Greedy

**Pattern Name:** Greedy

**When to use:**
- Local optimal choice leads to global optimal (provably)
- Interval scheduling, activity selection
- Minimum resources / maximum non-overlapping tasks
- Jump games, gas station

**Time Complexity:** O(n log n) typically (due to sorting), O(n) for some variants

**Space Complexity:** O(1) to O(n)

**Template Code:**
```c
// Greedy: maximum non-overlapping intervals (activity selection)
// Assumes intervals sorted by end time
typedef struct { int start, end; } Interval;

int cmpByEnd(const void *a, const void *b) {
    return ((Interval*)a)->end - ((Interval*)b)->end;
}
int maxNonOverlapping(Interval *intervals, int n) {
    qsort(intervals, n, sizeof(Interval), cmpByEnd);
    int count = 0, lastEnd = -2147483648;
    for (int i = 0; i < n; i++) {
        if (intervals[i].start >= lastEnd) {
            count++;
            lastEnd = intervals[i].end;
        }
    }
    return count;
}
```

**5 Practice Problems:**
1. Non-overlapping Intervals (LeetCode 435)
2. Jump Game (LeetCode 55)
3. Jump Game II (LeetCode 45)
4. Gas Station (LeetCode 134)
5. Meeting Rooms II (LeetCode 253)

---

## 16. Dynamic Programming — 1D

**Pattern Name:** Dynamic Programming (1D)

**When to use:**
- Overlapping subproblems + optimal substructure
- "Count ways", "min/max cost", "can we reach end?"
- Problem can be broken into f(i) depending on f(i-1), f(i-2) etc.

**Time Complexity:** O(n)

**Space Complexity:** O(1) optimized, O(n) table

**Template Code:**
```c
// DP 1D: House Robber — max sum, no two adjacent
int rob(int *nums, int n) {
    if (n == 0) return 0;
    if (n == 1) return nums[0];
    int prev2 = 0, prev1 = nums[0];
    for (int i = 1; i < n; i++) {
        int cur = (prev2 + nums[i] > prev1) ? prev2 + nums[i] : prev1;
        prev2 = prev1;
        prev1 = cur;
    }
    return prev1;
}
```

**5 Practice Problems:**
1. Climbing Stairs (LeetCode 70)
2. House Robber (LeetCode 198)
3. Min Cost Climbing Stairs (LeetCode 746)
4. Decode Ways (LeetCode 91)
5. Coin Change (LeetCode 322)

---

## 17. Dynamic Programming — 2D

**Pattern Name:** Dynamic Programming (2D)

**When to use:**
- Two sequences/dimensions: LCS, edit distance, grid paths
- Knapsack variants (items × capacity)
- String matching with wildcards

**Time Complexity:** O(m × n)

**Space Complexity:** O(m × n) or O(n) with row optimization

**Template Code:**
```c
// DP 2D: Longest Common Subsequence
int lcs(char *a, int m, char *b, int n) {
    int dp[1001][1001]; // adjust size as needed
    for (int i = 0; i <= m; i++) dp[i][0] = 0;
    for (int j = 0; j <= n; j++) dp[0][j] = 0;
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (a[i-1] == b[j-1])
                dp[i][j] = dp[i-1][j-1] + 1;
            else
                dp[i][j] = (dp[i-1][j] > dp[i][j-1]) ? dp[i-1][j] : dp[i][j-1];
        }
    }
    return dp[m][n];
}
```

**5 Practice Problems:**
1. Longest Common Subsequence (LeetCode 1143)
2. Edit Distance (LeetCode 72)
3. 0/1 Knapsack (classic)
4. Unique Paths (LeetCode 62)
5. Minimum Path Sum (LeetCode 64)

---

## 18. Union-Find (Disjoint Set Union)

**Pattern Name:** Union-Find / DSU

**When to use:**
- Dynamic connectivity queries
- "Are X and Y connected?" after unions
- Cycle detection in undirected graph
- Kruskal's MST

**Time Complexity:** O(α(n)) ≈ O(1) amortized per operation

**Space Complexity:** O(n)

**Template Code:**
```c
// Union-Find with path compression + union by rank
int parent[100001], rank_arr[100001];

void init(int n) {
    for (int i = 0; i < n; i++) { parent[i] = i; rank_arr[i] = 0; }
}
int find(int x) {
    if (parent[x] != x) parent[x] = find(parent[x]); // path compression
    return parent[x];
}
void unite(int a, int b) {
    a = find(a); b = find(b);
    if (a == b) return;
    if (rank_arr[a] < rank_arr[b]) parent[a] = b;
    else if (rank_arr[a] > rank_arr[b]) parent[b] = a;
    else { parent[b] = a; rank_arr[a]++; }
}
int connected(int a, int b) { return find(a) == find(b); }
```

**5 Practice Problems:**
1. Number of Provinces (LeetCode 547)
2. Redundant Connection (LeetCode 684)
3. Graph Valid Tree (LeetCode 261)
4. Accounts Merge (LeetCode 721)
5. Most Stones Removed (LeetCode 947)

---

## 19. Dijkstra's Algorithm (Shortest Path)

**Pattern Name:** Dijkstra's Shortest Path

**When to use:**
- Weighted graph with non-negative edge weights
- Single-source shortest path
- Network delay, cheapest route problems

**Time Complexity:** O((V + E) log V) with min-heap

**Space Complexity:** O(V + E)

**Template Code:**
```c
// Dijkstra using adjacency list + manual min-heap
#define INF 2147483647
#define MAXN 10001

int dist[MAXN];
// Simplified: using min-extraction loop (O(V^2) version for clarity)
void dijkstra(int n, int adj[][MAXN], int src) {
    int visited[MAXN] = {0};
    for (int i = 0; i < n; i++) dist[i] = INF;
    dist[src] = 0;
    for (int iter = 0; iter < n; iter++) {
        // Find unvisited node with smallest dist
        int u = -1;
        for (int i = 0; i < n; i++)
            if (!visited[i] && (u == -1 || dist[i] < dist[u])) u = i;
        if (u == -1 || dist[u] == INF) break;
        visited[u] = 1;
        for (int v = 0; v < n; v++) {
            if (adj[u][v] > 0 && dist[u] + adj[u][v] < dist[v])
                dist[v] = dist[u] + adj[u][v];
        }
    }
}
```

**5 Practice Problems:**
1. Network Delay Time (LeetCode 743)
2. Path With Minimum Effort (LeetCode 1631)
3. Cheapest Flights Within K Stops (LeetCode 787)
4. Swim in Rising Water (LeetCode 778)
5. Shortest Path in Weighted Graph (generic)

---

## 20. Topological Sort (Kahn's BFS)

**Pattern Name:** Topological Sort

**When to use:**
- DAG (directed acyclic graph) ordering
- Task scheduling with prerequisites
- Detect cycle in directed graph (if sort fails)

**Time Complexity:** O(V + E)

**Space Complexity:** O(V + E)

**Template Code:**
```c
// Topological Sort: Kahn's algorithm (BFS-based)
int topoSort(int n, int adj[][100], int *result) {
    int indegree[100] = {0};
    for (int u = 0; u < n; u++)
        for (int v = 0; v < n; v++)
            if (adj[u][v]) indegree[v]++;

    int q[100], front = 0, back = 0, idx = 0;
    for (int i = 0; i < n; i++)
        if (indegree[i] == 0) q[back++] = i;

    while (front < back) {
        int u = q[front++];
        result[idx++] = u;
        for (int v = 0; v < n; v++) {
            if (adj[u][v]) {
                indegree[v]--;
                if (indegree[v] == 0) q[back++] = v;
            }
        }
    }
    return idx == n; // returns 0 if cycle exists
}
```

**5 Practice Problems:**
1. Course Schedule (LeetCode 207)
2. Course Schedule II (LeetCode 210)
3. Alien Dictionary (LeetCode 269)
4. Parallel Courses (LeetCode 1136)
5. Sequence Reconstruction (LeetCode 444)

---

## 21. Bit Manipulation

**Pattern Name:** Bit Manipulation

**When to use:**
- Single number / find missing / find duplicate (XOR tricks)
- Power of two checks
- Subset enumeration via bitmasks
- Compact state representation

**Time Complexity:** O(n) or O(1) per operation

**Space Complexity:** O(1)

**Template Code:**
```c
// Bit Manipulation: common operations
int isPowerOfTwo(int x) {
    return x > 0 && (x & (x - 1)) == 0;
}
int countSetBits(int x) {
    int count = 0;
    while (x) { x &= (x - 1); count++; }
    return count;
}
int singleNumber(int *a, int n) {
    int result = 0;
    for (int i = 0; i < n; i++) result ^= a[i];
    return result;  // all duplicates cancel; lone element remains
}
// Enumerate all subsets of set with bitmask
void allSubsets(int *a, int n) {
    for (int mask = 0; mask < (1 << n); mask++) {
        for (int i = 0; i < n; i++)
            if (mask & (1 << i)) { /* a[i] is in this subset */ }
    }
}
```

**5 Practice Problems:**
1. Single Number (LeetCode 136)
2. Number of 1 Bits (LeetCode 191)
3. Missing Number (LeetCode 268)
4. Reverse Bits (LeetCode 190)
5. Subsets using bitmask (LeetCode 78 alternate)

---

## 22. KMP String Matching

**Pattern Name:** KMP (Knuth-Morris-Pratt)

**When to use:**
- Find pattern in text in O(n + m)
- Repeated substring detection
- Avoid brute-force O(n × m) matching

**Time Complexity:** O(n + m)

**Space Complexity:** O(m) for LPS array

**Template Code:**
```c
// KMP: build LPS (longest prefix suffix) + search
void buildLPS(char *pattern, int m, int *lps) {
    int len = 0;
    lps[0] = 0;
    int i = 1;
    while (i < m) {
        if (pattern[i] == pattern[len]) {
            lps[i++] = ++len;
        } else if (len) {
            len = lps[len - 1];
        } else {
            lps[i++] = 0;
        }
    }
}
int kmpSearch(char *text, char *pattern) {
    int n = strlen(text), m = strlen(pattern);
    int lps[10000];
    buildLPS(pattern, m, lps);
    int i = 0, j = 0;
    while (i < n) {
        if (text[i] == pattern[j]) { i++; j++; }
        if (j == m) return i - j; // found at index i-j
        else if (i < n && text[i] != pattern[j]) {
            if (j) j = lps[j - 1];
            else i++;
        }
    }
    return -1; // not found
}
```

**5 Practice Problems:**
1. Implement strStr() (LeetCode 28)
2. Repeated Substring Pattern (LeetCode 459)
3. Shortest Palindrome (LeetCode 214)
4. Count occurrences of pattern in text
5. Longest Happy Prefix (LeetCode 1392)

---

## 23. Tree Traversals (Inorder, Preorder, Postorder)

**Pattern Name:** Binary Tree Traversals

**When to use:**
- BST validation (inorder gives sorted order)
- Serialize/deserialize trees
- Build tree from traversals
- Any recursive tree problem

**Time Complexity:** O(n)

**Space Complexity:** O(h) where h = height (O(n) worst case)

**Template Code:**
```c
typedef struct TreeNode {
    int val;
    struct TreeNode *left, *right;
} TreeNode;

// Inorder (Left, Root, Right) — sorted for BST
void inorder(TreeNode *root, int *res, int *idx) {
    if (!root) return;
    inorder(root->left, res, idx);
    res[(*idx)++] = root->val;
    inorder(root->right, res, idx);
}
// Preorder (Root, Left, Right)
void preorder(TreeNode *root, int *res, int *idx) {
    if (!root) return;
    res[(*idx)++] = root->val;
    preorder(root->left, res, idx);
    preorder(root->right, res, idx);
}
// Max depth
int maxDepth(TreeNode *root) {
    if (!root) return 0;
    int l = maxDepth(root->left), r = maxDepth(root->right);
    return 1 + (l > r ? l : r);
}
```

**5 Practice Problems:**
1. Binary Tree Inorder Traversal (LeetCode 94)
2. Maximum Depth of Binary Tree (LeetCode 104)
3. Validate Binary Search Tree (LeetCode 98)
4. Symmetric Tree (LeetCode 101)
5. Lowest Common Ancestor (LeetCode 236)

---

## Quick Reference Table

| # | Pattern | Trigger Words | Complexity |
|---|---------|--------------|------------|
| 1 | Two Pointers | sorted, pair, partition | O(n) |
| 2 | Sliding Window Fixed | subarray size K | O(n) |
| 3 | Sliding Window Variable | shortest/longest subarray | O(n) |
| 4 | Prefix Sum | range sum, subarray sum | O(n) + O(1) |
| 5 | Hashing | duplicates, complement, frequency | O(n) |
| 6 | Binary Search | sorted, first/last | O(log n) |
| 7 | BS on Answer | minimize max, feasibility | O(n log S) |
| 8 | Fast/Slow Pointer | cycle, middle, linked list | O(n) |
| 9 | Monotonic Stack | next greater/smaller | O(n) |
| 10 | BFS | shortest unweighted, levels | O(V+E) |
| 11 | DFS | connectivity, paths, tree | O(V+E) |
| 12 | Backtracking | all combos, permutations | O(2^n / n!) |
| 13 | Merge Sort | inversions, stable sort | O(n log n) |
| 14 | Heap | top-K, merge streams | O(n log n) |
| 15 | Greedy | intervals, scheduling | O(n log n) |
| 16 | DP 1D | ways, min cost, sequence | O(n) |
| 17 | DP 2D | two strings, grid, knapsack | O(m×n) |
| 18 | Union-Find | connectivity, MST | O(α(n)) |
| 19 | Dijkstra | weighted shortest path | O((V+E)logV) |
| 20 | Topological Sort | prerequisites, DAG order | O(V+E) |
| 21 | Bit Manipulation | XOR, masks, subsets | O(n)/O(1) |
| 22 | KMP | pattern match, substring | O(n+m) |
| 23 | Tree Traversal | BST, depth, ancestor | O(n) |

---

## Blank Template (copy and fill for new patterns you discover)

```
Pattern Name:
When to use:
Time Complexity:
Space Complexity:
Template Code:

```c
// Your template here
```

5 Practice Problems:
1.
2.
3.
4.
5.
```
