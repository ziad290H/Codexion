/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helper_c_build.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zdaouari <zdaouari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 18:41:57 by zdaouari          #+#    #+#             */
/*   Updated: 2026/08/27 18:43:57 by zdaouari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

bool	is_stoped(t_sim *sim)
{
	bool	val;

	pthread_mutex_lock(&sim->state_lock);
	val = sim->stop;
	pthread_mutex_unlock(&sim->state_lock);
	return (val);
}

bool	done_compiling(t_sim *s, t_coder *c)
{
	int	target;

	target = s->cfg.compiles_required;
	if (c->compiles_done >= target)
		return (true);
	return (false);
}

bool	is_start(t_sim *sim)
{
	bool	status;

	pthread_mutex_lock(&sim->state_lock);
	status = sim->start;
	pthread_mutex_unlock(&sim->state_lock);
	return (status);
}

bool	beginning(t_sim *s, t_coder *c)
{
	while (!is_start(s))
		usleep(500);
	if (is_stoped(s))
		return (false);
	if (c -> id % 2 == 0)
		smarte_sleep(s, (s->cfg.time_to_compile + s->cfg.dongle_cooldown) / 2);
	return (true);
}
