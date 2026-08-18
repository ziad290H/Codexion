#include "codexion.h"

bool    do_compile(t_sim *sim, t_coder *c)
{
    if (c->left == c->right)
    {
        fprintf(stdout, " dongle left == dongle right\n");
        return (false);
    }
    if (is_stoped(sim))
        return (false);
    // dead lock avoidance , the way we distrubute what coders take first
    
    if (c->id % 2 == 0)
    {
        acquire_dognle(sim, c->right, c->id);
        acquire_dognle(sim, c->left, c->id);
        // reopen the lock of sim -> heap_lock
    }
    else
    {
        acquire_dognle(sim, c->left, c->id);
        acquire_dognle(sim, c->right, c->id);
        // reopen the lock of sim -> heap_lock
    }
    pthread_mutex_lock(&sim->state_lock);
    c->last_compile_start = elapsed_ms(sim);
    pthread_mutex_unlock(&sim->state_lock);

    log_state(sim, c->id, "is compiling");
    //usleep uses microsecendes
    // multiply by 1000 to convert time_to_complie from miliseceds to microsecends
    usleep(sim->cfg.time_to_compile * 1000);

    release_dongle(sim , c-> right);
    release_dongle(sim , c-> left);

    if (!(is_stoped(sim)))
    {
        // this condition is to prevent to add +1 compilation on a stopped sumulation
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

void *coder_routine(void *arg)
{
    t_coder *c;
    t_sim *s;
    // you mean here treat this adress of arg as a pointer to t_coder
    c = (t_coder *)arg;
    s = c->sim;

    while (!is_stoped(s))
    {
        if (done_compiling(s, c))
            break;
        if(!do_compile(s, c))
            return (void *)1;
        log_state(s, c->id, "is debugging");
        usleep(s->cfg.time_to_debug * 1000);
        if (is_stoped(s))
            break;
        log_state(s, c->id, "is refactoring");
        usleep(s->cfg.time_to_refactor * 1000);
    }

    return (NULL);
}