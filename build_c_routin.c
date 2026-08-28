/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_c_routin.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zdaouari <zdaouari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 14:30:09 by zdaouari          #+#    #+#             */
/*   Updated: 2026/08/28 10:42:23 by zdaouari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_request	prepare_the_request(t_coder *coder, t_sim *sim)
{
	t_request	req;

	pthread_mutex_lock(&sim->state_lock);
	sim->request_counter++;
	req.coder_id = coder->id;
	if (sim->cfg.scheduler == 0)
	{
		req.key = sim->request_counter;
	}
	else
	{
		req.key = coder->last_compile_start + sim->cfg.time_to_burnout;
		req.seq = sim->request_counter;
	}
	pthread_mutex_unlock(&sim->state_lock);
	return (req);
}

void	pushing_to_heap(t_coder *c, t_sim *sim)
{
	t_request	req;

	req = prepare_the_request(c, sim);
	if ((c->id % 2) == 0)
	{
		pthread_mutex_lock(&c->right->lock);
		pthread_mutex_lock(&c->left->lock);
		heap_push(&c->right->waiting, req);
		heap_push(&c->left->waiting, req);
		pthread_mutex_unlock(&c->left->lock);
		pthread_mutex_unlock(&c->right->lock);
	}
	else if ((c->id % 2) != 0)
	{
		pthread_mutex_lock(&c->left->lock);
		pthread_mutex_lock(&c->right->lock);
		heap_push(&c->left->waiting, req);
		heap_push(&c->right->waiting, req);
		pthread_mutex_unlock(&c->left->lock);
		pthread_mutex_unlock(&c->right->lock);
	}
}

bool	do_compile(t_sim *sim, t_coder *c)
{
	if (c->left == c->right)
		return (false);
	if (is_stoped(sim))
		return (false);
	pushing_to_heap(c, sim);
	acquire_dognles(sim, c);
	if (is_stoped(sim))
		return (false);
	pthread_mutex_lock(&sim->state_lock);
	c->last_compile_start = elapsed_ms(sim);
	pthread_mutex_unlock(&sim->state_lock);
	pthread_mutex_lock(&sim->log_lock);
	log_state(sim, c->id, "is compiling");
	pthread_mutex_unlock(&sim->log_lock);
	smarte_sleep(sim, sim->cfg.time_to_compile);
	release_dongle(sim, c->right);
	release_dongle(sim, c->left);
	if (!is_stoped(sim))
	{
		pthread_mutex_lock(&sim->state_lock);
		c->compiles_done += 1;
		pthread_mutex_unlock(&sim->state_lock);
	}
	return (true);
}

void	*coder_routine(void *arg)
{
	t_coder	*c;
	t_sim	*s;

	c = (t_coder *)arg;
	s = c->sim;
	if (beginning(s, c))
	{
		while (!is_stoped(s))
		{
			if (done_compiling(s, c))
				break ;
			if (!do_compile(s, c))
				break ;
			if (is_stoped(s))
				break ;
			pthread_mutex_lock(&s->log_lock);
			log_state(s, c->id, "is debugging");
			pthread_mutex_unlock(&s->log_lock);
			smarte_sleep(s, s->cfg.time_to_debug);
			if (is_stoped(s))
				break ;
			pthread_mutex_lock(&s->log_lock);
			log_state(s, c->id, "is refactoring");
			pthread_mutex_unlock(&s->log_lock);
			smarte_sleep(s, s->cfg.time_to_refactor);
		}
	}
	return (NULL);
}
