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

void	wait_for_dongle(t_sim *sim, t_dongle *d1, t_dongle *d2)
{
	/// waiting for the max time beetwen the 2 deadlines
	long			remaining_ms;
	t_dongle	*dongl_with_farrest_deadline;
	struct timeval	now;
	struct timespec	deadline;

	// how much cooldown time is left, in ms (from your sim's relative clock)
	if (d1->available_at_ms > d2->available_at_ms)
	{
		remaining_ms = d1->available_at_ms - elapsed_ms(sim);
		dongl_with_farrest_deadline = d1; 
	}
	else if (d1->available_at_ms <= d2->available_at_ms)
		remaining_ms = d2->available_at_ms - elapsed_ms(sim);
	//printf("remaining_ms : %ld", remaining_ms);
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
	pthread_cond_timedwait(&dongl_with_farrest_deadline->cond, &dongl_with_farrest_deadline->lock, &deadline);
}


void smarte_sleep(t_sim *sim, long time)
{

	long	start;
	long	remaining;
	long	sleep_ms;

	start = elapsed_ms(sim);

	while(elapsed_ms(sim) - start < time)
	{
		remaining = time - (elapsed_ms(sim) - start);
		if (remaining < 10)
			sleep_ms = remaining;
		else
			sleep_ms = 10;
		usleep(sleep_ms  * 1000);
		if (sim->stop)
			break;
	}
}