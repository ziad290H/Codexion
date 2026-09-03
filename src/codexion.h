/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zdaouari <zdaouari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 14:30:27 by zdaouari          #+#    #+#             */
/*   Updated: 2026/09/03 11:17:11 by zdaouari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <stdbool.h>
# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <sys/time.h>

typedef struct s_config
{
	int		num_coders;
	long	time_to_burnout;
	long	time_to_compile;
	long	time_to_debug;
	long	time_to_refactor;
	int		compiles_required;
	long	dongle_cooldown;
	int		scheduler;
}	t_config;

typedef struct s_request
{
	int		coder_id;
	long	key;
	long	seq;
}	t_request;

typedef struct s_heap
{
	t_request	*items;
	int			size;
}	t_heap;
typedef struct s_dongle
{
	pthread_mutex_t		lock;
	pthread_cond_t		cond;
	bool				in_use;
	long				available_at_ms;
	t_heap				waiting;
}	t_dongle;

typedef struct s_coder
{
	int				id;
	pthread_t		thread;
	int				compiles_done;
	long			last_compile_start;
	t_dongle		*left;
	t_dongle		*right;
	struct s_sim	*sim;
}	t_coder;

typedef struct s_sim
{
	t_config		cfg;
	t_dongle		*dongles;
	t_coder			*coders;
	pthread_mutex_t	log_lock;
	pthread_mutex_t	state_lock;
	bool			stop;
	bool			start;
	bool			fail_monitor;
	bool			someone_burned_out;
	long			start_time_ms;
	long			request_counter;
}	t_sim;

bool		parsing(char **arg, t_config *config);
bool		ft_parse_scheduler(const char *str, int *out_sched);
bool		ft_parse_positive_field(const char *str, long min_val, long *out);
long		elapsed_ms(t_sim *sim);
long		get_timesstamp_ms(void);
bool		init_sim(t_sim *sim, t_config *cfg);

bool		is_stoped(t_sim *sim);
void		smarte_sleep(t_sim *sim, long time);

void		acquire_dognles(t_sim *sim, t_coder *c);
void		release_dongle(t_sim *sim, t_dongle *d);
void		log_state(t_sim *sim, int coder_id, const char *msg);

bool		check_burnout(t_sim *sim);
bool		check_all_compiled(t_sim *sim);

void		*monitor_routine(void *arg);
void		*coder_routine(void *arg);
void		destroy_sim(t_sim *sim);

t_request	peek_the_min(t_heap *heap);
void		heap_swap(t_request *a, t_request *b);
void		heap_remove(t_heap *heap, int coder_id);
void		heap_extract_min(t_heap *heap);
void		heap_push(t_heap *heap, t_request a);
void		heap_sift_down(t_heap *heap, int i);
bool		heap_less(t_request *a, t_request *b);
void		wait_on_dongle(t_sim *sim, t_dongle *released, t_dongle *target);
bool		beginning(t_sim *s, t_coder *c);
bool		is_start(t_sim *sim);
bool		done_compiling(t_sim *s, t_coder *c);

bool		is_cooldown_active(t_dongle *d1, t_dongle *d2, long now);
int			check_dongles(t_dongle *d1, t_dongle *d2);
void		handle_status(int status, t_dongle *d1, t_dongle *d2);
#endif