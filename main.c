#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <locale.h>

int main() {
    setlocale(LC_ALL, "");

    pid_t pid = fork();

    if (pid == 0) {
        execlp("ps", "ps", "-e", "-o", "pid,ppid,cmd,%cpu,%mem,time", "--sort=-%cpu", (char *)NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    wait(NULL);

    printf("Press 'q' to quit: ");
    int ch;
    while ((ch = getchar()) != 'q') {

        while (getchar() != '\n');
    }

    kill(pid, SIGTERM);
    wait(NULL);

    return 0;
}
