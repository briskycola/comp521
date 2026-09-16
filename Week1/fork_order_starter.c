#include <stdio.h>
#include <unistd.h>

int main(void) {
    pid_t pid = fork();
    // TODO: Child prints: child
    // TODO: Parent prints: parent

    // Failure
    if (pid < 0) {
        perror("fork");
        return 1;
    }

    // Child
    if (pid == 0) {
        printf("child\n");
    }

    // Parent
    if (pid > 0) {
        printf("parent\n");
    }
    return 0;
}
