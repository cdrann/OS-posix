#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define SUCCESS 0
#define ERROR_THREAD_CREATE 1
#define ERROR_THREAD_JOIN 2

void print_lines(char *line) {
    for (int i = 0; i < 10; i++) {
        printf("%s\n", line);
    }
}

void exit_error(int err_code, char *message, int err_returns) {
    char buff[256];
    strerror_r(err_code, buff, sizeof(buff));

    fprintf(stderr, "%s: creating thread: %s\n", buff);

    exit(err_returns);
}


void *start_routine(void * param) {
    print_lines("Child\n");

    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t thread;
    int err_code = pthread_create(&thread, NULL, start_routine, NULL);

    if (err_code != SUCCESS) {
        exit_error(err_code,"%s: creating thread: %s\n", ERROR_THREAD_CREATE);
    }

    err_code = pthread_join(thread, NULL);
    if (err_code != SUCCESS) {
        exit_error(err_code,"%s: joining thread: %s\n", ERROR_THREAD_JOIN);
    }

    print_lines("Parent\n");

    pthread_exit(NULL);
    return (EXIT_SUCCESS);
}
