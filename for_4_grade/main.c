#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#define BUFFER_SIZE 1024

void read_from_file(int pipe_fd, const char *file_name);
void execute_punctuation_counter(int read_fd, int write_fd);
void write_to_file(int pipe_fd, const char *file_name);

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

    pid_t child1, child2, child3;

    child1 = fork();
    if (child1 == 0) {
        close(pipe1[0]);
        read_from_file(pipe1[1], argv[1]);
        close(pipe1[1]);
        exit(0);
    }

    child2 = fork();
    if (child2 == 0) {
        close(pipe1[1]);
        close(pipe2[0]);
        execute_punctuation_counter(pipe1[0], pipe2[1]);
        exit(0);
    }

    child3 = fork();
    if (child3 == 0) {
        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[1]);
        write_to_file(pipe2[0], argv[2]);
        close(pipe2[0]);
        exit(0);
    }

    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);

    waitpid(child1, NULL, 0);
    waitpid(child2, NULL, 0);
    waitpid(child3, NULL, 0);

    return 0;
}

void read_from_file(int pipe_fd, const char *file_name) {
    char buffer[BUFFER_SIZE];
    int input_file = open(file_name, O_RDONLY);
    if (input_file == -1) {
        perror("open input_file");
        exit(1);
    }

    int bytes_read;
    while ((bytes_read = read(input_file, buffer, BUFFER_SIZE)) > 0) {
        write(pipe_fd, buffer, bytes_read);
    }

    close(input_file);
}

void execute_punctuation_counter(int read_fd, int write_fd) {
    if (dup2(read_fd, STDIN_FILENO) == -1 || dup2(write_fd, STDOUT_FILENO) == -1) {
        perror("dup2");
        exit(1);
    }

    execlp("./punctuation_counter", "punctuation_counter", (char *)NULL);
    perror("execlp");
    exit(1);
}

void write_to_file(int pipe_fd, const char *file_name) {
    char buffer[BUFFER_SIZE];
    int output_file = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (output_file == -1) {
        perror("open output_file");
        exit(1);
    }

    int bytes_read;
    while ((bytes_read = read(pipe_fd, buffer, BUFFER_SIZE)) > 0) {
        write(output_file, buffer, bytes_read);
    }

    close(output_file);
}