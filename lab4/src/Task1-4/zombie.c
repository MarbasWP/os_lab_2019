#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    pid_t child_pid = fork();

    if (child_pid == 0) {
        // Это дочерний процесс
        printf("Дочерний процесс\n");
        exit(0); // Завершаем дочерний процесс
    } else if (child_pid > 0) {
        // Это родительский процесс
        printf("Родительский процесс\n");
        sleep(10); // Родитель ждет некоторое время
        printf("Родитель завершился\n");
    } else {
        // Ошибка при fork()
        fprintf(stderr, "Ошибка fork()\n");
        return 1;
    }

    return 0;
}
