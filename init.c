#include "codexion.h"

bool init_dongles(t_sim *sim)
{
    int	i;

	sim->dongles = malloc(sizeof(t_dongle) * sim->cfg.num_coders);
	if (!sim->dongles)
		return (false);
	i = 0;
	while (i < sim->cfg.num_coders)
	{
		if(pthread_mutex_init(&sim->dongles[i].lock, NULL) != 0)
			return (false);
		if (pthread_cond_init(&sim->dongles[i].cond, NULL) != 0)
			return (false);
		sim->dongles[i].in_use = false;
		sim->dongles[i].available_at_ms = 0;
		i++;
	}
	return (true);
}

bool init_coders(t_sim *sim)
{
	int	i;
	int n;

	n = sim ->cfg.num_coders;
	sim->coders = malloc(sizeof(t_coder) * n);
	if (!sim->coders)
	return (false);
	i = 0;
	while (i < sim->cfg.num_coders)
	{
		sim->coders[i].id = 0 + 1;
		sim->coders[i].compiles_done = 0;
		sim->coders[i].last_compile_start = 0;
		sim->coders[i].sim = sim;

		sim->coders[i].left = &sim->dongles[i];
		sim->coders[i].right = &sim->dongles[(i+ 1) % n];
		i++;
	}
	return (true);
}

bool init_sim(t_sim *sim, t_config *cfg)
{
	sim->cfg = *cfg;
	sim->stop = false;
	sim->someone_burned_out = false;
	sim->start_time_ms = get_timestamp_ms();
	sim->dongles = NULL;
	sim->coders = NULL;
	if (pthread_mutex_init(&sim->log_lock, NULL) != 0)
		return (false);
	if (pthread_mutex_init(&sim->state_lock, NULL) != 0)
		return (false);
	if (!init_dongles(sim))
		return (false);
	if (!init_coders(sim))
		return (false);
	return (true);
}
bool	init_dongles(t_sim *sim)
{
	int	i;

	sim->dongles = malloc(sizeof(t_dongle) * sim->cfg.num_coders);
	if (!sim->dongles)
		return (false);
	i = 0;
	while (i < sim->cfg.num_coders)
	{
		if (pthread_mutex_init(&sim->dongles[i].lock, NULL) != 0)
			return (false);
		if (pthread_cond_init(&sim->dongles[i].cond, NULL) != 0)
			return (false);
		sim->dongles[i].in_use = false;
		sim->dongles[i].available_at_ms = 0;
		i++;
	}
	return (true);
}

bool	init_coders(t_sim *sim)
{
	int	i;
	int	n;

	n = sim->cfg.num_coders;
	sim->coders = malloc(sizeof(t_coder) * n);
	if (!sim->coders)
		return (false);
	i = 0;
	while (i < n)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].compiles_done = 0;
		sim->coders[i].last_compile_start = 0;
		sim->coders[i].sim = sim;
		// coder i (0-indexed) has left = dongle[i], right = dongle[(i+1)%n]
		sim->coders[i].left = &sim->dongles[i];
		sim->coders[i].right = &sim->dongles[(i + 1) % n];
		i++;
	}
	return (true);
}

bool	init_sim(t_sim *sim, t_config *cfg)
{
	sim->cfg = *cfg;
	sim->stop = false;
	sim->someone_burned_out = false;
	sim->start_time_ms = get_timestamp_ms();
	sim->dongles = NULL;
	sim->coders = NULL;
	if (pthread_mutex_init(&sim->log_lock, NULL) != 0)
		return (false);
	if (pthread_mutex_init(&sim->state_lock, NULL) != 0)
		return (false);
	if (!init_dongles(sim))
		return (false);
	if (!init_coders(sim))
		return (false);
	return (true);
}

void	destroy_sim(t_sim *sim)
{
	int	i;

	if (sim->dongles)
	{
		i = 0;
		while (i < sim->cfg.num_coders)
		{
			pthread_mutex_destroy(&sim->dongles[i].lock);
			pthread_cond_destroy(&sim->dongles[i].cond);
			i++;
		}
		free(sim->dongles);
	}
	if (sim->coders)
		free(sim->coders);
	pthread_mutex_destroy(&sim->log_lock);
	pthread_mutex_destroy(&sim->state_lock);
}