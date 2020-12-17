#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

pthread_mutex_t mtx;
pthread_cond_t cond;
int turnToWork = 0;

#define SUCCESS 0

#define ERROR_TREAD_CREATE 1
#define ERROR_THREAD_JOIN 12
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
#define NUM_THREADS 3

typedef struct someArgs_tag {
    const char *threadName;
    int id;
} someArgs_t;

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

void pthread_join_(const pthread_t* pthread, void *attr) {
    int err_code = pthread_join(*pthread, attr);
    if (err_code != SUCCESS) {
        exit_error(err_code, "%s: error pthread_join", ERROR_THREAD_JOIN);
    }
}

void *printLines(void *params) {
    someArgs_t* arg = (someArgs_t *) params;

    mutex_lock(&mtx);
    for(int i = 0; i < NUM_LINES; i++) {
        while (arg->id != turnToWork) {
            cond_wait(&cond, &mtx);
        }

        printf("String %i from %s\n", i, arg->threadName);

        turnToWork = (turnToWork + 1) % NUM_THREADS;
        cond_signal(&cond);
    }
    mutex_unlock(&mtx);

    return NULL;
}

int main(int argc, char* argv[]) {
    errorcheck_mutex_init(&mtx);
    cond_init(&cond, NULL);

    pthread_t children[NUM_THREADS];
    someArgs_t args[NUM_THREADS];
    const char *threadNames[] = {"Parent", "Child", "Child2"};
    for(int i = 0; i < NUM_THREADS; i++) {
        args[i].id = i;
        args[i].threadName = threadNames[i];
        
        if(i != 0) {
            create_thread(&children[i - 1], NULL, printLines, (void *) &args[i]);
        }
    }
    printLines((void *)&args[0]);

    for(int i = 1; i < NUM_THREADS; i++) {
        pthread_join_(&children[i], NULL);
    }
    mutex_destroy(&mtx);
    cond_destroy(&cond);
    pthread_exit(NULL);
}
