#include <stdio.h>
#include <unistd.h>

int main(void) {
    // TODO: Create one child process here.
    printf("A\n");
    fork();
    printf("B\n");
    return 0;
}
