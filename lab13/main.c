#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>

sem_t sem1, sem2;

#define SUCCESS 0
#define ERROR_TREAD_CREATE 1
#define ERROR_SEM_INIT 12
#define ERROR_SEM_WAIT 13
#define ERROR_SEM_POST 14
#define ERROR_SEM_DESTROY 15
#define ERROR_THREAD_JOIN 16

#define NUM_LINES 10

void exit_error(int err_code, char *message, int err_returns) {
    char buff[256];
    strerror_r(err_code, buff, sizeof(buff));

    fprintf(stderr, message, buff);

    exit(err_returns);
}

void pthread_join_(const pthread_t* pthread, void *attr) {
    int err_code = pthread_join(*pthread, attr);
    if (err_code != SUCCESS) {
        exit_error(err_code, "%s: error pthread_join", ERROR_THREAD_JOIN);
    }
}

void create_thread(pthread_t *thread, pthread_attr_t *attr, void* func (void *), void *arg) {
    int err_code = pthread_create(thread, attr, func, arg);
    if (err_code != SUCCESS) {
        exit_error(err_code, "%s: error creating thread", ERROR_TREAD_CREATE);
    }
}

void *childFunction(void *parameter) {
    for(int i = 0; i < NUM_LINES; i++) {
        sem_wait(&sem2);
        printf("String %i from Child\n", i);
        sem_post(&sem1);
    }
    pthread_exit(NULL);
}

void sem_init_(sem_t* sem, int pshared, int value) {
    int err_code = sem_init(sem, pshared, value);
    if (err_code != SUCCESS) {
        exit_error(err_code, "%s: error sem_init", ERROR_SEM_INIT);
    }
}

void sem_wait_(sem_t* sem) {
    int err_code = sem_wait(sem);
    if (err_code != SUCCESS) {
        exit_error(err_code, "%s: error sem_wait", ERROR_SEM_WAIT);
    }
}


void sem_post_(sem_t* sem) {
    int err_code = sem_post(sem);
    if (err_code != SUCCESS) {
        exit_error(err_code, "%s: error sem_post", ERROR_SEM_POST);
    }
}

void sem_destroy_(sem_t* sem) {
    int err_code = sem_destroy(sem);
    if (err_code != SUCCESS) {
        exit_error(err_code, "%s: error sem_destroy", ERROR_SEM_DESTROY);
    }
}

int main(int argc, char* argv[]) {
    pthread_t child;

    sem_init_(&sem1, 0, 0);
    sem_init_(&sem2, 0, 1);

    create_thread(&child, NULL, childFunction, NULL);

    for(int i = 0; i < NUM_LINES; i++) {
        sem_wait_(&sem1);
        printf("String %i from Parent\n", i);
        sem_post_(&sem2);
    }

    pthread_join_(&child, NULL);

    sem_destroy_(&sem1);
    sem_destroy_(&sem2);

    return (EXIT_SUCCESS);
}
