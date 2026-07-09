#include <stdio.h>

/*
 * Count the number of set bits that are in odd positions of a 32-bit number.
 * Positions are counted from 0 (least significant bit).
 * Odd positions are bit indices 1, 3, 5, ..., 31.
 */
int count_set_bits_odd_position(unsigned int num)
{
    int count = 0;

    for (int pos = 1; pos < 32; pos += 2)
    {
        if (num & (1u << pos))
            count++;
    }

    return count;
}

int main(void)
{
    unsigned int num;

    printf("Enter a 32-bit unsigned number: ");
    if (scanf("%u", &num) != 1)
    {
        printf("Invalid input.\n");
        return 1;
    }

    printf("Number of set bits in odd positions: %d\n",
           count_set_bits_odd_position(num));

    return 0;
}
