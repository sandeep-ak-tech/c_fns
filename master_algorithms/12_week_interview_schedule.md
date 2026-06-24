# 12-Week C Coding Interview Schedule (Day-by-Day)

A structured plan to take you from beginner to interview-ready with **~180 problems** solved in order (easy → medium → hard). Each week targets one or two patterns, includes a **C-only template**, and ends with a mixed-revision day.

> **Daily routine (every study day):**
> 1. Re-write the week's template from memory (5 min)
> 2. Solve the day's problems (in order)
> 3. Log each problem: trigger → pattern → mistake → fix
> 4. Re-solve any problem you failed, 2 days later
>
> **Suggested daily time:** 1.5-2.5 hours. Each "Day" = one sitting; skip/merge as your pace allows.

---

## Week 1 — C Foundations + Arrays/Strings

**Goal:** Solidify pointers, arrays, strings, and Big-O before patterns begin.

| Day | Problems (in order) | Difficulty |
|-----|---------------------|-----------|
| 1 | Running Sum of 1d Array (1480); Build Array from Permutation (1920) | Easy |
| 2 | Concatenation of Array (1929); Find Numbers with Even Digits (1295) | Easy |
| 3 | Richest Customer Wealth (1672); Shuffle the Array (1470) | Easy |
| 4 | Maximum Subarray (53, Kadane); Best Time to Buy/Sell Stock (121) | Easy |
| 5 | Reverse String (344); Valid Palindrome (125) | Easy |
| 6 | Move Zeroes (283); Plus One (66) | Easy |
| 7 | **Revision** — re-solve 4 problems you found hardest | Mixed |

**Template — Kadane's Max Subarray (foundation DP/array scan):**
```c
int maxSubArray(int *a, int n) {
    int best = a[0], cur = a[0];
    for (int i = 1; i < n; i++) {
        cur = (a[i] > cur + a[i]) ? a[i] : cur + a[i];
        if (cur > best) best = cur;
    }
    return best;
}
```

---

## Week 2 — Two Pointers

**Goal:** Master converging/diverging pointers on sorted data and in-place ops.

| Day | Problems (in order) | Difficulty |
|-----|---------------------|-----------|
| 1 | Two Sum II — Sorted (167); Squares of a Sorted Array (977) | Easy |
| 2 | Remove Duplicates from Sorted Array (26); Remove Element (27) | Easy |
| 3 | Merge Sorted Array (88); Is Subsequence (392) | Easy |
| 4 | 3Sum (15) | Medium |
| 5 | 3Sum Closest (16); 4Sum (18) | Medium |
| 6 | Container With Most Water (11); Sort Colors (75) | Medium |
| 7 | **Revision** — re-solve 3Sum + 2 weak spots | Mixed |

**Template — Two Pointers on sorted array:**
```c
int twoPointer(int *a, int n, int target) {
    int l = 0, r = n - 1;
    while (l < r) {
        int s = a[l] + a[r];
        if (s == target) return 1;
        else if (s < target) l++;
        else r--;
    }
    return 0;
}
```

---

## Week 3 — Sliding Window

**Goal:** Fixed and variable windows for subarray/substring problems.

| Day | Problems (in order) | Difficulty |
|-----|---------------------|-----------|
| 1 | Maximum Average Subarray I (643); Max Consecutive Ones III (1004) | Easy/Med |
| 2 | Longest Substring Without Repeating Chars (3) | Medium |
| 3 | Minimum Size Subarray Sum (209); Fruit Into Baskets (904) | Medium |
| 4 | Longest Repeating Character Replacement (424) | Medium |
| 5 | Permutation in String (567); Find All Anagrams (438) | Medium |
| 6 | Minimum Window Substring (76) | Hard |
| 7 | **Revision** — re-solve LC 3 + LC 76 from memory | Mixed |

**Template — Variable sliding window:**
```c
int minSubArrayLen(int target, int *a, int n) {
    int left = 0, sum = 0, best = n + 1;
    for (int right = 0; right < n; right++) {
        sum += a[right];
        while (sum >= target) {
            int len = right - left + 1;
            if (len < best) best = len;
            sum -= a[left++];
        }
    }
    return (best == n + 1) ? 0 : best;
}
```

---

## Week 4 — Hashing + Prefix Sum

**Goal:** O(1) lookups, frequency counting, and range/subarray sums.

| Day | Problems (in order) | Difficulty |
|-----|---------------------|-----------|
| 1 | Two Sum (1); Contains Duplicate (217) | Easy |
| 2 | Valid Anagram (242); First Unique Character (387) | Easy |
| 3 | Group Anagrams (49); Top K Frequent Elements (347) | Medium |
| 4 | Range Sum Query Immutable (303); Find Pivot Index (724) | Easy |
| 5 | Subarray Sum Equals K (560) | Medium |
| 6 | Contiguous Array (525); Product of Array Except Self (238) | Medium |
| 7 | **Revision** — re-solve LC 560 + LC 49 | Mixed |

**Template — Prefix sum + hash idea (subarray sum = K, value-range hash):**
```c
// Prefix sum build
void buildPrefix(int *a, int n, int *pref) {
    pref[0] = 0;
    for (int i = 1; i <= n; i++) pref[i] = pref[i - 1] + a[i - 1];
}
int rangeSum(int *pref, int l, int r) { return pref[r + 1] - pref[l]; }
```

---

## Week 5 — Binary Search (Standard + On Answer)

**Goal:** Index search, lower/upper bound, and feasibility-based search.

| Day | Problems (in order) | Difficulty |
|-----|---------------------|-----------|
| 1 | Binary Search (704); Search Insert Position (35) | Easy |
| 2 | First and Last Position (34); Find Peak Element (162) | Medium |
| 3 | Search in Rotated Sorted Array (33); Find Min in Rotated (153) | Medium |
| 4 | Koko Eating Bananas (875) | Medium |
| 5 | Capacity to Ship Packages in D Days (1011) | Medium |
| 6 | Split Array Largest Sum (410) | Hard |
| 7 | **Revision** — re-solve LC 33 + LC 875 | Mixed |

**Template — Binary search on answer:**
```c
int feasible(int *a, int n, int limit, int budget); // problem-specific
int bsOnAnswer(int *a, int n, int lo, int hi, int budget) {
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (feasible(a, n, mid, budget)) hi = mid;
        else lo = mid + 1;
    }
    return lo;
}
```

---

## Week 6 — Linked Lists + Fast/Slow Pointers

**Goal:** Pointer manipulation, cycle detection, reversal.

| Day | Problems (in order) | Difficulty |
|-----|---------------------|-----------|
| 1 | Reverse Linked List (206); Middle of the Linked List (876) | Easy |
| 2 | Merge Two Sorted Lists (21); Linked List Cycle (141) | Easy |
| 3 | Linked List Cycle II (142); Happy Number (202) | Medium |
| 4 | Remove Nth Node From End (19); Palindrome Linked List (234) | Medium |
| 5 | Reorder List (143); Find the Duplicate Number (287) | Medium |
| 6 | Add Two Numbers (2); Copy List with Random Pointer (138) | Medium |
| 7 | **Revision** — re-solve LC 206 + LC 142 | Mixed |

**Template — Fast & slow pointer (cycle + middle):**
```c
typedef struct ListNode { int val; struct ListNode *next; } ListNode;

int hasCycle(ListNode *head) {
    ListNode *slow = head, *fast = head;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) return 1;
    }
    return 0;
}
ListNode* reverseList(ListNode *head) {
    ListNode *prev = NULL;
    while (head) {
        ListNode *next = head->next;
        head->next = prev;
        prev = head;
        head = next;
    }
    return prev;
}
```

---

## Week 7 — Stacks, Queues & Monotonic Stack

**Goal:** LIFO/FIFO logic and nearest-greater/smaller scans.

| Day | Problems (in order) | Difficulty |
|-----|---------------------|-----------|
| 1 | Valid Parentheses (20); Min Stack (155) | Easy |
| 2 | Implement Queue using Stacks (232); Baseball Game (682) | Easy |
| 3 | Next Greater Element I (496); Daily Temperatures (739) | Medium |
| 4 | Evaluate Reverse Polish Notation (150); Asteroid Collision (735) | Medium |
| 5 | Largest Rectangle in Histogram (84) | Hard |
| 6 | Trapping Rain Water (42) | Hard |
| 7 | **Revision** — re-solve LC 739 + LC 84 | Mixed |

**Template — Monotonic stack (next greater element):**
```c
void nextGreater(int *a, int n, int *res) {
    int st[100000], top = -1;
    for (int i = n - 1; i >= 0; i--) {
        while (top >= 0 && st[top] <= a[i]) top--;
        res[i] = (top >= 0) ? st[top] : -1;
        st[++top] = a[i];
    }
}
```

---

## Week 8 — Trees (Traversal + BST)

**Goal:** Recursion on trees, BFS levels, BST properties.

| Day | Problems (in order) | Difficulty |
|-----|---------------------|-----------|
| 1 | Maximum Depth of Binary Tree (104); Invert Binary Tree (226) | Easy |
| 2 | Same Tree (100); Symmetric Tree (101) | Easy |
| 3 | Binary Tree Inorder Traversal (94); Level Order Traversal (102) | Medium |
| 4 | Validate Binary Search Tree (98); Kth Smallest in BST (230) | Medium |
| 5 | Lowest Common Ancestor of BST (235); LCA of Binary Tree (236) | Medium |
| 6 | Construct Tree from Preorder+Inorder (105); Right Side View (199) | Medium |
| 7 | **Revision** — re-solve LC 98 + LC 102 | Mixed |

**Template — Tree traversals + depth:**
```c
typedef struct TreeNode { int val; struct TreeNode *left, *right; } TreeNode;

void inorder(TreeNode *root, int *res, int *idx) {
    if (!root) return;
    inorder(root->left, res, idx);
    res[(*idx)++] = root->val;
    inorder(root->right, res, idx);
}
int maxDepth(TreeNode *root) {
    if (!root) return 0;
    int l = maxDepth(root->left), r = maxDepth(root->right);
    return 1 + (l > r ? l : r);
}
```

---

## Week 9 — Graphs (BFS, DFS, Union-Find, Topo Sort)

**Goal:** Traversal, connectivity, ordering with prerequisites.

| Day | Problems (in order) | Difficulty |
|-----|---------------------|-----------|
| 1 | Number of Islands (200); Flood Fill (733) | Medium |
| 2 | Max Area of Island (695); Rotting Oranges (994) | Medium |
| 3 | Clone Graph (133); Number of Connected Components (323) | Medium |
| 4 | Course Schedule (207); Course Schedule II (210) | Medium |
| 5 | Number of Provinces (547); Redundant Connection (684) | Medium |
| 6 | Pacific Atlantic Water Flow (417); Surrounded Regions (130) | Medium |
| 7 | **Revision** — re-solve LC 200 + LC 207 | Mixed |

**Template — BFS grid + Union-Find:**
```c
// Union-Find
int parent[100001], rnk[100001];
int find(int x) { return parent[x] == x ? x : (parent[x] = find(parent[x])); }
void unite(int a, int b) {
    a = find(a); b = find(b);
    if (a == b) return;
    if (rnk[a] < rnk[b]) parent[a] = b;
    else if (rnk[a] > rnk[b]) parent[b] = a;
    else { parent[b] = a; rnk[a]++; }
}
```

---

## Week 10 — Backtracking + Recursion

**Goal:** Generate all valid configurations with pruning.

| Day | Problems (in order) | Difficulty |
|-----|---------------------|-----------|
| 1 | Subsets (78); Subsets II (90) | Medium |
| 2 | Permutations (46); Permutations II (47) | Medium |
| 3 | Combination Sum (39); Combination Sum II (40) | Medium |
| 4 | Letter Combinations of a Phone Number (17); Generate Parentheses (22) | Medium |
| 5 | Word Search (79); Palindrome Partitioning (131) | Medium |
| 6 | N-Queens (51) | Hard |
| 7 | **Revision** — re-solve LC 39 + LC 51 | Mixed |

**Template — Backtracking skeleton:**
```c
void backtrack(int *nums, int n, int start, int *path, int len) {
    // record path[0..len-1] as one valid result
    for (int i = start; i < n; i++) {
        path[len] = nums[i];            // choose
        backtrack(nums, n, i + 1, path, len + 1); // explore
        // undo is implicit (overwrite on next iteration)
    }
}
```

---

## Week 11 — Dynamic Programming (1D + 2D)

**Goal:** Recognize overlapping subproblems; build bottom-up tables.

| Day | Problems (in order) | Difficulty |
|-----|---------------------|-----------|
| 1 | Climbing Stairs (70); Min Cost Climbing Stairs (746) | Easy |
| 2 | House Robber (198); House Robber II (213) | Medium |
| 3 | Coin Change (322); Coin Change II (518) | Medium |
| 4 | Longest Increasing Subsequence (300); Word Break (139) | Medium |
| 5 | Unique Paths (62); Minimum Path Sum (64) | Medium |
| 6 | Longest Common Subsequence (1143); Edit Distance (72) | Medium/Hard |
| 7 | **Revision** — re-solve LC 322 + LC 1143 | Mixed |

**Template — DP 1D + 2D shells:**
```c
// 1D: House Robber
int rob(int *a, int n) {
    int prev2 = 0, prev1 = 0;
    for (int i = 0; i < n; i++) {
        int cur = (prev2 + a[i] > prev1) ? prev2 + a[i] : prev1;
        prev2 = prev1; prev1 = cur;
    }
    return prev1;
}
// 2D: LCS
int lcs(char *a, int m, char *b, int n) {
    static int dp[1001][1001];
    for (int i = 1; i <= m; i++)
        for (int j = 1; j <= n; j++)
            dp[i][j] = (a[i-1] == b[j-1]) ? dp[i-1][j-1] + 1
                     : (dp[i-1][j] > dp[i][j-1] ? dp[i-1][j] : dp[i][j-1]);
    return dp[m][n];
}
```

---

## Week 12 — Heap, Greedy, Bits + Mock Interviews

**Goal:** Final patterns + full timed mock interviews.

| Day | Problems (in order) | Difficulty |
|-----|---------------------|-----------|
| 1 | Kth Largest Element in an Array (215); Last Stone Weight (1046) | Medium |
| 2 | Merge K Sorted Lists (23); Find Median from Data Stream (295) | Hard |
| 3 | Non-overlapping Intervals (435); Jump Game (55); Gas Station (134) | Medium |
| 4 | Single Number (136); Number of 1 Bits (191); Missing Number (268) | Easy |
| 5 | **Mock Interview 1** — 2 random mediums, 45 min, timed | Mixed |
| 6 | **Mock Interview 2** — 1 medium + 1 hard, 60 min, timed | Mixed |
| 7 | **Final review** — redo every problem you ever failed | Mixed |

**Template — Min-heap (priority queue):**
```c
int heap[100001], heapSize = 0;
void push(int x) {
    int i = heapSize++; heap[i] = x;
    while (i > 0) {
        int p = (i - 1) / 2;
        if (heap[p] <= heap[i]) break;
        int t = heap[p]; heap[p] = heap[i]; heap[i] = t; i = p;
    }
}
int pop() {
    int top = heap[0]; heap[0] = heap[--heapSize]; int i = 0;
    while (1) {
        int l = 2*i+1, r = 2*i+2, m = i;
        if (l < heapSize && heap[l] < heap[m]) m = l;
        if (r < heapSize && heap[r] < heap[m]) m = r;
        if (m == i) break;
        int t = heap[i]; heap[i] = heap[m]; heap[m] = t; i = m;
    }
    return top;
}
```

---

## Problem Count Summary

| Weeks | Theme | Approx. Problems |
|-------|-------|------------------|
| 1 | Foundations / Arrays | 12 |
| 2 | Two Pointers | 12 |
| 3 | Sliding Window | 11 |
| 4 | Hashing + Prefix Sum | 12 |
| 5 | Binary Search | 11 |
| 6 | Linked Lists | 12 |
| 7 | Stacks / Monotonic | 11 |
| 8 | Trees | 12 |
| 9 | Graphs | 12 |
| 10 | Backtracking | 11 |
| 11 | Dynamic Programming | 12 |
| 12 | Heap/Greedy/Bits + Mocks | 14 |
| **Total** | | **~150-180** |

---

## How to Use This Plan

1. **Don't skip the revision day** — it's where retention happens.
2. **Time-box each problem:** 25 min easy, 40 min medium, 60 min hard. If stuck, read the editorial, understand it, then re-solve from scratch the next day.
3. **After each week**, re-write that week's template from a blank file with no reference.
4. **Track your failures** in a separate log; Week 12 Day 7 is dedicated to clearing it.
5. Pair this with the pattern reference in [interview_algorithms_templates.md](interview_algorithms_templates.md).

You've got this — 12 weeks of consistent daily practice will make these patterns automatic.
