#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
   
pthread_mutex_t mutex_1;
pthread_mutex_t mutex_2;
pthread_mutex_t mutex_3;

#define SUCCESS 0
#define ERROR_TREAD_CREATE 1

void exit_error(int err_code, int err_returns) {
    char buff[256];
    strerror_r(err_code, buff, sizeof(buff));
    fprintf(stderr, "%s: creating thread: %s\n", buff);
    exit(err_returns);
}

void *thread_body(void *param) {
    pthread_mutex_lock(&mutex_1);

    for(int i = 0; i < 5; ++i) {
        pthread_mutex_lock(&mutex_2);

        printf("String %i from child\n", i);

        pthread_mutex_unlock(&mutex_1);

        pthread_mutex_lock(&mutex_3);

        pthread_mutex_unlock(&mutex_2);

        pthread_mutex_lock(&mutex_1);

        pthread_mutex_unlock(&mutex_3);
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_mutex_init(&mutex_1, NULL);
    pthread_mutex_init(&mutex_2, NULL);
    pthread_mutex_init(&mutex_3, NULL);

    pthread_t thread;

    int err_code = pthread_create(&thread, NULL, thread_body, NULL);

    if (err_code != SUCCESS) {
        exit_error(err_code, ERROR_TREAD_CREATE);
    }

    pthread_mutex_lock(&mutex_2);

    for(int i = 0; i < 5; ++i) {
        pthread_mutex_lock(&mutex_3);

        printf("String %i from parent\n", i);

        pthread_mutex_unlock(&mutex_2);

        pthread_mutex_lock(&mutex_1);

        pthread_mutex_unlock(&mutex_3);

        pthread_mutex_lock(&mutex_2);

        pthread_mutex_unlock(&mutex_1);
    }
    
    pthread_mutex_destroy(&mutex_1);
    pthread_mutex_destroy(&mutex_2);
    pthread_mutex_destroy(&mutex_3);
    pthread_exit(NULL);   
}
