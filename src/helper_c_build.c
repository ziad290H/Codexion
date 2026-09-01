/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helper_c_build.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zdaouari <zdaouari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 14:29:52 by zdaouari          #+#    #+#             */
/*   Updated: 2026/08/30 20:11:49 by zdaouari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

bool	check_periority(t_dongle *d1, t_dongle *d2, int coder_id)
{
	int	per_d1;
	int	per_d2;

	per_d1 = peek_the_min(&d1->waiting).coder_id;
	per_d2 = peek_the_min(&d2->waiting).coder_id;
	return (per_d1 == coder_id && per_d2 == coder_id);
}

void	wait_till_available(t_sim *sim, t_dongle *d1, t_dongle *d2, t_coder *c)
{
	long	now;
	int		status;

	now = elapsed_ms(sim);
	while (!is_stoped(sim))
	{
		status = check_dongles(d1, d2);
		if (status == 0 && !is_cooldown_active(d1, d2, now)
			&& check_periority(d1, d2, c->id))
		{
			break ;
		}
		if (status == 1 || status == 2 || status == 3)
			handle_status(status, d1, d2);
		else
			wait_on_dongle(sim, d1, d2);
		now = elapsed_ms(sim);
	}
}

void	try_take_dongles(t_sim *sim, t_dongle *d1, t_dongle *d2, t_coder *c)
{
	bool	condition;

	d1->in_use = true;
	d2->in_use = true;
	heap_extract_min(&d1->waiting);
	heap_extract_min(&d2->waiting);
	pthread_mutex_unlock(&d2->lock);
	pthread_mutex_unlock(&d1->lock);
	pthread_mutex_lock(&sim->state_lock);
	condition = sim -> stop;
	pthread_mutex_unlock(&sim->state_lock);
	if (!condition)
	{
		pthread_mutex_lock(&sim->log_lock);
		//pthread_mutex_lock(&sim->state_lock);
		log_state(sim, c->id, "has taken a dongle");
		log_state(sim, c->id, "has taken a dongle");
		pthread_mutex_unlock(&sim->log_lock);
		//pthread_mutex_unlock(&sim->state_lock);
	}
}

void	acquire_dognles(t_sim *sim, t_coder *c)
{
	t_dongle	*d1;
	t_dongle	*d2;

	if (c->left < c->right)
	{
		d1 = c->left;
		d2 = c->right;
	}
	else
	{
		d1 = c->right;
		d2 = c->left;
	}
	pthread_mutex_lock(&d1->lock);
	pthread_mutex_lock(&d2->lock);
	wait_till_available(sim, d1, d2, c);
	if (is_stoped(sim))
	{
		heap_remove(&d1->waiting, c->id);
		heap_remove(&d2->waiting, c->id);
		pthread_mutex_unlock(&d2->lock);
		pthread_mutex_unlock(&d1->lock);
		return ;
	}
	try_take_dongles(sim, d1, d2, c);
}

void	release_dongle(t_sim *sim, t_dongle *d)
{
	pthread_mutex_lock(&d->lock);
	if (d->in_use)
	{
		d->in_use = false;
		d->available_at_ms = elapsed_ms(sim) + sim->cfg.dongle_cooldown;
		pthread_cond_broadcast(&d->cond);
		pthread_mutex_unlock(&d->lock);
		return ;
	}
	pthread_mutex_unlock(&d->lock);
}
