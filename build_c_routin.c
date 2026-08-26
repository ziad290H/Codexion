/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_c_routin.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zdaouari <zdaouari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 14:30:09 by zdaouari          #+#    #+#             */
/*   Updated: 2026/08/26 14:30:12 by zdaouari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_request prepare_the_request(t_coder *coder, t_sim *sim)
{
    t_request req;

    pthread_mutex_lock(&sim->state_lock);
    sim->request_counter++;
    req.coder_id = coder->id;
    if (sim->cfg.scheduler == 0)
    {
        //case for fifo;
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

bool	do_compile(t_sim *sim, t_coder *c)
{
	t_request	req;

	if (c->left == c->right)
		return (false);
	if (is_stoped(sim))
		return (false);
	req = prepare_the_request(c, sim);
	// push request to both heaps before attempting acquisition
	if ((c->id % 2) == 0)
    {
        pthread_mutex_lock(&c->right->lock);
        pthread_mutex_lock(&c->left->lock);
        heap_push(&c->right->waiting, req);
        heap_push(&c->left->waiting, req);
        pthread_mutex_unlock(&c->left->lock);
        pthread_mutex_unlock(&c->right->lock);
        printf("coder %d pushed to heap %p\n", c->id, &c->right->waiting);
        
        printf("coder %d pushed to heap %p\n", c->id, &c->left->waiting);
    }
    else if ((c->id % 2) != 0)
    {
        pthread_mutex_lock(&c->left->lock);
        pthread_mutex_lock(&c->right->lock);
        heap_push(&c->left->waiting, req);
        
        heap_push(&c->right->waiting, req);
        pthread_mutex_unlock(&c->left->lock);
        pthread_mutex_unlock(&c->right->lock);
        
        printf("coder %d pushed to heap %p\n", c->id, &c->left->waiting);
        printf("coder %d pushed to heap %p\n", c->id, &c->right->waiting);
    }
    acquire_dognles(sim, c);
	if (is_stoped(sim))
		return (false);

	pthread_mutex_lock(&sim->state_lock);
	c->last_compile_start = elapsed_ms(sim);
	pthread_mutex_unlock(&sim->state_lock);

	log_state(sim, c->id, "is compiling");
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

bool is_stoped(t_sim *sim)
{
    bool    val;

    pthread_mutex_lock(&sim->state_lock);
    val = sim->stop;
    pthread_mutex_unlock(&sim->state_lock);
    return(val);
}

bool done_compiling(t_sim *s,t_coder *c)
{
    int target;

    target = s->cfg.compiles_required;
    if (c->compiles_done >= target)
        return (true);
    return (false);
}
bool is_start(t_sim *sim)
{
    bool status;

    pthread_mutex_lock(&sim->state_lock);
    status = sim->start;
    pthread_mutex_unlock(&sim->state_lock);
    return (status);
}

void *coder_routine(void *arg)
{
    t_coder *c;
    t_sim *s;
    // you mean here treat this adress of arg as a pointer to t_coder
    c = (t_coder *)arg;
    s = c->sim;
    while (!is_start(s))
        usleep(500);
    if (is_stoped(s))
        return (NULL);
    if (c -> id % 2 == 0)
        smarte_sleep(s, (s->cfg.time_to_compile + s->cfg.dongle_cooldown) / 2);
    while (!is_stoped(s))
    {
        if (done_compiling(s, c))
            break;

        if(!do_compile(s, c))
            break;
        if (is_stoped(s))
            break;

        log_state(s, c->id, "is debugging");
        smarte_sleep(s, s->cfg.time_to_debug );

        if (is_stoped(s))
            break;
        log_state(s, c->id, "is refactoring");
        smarte_sleep(s, s->cfg.time_to_refactor);
    }
    return (NULL);
}