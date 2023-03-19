#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#define PIPE1_NAME "pipe1"
#define PIPE2_NAME "pipe2"
#define SYNC_PIPE_NAME "sync_pipe"

void execute_punctuation_counter(const char *read_pipe, const char *write_pipe);
void wait_for_signal(const char *sync_pipe);

int main() {
    wait_for_signal(SYNC_PIPE_NAME);
    execute_punctuation_counter(PIPE1_NAME, PIPE2_NAME);
    return 0;
}

void wait_for_signal(const char *sync_pipe) {
    int sync_pipe_fd = open(sync_pipe, O_RDONLY);
    if (sync_pipe_fd == -1) {
        perror("open sync_pipe");
        exit(1);
    }
    char buffer;
    read(sync_pipe_fd, &buffer, 1);
    close(sync_pipe_fd);
}

void execute_punctuation_counter(const char *read_pipe, const char *write_pipe) {
    int read_pipe_fd = open(read_pipe, O_RDONLY);
    if (read_pipe_fd == -1) {
        perror("open read_pipe");
        exit(1);
    }

    int write_pipe_fd = open(write_pipe, O_WRONLY);
    if (write_pipe_fd == -1) {
        perror("open write_pipe");
        exit(1);
    }

    if (dup2(read_pipe_fd, STDIN_FILENO) == -1 ||
        dup2(write_pipe_fd, STDOUT_FILENO) == -1) {
        perror("dup2");
        exit(1);
    }

    execlp("./punctuation_counter", "punctuation_counter", (char *)NULL);
    perror("execlp");
    exit(1);
}