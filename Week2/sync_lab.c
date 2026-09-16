#include <stdio.h>
#include <pthread.h>

#define THREADS 4
#define LOOPS 1000000

long counter = 0;
pthread_mutex_t lock;

// Set to 0 for the unsynchronized experiment.
// Set to 1 for the mutex-protected experiment.
int use_mutex = 0;

void *worker(void *arg)
{
    (void)arg;
    for (int i = 0; i < LOOPS; i++)
    {
        if (use_mutex) pthread_mutex_lock(&lock);
        counter++;
        if (use_mutex) pthread_mutex_unlock(&lock);
    }

    return NULL;
}

int main(void)
{
    pthread_t threads[THREADS];

    pthread_mutex_init(&lock, NULL);

    for (int i = 0; i < THREADS; i++)
    {
        pthread_create(&threads[i], NULL, worker, NULL);
    }

    for (int i = 0; i < THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&lock);

    printf("Mode:     %s\n", use_mutex ? "synchronized" : "unsynchronized");
    printf("Expected: %d\n", THREADS * LOOPS);
    printf("Actual:   %ld\n", counter);

    return 0;
}
