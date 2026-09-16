#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>

typedef struct
{
    int thread_id;
    int samples;
    long delay_ms;
} thread_config;

pthread_mutex_t output_lock = PTHREAD_MUTEX_INITIALIZER;

void sleep_ms(long miliseconds)
{
    struct timespec delay;

    delay.tv_sec = miliseconds / 100;
    delay.tv_nsec = (miliseconds % 1000) * 1000000L;
    nanosleep(&delay, NULL);
}

void* observe_cpu(void *arg)
{
    thread_config *config = (thread_config*)arg;

    for (int sample = 0; sample < config->samples; sample++)
    {
        int cpu = sched_getcpu();

        pthread_mutex_lock(&output_lock);

        printf("thread=%d sample=%d cpu=%d\n", config->thread_id, sample, cpu);

        pthread_mutex_unlock(&output_lock);

        sleep_ms(config->delay_ms);
    }
    return NULL;
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "usage: %s THREADS SAMPLES\n", argv[0]);
        return 1;
    }

    int thread_count = atoi(argv[1]);
    int samples = atoi(argv[2]);

    if (thread_count < 1 || thread_count > 64 || samples < 1 || samples > 1000)
    {
        fprintf(stderr, "invalid thread or sample count\n");
        return 1;
    }

    pthread_t *threads = malloc((size_t)thread_count * sizeof(*threads));
    thread_config *configs = malloc((size_t)thread_count * sizeof(*configs));

    if (threads == NULL || configs == NULL)
    {
        fprintf(stderr, "memory allocation failed\n");
        free(threads);
        free(configs);
        return 1;
    }

    for (int i = 0; i < thread_count; i++)
    {
        configs[i].thread_id = i;
        configs[i].samples = samples;
        configs[i].delay_ms = 250;

        pthread_create(&threads[i], NULL, observe_cpu, &configs[i]);
    }

    for (int i = 0; i < thread_count; i++)
    {
        pthread_join(threads[i], NULL);
    }

    free(threads);
    free(configs);
    return 0;
}
