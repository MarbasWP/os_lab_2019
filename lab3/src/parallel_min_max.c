#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  bool with_files = false;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {
        {"seed", required_argument, 0, 0},
        {"array_size", required_argument, 0, 0},
        {"pnum", required_argument, 0, 0},
        {"by_files", no_argument, 0, 'f'},
        {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            if (seed <= 0) {
              printf("seed is a positive number\n");
              return 1;
            }
            break;
          case 1:
            array_size = atoi(optarg);
            if (array_size <= 0) {
              printf("array_size is a positive number\n");
              return 1;
            }
            break;
          case 2:
            pnum = atoi(optarg);
            if (pnum <= 0) {
              printf("pnum is a positive number\n");
              return 1;
            }
            break;
          case 3:
            with_files = true;
            break;

          default:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" [--by_files]\n",
           argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0;

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  int step = array_size / pnum;

  FILE *output_file = NULL;
  int (*pipe_fds)[2] = NULL;

  if (with_files) {
    output_file = fopen("output.txt", "w");
  } else {
    pipe_fds = malloc(sizeof(int[2]) * pnum);
  }

  for (int i = 0; i < pnum; i++) {
    if (!with_files) {
      if (pipe(pipe_fds[i]) == -1) {
        perror("pipe");
        return 1;
      }
    }

    pid_t child_pid = fork();
    if (child_pid >= 0) {
      // successful fork
      active_child_processes += 1;
      if (child_pid == 0) {
        // child process

        // Вычисление начальных и конечных индексов для каждого процесса
        int chunk_size = array_size / pnum;
        int start_index = i * chunk_size;
        int end_index = (i == (pnum - 1)) ? array_size : start_index + chunk_size;

        // Вычисление минимума и максимума для части массива
        struct MinMax local_min_max = GetMinMax(array, start_index, end_index);

        // Сохранение результатов в соответствующих местах (файлы или пайпы)
        if (with_files) {
          fprintf(output_file, "%d %d\n", local_min_max.min, local_min_max.max);
        } else {
          write(pipe_fds[i][1], &local_min_max, sizeof(struct MinMax));
        }
        return 0;
      }
    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }

  while (active_child_processes > 0) {
    int status;
    wait(&status);
    if (WIFEXITED(status)) {
      active_child_processes -= 1;
    }
  }

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  for (int i = 0; i < pnum; i++) {
    int min = INT_MAX;
    int max = INT_MIN;

    if (with_files) {
      // Чтение из файлов
      FILE *file = fopen("output.txt", "r");
      if (file == NULL) {
        perror("fopen");
        return 1;
      }
      fscanf(file, "%d %d", &min, &max);
      fclose(file);
    } else {
      // Чтение из pipe
      struct MinMax local_min_max;
      read(pipe_fds[i][0], &local_min_max, sizeof(struct MinMax));
      min = local_min_max.min;
      max = local_min_max.max;
    }

    if (min < min_max.min) min_max.min = min;
    if (max > min_max.max) min_max.max = max;
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);

  if (with_files) {
    fclose(output_file);
  } else {
    free(pipe_fds);
  }

  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  return 0;
}
