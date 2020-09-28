#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define SUCCESS 0
#define ERROR_TREAD_CREATE 1

void print_lines(char *line) {
    for (int i = 0; i < 10; i++) {
        printf("%s\n", line);
    }
}

void exit_error(int err_code) {
    char buff[256];
    strerror_r(err_code, buff, sizeof(buff));

    fprintf(stderr, "%s: creating thread: %s\n", buff);

    exit(ERROR_TREAD_CREATE);
}


void *start_routine(void * param) {
    print_lines("Child\n");

    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t thread;
    int err_code = pthread_create(&thread, NULL, start_routine, NULL);

    if (err_code != SUCCESS) {
        exit_error(err_code);
    }

    print_lines("Parent\n");


    pthread_exit(NULL);
    return (EXIT_SUCCESS);
}
