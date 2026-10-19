#include <unistd.h>
#include <pthread.h>
#include <stdio.h>

typedef struct sumulation
{
    pthread_mutex_t lock;
    pthread_cond_t cond;
    int n;
}s_sim;
int i = 0;
void *function(void *arg)
{
    s_sim *sim = arg;

    pthread_mutex_lock(&sim->lock);
    printf("thread 1 entred the chat\n");
    if (i != 0)
    {
        printf("puting the thread to sleep\n");
        pthread_cond_wait(&sim->cond, &sim->lock);
        printf("the thread woook up \n");
    }
    i += 1;
    sim->n += 1;
    printf("n : %d\n", sim->n);
    sim->n -= 1;
    printf("the value was returned %d\n", sim->n);
    pthread_mutex_unlock(&sim->lock);
    printf("waking up the threads");
    usleep(100);
    pthread_cond_broadcast(&sim->cond);
    return (NULL);
}
int main()
{
    s_sim sim;
    pthread_mutex_init(&sim.lock, NULL);
    pthread_cond_init(&sim.cond, NULL);
    sim.n = 42;
    
    pthread_t car1;
    pthread_t car2;
    pthread_t car3;

    

    pthread_create(&car1, NULL, function, &sim);
    pthread_create(&car2, NULL, function, &sim);
    pthread_create(&car3, NULL, function, &sim);

    pthread_join(car1, NULL);
    pthread_join(car2, NULL);
    pthread_join(car3, NULL);
}