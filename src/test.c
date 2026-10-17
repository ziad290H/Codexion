#include <unistd.h>
#include <pthread.h>
#include <stdio.h>


void *function(void *arg)
{
    int *n = arg;
    printf("n : %d", *n);
    *n = 45;
    printf("n : %d", *n);
    return (NULL);
}
int main()
{
    int i = 0;
    pthread_t thread1;
    pthread_t thread2;
    pthread_t thread3;
    int n = 42;

    pthread_create(&thread1, NULL, function, &n);
    pthread_create(&thread2, NULL, function, &n);
    pthread_create(&thread3, NULL, function, &n);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);
    printf("n : %d", n);
}