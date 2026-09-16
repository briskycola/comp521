#include <stdio.h>

#define LOOPS 1000000

int main(void)
{
    long counter = 0;

    for (int i = 0; i < LOOPS; i++)
    {
        counter++;
    }

    printf("Expected: %d\n", LOOPS);
    printf("Actual:   %ld\n", counter);

    return 0;
}
