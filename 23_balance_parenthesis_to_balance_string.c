/*
 * PROBLEM: Remove Minimum Parentheses to Make String Valid
 * ==========================================================
 * Given a string with '(' ')' and letters, remove the minimum number of
 * parentheses so that the resulting string is valid (balanced).
 *
 * A string is valid when:
 *   - Every '(' has a matching ')' to its right.
 *   - Every ')' has a matching '(' to its left.
 *   - Matched pairs are properly nested.
 *
 * EXAMPLES:
 *   Input : "lee(t(c)o)de)"   Output: "lee(t(c)o)de"
 *   Input : "a)b(c)d"         Output: "ab(c)d"
 *   Input : "))(("             Output: ""
 *   Input : "(a(b(c)d)"        Output: "a(b(c)d)"
 *
 * APPROACH: Two-Pass with a removal marker array
 * ================================================
 * Pass 1 — Left to Right  → find unmatched ')'
 *   - Keep a counter `open` for unmatched '(' seen so far.
 *   - When we see '(':  open++
 *   - When we see ')':
 *       if open > 0  → valid pair found, open--
 *       if open == 0 → no '(' to pair with, mark this ')' for removal
 *
 * Pass 2 — Right to Left → find unmatched '('
 *   - Keep a counter `close` for unmatched ')' seen so far.
 *   - When we see ')':  close++
 *   - When we see '(':
 *       if close > 0  → valid pair found, close--
 *       if close == 0 → no ')' to pair with, mark this '(' for removal
 *
 * Pass 3 — Build output string skipping all marked positions.
 *
 * TIME  COMPLEXITY : O(n)
 * SPACE COMPLEXITY : O(n)  (removal marker array + output buffer)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LEN 1024

/*
 * removeMinParentheses
 * --------------------
 * Parameters:
 *   s      - input string (null-terminated)
 *   result - output buffer (must be at least strlen(s)+1 bytes)
 *
 * Fills `result` with the valid string after minimum removals.
 */
void removeMinParentheses(const char *s, char *result) {
    int n = (int)strlen(s);

    /* Mark array: 1 = this character should be removed, 0 = keep */
    int *remove = (int *)calloc((size_t)n, sizeof(int));
    if (!remove) {
        result[0] = '\0';
        return;
    }

    /* -------- Pass 1: Left → Right, mark unmatched ')' -------- */
    int open = 0;
    for (int i = 0; i < n; i++) {
        if (s[i] == '(') {
            open++;
        } else if (s[i] == ')') {
            if (open > 0) {
                open--;          /* matched with a previous '(' */
            } else {
                remove[i] = 1;   /* no '(' available → mark for removal */
            }
        }
    }

    /* -------- Pass 2: Right → Left, mark unmatched '(' -------- */
    int close = 0;
    for (int i = n - 1; i >= 0; i--) {
        if (s[i] == ')') {
            close++;
        } else if (s[i] == '(') {
            if (close > 0) {
                close--;         /* matched with a later ')' */
            } else {
                remove[i] = 1;   /* no ')' available → mark for removal */
            }
        }
    }

    /* -------- Pass 3: Build result, skip removed positions -------- */
    int idx = 0;
    for (int i = 0; i < n; i++) {
        if (!remove[i]) {
            result[idx++] = s[i];
        }
    }
    result[idx] = '\0';

    free(remove);
}

/* Helper: count total removals (for display) */
int countRemovals(const char *original, const char *valid) {
    return (int)strlen(original) - (int)strlen(valid);
}

/* Helper: verify the result is truly balanced (for sanity check) */
int isValid(const char *s) {
    int balance = 0;
    for (int i = 0; s[i] != '\0'; i++) {
        if (s[i] == '(') balance++;
        else if (s[i] == ')') {
            if (balance == 0) return 0;
            balance--;
        }
    }
    return balance == 0;
}

int main(void) {
    const char *testCases[] = {
        "lee(t(c)o)de)",
        "a)b(c)d",
        ")))(((", 
        "(a(b(c)d)",
        "()",
        "(((",
        ")))",
        "a(b)c)d(e(f)g",
        ""
    };

    int total = (int)(sizeof(testCases) / sizeof(testCases[0]));
    char result[MAX_LEN];

    printf("Remove Minimum Parentheses to Make String Valid\n");
    printf("================================================\n\n");

    for (int i = 0; i < total; i++) {
        removeMinParentheses(testCases[i], result);

        printf("Input    : \"%s\"\n", testCases[i]);
        printf("Output   : \"%s\"\n", result);
        printf("Removals : %d\n", countRemovals(testCases[i], result));
        printf("Valid?   : %s\n", isValid(result) ? "YES" : "NO");
        printf("---\n");
    }

    return 0;
}

/*
 * WHY TWO PASSES? (Left-to-Right AND Right-to-Left)
 * ==================================================
 * A single pass cannot catch both types of invalid parentheses.
 * Each pass targets a different kind of mismatch.
 *
 * Pass 1 — Left to Right  →  catches unmatched ')'
 * --------------------------------------------------
 * When scanning left-to-right, you see ')' before knowing what came before it.
 * If there is no open '(' in the running count, this ')' has nothing to close
 * — it is invalid and gets marked immediately.
 *
 *   Input:  a ) b ( c ) d
 *               ↑
 *   open=0, found ')' with no '(' before it  →  REMOVE
 *
 * Pass 1 CANNOT catch unmatched '(' because when you first see '(',
 * you don't yet know if a ')' will appear later to close it.
 *
 * Pass 2 — Right to Left  →  catches unmatched '('
 * --------------------------------------------------
 * Scanning right-to-left, you now treat '(' like a closing bracket and
 * ')' like an opener. If there is no ')' available in the count when
 * you hit a '(', that '(' will never be closed — it is invalid.
 *
 *   Input:  ( a ( b c ) d
 *                     ↑
 *   close=0, found '(' with no ')' after it  →  REMOVE
 *
 * Why one pass is NOT enough
 * --------------------------
 * Consider "(((" :
 *
 *   Left-to-right scan:
 *     i=0 '('  →  open=1  (might be matched later, don't remove yet)
 *     i=1 '('  →  open=2  (same)
 *     i=2 '('  →  open=3  (same)
 *
 *   After pass 1: nothing is marked. But all three '(' are unmatched!
 *
 * You can only know a '(' is unmatched AFTER finishing the right scan
 * and confirming no ')' follows it. The right-to-left pass does exactly that.
 *
 * Summary
 * -------
 * Pass 1  (→ Left  to Right) : tracks open '(' count,  marks excess ')' with no prior '('
 * Pass 2  (← Right to Left)  : tracks close ')' count, marks excess '(' with no later ')'
 *
 * Together they guarantee MINIMUM removals — only characters that truly
 * have no valid partner are marked. No valid pair is ever touched.
 */
