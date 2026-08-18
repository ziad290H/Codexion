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
	t_request		waiting;
	int	granted_to;
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

typedef struct s_request
{
	int	coder_id;
	long	key;
	long	seq;
}	t_request;

typedef struct s_heap
{
	t_request	*items;
	int	size;
}	t_heap;

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
	long			request_counter;
}	t_sim;





//fucntions 
bool    parsing(char **arg,t_config *config);
bool	ft_parse_scheduler(const char *str, int *out_sched);
bool	ft_parse_positive_field(const char *str, long min_val, long *out);
long elapsed_ms(t_sim *sim);
void	wait_for_dongle(t_sim *sim, t_dongle *d);
long get_timesstamp_ms(void);
bool init_sim(t_sim *sim, t_config *cfg);

bool is_stoped(t_sim *sim);

void acquire_dognle(t_sim *sim, t_dongle *d, t_coder *c);
void release_dongle(t_sim *sim, t_dongle *d);
void log_state(t_sim *sim, int coder_id,const char *msg);

void	*monitor_routine(void *arg);
bool    check_burnout(t_sim *sim);
bool    check_all_compiled(t_sim *sim);

void	*coder_routine(void *arg);
void	destroy_sim(t_sim *sim);

//heap fucnitons

#endif