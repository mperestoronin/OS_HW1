#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#define BUFFER_SIZE 5000
#define PIPE1_NAME "pipe1"
#define PIPE2_NAME "pipe2"

void read_and_write(const char *read_pipe, const char *write_pipe, const char *input_file, const char *output_file);
void execute_punctuation_counter(const char *read_pipe, const char *write_pipe);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        exit(1);
    }

    if (mkfifo(PIPE1_NAME, 0666) == -1 || mkfifo(PIPE2_NAME, 0666) == -1) {
        perror("mkfifo");
        exit(1);
    }

    pid_t child1, child2;

    child1 = fork();
    if (child1 == 0) {
        read_and_write(PIPE2_NAME, PIPE1_NAME, argv[1], argv[2]);
        exit(0);
    }

    child2 = fork();
    if (child2 == 0) {
        execute_punctuation_counter(PIPE1_NAME, PIPE2_NAME);
        exit(0);
    }

    waitpid(child1, NULL, 0);
    waitpid(child2, NULL, 0);

    unlink(PIPE1_NAME);
    unlink(PIPE2_NAME);

    return 0;
}

void read_and_write(const char *read_pipe, const char *write_pipe, const char *input_file, const char *output_file) {
    char buffer[BUFFER_SIZE];
    int input_fd = open(input_file, O_RDONLY);
    if (input_fd == -1) {
        perror("open input_file");
        exit(1);
    }

    int write_pipe_fd = open(write_pipe, O_WRONLY);
    if (write_pipe_fd == -1) {
        perror("open write_pipe");
        exit(1);
    }

    int bytes_read;
    while ((bytes_read = read(input_fd, buffer, BUFFER_SIZE)) > 0) {
        write(write_pipe_fd, buffer, bytes_read);
    }

    close(write_pipe_fd);
    close(input_fd);

    int read_pipe_fd = open(read_pipe, O_RDONLY);
    if (read_pipe_fd == -1) {
        perror("open read_pipe");
        exit(1);
    }

    int output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (output_fd == -1) {
        perror("open output_file");
        exit(1);
    }

    while ((bytes_read = read(read_pipe_fd, buffer, BUFFER_SIZE)) > 0) {
        write(output_fd, buffer, bytes_read);
    }

    close(read_pipe_fd);
    close(output_fd);
}

void execute_punctuation_counter(const char *read_pipe, const char *write_pipe) {
    if (dup2(open(read_pipe, O_RDONLY), STDIN_FILENO) == -1 ||
        dup2(open(write_pipe, O_WRONLY), STDOUT_FILENO) == -1) {
        perror("dup2");
        exit(1);
    }

    execlp("./punctuation_counter", "punctuation_counter", (char *)NULL);
    perror("execlp");
    exit(1);
}

