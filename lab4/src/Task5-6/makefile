CC = gcc
CFLAGS = -pthread -lm

all: parallel_sum

parallel_sum : utils.o parallel_sum
	$(CC) -o parallel_sum utils.o parallel_sum.c $(CFLAGS)

clean:
	rm -f parallel_sum