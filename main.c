/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zdaouari <zdaouari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 14:31:08 by zdaouari          #+#    #+#             */
/*   Updated: 2026/08/26 14:41:44 by zdaouari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char **argv)
{
    t_config	config;
    int			i;
	t_sim		sim;
    pthread_t	monitor_tid;
	bool		fail_monitor;
	void		*result;
	int			j;

	fail_monitor = false;
	if (argc != 9)
	{
		fprintf(stderr, "Usage: ./codexion num_coders time_to_burnout "
			"time_to_compile time_to_debug time_to_refactor "
			"num_compiles dongle_cooldown scheduler(fifo|edf)\n");
		return (1);
	}

    if (!parsing(argv, &config))
        return (1);
	if (!init_sim(&sim, &config))
	{
		fprintf(stderr, "init failed\n");
		return(1);
	}
    i = 0;
    while(i < sim.cfg.num_coders)
    {
		if (pthread_create(&sim.coders[i].thread, NULL, coder_routine, &sim.coders[i]) != 0)
		{
			fprintf(stderr, "thread creation failed\n");
			pthread_mutex_lock(&sim.state_lock);
			sim.start = 1;
			sim.stop = 1;
			pthread_mutex_unlock(&sim.state_lock);
			j = 0;
			while(j < i)
			{
				pthread_join(sim.coders[j].thread, NULL);
				j++;
			}
			destroy_sim(&sim);
			return (1);
		}
        i++;
    }
	
	if (pthread_create(&monitor_tid, NULL, monitor_routine, &sim) != 0)
	{
		fprintf(stderr, "monitor creation failed\n");
		fail_monitor = true;
		pthread_mutex_lock(&sim.state_lock);
		sim.start = true;
		sim.stop = true;
		pthread_mutex_unlock(&sim.state_lock);
	}

	if (!fail_monitor)
	{	
		pthread_mutex_lock(&sim.state_lock);
		sim.start =  true;
		pthread_mutex_unlock(&sim.state_lock);
	}
	j = 0;
	while (j < i)
	{
		pthread_join(sim.coders[j].thread, NULL);
		j++;
	}
	if (!fail_monitor)
		pthread_join(monitor_tid, NULL);
	destroy_sim(&sim);
	if (fail_monitor)
		return (1);
	return (0);
}
