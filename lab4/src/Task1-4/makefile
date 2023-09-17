CC=gcc
CFLAGS=-I.

all : parallel_min_max process_memory

parallel_min_max : utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o parallel_min_max utils.o find_min_max.o parallel_min_max.c $(CFLAGS)

process_memory : process_memory
	$(CC) -o process_memory process_memory.c $(CFLAGS)

clean :
	rm process_memory parallel_min_max