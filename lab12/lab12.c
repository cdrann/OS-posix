#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

pthread_mutex_t mtx;
pthread_cond_t cond;
int worked_first = 1;

#define SUCCESS 0

#define ERROR_TREAD_CREATE 1
#define ERROR_MUTEXATTR_INIT 2
#define ERROR_MUTEXATTR_SETTYPE 3
#define ERROR_MUTEX_INIT 4
#define ERROR_MUTEX_LOCK 5
#define ERROR_MUTEX_UNLOCK 6
#define ERROR_MUTEX_DESTROY 7

#define ERROR_COND_INIT 8
#define ERROR_COND_SIGNAL 9
#define ERROR_COND_WAIT 10
#define ERROR_COND_DESTROY 11

#define NUM_LINES 10

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

void cond_destroy(pthread_cond_t* cond) {
    int err_code = pthread_cond_destroy(cond);
    if (err_code != SUCCESS) {
        exit_error(err_code, "%s: cannot desroy cond", ERROR_COND_DESTROY);
    }
}

void create_thread(pthread_t *thread, pthread_attr_t *attr, void* func (void *), void *arg) {
    int err_code = pthread_create(thread, attr, func, arg);
    if (err_code != SUCCESS) {
        exit_error(err_code, "%s: error creating thread", ERROR_TREAD_CREATE);
    }
}

void *printLines(void *parameter) {
    char *threadName = (char *) parameter;

    mutex_lock(&mtx);
    for(int i = 0; i < NUM_LINES; i++) {
        while (worked_first) {
            cond_wait(&cond, &mtx);
        }

        printf("String %i from %s\n", i, threadName);

        worked_first = 1;
        cond_signal(&cond);
    }
    mutex_unlock(&mtx);

    return NULL;
}

int main(int argc, char* argv[]) {
    pthread_t child;

    errorcheck_mutex_init(&mtx);
    cond_init(&cond, NULL);

    create_thread(&child, NULL, printLines, (void *) "Child");

    mutex_lock(&mtx);
    for(int i = 0; i < NUM_LINES; i++) {
        while (!worked_first) {
            cond_wait(&cond, &mtx);
        }

        printf("String %i from %s\n", i, "Parent");

        worked_first = 0;
        cond_signal(&cond);
    }
    mutex_unlock(&mtx);

    pthread_join(child, NULL);

    mutex_destroy(&mtx);
    cond_destroy(&cond);

    pthread_exit(NULL);
}
