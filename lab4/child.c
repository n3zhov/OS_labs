//
// Created by nezhov on 14.12.2020.
//
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <semaphore.h>
#include <stdbool.h>

const size_t map_size = 4096;

const char * BackingFile = "os_lab4.back";
const char * SemaphoreName = "os_lab4.semaphore";
unsigned AccessPerms = S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH;

void send_parent(caddr_t memptr, sem_t* semptr, const char *empty_string, int res){
    while(true) {
        if (sem_wait(semptr) == 0) {
            if(strcmp(memptr, empty_string) != 0) {
                if (sem_post(semptr) != 0) {
                    perror("sem_post");
                    exit(EXIT_FAILURE);
                }
                continue;
            }
            sprintf(memptr, "%d", res);
            if (sem_post(semptr) != 0) {
                perror("sem_post");
                exit(EXIT_FAILURE);
            }
            break;
        } else {
            perror("sem_wait");
            exit(EXIT_FAILURE);
        }
    }
}



int main(int argc, char **argv){
    int a;
    char c;
    int res = 0;

    if(argc != 2){
        printf("Error! Wrong count of arguments!\n");
        return -1;
    }

    char * empty_string = malloc(sizeof(char) * map_size);
    memset(empty_string, '\0', map_size);

    const char *filename = argv[1];

    FILE *file = fopen(filename, "r");
    int map_fd = shm_open(BackingFile, O_RDWR, AccessPerms);
    if(map_fd < 0){
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    caddr_t memptr = mmap(
            NULL,
            map_size,
            PROT_READ | PROT_WRITE,
            MAP_SHARED,
            map_fd,
            0
    );
    if(memptr == MAP_FAILED){
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    sem_t *semptr = sem_open(SemaphoreName, O_RDWR, AccessPerms, 2);


    if(semptr == SEM_FAILED){
        perror("semptr");
        exit(EXIT_FAILURE);
    }

    if(sem_wait(semptr) != 0){
        perror("sem_wait");
        exit(EXIT_FAILURE);
    }


    while(fscanf(file,"%d%c", &a, &c) != EOF) {
        res += a;
        if(c == '\n') {
            send_parent(memptr, semptr, empty_string, res);
            res = 0;
            continue;
        }
    }


    while(true) {
        if (sem_wait(semptr) == 0) {
            if(strcmp(memptr, empty_string) != 0) {
                if (sem_post(semptr) != 0) {
                    perror("sem_post");
                    exit(EXIT_FAILURE);
                }
                continue;
            }
            memptr[0] = EOF;
            if (sem_post(semptr) != 0) {
                perror("sem_post");
                exit(EXIT_FAILURE);
            }
            break;
        } else {
            perror("sem_wait");
            exit(EXIT_FAILURE);
        }
    }

    if(munmap(memptr, map_size) != 0){
        perror("munmap");
        exit(EXIT_FAILURE);
    }
    sem_close(semptr);

    return EXIT_SUCCESS;
}