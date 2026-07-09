# Matrix Multiplication: Naive vs CPU-Optimized

Notes for CPU-optimization interview prep. Covers the naive baseline, the optimized
version, why each design choice was made, and a small worked example.

---

## 0. X vs Y: two loop orders, same result

Both versions compute `C = A x B` and produce identical results. The difference is
the **order they touch memory** and **how they accumulate**.

```c
/* X: cell-by-cell (naive) */
for (int row = 0; row < ROW_A; row++) {
    for (int col = 0; col < COL_B; col++) {
        int sum = 0;
        for (int k = 0; k < COL_A; k++) {
            sum += A[row][k] * B[k][col];   /* B walks DOWN a column */
        }
        C[row][col] = sum;                  /* write the cell once */
    }
}

/* Y: row-at-a-time (optimized) */
for (int row = 0; row < ROW_A; row++) {
    for (int k = 0; k < COL_A; k++) {
        int aValue = A[row][k];             /* reused across the row */
        for (int col = 0; col < COL_B; col++) {
            C[row][col] += aValue * B[k][col];  /* B row read left-to-right */
        }
    }
}
```

### Layman's picture
Think of A, B, C as spreadsheets. To fill a cell of C you need a **row of A** and a
**column of B**.
- **X (cell-by-cell):** finish C[0][0] completely, then C[0][1]... For each cell it
  slides along an A row and slides DOWN a B column.
- **Y (row-at-a-time):** take one A number and smear it across an entire C row, then
  the next A number adding on top. It only reads B ACROSS (left-to-right), never down.

### Memory layout: stride
A 2D array is stored as one long 1D line, row by row (row-major). `B[k][col]` lives at
address `k*COL_B + col`.
- Moving `col -> col+1` = next address (**stride 1**, neighbors).
- Moving `k -> k+1` = jump `COL_B` elements (**big stride**, far away).

So X's inner loop (varies `k`) column-walks B with big strides; Y's inner loop
(varies `col`) row-walks B with stride 1.

### Cache lines & hotness
RAM is slow, so the CPU pulls memory in 64-byte **cache lines** (~16 ints) into fast
cache. Data in cache is "hot."
- **Y:** reads B left-to-right -> one cache line serves ~16 multiplications. Sequential,
  prefetcher-friendly, near-zero misses.
- **X:** each `B[k][col]` is a row apart -> a fresh cache line almost every access; big
  matrices evict lines before reuse -> constant misses (RAM trips).

### Does Y access C every iteration? Yes, but it's fine
Y does `C[row][col] += ...`, reading+writing C in the inner loop, while X writes C once
per cell. But:
1. Y sweeps `col` sequentially, so that C row stays hot -> the `+=` hits cache, not RAM.
2. The compiler keeps the C-row chunk in cache/registers (SIMD) during the pass.
3. The win on B (no column walk) plus SIMD outweighs C's repeated touches.

### Is `sum` in a register or on the stack?
`sum` is a local auto variable, so conceptually it lives on the stack. But at `-O1`+ the
compiler does **register allocation**: hot locals with no address taken stay in CPU
registers; the stack slot is only used if it must "spill." `sum` is small, has no `&sum`,
and is updated in a tight loop -> ideal register candidate, written to C once at the end.
Check with `gcc -O3 -S` (you'll see it in `eax`, no memory store in the loop). Taking
`&sum` or marking it `volatile` would force it onto the stack and slow it down.

### Interview soundbite
> X writes C once but column-walks B (cache misses). Y touches C every iteration, but
> sequentially while hot, reuses A in a register, and reads B stride-1 -> enabling SIMD.
> The compiler keeps `sum`/`aValue` in registers via allocation; stack is only used on spill.

---

## 1. Naive version (`matmul_naive.c`)

### Why a `sum` variable instead of updating `C` inside the inner loop?

```c
int sum = 0;
for (int k = 0; k < COL_SIZE_A; k++) {
    sum += A[row * COL_SIZE_A + k] * B[k * COL_SIZE_B + col];
}
C[row * COL_SIZE_B + col] = sum;   // written once
```

- `sum` is a local variable, so the compiler keeps it in a **CPU register** — the
  fastest storage. The inner loop accumulates into that register.
- If we instead wrote `C[row*COL_SIZE_B+col] += A[...] * B[...]`, every iteration
  would **read C from memory, add, write C back** — a memory round-trip each step.
  Much slower, and without `restrict` the compiler can't prove `C` doesn't overlap
  `A`/`B`, so it cannot optimize.
- So `sum` is updated `COL_SIZE_A` times in a register and `C` is written **once**
  after the inner `k` loop finishes.

### Why `malloc` instead of static arrays?
- 512x512 ints = 1 MB per matrix; three of them = ~3 MB. The **stack** is usually
  ~1 MB, so static/local arrays would overflow and crash.
- `malloc` uses the **heap**, which holds large blocks, and sizes can be chosen at
  runtime. `malloc(rows*cols*sizeof(int))` reserves one contiguous block; we index it
  as `A[row*cols+col]`. `free()` returns it to avoid leaks.

### What is `restrict`?
A promise that the pointers don't overlap (no aliasing). It lets the compiler keep
values in registers and auto-vectorize (SIMD). Biggest impact in the optimized file.

### Line by line
```c
for (int row = 0; row < ROW_SIZE_A; row++)   // each output row
  for (int col = 0; col < COL_SIZE_B; col++) // each output col
    int sum = 0;                             // register accumulator
    for k: sum += A[row][k]*B[k][col];       // dot product
    C[row][col] = sum;                       // one write
```

---

## 2. Optimized version (`matmul_opt.c`)

### How the optimization is done
Same N^3 math, reordered so memory is read **sequentially** and reused while it's hot
in cache.

```c
for (colStart ...)              // pick a 64-col block (tiling)
  for (row ...)                 // pick a row of C
    cRow = &C[row*COL_SIZE_B];  // compute row start ONCE
    for (k ...)                 // walk the shared dimension
      aValue = A[row][k];       // hoist A once
      for (col ...) cRow[col] += aValue * bRow[col];  // stride-1
```

- Computing `cRow` in the **row loop** (not the inner loop) means we build the row
  pointer once per row; the inner loop just streams along it.

### Cache optimizations
1. **Stride-1 access** — inner `col` loop reads B and C left-to-right, fully using each cache line.
2. **Column tiling (`COL_BLOCK`)** — keeps the active slice in cache, reused across all `k`.
3. **Hoisting** — `aValue` lives in a register, no repeated reads.
4. **`restrict`** — enables SIMD; can be 2-4x on its own.

### `double` over `int`
No overflow on large sums, keeps decimals; `int` is smaller/faster/exact for small
values. `double` trades speed for safety/precision (`float` is the SIMD-fast middle ground).

---

## 3. Worked example

- A = 2x3, B = 3x2, C = 2x2

A = [1 2 3; 4 5 6], B = [7 8; 9 10; 11 12]

Flat memory: `A = [1,2,3, 4,5,6]`, `B = [7,8, 9,10, 11,12]`

### Naive (finish one cell using `sum`)
C[0][0] = 1*7 + 2*9 + 3*11 = 58. `sum`: 0 -> 7 -> 25 -> 58, then one write.
B read down a column: B[0]=7, B[2]=9, B[4]=11 (jumps of COL_SIZE_B) -> column walking.

### Optimized (one A value updates a whole row of C)
- row=0,k=0: aValue=1, Brow0=[7,8]   -> C[0]=[7,8]
- row=0,k=1: aValue=2, Brow1=[9,10]  -> C[0]=[25,28]
- row=0,k=2: aValue=3, Brow2=[11,12] -> C[0]=[58,64]

Same answer, but B is read left-to-right (stride-1, cache-friendly). C accumulates
with `+=`, which is why `calloc` zeroes it first.

| | Naive | Optimized |
|---|---|---|
| Order | finish one C cell | spread one A value across a C row |
| B access | down columns (jumps) | along rows (sequential) |
| Cache | misses | hits, SIMD-able |

The column-block loop limits how many C columns are updated at once so they stay in
cache for large matrices.
