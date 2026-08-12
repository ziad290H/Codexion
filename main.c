#include "codexion.h"

int	main(int argc, char **argv)
{
    t_config config;
	if (argc != 9)
	{
		fprintf(stderr, "Usage: ./codexion num_coders time_to_burnout "
			"time_to_compile time_to_debug time_to_refactor "
			"num_compiles dongle_cooldown scheduler(fifo|edf)\n");
	}
    int *result;

    if (!parsing(argv + 1, &config))
        fprintf(stderr, "wrong type of data entred, please recheck your input");
        return (1);
    printf("sucsses");
    

}