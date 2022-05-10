CC = gcc
CFALGS = -Wall -Wextra -std=c99 -g

build:
	$(CC) $(CFALGS) *.c -o main

run: main
	./main

clean:
	rm main