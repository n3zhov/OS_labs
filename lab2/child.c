#include <unistd.h>
#include <stdio.h>

int main(){
    char c;
    int num = 0, res = 0, minus = 0;
    while (scanf("%c", &c) > 0) {
        if (c == ' ' || c == '\t') {
            if (minus) {
                res = res - num;
            }
            else {
                res = res + num;
            }
            num = 0;
            minus = 0;
        }
        else if (c == '-') {
            minus = 1;
        }
        else if (c == '\n') {
            if (minus) {
                res = res - num;
            }
            else {
                res = res + num;
            }
            num = 0;
            minus = 0;
            write(STDOUT_FILENO, &res, sizeof(int));
            res = 0;
        }
        else if ('0' <= c && c <= '9') {
            num = num * 10 + c - '0';
        }
    }
    return 0;
}

