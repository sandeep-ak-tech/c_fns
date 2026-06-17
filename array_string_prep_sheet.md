# Array and String Prep Sheet (C Programming)

Date: 2026-06-16

## 0) C Program: Longest Substring Without Repeating Characters

```c
#include <stdio.h>
#include <string.h>

#define MAX_CHARS 256

int lengthOfLongestSubstring(const char *s) {
    if (s == NULL || s[0] == '\0') {
        return 0;
    }

    int lastIndex[MAX_CHARS];
    memset(lastIndex, -1, sizeof(lastIndex));

    int maxLength = 0;
    int windowStart = 0;

    for (int i = 0; s[i] != '\0'; i++) {
        unsigned char ch = (unsigned char)s[i];

        if (lastIndex[ch] >= windowStart) {
            windowStart = lastIndex[ch] + 1;
        }

        lastIndex[ch] = i;

        int currentLength = i - windowStart + 1;
        if (currentLength > maxLength) {
            maxLength = currentLength;
        }
    }

    return maxLength;
}

void getLongestSubstring(const char *s, char *result) {
    if (s == NULL || s[0] == '\0') {
        result[0] = '\0';
        return;
    }

    int lastIndex[MAX_CHARS];
    memset(lastIndex, -1, sizeof(lastIndex));

    int maxLength = 0;
    int maxStart = 0;
    int windowStart = 0;

    for (int i = 0; s[i] != '\0'; i++) {
        unsigned char ch = (unsigned char)s[i];

        if (lastIndex[ch] >= windowStart) {
            windowStart = lastIndex[ch] + 1;
        }

        lastIndex[ch] = i;

        int currentLength = i - windowStart + 1;
        if (currentLength > maxLength) {
            maxLength = currentLength;
            maxStart = windowStart;
        }
    }

    strncpy(result, s + maxStart, (size_t)maxLength);
    result[maxLength] = '\0';
}

int main(void) {
    const char *tests[] = {
        "abcabcbb",
        "bbbbb",
        "pwwkew",
        "au",
        "dvdf",
        ""
    };
    int total = (int)(sizeof(tests) / sizeof(tests[0]));

    char substring[1024];

    printf("Longest Substring Without Repeating Characters\n");
    printf("===============================================\n\n");

    for (int i = 0; i < total; i++) {
        int len = lengthOfLongestSubstring(tests[i]);
        getLongestSubstring(tests[i], substring);

        printf("Input: \"%s\"\n", tests[i]);
        printf("Length: %d\n", len);
        printf("Substring: \"%s\"\n\n", substring);
    }

    return 0;
}
```

### Build and run

```bash
gcc longest_substring.c -o longest_substring
./longest_substring
```

## 1) Longest Substring Without Repeating Characters (C) - Step by Step

### Problem
Given a string, find the length of the longest substring without repeating characters.

Example:
- Input: abcabcbb
- Output: 3
- Valid longest substring: abc

### Core Idea
Use a sliding window and track the last seen index of each character.

### Variables used in the solution
- `lastIndex[256]`: stores last seen index for each ASCII character, initialized to -1
- `windowStart`: left boundary of current window
- `i`: right boundary while scanning the string
- `maxLength`: best answer seen so far

### Walkthrough on abcabcbb
1. Start: `windowStart = 0`, `maxLength = 0`, all `lastIndex = -1`
2. i=0, char='a': not seen in window, window = a, max=1
3. i=1, char='b': window = ab, max=2
4. i=2, char='c': window = abc, max=3
5. i=3, char='a': repeated inside window (last a at 0), move `windowStart` to `0+1=1`, window becomes bca
6. i=4, char='b': repeated inside current window, move `windowStart` to `1+1=2`, window becomes cab
7. i=5, char='c': repeated, move `windowStart` to `2+1=3`, window becomes abc
8. i=6, char='b': repeated, move `windowStart` to `4+1=5`, window becomes cb
9. i=7, char='b': repeated, move `windowStart` to `6+1=7`, window becomes b
10. Final `maxLength = 3`

### Why this is O(n)
Each character index is processed by the right pointer once. The left pointer only moves forward, never backward. Total pointer moves are linear.

Time complexity: O(n)
Space complexity: O(1) for ASCII (fixed 256-size array)

---

## 2) Sliding Window Approach - In Depth

### What is sliding window?
A technique for contiguous segments (subarray/substring) where you maintain a range [left..right] and move it across input while preserving a rule.

### Two major types
1. Fixed-size window
- Window size k is constant.
- Example: max sum subarray of size k.

2. Variable-size window
- Window grows and shrinks based on validity.
- Example: longest substring without repeat.

### Generic variable window pattern
1. Expand right pointer by one element.
2. Update data structure (frequency map/last index/sum).
3. While window invalid, shrink from left and update data structure.
4. When valid, update answer.
5. Continue until end.

### Validity examples
- No repeated characters
- At most K distinct characters
- Sum <= target (or >= target)
- Contains all chars of pattern

### Signals that sliding window may fit
- Problem says substring or subarray
- Contiguous elements are required
- Need min/max length/count under a condition
- Asked for efficient alternative to O(n^2)

### When sliding window is not ideal
- Non-contiguous subsequence problems
- Global graph traversal/path search
- Heavy backtracking/combinatorics without contiguous structure

---

## 3) Similar C Questions where Sliding Window is useful

1. Longest substring with at most K distinct characters
2. Minimum window substring
3. Max sum subarray of size K
4. Find all anagrams of a pattern in a string
5. Longest repeating character replacement
6. Max consecutive ones with at most K flips
7. Subarrays with product less than K
8. Count substrings with exactly K distinct characters
9. Permutation in string
10. Smallest subarray with sum >= target

---

## 4) Other Common Techniques Like Sliding Window

## A) Two Pointers
Use two indices moving under a relation (often sorted arrays or palindrome checks).
- Typical: pair sum in sorted array, remove duplicates, reverse in-place
- Time: often O(n)

## B) Prefix Sum
Precompute running totals to answer range-sum quickly.
- Typical: subarray sum equals K, range sum queries
- Time: O(n), with hashmap often for counting

## C) Hashing / Frequency Array
Track counts or existence for O(1) average lookup.
- Typical: anagrams, duplicates, first non-repeating char

## D) Binary Search
Use on sorted data or monotonic answer space.
- Typical: find value, lower/upper bound, search on answer
- Time: O(log n) per search

## E) Dynamic Programming (DP)
Break into overlapping subproblems and reuse results.
- Typical: LIS, edit distance, knapsack
- Time/space depends on state design

## F) Greedy
Make best local choice with proof of global optimality.
- Typical: interval scheduling, jump game variants

## G) Stack / Monotonic Stack
Track nearest greater/smaller efficiently.
- Typical: next greater element, largest rectangle in histogram

## H) BFS/DFS
Traversal for graphs/trees/grids.
- BFS for shortest path in unweighted graphs
- DFS for connectivity/cycle checks

---

## 5) How to identify which technique to use

Use this quick decision flow:
1. Is data contiguous and asking min/max over substring/subarray?
- Try sliding window.

2. Is it sorted or monotonic?
- Try binary search or two pointers.

3. Is it range sum/count query style?
- Try prefix sum (+ hashmap).

4. Is it frequency/anagram/duplicate-heavy?
- Try hashmap/frequency array.

5. Are there repeated subproblems with choices?
- Try DP.

6. Is it path/traversal over nodes/cells?
- Try BFS/DFS.

7. Need nearest greater/smaller pattern?
- Try monotonic stack.

---

## 6) How to evaluate a String question in C

### Step 1: Decode the question
- Input type: char array/string?
- Output: length/index/substr/count/boolean?
- Constraints: length, charset (ASCII/lowercase/Unicode), memory limits

### Step 2: Check structure
- Substring (contiguous) vs subsequence (non-contiguous)
- Exact match vs at most / at least constraints
- Single query vs multiple queries

### Step 3: Pick baseline and optimize
- Start with brute force mentally
- Estimate complexity against constraints
- Move to efficient pattern (window/hash/prefix/etc.)

### Step 4: C-specific implementation concerns
- Null checks (`s == NULL`)
- Empty string handling (`s[0] == '\0'`)
- Use `unsigned char` for safe indexing in 0..255
- Initialize arrays carefully (`memset(arr, -1, sizeof(arr))`)
- Avoid buffer overflow for extracted substrings
- Manage memory for dynamically allocated outputs

### Step 5: Test with edge cases
- Empty string
- One character
- All same characters
- All unique characters
- Repeated boundary chars
- Special characters and spaces

### Step 6: Validate complexity
- Time target usually O(n) or O(n log n) for large n
- Space target should be justified (constant, charset-based, or O(n))

---

## 7) Reusable Sliding Window Template (C-like pseudocode)

```c
int solve(char *s) {
    if (s == NULL) return 0;

    int left = 0;
    int best = 0;

    // Data structure depends on problem:
    // freq[256], lastIndex[256], currentSum, distinctCount, etc.

    for (int right = 0; s[right] != '\0'; right++) {
        // 1) include s[right]

        // 2) while window invalid: remove s[left], left++

        // 3) update answer using (right - left + 1)
    }

    return best;
}
```

---

## 8) Practical interview tips (Array + String in C)

1. Clarify constraints before coding.
2. State brute force first, then optimize.
3. Explain chosen pattern in 2 lines before implementation.
4. Keep variable names meaningful (`left`, `right`, `maxLen`, `freq`).
5. Mention complexity clearly after coding.
6. Run one dry-run on sample input verbally.
7. Cover edge cases explicitly.
8. Avoid overengineering.

---

## 9) Fast revision checklist

- Can I detect contiguous-window problems quickly?
- Can I write fixed and variable window patterns from memory?
- Can I switch between hashmap and array frequency?
- Can I defend O(n) vs O(n^2) trade-off?
- Can I handle C string edge cases safely?

If yes, you are interview-ready for most array/string medium-level problems.
