#include <stdio.h>

/*
 * Count the number of set bits in odd and even positions of a 32-bit number.
 * Positions are counted from 0 (least significant bit).
 * Even positions: bit indices 0, 2, 4, ..., 30
 * Odd  positions: bit indices 1, 3, 5, ..., 31
 */
void count_set_bits(unsigned int num, int *even_count, int *odd_count)
{
    *even_count = 0;
    *odd_count = 0;

    for (int pos = 0; pos < 32; pos++)
    {
        if (num & (1u << pos))
        {
            if (pos % 2 == 0)
                (*even_count)++;
            else
                (*odd_count)++;
        }
    }
}

int main(void)
{
    unsigned int num;
    int even_count, odd_count;

    printf("Enter a 32-bit unsigned number: ");
    if (scanf("%u", &num) != 1)
    {
        printf("Invalid input.\n");
        return 1;
    }

    count_set_bits(num, &even_count, &odd_count);

    printf("Set bits in even positions: %d\n", even_count);
    printf("Set bits in odd positions:  %d\n", odd_count);

    return 0;
}
