// By Eymard Alarcon //

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <inp> <cmd> <out>\n", argv[0]);
        return 1;
    }

    char *inp_file = argv[1];
    char *cmd = argv[2];
    char *out_file = argv[3];

    char *args[100];
    int i = 0;
    char *token = strtok(cmd, " ");
    while (token != NULL && i < 100) {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        return 1;
    }

    if (pid == 0) {
        if (strcmp(inp_file, "-") != 0) {
            int in_fd = open(inp_file, O_RDONLY);
            if (in_fd < 0) {
                perror("Error opening input file");
                return 1;
            }
            dup2(in_fd, STDIN_FILENO);
            close(in_fd);
        }

        if (strcmp(out_file, "-") != 0) {
            int out_fd = open(out_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (out_fd < 0) {
                perror("Error opening output file");
                return 1;
            }
            dup2(out_fd, STDOUT_FILENO);
            close(out_fd);
        }

        execvp(args[0], args);
        perror("execvp failed");
        return 1;
    } else {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            printf("Child exited with status %d\n", WEXITSTATUS(status));
        }
    }

    return 0;
}
