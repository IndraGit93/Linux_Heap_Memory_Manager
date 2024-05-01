CC = gcc
CFLAGS = -Wall -Wextra -std=gnu99

mm: memory_manager.c
	$(CC) $(CFLAGS) -o mm memory_manager.c
clean:
	rm -f mm