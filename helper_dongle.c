/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helper_dongle.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zdaouari <zdaouari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 14:29:52 by zdaouari          #+#    #+#             */
/*   Updated: 2026/08/26 14:29:58 by zdaouari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void log_state(t_sim *sim, int coder_id,const char *msg)
{

    pthread_mutex_lock(&sim->state_lock);
    if (sim->stop)
    {
        if (strcmp(msg, "burned out") == 0)
            printf("%ld %d %s\n", elapsed_ms(sim), coder_id, msg);
        // claude said it should revesre the order of how i locked them with how i
        // will unlocked them , idk why ?
        pthread_mutex_unlock(&sim->state_lock);
        return;
    }
    pthread_mutex_unlock(&sim->state_lock);
    fprintf(stdout, "%ld %d %s\n", elapsed_ms(sim), coder_id, msg);
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

bool	check_periority(t_dongle *d1, t_dongle *d2, int coder_id)
{
	int	per_d1;
	int	per_d2;

	// heaps already protected by d1->lock and d2->lock held by caller
	per_d1 = peek_the_min(&d1->waiting).coder_id;
	per_d2 = peek_the_min(&d2->waiting).coder_id;
    //printf("coder : %d, per_d1= %d, per:d2: %d", coder_id, per_d1, per_d2);
	return (per_d1 == coder_id && per_d2 == coder_id);
}


void	acquire_dognles(t_sim *sim, t_coder *c)
{
	t_dongle	*d1;
	t_dongle	*d2;
	long		now;
	int			status;

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
    now = elapsed_ms(sim);
	while (!is_stoped(sim))
	{
		status = check_dongles(d1, d2);
        //printf(" status %d\n", status);
		if (status == 0
			&& !is_cooldown_active(d1, d2, now)
			&& check_periority(d1, d2, c->id))
            {
			    break;
            } // all conditions satisfied — take both dongles
		if (status == 1)
		{
			// both in use — wait on d1, release d2 temporarily
			pthread_mutex_unlock(&d2->lock);
			pthread_cond_wait(&d1->cond, &d1->lock);
			pthread_mutex_lock(&d2->lock);
		}
		else if (status == 2)
		{
			// d1 in use — wait on d1, release d2 temporarily
			pthread_mutex_unlock(&d2->lock);
			pthread_cond_wait(&d1->cond, &d1->lock);
			pthread_mutex_lock(&d2->lock);
		}
		else if (status == 3)
		{
			// d2 in use — wait on d2, release d1 temporarily
			pthread_mutex_unlock(&d1->lock);
			pthread_cond_wait(&d2->cond, &d2->lock);
			pthread_mutex_lock(&d1->lock);
		}
		else
		{
			// neither in use but cooldown or priority blocking
			// wait on whichever dongle has the farthest cooldown deadline
			if (d1->available_at_ms >= d2->available_at_ms)
				wait_on_dongle(sim, d1, d2, d1);
			else
				wait_on_dongle(sim, d2, d1, d2);
		}
		now = elapsed_ms(sim);
	}
    //printf("coder %d out of the loop\n", c->id);
	if (is_stoped(sim))
	{
		// cleanup: remove our requests from both heaps before exiting
		heap_remove(&d1->waiting, c->id);
		heap_remove(&d2->waiting, c->id);
		pthread_mutex_unlock(&d2->lock);
		pthread_mutex_unlock(&d1->lock);
		return ;
	}
	d1->in_use = true;
	d2->in_use = true;
	heap_extract_min(&d1->waiting);
	heap_extract_min(&d2->waiting);
	pthread_mutex_unlock(&d2->lock);
	pthread_mutex_unlock(&d1->lock);
	if (!sim->stop)
	{
		pthread_mutex_lock(&sim->log_lock);
		log_state(sim, c->id, "has taken a dongle");
		log_state(sim, c->id, "has taken a dongle");
		pthread_mutex_unlock(&sim->log_lock);
	}
}

void release_dongle(t_sim *sim, t_dongle *d)
{
    pthread_mutex_lock(&d->lock);
    if (d->in_use)
    {
        d->in_use= false;
        // adding the time for now + time to cooldown
        d->available_at_ms = elapsed_ms(sim) + sim->cfg.dongle_cooldown;
        //fprintf(stdout, "\n\available at cooldown = %ld $$$$$$$$$$$$$$$$$$$$", d->available_at_ms);
       //fprintf(stdout, "the dongle is available at %ld\n", d->available_at_ms);
       // return this, i only delet it beceaus of test
       pthread_cond_broadcast(&d->cond);
       pthread_mutex_unlock(&d->lock);
       return;
    }
    pthread_mutex_unlock(&d->lock);
}