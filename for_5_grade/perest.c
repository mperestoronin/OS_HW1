#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#define BUFFER_SIZE 1024
#define PIPE1_NAME "pipe1"
#define PIPE2_NAME "pipe2"

void read_from_file(const char *pipe_name, const char *file_name);
void execute_punctuation_counter(const char *read_pipe, const char *write_pipe);
void write_to_file(const char *pipe_name, const char *file_name);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        exit(1);
    }

    if (mkfifo(PIPE1_NAME, 0666) == -1 || mkfifo(PIPE2_NAME, 0666) == -1) {
        perror("mkfifo");
        exit(1);
    }

    pid_t child1, child2, child3;

    child1 = fork();
    if (child1 == 0) {
        read_from_file(PIPE1_NAME, argv[1]);
        exit(0);
    }

    child2 = fork();
    if (child2 == 0) {
        execute_punctuation_counter(PIPE1_NAME, PIPE2_NAME);
        exit(0);
    }

    child3 = fork();
    if (child3 == 0) {
        write_to_file(PIPE2_NAME, argv[2]);
        exit(0);
    }

    waitpid(child1, NULL, 0);
    waitpid(child2, NULL, 0);
    waitpid(child3, NULL, 0);

    unlink(PIPE1_NAME);
    unlink(PIPE2_NAME);

    return 0;
}

void read_from_file(const char *pipe_name, const char *file_name) {
    char buffer[BUFFER_SIZE];
    int input_file = open(file_name, O_RDONLY);
    if (input_file == -1) {
        perror("open input_file");
        exit(1);
    }

    int pipe_fd = open(pipe_name, O_WRONLY);
    if (pipe_fd == -1) {
        perror("open pipe");
        exit(1);
    }

    int bytes_read;
    while ((bytes_read = read(input_file, buffer, BUFFER_SIZE)) > 0) {
        write(pipe_fd, buffer, bytes_read);
    }

    close(input_file);
    close(pipe_fd);
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

void write_to_file(const char *pipe_name, const char *file_name) {
    char buffer[BUFFER_SIZE];
    int output_file = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (output_file == -1) {
        perror("open output_file");
    exit(1);
    }

    int pipe_fd = open(pipe_name, O_RDONLY);
    if (pipe_fd == -1) {
        perror("open pipe");
        exit(1);
    }

    int bytes_read;
    while ((bytes_read = read(pipe_fd, buffer, BUFFER_SIZE)) > 0) {
        write(output_file, buffer, bytes_read);
    }

    close(output_file);
    close(pipe_fd);
}



