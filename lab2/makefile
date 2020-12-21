FLAGS = -std=c99
СС = gcc
all: main child

main: main.c
	$(CC) $(FLAGS) main.c -o main

child: child.c
	$(CC) $(FLAGS) child.c -o child

clean:
	rm main child
