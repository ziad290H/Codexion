#include "codexion.h"

int	main(int argc, char **argv)
{
    t_config config;
    int i;
	t_sim	sim;
    pthread_t	monitor_tid;
	void	*result;

	if (argc != 9)
	{
		fprintf(stderr, "Usage: ./codexion num_coders time_to_burnout "
			"time_to_compile time_to_debug time_to_refactor "
			"num_compiles dongle_cooldown scheduler(fifo|edf)\n");
		return (1);
	}

    if (!parsing(argv, &config))
        return (1);
	if (!init_sim(&sim, &config))
	{
		fprintf(stderr, "init failed\n");
		return(1);
	}
    i = 0;
    while(i < sim.cfg.num_coders)
    {
        if (pthread_create(&sim.coders[i].thread, NULL, coder_routine, &sim.coders[i]) != 0)
		{
			fprintf(stderr, "thread creation failed\n");
			destroy_sim(&sim);
			return(1);
		}
        i++;
    }
	if (pthread_create(&monitor_tid, NULL, monitor_routine, &sim) != 0)
	{
		fprintf(stderr, "monitor creation failed\n");
		destroy_sim(&sim);
		return(1);
	}
	i = 0;
	while (i < sim.cfg.num_coders)
	{
		pthread_join(sim.coders[i].thread, &result);
		if (result!= NULL)
		{
			fprintf(stderr, "compilation failed \n");
			return (1);
		}

		i++;
	}
	pthread_join(monitor_tid, NULL);
	printf("%d\n", sim.coders[1].compiles_done);
	printf("%d\n", sim.coders[0].compiles_done);

	destroy_sim(&sim);
	return (0);
}
