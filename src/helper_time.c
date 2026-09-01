/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helper_time.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zdaouari <zdaouari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 14:30:45 by zdaouari          #+#    #+#             */
/*   Updated: 2026/08/30 21:23:02 by zdaouari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	get_timesstamp_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000L + tv.tv_usec / 1000L);
}

long	elapsed_ms(t_sim *sim)
{
	return (get_timesstamp_ms() - sim->start_time_ms);
}

struct timespec	calculate_deadline(t_sim *sim, t_dongle *target)
{
	long			remaining_ms;
	struct timeval	now_tv;
	struct timespec	deadline;

	remaining_ms = target->available_at_ms - elapsed_ms(sim);
	if (remaining_ms < 0)
		remaining_ms = 0;
	gettimeofday(&now_tv, NULL);
	deadline.tv_sec = now_tv.tv_sec + (remaining_ms / 1000);
	deadline.tv_nsec = (now_tv.tv_usec * 1000L)
		+ ((remaining_ms % 1000) * 1000000L);
	if (deadline.tv_nsec >= 1000000000L)
	{
		deadline.tv_sec += 1;
		deadline.tv_nsec -= 1000000000L;
	}
	return (deadline);
}

void	wait_on_dongle(t_sim *sim, t_dongle *d1, t_dongle *d2)
{
	struct timespec	deadline;
	t_dongle		*released;
	t_dongle		*target;

	if (d1->available_at_ms >= d2->available_at_ms)
	{
		released = d2;
		target = d1;
	}
	else
	{
		target = d2;
		released = d1;
	}
	deadline = calculate_deadline(sim, target);
	pthread_mutex_unlock(&released->lock);
	pthread_cond_timedwait(&target->cond, &target->lock, &deadline);
	pthread_mutex_lock(&released->lock);
}

void	smarte_sleep(t_sim *sim, long time)
{
	long	start;
	long	remaining;
	long	sleep_ms;

	start = elapsed_ms(sim);
	while (elapsed_ms(sim) - start < time)
	{
		remaining = time - (elapsed_ms(sim) - start);
		if (remaining < 10)
			sleep_ms = remaining;
		else
			sleep_ms = 10;
		usleep(sleep_ms * 1000);
		pthread_mutex_lock(&sim->state_lock);
		if (sim->stop)
		{
			pthread_mutex_unlock(&sim->state_lock);
			break ;
		}
		pthread_mutex_unlock(&sim->state_lock);
	}
}
