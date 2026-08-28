/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helper2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zdaouari <zdaouari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 20:22:14 by zdaouari          #+#    #+#             */
/*   Updated: 2026/08/28 10:31:02 by zdaouari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	log_state(t_sim *sim, int coder_id, const char *msg)
{
	pthread_mutex_lock(&sim->state_lock);
	if (sim->stop && strcmp(msg, "burned out") == 0)
	{
		printf("%ld %d %s\n", elapsed_ms(sim), coder_id, msg);
		pthread_mutex_unlock(&sim->state_lock);
		return ;
	}
		
	if (!sim->stop)
		fprintf(stdout, "%ld %d %s\n", elapsed_ms(sim), coder_id, msg);
	pthread_mutex_unlock(&sim->state_lock);

}

int	check_dongles(t_dongle *d1, t_dongle *d2)
{
	if (d1->in_use && d2->in_use)
		return (1);
	else if (d1->in_use)
		return (2);
	else if (d2->in_use)
		return (3);
	return (0);
}

bool	is_cooldown_active(t_dongle *d1, t_dongle *d2, long now)
{
	return (now < d1->available_at_ms || now < d2->available_at_ms);
}

void	handle_status(int status, t_dongle *d1, t_dongle *d2)
{
	if (status == 1 || status == 2)
	{
		pthread_mutex_unlock(&d2->lock);
		pthread_cond_wait(&d1->cond, &d1->lock);
		pthread_mutex_lock(&d2->lock);
	}
	else if (status == 3)
	{
		pthread_mutex_unlock(&d1->lock);
		pthread_cond_wait(&d2->cond, &d2->lock);
		pthread_mutex_lock(&d1->lock);
	}
}
