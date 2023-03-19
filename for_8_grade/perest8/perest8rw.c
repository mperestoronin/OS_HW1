#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#define BUFFER_SIZE 1024
#define PIPE1_NAME "pipe1"
#define PIPE2_NAME "pipe2"
#define SYNC_PIPE_NAME "sync_pipe"

void read_and_write(const char *read_pipe, const char *write_pipe, const char *input_file, const char *output_file);
void send_signal(const char *sync_pipe);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        exit(1);
    }

    send_signal(SYNC_PIPE_NAME);
    read_and_write(PIPE2_NAME, PIPE1_NAME, argv[1], argv[2]);

    return 0;
}

void send_signal(const char *sync_pipe) {
    int sync_pipe_fd = open(sync_pipe, O_WRONLY);
    if (sync_pipe_fd == -1) {
        perror("open sync_pipe");
        exit(1);
    }
    char buffer = 1;
    write(sync_pipe_fd, &buffer, 1);
    close(sync_pipe_fd);
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
