#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>

#define SUCCESS 0

#define ERROR_TREAD_CREATE 1
#define ERROR_THREAD_JOIN 2
#define ERROR_THREAD_CANCEL 3

#define ERROR_SEM_INIT 4
#define ERROR_SEM_WAIT 5
#define ERROR_SEM_POST 6
#define ERROR_SEM_DESTROY 7

#define MAX_A 100
#define MAX_B 80
#define MAX_C 60
#define MAX_AB 60
#define MAX_WIDGET 3

sem_t sem[5];
pthread_t creater[4];

void exit_error(int err_code, char *message, int err_returns) {
    char buff[256];
    strerror_r(err_code, buff, sizeof(buff));

    fprintf(stderr, message, buff);

    exit(err_returns);
}

void create_thread(pthread_t *thread, pthread_attr_t *attr, void* func (void *), void *arg) {
    int err_code = pthread_create(thread, attr, func, arg);
    if (err_code != SUCCESS) {
        exit_error(err_code, "%s: error creating thread", ERROR_TREAD_CREATE);
    }
}

void pthread_join_(const pthread_t* pthread, void *attr) {
    int err_code = pthread_join(*pthread, attr);
    if (err_code != SUCCESS) {
        exit_error(err_code, "%s: error pthread_join", ERROR_THREAD_JOIN);
    }
}

void pthread_cancel_(const pthread_t* pthread) {
    int err_code = pthread_cancel(*pthread);
    if (err_code != SUCCESS) {
        exit_error(err_code, "%s: error pthread_cancel", ERROR_THREAD_CANCEL);
    }
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

void *createA(void* arg) {
    for (;;) {
        pthread_testcancel();
        int A;
        sem_getvalue(&sem[0], &A);
        if (A != MAX_A) {
            sleep(1);
            sem_post_(&sem[0]);

            printf("A created\n");
        }
    }
}

void *createB(void* arg) {
    for (;;) {
        pthread_testcancel();
        int B;
        sem_getvalue(&sem[1], &B);
        if (B != MAX_B) {
            sleep(2);
            sem_post_(&sem[1]);

            printf("B created\n");
        }
    }
}

void *createC(void* arg) {
    for (;;) {
        pthread_testcancel();
        int C;
        sem_getvalue(&sem[2], &C);
        if (C != MAX_C) {
            sleep(3);
            sem_post_(&sem[2]);

            printf("C created\n");
        }
    }
}

void *createAB(void* arg) {
    for (;;) {
        pthread_testcancel();
        int AB;
        sem_getvalue(&sem[3], &AB);
        if (AB != MAX_AB) {
            sem_wait_(&sem[0]);
            sem_wait_(&sem[1]);
            sem_post_(&sem[3]);

            printf("AB created\n");
        }
    }
}

void *create_widget() {
    for (;;) {
        pthread_testcancel();
        int widget;
        sem_getvalue(&sem[4], &widget);
        if (widget != MAX_WIDGET) {
            sem_wait_(&sem[3]);
            sem_wait_(&sem[2]);
            sem_post_(&sem[4]);

            printf("widget created\n");
        }
    }
}

void exit_sig() {
    for (int i = 0; i < 4; i++) {
        pthread_cancel_(&creater[i]);
    }
    for (int i = 0; i < 4; i++) {
        pthread_join_(&creater[i], NULL);
    }
    for (int i = 0; i < 5; i++) {
        sem_destroy_(&sem[i]);
    }
    pthread_exit(NULL);
}

int main() {
    sigset(SIGINT, exit_sig);

    for (int i = 0; i < 5; i++) {
        sem_init_(&sem[i], 0, 0);
    }

    create_thread(&creater[0], NULL, createA, NULL);
    create_thread(&creater[1], NULL, createB, NULL);
    create_thread(&creater[2], NULL, createC, NULL);
    create_thread(&creater[3], NULL, createAB, NULL);

    create_widget();
}
