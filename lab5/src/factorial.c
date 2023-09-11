#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

// Глобальные переменные
pthread_mutex_t mutex;
unsigned long long result = 1; // Результат факториала
int k, pnum, mod;

// Функция, которую выполняют потоки для вычисления факториала
void *calculate_factorial(void *arg) {
    int thread_num = *(int *)arg;
    unsigned long long local_result = 1;
    
    for (int i = thread_num + 1; i <= k; i += pnum) {
        local_result = (local_result * i) % mod;
    }

    pthread_mutex_lock(&mutex);
    result = (result * local_result) % mod;
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 7) {
        printf("Usage: %s -k <number> --pnum=<number> --mod=<number>\n", argv[0]);
        return 1;
    }

    // Парсинг аргументов командной строки
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-k") == 0) {
            k = atoi(argv[i + 1]);
        } else if (strcmp(argv[i], "--pnum") == 0) {
            pnum = atoi(argv[i + 1]);
        } else if (strcmp(argv[i], "--mod") == 0) {
            mod = atoi(argv[i + 1]);
        }
    }

    if (k <= 0 || pnum <= 0 || mod <= 0) {
        printf("Invalid input values. All values must be positive integers.\n");
        return 1;
    }

    pthread_t *threads = (pthread_t *)malloc(pnum * sizeof(pthread_t));
    int *thread_ids = (int *)malloc(pnum * sizeof(int));

    pthread_mutex_init(&mutex, NULL);

    // Создание потоков
    for (int i = 0; i < pnum; i++) {
        thread_ids[i] = i;
        if (pthread_create(&threads[i], NULL, calculate_factorial, &thread_ids[i]) != 0) {
            perror("pthread_create");
            return 1;
        }
    }

    // Ожидание завершения всех потоков
    for (int i = 0; i < pnum; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join");
            return 1;
        }
    }

    // Вывод результата
    if (mod == 1) {
        printf("%d! mod 1 = 0\n", k);
    } else {
        printf("%d! mod %d = %llu\n", k, mod, result);
    }

    pthread_mutex_destroy(&mutex);
    free(threads);
    free(thread_ids);

    return 0;
}
