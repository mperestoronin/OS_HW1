#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

#define BUFFER_SIZE 1024

void read_and_write(int read_pipe, int write_pipe, const char *input_file, const char *output_file);
void execute_punctuation_counter(int read_pipe, int write_pipe);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        exit(1);
    }

    int pipe1[2], pipe2[2];

    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("pipe");
        exit(1);
    }

    pid_t child1, child2;

    child1 = fork();
    if (child1 == 0) {
        close(pipe1[0]);
        close(pipe2[1]);
        read_and_write(pipe2[0], pipe1[1], argv[1], argv[2]);
        exit(0);
    }

    child2 = fork();
    if (child2 == 0) {
        close(pipe1[1]);
        close(pipe2[0]);
        execute_punctuation_counter(pipe1[0], pipe2[1]);
        exit(0);
    }

    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);

    waitpid(child1, NULL, 0);
    waitpid(child2, NULL, 0);

    return 0;
}

void read_and_write(int read_pipe, int write_pipe, const char *input_file, const char *output_file) {
    char buffer[BUFFER_SIZE];
    int input_fd = open(input_file, O_RDONLY);
    if (input_fd == -1) {
        perror("open input_file");
        exit(1);
    }

    int bytes_read;
    while ((bytes_read = read(input_fd, buffer, BUFFER_SIZE)) > 0) {
        write(write_pipe, buffer, bytes_read);
    }

    close(write_pipe);
    close(input_fd);

    int output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (output_fd == -1) {
        perror("open output_file");
        exit(1);
    }

    while ((bytes_read = read(read_pipe, buffer, BUFFER_SIZE)) > 0) {
        write(output_fd, buffer, bytes_read);
    }

    close(read_pipe);
    close(output_fd);
}

void execute_punctuation_counter(int read_pipe, int write_pipe) {
    if (dup2(read_pipe, STDIN_FILENO) == -1 ||
        dup2(write_pipe, STDOUT_FILENO) == -1) {
        perror("dup2");
        exit(1);
    }

    execlp("./punctuation_counter", "punctuation_counter", (char *)NULL);
    perror("execlp");
    exit(1);
}
