#include <stdio.h>
#include <omp.h>

#define THREADS 4
#define LOOPS 1000000

long counter = 0;

void worker()
{
    #pragma omp parallel num_threads(THREADS)
    for (int i = 0; i < LOOPS; i++)
    {
        #pragma omp atomic
        counter++;
    }
}

int main(void)
{
    worker();
    printf("Expected: %d\n", THREADS * LOOPS);
    printf("Actual:   %ld\n", counter);
    return 0;
}
