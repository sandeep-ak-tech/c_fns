/*
Method 1
Use case :- For gaming etc...
*/

#include <stdio.h>

static uint64_t id = 0;

#define NEW_ID()   (++id)


int main()
{
    printf("The new id for player 1 is : %llu\r\n",NEW_ID());
    printf("The new id for player 2 is : %llu\r\n",NEW_ID());
    printf("The new id for player 3 is : %llu\r\n",NEW_ID());
}

/*
Method 2
Use case :- Online shopping and best for embedded applications
Unique Number from MAC + Time + Counter (Best for Embedded)
*/

#include <stdio.h>
#include <stdint.h>

uint64_t counter = 0;

uint64_t get_unique_id()
{
    uint64_t mac_address = 0x12345678;
    uint64_t timestamp = (uint64_t)time(NULL);
    uint64_t count = counter++;

    return ((mac_address <<40) | (timestamp <<16) | count);
 /*
 64 bits total:
+---------------------------------------------------------------+
|  mac_like (24 bits) | timestamp (24 bits) | counter (16 bits) |
+---------------------+---------------------+-------------------+
63                   40                    16                  0   ← bit positions
 */        
 /* For example: 
    mac address = 0x123456
    Timestamp = 1700000000 (year ~2023) → fits in 24 bits easily
    Counter = 42

    mac_like << 40     = 0x1234560000000000
    timestamp << 16    = 0x000000654D75D800
    count              = 0x000000000000002A
                     ---------------------------------
    OR result          = 0x123456654D75D82A   ← Your unique ID!
 */
}

int main()
{
    printf("The new id for player 1 is : %llu\r\n",get_unique_id());
    printf("The new id for player 2 is : %llu\r\n",get_unique_id());
    printf("The new id for player 3 is : %llu\r\n",get_unique_id());
}