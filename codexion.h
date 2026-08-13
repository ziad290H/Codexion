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
#include <sys/time.h>


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
///structyure 

typedef struct s_dongle
{
	pthread_mutex_t	lock;
	pthread_cond_t		cond;
	bool				in_use;
	long				available_at_ms; // 0 or timestamp when cooldown ends
}	t_dongle;

typedef struct s_coder
{
	int				id;              // 1..N
	pthread_t		thread;
	int				compiles_done;
	long			last_compile_start; // ms, for burnout + edf deadline
	t_dongle		*left;
	t_dongle		*right;
	struct s_sim	*sim;            // back-pointer to shared state
}	t_coder;

typedef struct s_sim
{
	t_config		cfg;
	t_dongle		*dongles;     // array, size = num_coders
	t_coder			*coders;      // array, size = num_coders
	pthread_mutex_t	log_lock;
	pthread_mutex_t	state_lock;   // protects stop flags / shared counters
	bool			stop;
	bool			someone_burned_out;
	long			start_time_ms;
}	t_sim;



//fucntions 
bool    parsing(char **arg,t_config *config);
bool	ft_parse_scheduler(const char *str, int *out_sched);
bool	ft_parse_positive_field(const char *str, long min_val, long *out);
long elapsed_ms(t_sim *sim);
long get_timesstamp_ms(void);

#endif