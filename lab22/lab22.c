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
#define ERROR_MUTEXATTR_INIT 8
#define ERROR_MUTEXATTR_SETTYPE 9
#define ERROR_MUTEX_INIT 10
#define ERROR_MUTEX_LOCK 11
#define ERROR_MUTEX_UNLOCK 12
#define ERROR_MUTEX_DESTROY 13
#define ERROR_COND_INIT 14
#define ERROR_COND_SIGNAL 15
#define ERROR_COND_WAIT 16
#define ERROR_COND_DESTROY 17
#define ERROR_COND_BROADCAST 18

#define MAX_A 20
#define MAX_B 20
#define MAX_C 20
#define MAX_AB 20
#define MAX_WIDGET 20

//A B C AB widget
sem_t sem[5];
pthread_t creater[4];
pthread_cond_t cond[4];
pthread_mutex_t mtx[4];

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

void cond_init(pthread_cond_t* cond, pthread_condattr_t *attr) {
    int err_code = pthread_cond_init(cond, attr);
    if (err_code != SUCCESS) {
        exit_error(err_code, "%s: cannot init cond", ERROR_COND_INIT);
    }
}

void cond_signal(pthread_cond_t* cond) {
    int err_code = pthread_cond_signal(cond);
    if (err_code != SUCCESS) {
        exit_error(err_code, "%s: cond_signal error", ERROR_COND_SIGNAL);
    }
}

void cond_wait(pthread_cond_t* cond, pthread_mutex_t* mutex) {
    int err_code = pthread_cond_wait(cond, mutex);
    if (err_code != SUCCESS) {
        exit_error(err_code, "%s: cond_wait error", ERROR_COND_WAIT);
    }
}

void cond_broadcast(pthread_cond_t* cond) {
    int err_code = pthread_cond_broadcast(cond);
    if (err_code != SUCCESS) {
        exit_error(err_code, "%s: cond_wait error", ERROR_COND_BROADCAST);
    }
}

void cond_destroy(pthread_cond_t* cond) {
    int err_code = pthread_cond_destroy(cond);
    if (err_code != SUCCESS) {
        exit_error(err_code, "%s: cannot desroy cond", ERROR_COND_DESTROY);
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
        if (A <= MAX_A) {
            sleep(1);
            sem_post_(&sem[0]);

            printf("A created\n");
        } else {
		printf("A is full. Wait\n");
		mutex_lock(&mtx[0]);
		cond_wait(&cond[0], &mtx[0]);
		mutex_unlock(&mtx[0]);
	}
    }
}

void *createB(void* arg) {
    for (;;) {
        pthread_testcancel();
        int B;
        sem_getvalue(&sem[1], &B);
        if (B <= MAX_B) {
            sleep(2);
            sem_post_(&sem[1]);

            printf("B created\n");
        }  else {
		printf("B is full. Wait\n");
		mutex_lock(&mtx[1]);
		cond_wait(&cond[1], &mtx[1]);
		mutex_unlock(&mtx[1]);
	}
    }
}

void *createC(void* arg) {
    for (;;) {
        pthread_testcancel();
        int C;
        sem_getvalue(&sem[2], &C);
        if (C <= MAX_C) {
            sleep(3);
            sem_post_(&sem[2]);

            printf("C created\n");
        } else {
		printf("C is full. Wait\n");
		mutex_lock(&mtx[2]);
		cond_wait(&cond[2], &mtx[2]);
		mutex_unlock(&mtx[2]);
	}
    }
}

void *createAB(void* arg) {
    for (;;) {
        pthread_testcancel();
        int AB;
        sem_getvalue(&sem[3], &AB);
        if (AB <= MAX_AB) {
            sem_wait_(&sem[0]);
	    cond_signal(&cond[0]);

            sem_wait_(&sem[1]);
            cond_signal(&cond[1]);

            sem_post_(&sem[3]);

            printf("AB created\n");
        } else {
	    printf("AB is full. Wait\n");
	    mutex_lock(&mtx[3]);
	    cond_wait(&cond[3], &mtx[3]);
	    mutex_unlock(&mtx[3]);
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
	    cond_signal(&cond[3]);

            sem_wait_(&sem[2]);
	    cond_signal(&cond[2]);
	    
            sem_post_(&sem[4]);

            printf("widget created\n");
        } else {
	    printf("widget is full.\n");
        }
    }
}

void exit_sig() {
    for (int i = 0; i < 4; i++) {
        pthread_cancel_(&creater[i]);
    }
    for (int i = 0; i < 4; i++) {
        pthread_join_(&creater[i], NULL);
        mutex_destroy(&mtx[i]);
        cond_destroy(&cond[i]);
    }
    for (int i = 0; i < 5; i++) {
        sem_destroy_(&sem[i]);
    }
    
    pthread_exit(NULL);
}

int main() {
    printf("Starting... To finish use ctrl + C.\n");

    sigset(SIGINT, exit_sig);

    for (int i = 0; i < 5; i++) {
        sem_init_(&sem[i], 0, 0);
    }

    for (int i = 0; i < 4; i++) {
        errorcheck_mutex_init(&mtx[i]);
        cond_init(&cond[i], NULL);
    }
    
    create_thread(&creater[0], NULL, createA, NULL);
    create_thread(&creater[1], NULL, createB, NULL);
    create_thread(&creater[2], NULL, createC, NULL);
    create_thread(&creater[3], NULL, createAB, NULL);

    create_widget();
}
