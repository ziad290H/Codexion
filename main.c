/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zdaouari <zdaouari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 14:31:08 by zdaouari          #+#    #+#             */
/*   Updated: 2026/08/28 09:57:06 by zdaouari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	creat_threads(t_sim	*sim)
{
	int	i;
	int	j;

	i = 0;
	while (i < sim->cfg.num_coders)
	{
		if (pthread_create(&sim->coders[i].thread,
				NULL, coder_routine, &sim->coders[i]) != 0)
		{
			fprintf(stderr, "thread creation failed\n");
			pthread_mutex_lock(&sim->state_lock);
			sim->start = 1;
			sim->stop = 1;
			pthread_mutex_unlock(&sim->state_lock);
			j = 0;
			while (j < i)
			{
				pthread_join(sim->coders[j++].thread, NULL);
			}
			destroy_sim(sim);
			return (-1);
		}
		i++;
	}
	return (i);
}

pthread_t	creat_monitor(t_sim *sim)
{
	pthread_t	monitor_tid;

	monitor_tid = 0;
	if (pthread_create(&monitor_tid, NULL, monitor_routine, sim) != 0)
	{
		fprintf(stderr, "monitor creation failed\n");
		sim->fail_monitor = true;
		pthread_mutex_lock(&sim->state_lock);
		sim->start = true;
		sim->stop = true;
		pthread_mutex_unlock(&sim->state_lock);
	}
	sim->start_time_ms = get_timesstamp_ms();
	return (monitor_tid);
}

void	joining_and_freeing(t_sim *sim, int i, pthread_t monitor_tid)
{
	int	j;

	if (!sim->fail_monitor)
	{
		pthread_mutex_lock(&sim->state_lock);
		sim->start = true;
		pthread_mutex_unlock(&sim->state_lock);
	}
	j = 0;
	while (j < i)
	{
		pthread_join(sim->coders[j].thread, NULL);
		j++;
	}
	if (!sim->fail_monitor)
		pthread_join(monitor_tid, NULL);
	destroy_sim(sim);
}

int	main(int argc, char **argv)
{
	t_config	config;
	int			i;
	t_sim		sim;
	pthread_t	monitor_tid;

	if (argc != 9)
	{
		fprintf(stderr, "wrong usage of arguments");
		return (1);
	}
	if (!parsing(argv, &config))
		return (1);
	if (!init_sim(&sim, &config))
	{
		fprintf(stderr, "init failed\n");
		return (1);
	}
	i = creat_threads(&sim);
	if (i == -1)
		return (1);
	monitor_tid = creat_monitor(&sim);
	joining_and_freeing(&sim, i, monitor_tid);
	if (sim.fail_monitor)
		return (1);
}
