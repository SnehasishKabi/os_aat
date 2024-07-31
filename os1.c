#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define ARRAY_SIZE 5
#define BUFFER_SIZE (ARRAY_SIZE * sizeof(int))
void parent_child_communication() {
    int pipefd[2];
    pid_t pid;
    int numbers[] = {1, 2, 3, 4, 5};

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        close(pipefd[1]);
        int received_numbers[ARRAY_SIZE];
        ssize_t num_read = read(pipefd[0], received_numbers, BUFFER_SIZE);
        if (num_read == BUFFER_SIZE) {
            int sum = 0;
            for (int i = 0; i < ARRAY_SIZE; ++i) {
                sum += received_numbers[i];
            }
            printf("Child process calculated sum: %d\n", sum);
        } else if (num_read == 0) {
            printf("No data received from parent\n");
        } else {
            perror("read");
        }

        close(pipefd[0]);
        exit(EXIT_SUCCESS);
    } else {
        close(pipefd[0]);
        ssize_t num_written = write(pipefd[1], numbers, BUFFER_SIZE);
        if (num_written == -1) {
            perror("write");
        }

        close(pipefd[1]);
        wait(NULL);
    }
}
int main() {
    parent_child_communication();
    return 0;
}

