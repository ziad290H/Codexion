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

// long    compute_priority_key(t_sim *sim, t_coder *c)
// {
//     pthread_mutex_lock(&sim->state_lock);
//     sim->request_counter++;
//     if (sim->cfg.scheduler == 0)
//     {
//         //case for fifo;
//         return (sim->request_counter);
//     }
//     // edf
//     else if (sim->cfg.scheduler != 0)
//     {
//         // printf("wee eeeee&&&&&");    
//         return(c->last_compile_start + sim->cfg.time_to_burnout);
//     }
//     pthread_mutex_unlock(&sim->state_lock);
// }

int    check_dongles(t_dongle *left, t_dongle *right)
{
    int status;

    status = 0;

    if(left->in_use && right->in_use)
        status = 1;
    else if (left->in_use)
        status = 2;
    else if (right->in_use)
        status = 3;
    return (status);
}

bool is_cooldown_active(t_dongle *d1, t_dongle *d2, long now)
{
    return (now < d1->available_at_ms || now < d2->available_at_ms);
}

bool check_periority(t_sim *sim, t_dongle *d1 , t_dongle *d2, int coder_id)
{
    int per_d1;
    int per_d2;

    per_d1 = peek_the_min(&d1->waiting).coder_id;
    per_d2 = peek_the_min(&d2->waiting).coder_id;
    return (per_d1 == per_d2 && per_d2 == coder_id);
}

void acquire_dognles(t_sim *sim, t_coder *c)
{
    long    now;
    t_dongle    *d1;
    t_dongle    *d2;

    if ((c->id % 2) == 0)
    { 
        d2 =  c->left;
        d1 = c->right;
    }
    else if ((c->id % 2) != 0)
    {
        d1 = c->left;
        d2 = c->right;
    }
    now = elapsed_ms(sim);
    pthread_mutex_lock(&d1->lock);
    pthread_mutex_lock(&d2->lock);
    while (check_dongles(d1, d2) || is_cooldown_active(d1, d2, now) || !check_periority(sim, d1, d2, c->id))
    {
        fprintf(stderr, " slaaaak");
        if ((check_dongles(d1, d2)) == 0)
        {
            wait_for_dongle(sim, d1, d2);
        }
        else if(check_dongles(d1, d2) != 0)
        {
            if (check_dongles(d1, d2) == 2)
            {
                pthread_mutex_unlock(&d2->lock);
                pthread_cond_wait(&d1->cond, &d1->lock);
                pthread_mutex_lock(&d2->lock);
            }
            else if (check_dongles(d1, d2) == 3)
            {
                pthread_mutex_unlock(&d1->lock);
                pthread_cond_wait(&d2->cond, &d2->lock);
                pthread_mutex_lock(&d1->lock);
                
            }
        }
        else
            wait_for_dongle(sim, d1, d2);
        now = elapsed_ms(sim);

    }
    d1 -> in_use = true;
    d2-> in_use = true;
    heap_extract_min(&d1->waiting);
    heap_extract_min(&d2->waiting);
    pthread_mutex_unlock(&d2->lock);
    pthread_mutex_unlock(&d1->lock);
    log_state(sim, c->id, "has taken a dognle");
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