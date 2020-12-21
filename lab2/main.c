#include <stdio.h>
#include <unistd.h>

signed main() {
    char s [256];
    for (int i = 0; i < 256; i++) {
        s[i] = 0;
    }
    printf("Enter filename: ");
    scanf(" %s", s);
    FILE* input = NULL;
    input = freopen(s, "r", stdin);
    int fd[2];
    if (pipe(fd) == -1) {
        printf("Error creating pipe!");
        return 1;
    }
    int id = fork();
    if (id == -1) {
        printf("Error creating process!");
        return 2;
    }
    else if (id == 0) {
        close(fd[0]);
        if (input == NULL) {
            printf("Error opening file!\n");
            return 3;
        }
        if (dup2(fd[1], STDOUT_FILENO) == -1) {
            printf("Error changing stdout!\n");
            return 4;
        }
        char *argv[] = {"child", NULL};;
        if (execv("child", argv) == -1) {
            printf("Error executing child process!\n");
            return 5;
        }
    }
    else {
        close(fd[1]);
        int res;
        while (read(fd[0], &res, sizeof(int)) > 0) {
            printf("%d\n", res);
        }
        close(fd[0]);
    }
    return 0;
}