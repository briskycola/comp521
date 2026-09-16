#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdatomic.h>

#define TOTAL_WORK 40000000L

typedef struct
{
    long value;
} compact_slot;

typedef struct
{
    long value;
    char padding[64 - sizeof(long)];
} padded_slot;

int thread_count;
const char *mode;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

long shared_mutex = 0;
atomic_long shared_atomic = 0;

compact_slot *compact;
padded_slot *padded;

void *worker(void *arg)
{
    long id = (long)arg;
    long work = TOTAL_WORK / thread_count;

    for (long i = 0; i < work; i++)
    {
        // TODO
        if (strcmp(mode, "mutex") == 0)
        {
            pthread_mutex_lock(&lock);
            shared_mutex++;
            pthread_mutex_unlock(&lock);
        }

        if (strcmp(mode, "atomic") == 0)
        {
            atomic_fetch_add(&shared_atomic, 1);
        }

        if (strcmp(mode, "compact") == 0)
        {
            compact[id].value++;
        }

        if (strcmp(mode, "padded") == 0)
        {
            padded[id].value++;
        }
    }
    return NULL;
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "usage: %s mutex|atomic|compact|padded THREADS\n", argv[0]);
        return 1;
    }

    mode = argv[1];
    thread_count = atoi(argv[2]);

    if (thread_count < 1 || thread_count > 64 || TOTAL_WORK % thread_count != 0)
    {
        fprintf(stderr, "invalid thread count\n");
        return 1;
    }

    if (strcmp(mode, "mutex") != 0 &&
        strcmp(mode, "atomic") != 0 &&
        strcmp(mode, "compact") != 0 &&
        strcmp(mode, "padded") != 0)
    {
        fprintf(stderr, "invalid mode\n");
        return 1;
    }

    compact = calloc((size_t)thread_count, sizeof(*compact));
    padded = calloc((size_t)thread_count, sizeof(*padded));
    pthread_t *threads = malloc((size_t)thread_count * sizeof(*threads));

    if (compact == NULL || padded == NULL || threads == NULL)
    {
        fprintf(stderr, "memory allocation failed\n");
        return 1;
    }

    for (long i = 0; i < thread_count; i++)
    {
        pthread_create(&threads[i], NULL, worker, (void*)i);
    }

    for (long i = 0; i < thread_count; i++)
    {
        pthread_join(threads[i], NULL);
    }

    long total = 0;

    // TODO
    if (strcmp(mode, "mutex") == 0)
    {
        total = shared_mutex;
    }

    if (strcmp(mode, "atomic") == 0)
    {
        total = atomic_load(&shared_atomic);
    }

    if (strcmp(mode, "compact") == 0)
    {
        for (long i = 0; i < thread_count; i++)
        {
            total += compact[i].value;
        }
    }

    if (strcmp(mode, "padded") == 0)
    {
        for (long i = 0; i < thread_count; i++)
        {
            total += padded[i].value;
        }
    }
    printf("Mode: %s\n", mode);
    printf("Threads: %d\n", thread_count);
    printf("Expected: %ld\n", TOTAL_WORK);
    printf("Actual: %ld\n", total);

    free(threads);
    free(compact);
    free(padded);

    return 0;
}
