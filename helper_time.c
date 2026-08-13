#include "codexion.h"

// stat caounting from the day jan 01 1970
long get_timesstamp_ms(void)
{
    struct timeval tv;
    
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000L + tv.tv_usec /1000L);
}

// get you the time from when the sumulation begun

long elapsed_ms(t_sim *sim)
{
    return (get_timesstamp_ms() - sim->start_time_ms);
}

int main()
{
    long start = get_timesstamp_ms();
    printf(" start : %ld, ", start);
}
// int main()
// {
//     long start = get_timesstamp_ms();
//     usleep(25000);
//     printf("time passed = %ld", get_timesstamp_ms() - start);
// }