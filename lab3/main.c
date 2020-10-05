#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define THREAD_COUNT 4
#define BUFSIZE 256
#define SUCCESS 0
#define ERROR_THREAD_CREATE 1
#define ERROR_THREAD_JOIN 2

void *thread_body(void *param) {
    char **t;
    for(t = (char **)param; *t != NULL; t++) {
        printf("%s\n", *t);
    }
    return NULL;
}

void exit_error(int err_code, char *message, int err_returns) {
    char buff[BUFSIZE];
    strerror_r(err_code, buff, sizeof(buff));

    fprintf(stderr, "%s: creating thread: %s\n", buff);

    exit(err_returns);
}

int main(int argc, char *argv[]) {
    pthread_t thread[THREAD_COUNT];
    int err_code;
    
    static char *params[THREAD_COUNT][BUFSIZE] = {
		{"T1 l1", "T1 l2", "T1 l3", NULL},
		{"T2 l1", "T2 l2", "T2 l3", "T2 l4", NULL},
		{"T3 l1", "T3 l2", "T3 l3", NULL},
		{"T4 l1", "T4 l2", NULL}
    };

    for (int i = 0; i < THREAD_COUNT; i++) { 
        err_code = pthread_create(&thread[i], NULL, thread_body, params[i]);

        if (err_code != SUCCESS) {
		exit_error(err_code,"%s: creating thread: %s\n", ERROR_THREAD_CREATE);
        }
    }

    for (int i = 0; i < THREAD_COUNT; i++) { 
        err_code = pthread_join(thread[i], NULL);

        if (err_code != SUCCESS) {
	        exit_error(err_code,"%s: joining thread: %s\n", ERROR_THREAD_JOIN);
        }
    }
    
    pthread_exit(NULL);   
    return (EXIT_SUCCESS);
}
