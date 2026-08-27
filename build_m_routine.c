/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_m_routine.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zdaouari <zdaouari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 14:30:18 by zdaouari          #+#    #+#             */
/*   Updated: 2026/08/27 20:56:53 by zdaouari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	*monitor_routine(void *arg)
{
	t_sim	*sim;
	int		i;

	sim = (t_sim *)arg;
	while (true)
	{
		if (check_burnout(sim))
			break ;
		if (check_all_compiled(sim))
			break ;
		usleep(1000);
	}
	i = 0;
	while (i < sim->cfg.num_coders)
	{
		pthread_mutex_lock(&sim->dongles[i].lock);
		pthread_cond_broadcast(&sim->dongles[i].cond);
		pthread_mutex_unlock(&sim->dongles[i].lock);
		i++;
	}
	return (NULL);
}

bool	check_burnout(t_sim *sim)
{
	int		i;
	long	now;
	long	deadline;	

	i = 0;
	pthread_mutex_lock(&sim->state_lock);
	while (i < sim->cfg.num_coders)
	{
		now = elapsed_ms(sim);
		deadline = sim->coders[i].last_compile_start + sim->cfg.time_to_burnout;
		if (now > deadline)
		{
			sim->stop = true;
			sim->someone_burned_out = true;
			pthread_mutex_unlock(&sim->state_lock);
			log_state(sim, sim->coders[i].id, "burned out");
			return (true);
		}
		i++;
	}
	pthread_mutex_unlock(&sim->state_lock);
	return (false);
}

bool	check_all_compiled(t_sim *sim)
{
	int		i;
	long	target;

	pthread_mutex_lock(&sim->state_lock);
	i = 0;
	target = sim->cfg.compiles_required;
	while (i < sim->cfg.num_coders)
	{
		if (target > sim->coders[i].compiles_done)
		{
			pthread_mutex_unlock(&sim->state_lock);
			return (false);
		}
		i++;
	}
	sim->stop = true;
	pthread_mutex_unlock(&sim->state_lock);
	return (true);
}
