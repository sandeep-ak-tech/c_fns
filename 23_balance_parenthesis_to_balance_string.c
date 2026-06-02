/*
"Remove the minimum number of parentheses to make the string valid (balanced)"
Example: (()())() → valid
()) → remove one ) → ()
((( → remove two ( → ( or remove all → ""
This is asked at Google, Amazon, Microsoft, Bloomberg, Adobe — and it's a must-know for any coding interview.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char* removeInvalidParentheses(char* s) {
    int n = strlen(s);
    char* result = (char*)malloc(n + 1);
    int stack[n];
    int top = -1;
    int valid[n];  // 1 if char is valid, 0 if to be removed
    memset(valid, 1, sizeof(valid));

    // Phase 1: Mark invalid ')' using stack
    for (int i = 0; i < n; i++) {
        if (s[i] == '(') {
            stack[++top] = i;
        } else if (s[i] == ')') {
            if (top >= 0) {
                top--;  // match found
            } else {
                valid[i] = 0;  // extra ')'
            }
        }
    }

    // Phase 2: Mark unmatched '(' at the end of stack
    while (top >= 0) {
        valid[stack[top--]] = 0;
    }

    // Phase 3: Build result
    int j = 0;
    for (int i = 0; i < n; i++) {
        if (valid[i]) {
            result[j++] = s[i];
        }
    }
    result[j] = '\0';

    return result;
}

// Test function
int main() {
    char* tests[] = {
        "(()()",
        "())",
        "(((",
        "(())()",
        "())()(",
        "lee(t(c)o)de)"
    };

    int n = sizeof(tests)/sizeof(tests[0]);
    for (int i = 0; i < n; i++) {
        char* res = removeInvalidParentheses(tests[i]);
        printf("Input : %s\n", tests[i]);
        printf("Output: %s\n\n", res);
        free(res);
    }
    return 0;
}