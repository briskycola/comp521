#define _GNU_SOURCE

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

static double elapsed_seconds(const struct timespec *start,
                              const struct timespec *end) {
    double seconds = (double)(end->tv_sec - start->tv_sec);
    double nanoseconds =
        (double)(end->tv_nsec - start->tv_nsec) / 1000000000.0;

    return seconds + nanoseconds;
}

static void touch_pages(char *buffer, size_t bytes) {
    long page_size = sysconf(_SC_PAGESIZE);

    if (page_size <= 0) {
        page_size = 4096;
    }

    for (size_t offset = 0; offset < bytes; offset += (size_t)page_size) {
        buffer[offset] = 1;
    }
    buffer[bytes - 1] = 1;
}

static long read_rss_kb(void) {
    FILE *file = fopen("/proc/self/status", "r");
    char line[256];
    long rss_kb = -1;

    if (file == NULL) {
        perror("fopen");
        return -1;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        if (sscanf(line, "VmRSS: %ld kB", &rss_kb) == 1) {
            break;
        }
    }

    fclose(file);
    return rss_kb;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s MEGABYTES\n", argv[0]);
        return 1;
    }

    char *end = NULL;
    errno = 0;

    unsigned long megabytes = strtoul(argv[1], &end, 10);

    if (errno != 0 || end == argv[1] || *end != '\0' ||
        megabytes == 0) {
        fprintf(stderr, "MEGABYTES must be a positive integer\n");
        return 1;
    }

    size_t bytes = (size_t)megabytes * 1024 * 1024;
    struct timespec start;
    struct timespec end_time;

    printf("megabytes=%lu rss_before=%ld\n",
           megabytes,
           read_rss_kb());

    if (clock_gettime(CLOCK_MONOTONIC, &start) != 0) {
        perror("clock_gettime");
        return 1;
    }

    char *buffer = malloc(bytes);

    if (buffer == NULL) {
        perror("malloc");
        return 1;
    }

    if (clock_gettime(CLOCK_MONOTONIC, &end_time) != 0) {
        perror("clock_gettime");
        free(buffer);
        return 1;
    }

    double allocation_seconds =
        elapsed_seconds(&start, &end_time);

    if (clock_gettime(CLOCK_MONOTONIC, &start) != 0) {
        perror("clock_gettime");
        free(buffer);
        return 1;
    }

    touch_pages(buffer, bytes);

    if (clock_gettime(CLOCK_MONOTONIC, &end_time) != 0) {
        perror("clock_gettime");
        free(buffer);
        return 1;
    }

    double touch_seconds =
        elapsed_seconds(&start, &end_time);

    printf("megabytes=%lu allocation=%.6f touch=%.6f rss_after=%ld\n",
           megabytes,
           allocation_seconds,
           touch_seconds,
           read_rss_kb());

    free(buffer);
    return 0;
}
