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
        //fprintf(stdout, "coder %d trying to take right\n", c->id);
        acquire_dognle(sim, c->right, c->id);
        //fprintf(stdout, "coder %d trying to take left\n", c->id);
        acquire_dognle(sim, c->left, c->id);
    }
    else
    {
        //fprintf(stdout, "coder %d trying to take left\n", c->id);
        acquire_dognle(sim, c->left, c->id);
        //fprintf(stdout, "coder %d trying to take right\n", c->id);
        acquire_dognle(sim, c->right, c->id);
    }
    pthread_mutex_lock(&sim->state_lock);
    c->last_compile_start = elapsed_ms(sim);
    pthread_mutex_unlock(&sim->state_lock);

    log_state(sim, c->id, "is compiling");
    //usleep uses microsecendes
    // multiply by 1000 to convert time_to_complie from miliseceds to microsecends
    usleep(sim->cfg.time_to_compile * 1000);
    // end of compiling 
    //fprintf(stdout, "the coder %d trying to relleased dognle\n", c->id);
    release_dongle(sim , c-> right);
    release_dongle(sim , c-> left);
    //fprintf(stdout, "\nthe coder %d finished comlipe\n", c->id);
    //fprintf(stdout, "\nthe coder %d locked state\n", c->id);
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

void *coder_routine(void *arg)
{
    t_coder *c;
    t_sim *s;
    // you mean here treat this adress of arg as a pointer to t_coder
    c = (t_coder *)arg;
    s = c->sim;

    while (!is_stoped(s))
    {
        if(!do_compile(s, c))
            return (void *)1;
        //fprintf(stdout, "\n\npahse 2 debugging**********************\n\n");
        log_state(s, c->id, "is debugging");
        usleep(s->cfg.time_to_debug * 1000);
        //fprintf(stdout, "\n\n calling is_stoped");
        if (is_stoped(s))
            break;
        //fprintf(stdout, "\n\n refactoring");
        log_state(s, c->id, "is refactoring");
        usleep(s->cfg.time_to_refactor * 1000);
    }

    return (NULL);
}