#include "codexion.h"

void *monitor_routine(void *arg)
{
    t_sim *sim;

    sim = (t_sim *)arg;
    while(true)
    {
        if (check_burnout(sim))
            break;
        if (check_all_compiled(sim))
            break;
        usleep(1000);
    }
    return (NULL);
}

bool    check_burnout(t_sim *sim)
{
    int i;
    long    now;
    long    deadline;

    i = 0;
    pthread_mutex_lock(&sim->state_lock);
    while (i < sim->cfg.num_coders)
    {
        now = elapsed_ms(sim);
        deadline = sim->coders[i].last_compile_start + sim->cfg.time_to_burnout;
        // idk if we should do the >= here
        if (now > deadline)
        {
            sim->stop = true;
            sim->someone_burned_out = true;
            pthread_mutex_unlock(&sim->state_lock);
            log_state(sim, sim->coders[i].id, "burned out");
            return (true);
        }
        i++;
    }
    pthread_mutex_unlock(&sim->state_lock);
    return (false);
}

bool    check_all_compiled(t_sim *sim)
{
    int i;
    long    target;

    pthread_mutex_lock(&sim->state_lock);
    i = 0;
    target = sim->cfg.compiles_required;
    while (i < sim->cfg.num_coders)
    {
        if (target > sim->coders[i].compiles_done)
        {
            pthread_mutex_unlock(&sim->state_lock);
            return (false);
        }
		i++;
    }
	//fprintf(stdout, "\n we are out at %d\n",  sim->coders[0].compiles_done);
	//fprintf(stdout, "\n we are out at %d\n",  sim->coders[1].compiles_done);
	//fprintf(stdout, "\n we are out at %d\n",  sim->coders[2].compiles_done);
    sim->stop = true;
	pthread_mutex_unlock(&sim->state_lock);
    return (true);
}