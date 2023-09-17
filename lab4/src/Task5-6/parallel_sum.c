#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

#include "utils.h"

struct SumArgs {
    int *array;
    int begin;
    int end;
};

int Sum(const struct SumArgs *args) {
    int sum = 0;
    for (int i = args->begin; i < args->end; i++) {
        sum += args->array[i];
    }
    return sum;
}

void *ThreadSum(void *args) {
    struct SumArgs *sum_args = (struct SumArgs *)args;
    return (void *)(intptr_t)Sum(sum_args);
}

int main(int argc, char **argv) {
    if (argc != 7 || strcmp(argv[1], "--threads_num") != 0 || strcmp(argv[3], "--seed") != 0 || strcmp(argv[5], "--array_size") != 0) {
        printf("Usage: %s --threads_num <num> --seed <num> --array_size <num>\n", argv[0]);
        return 1;
    }

    uint32_t threads_num = atoi(argv[2]);
    uint32_t seed = atoi(argv[4]);
    uint32_t array_size = atoi(argv[6]);

    if (threads_num <= 0 || array_size <= 0) {
        printf("threads_num and array_size must be positive integers.\n");
        return 1;
    }

    int *array = (int *)malloc(array_size * sizeof(int));
    if (array == NULL) {
        perror("Failed to allocate memory for the array");
        return 1;
    }

    GenerateArray(array, array_size, seed);

    pthread_t threads[threads_num];
    struct SumArgs args[threads_num];
    int step = array_size / threads_num;

    for (uint32_t i = 0; i < threads_num; i++) {
        args[i].array = array;
        args[i].begin = i * step;
        args[i].end = (i == threads_num - 1) ? array_size : (i + 1) * step;

        if (pthread_create(&threads[i], NULL, ThreadSum, &args[i]) != 0) {
            perror("Failed to create thread");
            return 1;
        }
    }

    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);

    int total_sum = 0;
    for (uint32_t i = 0; i < threads_num; i++) {
        int sum = 0;
        pthread_join(threads[i], (void **)&sum);
        total_sum += sum;
    }

    gettimeofday(&end_time, NULL);
    double elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000.0;
    elapsed_time += (end_time.tv_usec - start_time.tv_usec) / 1000.0;

    free(array);
    printf("Total: %d\n", total_sum);
    printf("Elapsed time: %fms\n", elapsed_time);
    return 0;
}
