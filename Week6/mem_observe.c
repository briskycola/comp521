#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

static void touch_pages(char *buffer, size_t bytes) {
    /* TODO: write one byte at approximately every page. */
    long page_size = sysconf(_SC_PAGESIZE);
    if (page_size <= 0) {
        page_size = 4096;
    }

    for (size_t offset = 0; offset < bytes; offset += (size_t)page_size) {
        buffer[offset] = 1;
    }
    buffer[bytes - 1] = 1;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s MEGABYTES\n", argv[0]);
        return 1;
    }

    char *end = NULL;
    errno = 0;
    unsigned long megabytes = strtoul(argv[1], &end, 10);

    if (errno != 0 || end == argv[1] || *end != '\0' || megabytes == 0) {
        fprintf(stderr, "MEGABYTES must be a positive integer\n");
        return 1;
    }

    size_t bytes = (size_t)megabytes * 1024 * 1024;
    printf("before allocation: VmRSS=%ld kB\n", read_rss_kb());

    char *buffer = malloc(bytes);
    if (buffer == NULL) {
        perror("malloc");
        return 1;
    }

    printf("after allocation:  VmRSS=%ld kB\n", read_rss_kb());
    touch_pages(buffer, bytes);
    printf("after first touch:  VmRSS=%ld kB\n", read_rss_kb());

    free(buffer);
    return 0;
}
