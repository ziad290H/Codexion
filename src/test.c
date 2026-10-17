#include <unistd.h>
#include <pthread.h>
#include <stdio.h>

typedef struct sumulation
{
    pthread_mutex_t lock;
    int n;
}s_sim;

void *function(void *arg)
{
    s_sim *sim = arg;

    pthread_mutex_lock(&sim->lock);
    printf("thread 1 entred the chat");
    sim->n += 1;
    printf("n : %d\n", sim->n);
    sim->n -= 1;
    printf("the value was returned %d\n", sim->n);
    pthread_mutex_unlock(&sim->lock);
    return (NULL);
}
int main()
{
    s_sim sim;
    pthread_mutex_init(&sim.lock, NULL);
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