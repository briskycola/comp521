#include <stdio.h>
#include <pthread.h>

#define THREADS 4
#define LOOPS 1000000

long counter = 0;
pthread_mutex_t mutex;

void *worker(void *arg)
{
    (void)arg;
    for (int i = 0; i < LOOPS; i++)
    {
        pthread_mutex_lock(&mutex);
        counter++;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main(void)
{
    pthread_t threads[THREADS];

    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < THREADS; i++)
    {
        pthread_create(&threads[i], NULL, worker, NULL);
    }

    for (int i = 0; i < THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);

    printf("Expected: %d\n", THREADS * LOOPS);
    printf("Actual:   %ld\n", counter);
    return 0;
}
