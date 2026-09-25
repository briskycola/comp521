#define _GNU_SOURCE

#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct
{
    unsigned long long start;
    unsigned long long end;
    unsigned long long result;
} worker_arg_t;

static void *worker_main(void *arg)
{
    worker_arg_t *worker = (worker_arg_t *)arg;
    unsigned long long local = 0;

    for (unsigned long long i = worker->start; i < worker->end; i++)
    {
        local += (i * 31ULL) ^ (i >> 3);
    }

    worker->result = local;
    return NULL;
}

static double elapsed_seconds(const struct timespec *start,
                              const struct timespec *end)
{
    /* TODO: return elapsed time in seconds. */
    time_t seconds = end->tv_sec - start->tv_sec;
    long nanoseconds = end->tv_nsec - start->tv_nsec;
    return (double)seconds + (double)nanoseconds / 1e9;
}

static int parse_positive(const char *text, unsigned long long *value)
{
    char *end = NULL;
    errno = 0;
    unsigned long long parsed = strtoull(text, &end, 10);

    if (errno != 0 || end == text || *end != '\0' || parsed == 0)
    {
        return -1;
    }

    *value = parsed;
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "usage: %s WORKERS WORK_UNITS\n", argv[0]);
        return 1;
    }

    unsigned long long workers_value;
    unsigned long long work_units;

    if (parse_positive(argv[1], &workers_value) != 0 ||
        parse_positive(argv[2], &work_units) != 0 ||
        workers_value > 1024)
    {
        fprintf(stderr,
                "WORKERS and WORK_UNITS must be positive; WORKERS <= 1024\n");
        return 1;
    }

    size_t workers = (size_t)workers_value;
    pthread_t *threads = calloc(workers, sizeof(*threads));
    worker_arg_t *args = calloc(workers, sizeof(*args));

    if (threads == NULL || args == NULL)
    {
        perror("calloc");
        free(threads);
        free(args);
        return 1;
    }

    struct timespec start_time;
    struct timespec end_time;

    if (clock_gettime(CLOCK_MONOTONIC, &start_time) != 0)
    {
        perror("clock_gettime");
        free(threads);
        free(args);
        return 1;
    }

    unsigned long long base = work_units / workers;
    unsigned long long remainder = work_units % workers;
    unsigned long long next = 0;

    for (size_t i = 0; i < workers; i++)
    {
        unsigned long long portion = base + (i < remainder ? 1 : 0);
        args[i].start = next;
        args[i].end = next + portion;
        args[i].result = 0;
        next += portion;

        if (pthread_create(&threads[i], NULL, worker_main, &args[i]) != 0)
        {
            perror("pthread_create");
            free(threads);
            free(args);
            return 1;
        }
    }

    for (size_t i = 0; i < workers; i++)
    {
        if (pthread_join(threads[i], NULL) != 0)
        {
            perror("pthread_join");
            free(threads);
            free(args);
            return 1;
        }
    }

    if (clock_gettime(CLOCK_MONOTONIC, &end_time) != 0)
    {
        perror("clock_gettime");
        free(threads);
        free(args);
        return 1;
    }

    printf("workers=%zu work_units=%llu elapsed=%.6f\n",
           workers,
           work_units,
           elapsed_seconds(&start_time, &end_time));

    free(threads);
    free(args);
    return 0;
}
