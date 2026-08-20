#include "codexion.h"

void log_state(t_sim *sim, int coder_id,const char *msg)
{
    pthread_mutex_lock(&sim->log_lock);
// state lock is locked just to read the stop in simulation
// cause other threads read to it
    pthread_mutex_lock(&sim->state_lock);
    if (sim->stop)
    {
        if (strcmp(msg, "burned out") == 0)
            printf("%ld %d %s\n", elapsed_ms(sim), coder_id, msg);
        // claude said it should revesre the order of how i locked them with how i
        // will unlocked them , idk why ?
        pthread_mutex_unlock(&sim->state_lock);
        pthread_mutex_unlock(&sim->log_lock);
        return;
    }
    pthread_mutex_unlock(&sim->state_lock);
    fprintf(stdout, "%ld %d %s\n", elapsed_ms(sim), coder_id, msg);
// now we unlock the log_lock if any thred wants to write to it
    pthread_mutex_unlock(&sim->log_lock);
}

long    compute_priority_key(t_sim *sim, t_coder *c)
{
    if (sim->cfg.scheduler == 0)
    {
        //case for fifo;
        pthread_mutex_lock(&sim->state_lock);
        sim->request_counter++;
        pthread_mutex_unlock(&sim->state_lock);
        return (sim->request_counter);
    }
    else
        return(c->last_compile_start + sim->cfg.time_to_burnout);
}

void acquire_dognle(t_sim *sim, t_dongle *d, t_coder *c)
{
    long    now;
    t_request   req;
    // dont forget to delet it
    t_request tmp;
//  we lock a dongle when a want to change the metadata of it (is_use, available_at)
// we lock the dongle and mutex soo we can not be falling in the race-condition (another coder change it statue)
    // fprintf(stderr, "coder : %d willing to lock %p\n", c->id, d);
    pthread_mutex_lock(&d->lock);
    // fprintf(stderr, "coder : %d locked it \n", c->id);
    req.coder_id = c->id;
    req.key = compute_priority_key(sim, c);
    pthread_mutex_lock(&sim->state_lock);
    
    // the seq is used in caase in edf 2 coders have the same deadline
    // we sort them by whaat request is first
    req.seq = sim->request_counter;
    
    pthread_mutex_unlock(&sim->state_lock);
    now = elapsed_ms(sim);
    
    //pushing to the heap
    heap_push(&d->waiting, req);
    
    while (d->in_use || now < d->available_at_ms || peek_the_min(&d->waiting).coder_id != c->id)
    {
        // one or more thread will be sleeping waiting for the cond
        //fprintf(stdout, "coder %d waiiting to dongle to be available\n", coder_id);
        if ((d->in_use) == false)
        {
            wait_for_dongle(sim, d);
        }
        if((d->in_use) == true)
        {
            pthread_cond_wait(&d->cond, &d->lock); 
        }
        now = elapsed_ms(sim);

    }
    d -> in_use = true;
    tmp = heap_extract_min(&d->waiting);
    // fprintf(stderr, "extracte it sucssufulli %d\n", tmp.coder_id);
    pthread_mutex_unlock(&d->lock);
    log_state(sim, c->id, "has taken a dognle");

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