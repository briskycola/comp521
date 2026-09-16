#include <stdio.h>
#include <pthread.h>

void *worker(void *arg) {
    (void)arg;
    printf("Hello from worker thread\n");
    return NULL;
}

int main(void) {
    pthread_t t;
    pthread_create(&t, NULL, worker, NULL);
    pthread_join(t, NULL);
    return 0;
}
