#include <stdio.h>
#include <unistd.h>

int main(void) {

    /*
     * TODO 1:
     * Print this process's PID.
     *
     * Hint:
     * getpid()
     */
     printf("Original process PID: %d\n", getpid());

    /*
     * TODO 2:
     * Print this process's parent PID.
     *
     * Hint:
     * getppid()
     */
     int b = getppid();
     printf("Process PID: %d\n", b);
     printf("")

    return 1;
}
