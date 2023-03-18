#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s input_file output_file\n", argv[0]);
        return 1;
    }

    pid_t pipe_id1, pipe_id2;
    int file_discr1[2], file_discr2[2];

    if (pipe(file_discr1) < 0 || pipe(file_discr2) < 0) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    if ((pipe_id1 = fork()) < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pipe_id1 == 0) { // first child process
        close(file_discr1[0]);
        close(file_discr2[0]);
        close(file_discr2[1]);

        int fd = open(argv[1], O_RDONLY);
        if (fd < 0) {
            perror("open");
            exit(EXIT_FAILURE);
        }

        char buf[BUFSIZ];
        ssize_t read_bytes;
      
        while ((read_bytes = read(fd, buf, BUFSIZ)) > 0) {
            if (write(file_discr1[1], buf, read_bytes) != read_bytes) {
                perror("write");
                exit(EXIT_FAILURE);
            }
        }

        if (read_bytes < 0) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        close(file_discr1[1]);

        exit(EXIT_SUCCESS);
    }

    if ((pipe_id2 = fork()) < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pipe_id2 == 0) { // second child process
        close(file_discr1[1]);
        close(file_discr2[0]);

        if (dup2(file_discr1[0], STDIN_FILENO) < 0) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }

        if (dup2(file_discr2[1], STDOUT_FILENO) < 0) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }

        execlp("./punctuation_counter", "./punctuation_counter", NULL);

        close(file_discr1[0]);
        close(file_discr2[1]);
    }

    close(file_discr1[0]);
    close(file_discr1[1]);
    close(file_discr2[1]);

    int fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    char buf[BUFSIZ];
    ssize_t read_bytes;

    while ((read_bytes = read(file_discr2[0], buf, BUFSIZ)) > 0) {
        if (write(fd, buf, read_bytes) != read_bytes) {
            perror("write");
            exit(EXIT_FAILURE);
        }
    }

    if (read_bytes < 0) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    close(fd);
    close(file_discr2[0]);

    waitpid(pipe_id1, NULL, 0);
    waitpid(pipe_id2, NULL, 0);

    return 0;
}
