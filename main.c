#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/sysinfo.h> // Для использования sysinfo
#include <stdlib.h> // Для qsort
#include <fcntl.h>

typedef struct {
    char pid[256];
    char command[256];
    double memory_percent;
} ProcessInfo;

int is_number(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) {
            return 0;
        }
    }
    return 1;
}

int compare_processes(const void *a, const void *b) {
    ProcessInfo *processA = (ProcessInfo *)a;
    ProcessInfo *processB = (ProcessInfo *)b;
    if (processA->memory_percent < processB->memory_percent)
        return 1;
    else if (processA->memory_percent > processB->memory_percent)
        return -1;
    else
        return 0;
}
int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl");
        return -1;
    }
    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1) {
        perror("fcntl");
        return -1;
    }
    return 0;
}
int main() {
    // Установка стандартного ввода в неблокирующий режим
    if (set_nonblocking(STDIN_FILENO) == -1) {
        return 1;
    }

    DIR *dir;
    struct dirent *entry;
    char path[256];

    // Получение общего объема памяти
    struct sysinfo info;
    if (sysinfo(&info) == -1) {
        perror("sysinfo");
        return 1;
    }
    long long total_memory = info.totalram;
    total_memory *= info.mem_unit; // Преобразование в байты
    total_memory /= 1024; // Преобразование в килобайты



    ProcessInfo *processes = NULL;

    while (1) {

        system("clear");

        dir = opendir("/proc");
        if (dir == NULL) {
            perror("opendir");
            return 1;
        }
        int process_count = 0;

        while ((entry = readdir(dir)) != NULL) {
            if (is_number(entry->d_name)) {
                snprintf(path, sizeof(path), "/proc/%s/status", entry->d_name);
                FILE *status_file = fopen(path, "r");
                if (status_file) {
                    char line[256];
                    while (fgets(line, sizeof(line), status_file)) {
                        if (strncmp(line, "VmRSS:", 6) == 0) {
                            long mem_kb;
                            sscanf(line + 7, "%ld", &mem_kb);
                            double percent_used = (double) mem_kb / total_memory * 100;
                            // Расширение массива для хранения информации о процессе
                            processes = realloc(processes, (process_count + 1) * sizeof(ProcessInfo));
                            strncpy(processes[process_count].pid, entry->d_name, sizeof(processes[process_count].pid));

                            snprintf(path, sizeof(path), "/proc/%s/comm", entry->d_name);
                            FILE *comm_file = fopen(path, "r");
                            if (comm_file) {
                                if (fgets(processes[process_count].command, sizeof(processes[process_count].command),
                                          comm_file) != NULL) {
                                    processes[process_count].command[strcspn(processes[process_count].command,
                                                                             "\n")] = 0;
                                }
                                fclose(comm_file);
                            }

                            processes[process_count].memory_percent = percent_used;
                            process_count++;
                            break;
                        }
                    }
                    fclose(status_file);
                }
            }
        }

        closedir(dir);

        // Сортировка массива процессов по использованной памяти
        qsort(processes, process_count, sizeof(ProcessInfo), compare_processes);

        // Вывод отсортированного списка процессов
        for (int i = 0; i < process_count; i++) {
            printf("PID: %s, Command: %s, Memory: %.2f%%\n", processes[i].pid, processes[i].command,
                   processes[i].memory_percent);
        }
        char c;
        if (read(STDIN_FILENO, &c, 1) > 0) {
            if (c == 'q') {
                break; // Выход из цикла
            }
        }
        fflush(stdin);
        sleep(5);
    }
    // Освобождение памяти
    if(processes!=NULL)
    free(processes);

    return 0;
}
