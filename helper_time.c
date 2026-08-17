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
	//fprintf(stdout, "sim->start_time_ms) = %ld", sim->start_time_ms);
    return (get_timesstamp_ms() - sim->start_time_ms);
}

void	wait_for_dongle(t_sim *sim, t_dongle *d)
{
	long			remaining_ms;
	struct timeval	now;
	struct timespec	deadline;

	// how much cooldown time is left, in ms (from your sim's relative clock)
	remaining_ms = d->available_at_ms - elapsed_ms(sim);
	if (remaining_ms < 0)
		remaining_ms = 0;

	// get real wall-clock "now"
	gettimeofday(&now, NULL);

	// deadline = real now + remaining cooldown, converted to timespec
	deadline.tv_sec = now.tv_sec + (remaining_ms / 1000); //  this represent secodns
	deadline.tv_nsec = (now.tv_usec * 1000L) + ((remaining_ms % 1000) * 1000000L); // this repressent nanoseceonds

	// handle nanosecond overflow (must stay < 1,000,000,000)
	if (deadline.tv_nsec >= 1000000000L)
	{
		deadline.tv_sec += 1;
		deadline.tv_nsec -= 1000000000L;
	}
    //fprintf(stdout, "deadline : %ld", deadline.tv_sec);
	pthread_cond_timedwait(&d->cond, &d->lock, &deadline);
}

// int main()
// {
//     long start = get_timesstamp_ms();
//     printf(" start : %ld, ", start);
// }
// int main()
// {
//     long start = get_timesstamp_ms();
//     usleep(25000);
//     printf("time passed = %ld", get_timesstamp_ms() - start);
// }