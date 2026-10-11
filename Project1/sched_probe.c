#define _GNU_SOURCE

#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct
{
    int thread_id;
    int samples;
    unsigned long long work_units;
    unsigned long long result;
} worker_arg_t;

static pthread_mutex_t output_lock = PTHREAD_MUTEX_INITIALIZER;

static double elapsed_seconds(const struct timespec *start, const struct timespec *end)
{
    time_t seconds = end->tv_sec - start->tv_sec;
    long nanoseconds = end->tv_nsec - start->tv_nsec;

    return (double)seconds + (double)nanoseconds / 1000000000.0;
}

static int parse_positive(const char *text, unsigned long long *value)
{
    char *end = NULL;

    errno = 0;
    unsigned long long parsed = strtoull(text, &end, 10);

    if (errno != 0 || end == text || *end != '\0' || parsed == 0)
    {
        return 0;
    }

    *value = parsed;
    return 1;
}

static void *worker_main(void *arg)
{
    worker_arg_t *worker = arg;

    unsigned long long local = (unsigned long long)worker->thread_id;

    for (int sample = 0; sample < worker->samples; sample++)
    {
        int cpu = sched_getcpu();

        pthread_mutex_lock(&output_lock);

        printf("thread=%d sample=%d cpu=%d\n", worker->thread_id, sample,cpu);

        pthread_mutex_unlock(&output_lock);

        for (unsigned long long i = 0; i < worker->work_units; i++)
        {
            local += ((i + 1ULL) * 31ULL) ^ (i >> 3) ^ (unsigned long long)worker->thread_id;
        }
    }

    worker->result = local;
    return NULL;
}

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        fprintf(stderr, "usage: %s THREADS SAMPLES WORK_UNITS\n", argv[0]);
        return 1;
    }

    unsigned long long thread_count_value;
    unsigned long long samples_value;
    unsigned long long work_units;

    if (!parse_positive(argv[1], &thread_count_value) || !parse_positive(argv[2], &samples_value) || !parse_positive(argv[3], &work_units))
    {
        fprintf(stderr, "THREADS, SAMPLES, and WORK_UNITS " "must be positive integers\n");
        return 1;
    }

    if (thread_count_value > 1024 || samples_value > 1000000)
    {
        fprintf(stderr, "THREADS or SAMPLES is too large\n");
        return 1;
    }

    int thread_count = (int)thread_count_value;
    int samples = (int)samples_value;

    pthread_t *threads = calloc((size_t)thread_count, sizeof(*threads));

    worker_arg_t *args = calloc((size_t)thread_count, sizeof(*args));

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

    int created = 0;

    for (int i = 0; i < thread_count; i++)
    {
        args[i].thread_id = i;
        args[i].samples = samples;
        args[i].work_units = work_units;
        args[i].result = 0;

        int error = pthread_create(&threads[i], NULL, worker_main, &args[i]);

        if (error != 0)
        {
            fprintf(stderr, "pthread_create failed: %s\n", strerror(error));

            for (int j = 0; j < created; j++)
            {
                pthread_join(threads[j], NULL);
            }

            free(threads);
            free(args);
            return 1;
        }
        created++;
    }

    for (int i = 0; i < thread_count; i++)
    {
        int error = pthread_join(threads[i], NULL);

        if (error != 0)
        {
            fprintf(stderr, "pthread_join failed: %s\n", strerror(error));
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

    unsigned long long correctness = 0;

    for (int i = 0; i < thread_count; i++)
    {
        correctness ^= args[i].result;
    }

    printf("summary threads=%d samples=%d work_units=%llu "
           "elapsed_seconds=%.6f correctness=%llu\n",
           thread_count,
           samples,
           work_units,
           elapsed_seconds(&start_time, &end_time),
           correctness);

    pthread_mutex_destroy(&output_lock);
    free(threads);
    free(args);

    return 0;
}
