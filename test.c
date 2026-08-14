#include "codexion.h"
#include <pthread.h>


pthread_mutex_t mutux;

void *coder(int id)
{
    printf("Thread: %lu\n", pthread_self());
    if (pthread_mutex_lock(&mutux)  == 0)
    {
        printf("a coder %d is syuucass", id);
    }
    else if(pthread_mutex_lock(&mutux) != 0)
    {
        printf("a coder %d found a problem in locking it");
    }
    printf("Thread: %lu\n", pthread_self());
    printf("Coder %d started\n", id);
    // printf("befor making sleeep");
    // sleep(1);
    printf("Coder %d finished\n", id);
    if (id != 2)
        pthread_mutex_unlock(&mutux);
    return NULL;
}


int main()
{
    pthread_t p[3];
    int ids[3] = {1, 2, 3};
    int i = 0;


    pthread_mutex_init(&mutux, NULL);
    while (i < 3)
    {
        pthread_create(&p[i], NULL, coder, ids[i]);
        i++;
    }
    i = 0;
    while (i < 3)
    {
        pthread_join(p[i], NULL);
        i++;
    }

    printf("finishing all the threads");


}