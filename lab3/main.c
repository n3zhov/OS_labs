#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

typedef struct ThreadToken {
    char *str;
    char *pattern;
    int strLen;
    int patternLen;
    int startIndex;
    int endIndex;
    int res;
} ThreadToken;

void* ThreadFunc(void* token) {
    ThreadToken* tok = (ThreadToken*) token;
    char *pattern = tok->pattern;
    char *str = tok->str;
    int found = 1;
    if(tok->strLen < tok->patternLen){
        tok->res = -1;
        return NULL;
    }
    for (int i = tok->startIndex; i < tok->endIndex; ++i){
        for (int j = 0; j < tok->patternLen; ++j){
            if(i + j - tok->startIndex >= tok->strLen){
                found = 0;
                break;
            }
            if (str[i+j] != pattern[j]){
                found = 0;
                break;
            }
        }
        if(found){
            tok->res = i;
            return NULL;
        }
        else{
            found = 1;
        }
    }
    tok->res = -1;
    return NULL;
}

int main(int argc, char** argv) {
    int threadNumber = 0;
    int doprint = 1;
    for (int i = 0; argv[1][i] > 0; ++i) {
        if (argv[1][i] >= '0' && argv[1][i] <= '9') {
            threadNumber = threadNumber * 10 + argv[1][i] - '0';
        }
    }
    if(argc < 2){
        printf("Wrong count of arguments!\n");
        return -1;
    }
    if(strcmp(argv[2],"0") == 0){
        doprint = 0;
    }
    printf("Thread number: %d\n", threadNumber);
    pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t) * threadNumber);
    int n, m;
    if(doprint)
        printf("Enter string length: ");
    scanf(" %d", &n);
    if(doprint)
        printf("Enter pattern length: ");
    scanf(" %d", &m);
    char *text = malloc(sizeof(char) * (n+1));
    char *pattern = malloc(sizeof(char)*(m+1));
    if(doprint)
        printf("Enter text: ");
    scanf(" %s", text);
    if(doprint)
        printf("Enter pattern: ");
    scanf(" %s", pattern);
    ThreadToken* tokens = (ThreadToken*)malloc(sizeof(ThreadToken) * threadNumber);
    int step = n/threadNumber;
    for (int i = 0; i < threadNumber; ++i) {
        int curLen = step;
        if (i < threadNumber - 1) {
            curLen += m;
        }
        tokens[i].strLen = curLen;
        tokens[i].str = text;
        tokens[i].startIndex = step*i;
        tokens[i].endIndex = tokens[i].startIndex + curLen;
        tokens[i].pattern = pattern;
        tokens[i].patternLen = m;
    }
    for (int i = 0; i < threadNumber; ++i) {
        if (pthread_create(&threads[i], NULL, ThreadFunc, &tokens[i])) {
            printf("Error creating thread!\n");
            return -1;
        }
    }
    for (int i = 0; i < threadNumber; ++i){
        if (pthread_join(threads[i], NULL)) {
            printf("Error executing thread!\n");
            return -1;
        }
    }
    int min_index = -1;
    for(int i = 0; i < threadNumber; ++i){
        if((tokens[i].res != -1 && tokens[i].res < min_index) || (min_index == -1 && tokens[i].res != -1)){
            min_index = tokens[i].res;
        }
    }
    printf("%d\n", min_index);
    free(threads);
    free(tokens);
    return 0;
}
