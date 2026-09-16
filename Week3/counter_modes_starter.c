#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdatomic.h>

#define LOOPS 2000000L

long mutex_counter = 0;
atomic_long atomic_counter = 0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

const char *mode;

void *worker(void *arg)
{
    (void)arg;

    for (long i = 0; i < LOOPS; i++) {
        if (strcmp(mode, "mutex") == 0) {
            /*
             * TODO:
             * Lock the mutex.
             * Increment mutex_counter.
             * Unlock the mutex.
             */
        } else {
            /*
             * TODO:
             * Atomically increment atomic_counter.
             */
        }
    }

    return NULL;
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr,
                "usage: %s mutex|atomic THREADS\n",
                argv[0]);
        return 1;
    }

    mode = argv[1];
    int thread_count = atoi(argv[2]);

    if ((strcmp(mode, "mutex") != 0 &&
         strcmp(mode, "atomic") != 0) ||
        thread_count < 1 ||
        thread_count > 64) {
        fprintf(stderr,
                "invalid mode or thread count\n");
        return 1;
    }

    pthread_t *threads =
        malloc((size_t)thread_count * sizeof(*threads));

    if (threads == NULL) {
        fprintf(stderr, "memory allocation failed\n");
        return 1;
    }

    for (int i = 0; i < thread_count; i++) {
        pthread_create(&threads[i],
                       NULL,
                       worker,
                       NULL);
    }

    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }

    long actual;

    if (strcmp(mode, "mutex") == 0) {
        actual = mutex_counter;
    } else {
        actual = atomic_load(&atomic_counter);
    }

    printf("Mode:     %s\n", mode);
    printf("Threads:  %d\n", thread_count);
    printf("Expected: %ld\n",
           (long)thread_count * LOOPS);
    printf("Actual:   %ld\n", actual);

    free(threads);
    return 0;
}