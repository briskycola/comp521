#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {

    /*
     * TODO 1
     *
     * Print the PID of the original process.
     *
     * Example label:
     * Original process PID:
     */
     printf("Original process PID: %d\n", getpid());


    /*
     * TODO 2
     *
     * Create one child process with fork().
     *
     * Store the return value in a pid_t variable.
     */
     pid_t pid = fork();


    /*
     * TODO 3
     *
     * Check whether fork() failed.
     *
     * If it failed:
     *     print an error with perror()
     *     return a nonzero status
     */
     if (pid < 0) {
         perror("fork");
         return 1;
     }


    /*
     * TODO 4 — CHILD
     *
     * Determine whether this is the child.
     *
     * In the child:
     *
     * 1. Print the child's PID.
     *
     * 2. Use an exec-family function
     *    to execute:
     *
     *        /bin/ls
     *
     * 3. If exec fails:
     *      call perror()
     *      terminate with a failure status
     */
     // CHILD PROCESS
     if (pid == 0) {
         printf("Child process PID: %d\n", getpid());
         printf("Parent process PID: %d\n", getppid());
         execl("/bin/ls", "ls", (char*)NULL);
         perror("execl");
         return 1;
     }


    /*
     * TODO 5 — PARENT
     *
     * In the parent:
     *
     * 1. Print the parent's PID.
     *
     * 2. Print the child's PID.
     *
     * 3. Wait for the child.
     *
     * 4. After wait returns,
     *    print a completion message.
     */
     // PARENT PROCESS
     printf("Parent process PID: %d\n", getpid());
     printf("Child process PID: %d\n", pid);
     wait(NULL);
     printf("Parent: child has completed\n");

    return 0;
}
