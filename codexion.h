#ifndef CODEXION_H
# define CODEXION_H

//#define LONG_MAX = 9223372036854775807

#include <limits.h>
#include <stdbool.h>
# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>
#include <unistd.h>

typedef struct s_config
{
	int		num_coders;
	long	time_to_burnout;
	long	time_to_compile;
	long	time_to_debug;
	long	time_to_refactor;
	int		compiles_required;
	long	dongle_cooldown;
	int		scheduler;   // 0 = fifo, 1 = edf
}	t_config;

bool    parsing(char **arg,t_config *config);
bool	ft_parse_scheduler(const char *str, int *out_sched);
bool	ft_parse_positive_field(const char *str, long min_val, long *out);

#endif