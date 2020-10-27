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
#define ERROR_MUTEXATTR_INIT 2
#define ERROR_MUTEXATTR_SETTYPE 3
#define ERROR_MUTEX_INIT 4
#define ERROR_MUTEX_LOCK 5
#define ERROR_MUTEX_UNLOCK 6
#define ERROR_MUTEX_DESTROY 7

#define NUM_PARENT_LINES 10
#define NUM_CHILD_LINES 10


void exit_error(int err_code, char *message, int err_returns) {
    char buff[256];
    strerror_r(err_code, buff, sizeof(buff));

    fprintf(stderr, message, buff);

    exit(err_returns);
}

void mutex_lock(pthread_mutex_t* mutex) {
    int err_code = pthread_mutex_lock(mutex);
    if(err_code != SUCCESS) {
        exit_error(err_code, "%s: cannot lock mutex", ERROR_MUTEX_LOCK);
    }
}


void mutex_unlock(pthread_mutex_t* mutex) {
    int err_code = pthread_mutex_unlock(mutex);
    if(err_code != SUCCESS) {
        exit_error(err_code, "%s: cannot unlock mutex", ERROR_MUTEX_UNLOCK);
    }
}


void mutex_destroy(pthread_mutex_t* mutex) {
    int err_code;
    if((err_code = pthread_mutex_destroy(mutex)) != SUCCESS) {
        exit_error(err_code, "%s: cannot destroy mutex", ERROR_MUTEX_DESTROY);
    }
}

void errorcheck_mutex_init(pthread_mutex_t* mutex) {
    pthread_mutexattr_t attr_1;
    int err_code;

    err_code = pthread_mutexattr_init(&attr_1);
    if(err_code != SUCCESS) {
        exit_error(err_code, "%s: cannot init mutex attr", ERROR_MUTEXATTR_INIT);
    }

    err_code = pthread_mutexattr_settype(&attr_1, PTHREAD_MUTEX_ERRORCHECK);
    if(err_code != SUCCESS) {
        exit_error(err_code, "%s: cannot set mutex attr type", ERROR_MUTEXATTR_SETTYPE);
    }

    err_code = pthread_mutex_init(mutex, &attr_1);
    if(err_code != SUCCESS) {
        exit_error(err_code, "%s: cannot init mutex", ERROR_MUTEX_INIT);
    }
}

void create_thread(pthread_t *thread, pthread_attr_t *attr, void *func, void *arg) {
    int err_code = pthread_create(thread, attr, func, arg);
    if (err_code != SUCCESS) {
        exit_error(err_code, "%s: error creating thread", ERROR_TREAD_CREATE);
    }
}

void *thread_body(void *param) {
    mutex_lock(&mutex_1);

    for(int i = 0; i < NUM_CHILD_LINES; ++i) {
        mutex_lock(&mutex_2);
        printf("String %i from child\n", i);
        mutex_unlock(&mutex_1);
        mutex_lock(&mutex_3);
        mutex_unlock(&mutex_2);
        mutex_lock(&mutex_1);
        mutex_unlock(&mutex_3);
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    errorcheck_mutex_init(&mutex_1);
    errorcheck_mutex_init(&mutex_2);
    errorcheck_mutex_init(&mutex_3);

    pthread_t thread;
    create_thread(&thread, NULL, &thread_body, NULL);

    mutex_lock(&mutex_2);

    for(int i = 0; i < NUM_PARENT_LINES; ++i) {
        mutex_lock(&mutex_3);
        printf("String %i from parent\n", i);
        mutex_unlock(&mutex_2);
        mutex_lock(&mutex_1);
        mutex_unlock(&mutex_3);
        mutex_lock(&mutex_2);
        mutex_unlock(&mutex_1);
    }

    mutex_destroy(&mutex_1);
    mutex_destroy(&mutex_2);
    mutex_destroy(&mutex_3);

    pthread_exit(NULL);
}
