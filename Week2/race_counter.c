#include <stdio.h>
#include <pthread.h>

#define THREADS 4
#define LOOPS 1000000

long counter = 0;

void *worker(void *arg)
{
    (void)arg;
    for (int i = 0; i < LOOPS; i++)
    {
        counter++;
    }
    return NULL;
}

int main(void)
{
    pthread_t threads[THREADS];

    for (int i = 0; i < THREADS; i++)
    {
        pthread_create(&threads[i], NULL, worker, NULL);
    }

    for (int i = 0; i < THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }


    printf("Expected: %d\n", THREADS * LOOPS);
    printf("Actual:   %ld\n", counter);
    return 0;
}
