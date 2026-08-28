/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zdaouari <zdaouari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 14:31:20 by zdaouari          #+#    #+#             */
/*   Updated: 2026/08/27 21:15:11 by zdaouari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static bool	parse_coders(char **argv, t_config *config)
{
	long	tmp;

	if (!ft_parse_positive_field(argv[1], 1, &tmp))
	{
		fprintf(stderr, "number_of_coders should be positiv int bigger than 0");
		return (false);
	}
	if (tmp > INT_MAX)
	{
		printf("number of coders should not exceed the range INT_MAX\n");
		return (false);
	}
	config->num_coders = (int)tmp;
	return (true);
}

static bool	parse_times(char **argv, t_config *config)
{
	if (!ft_parse_positive_field(argv[2], 0, &config->time_to_burnout))
	{
		printf("time_to_burnout should be a positive int");
		return (false);
	}
	if (!ft_parse_positive_field(argv[3], 0, &config->time_to_compile))
	{
		printf("time_to_compile should be a positiv int");
		return (false);
	}
	if (!ft_parse_positive_field(argv[4], 0, &config->time_to_debug))
	{
		printf("time_to_debug should be a positiv int");
		return (false);
	}
	if (!ft_parse_positive_field(argv[5], 0, &config->time_to_refactor))
	{
		printf("time_to_refactor should be a positiv int");
		return (false);
	}
	return (true);
}

static bool	parse_compiles(char **argv, t_config *config)
{
	long	tmp;

	if (!ft_parse_positive_field(argv[6], 0, &tmp))
	{
		printf("compiles_required should be a positiv int");
		return (false);
	}
	if (tmp > INT_MAX)
	{
		printf("compiles_required should be a valid positiv int");
		return (false);
	}
	config->compiles_required = (int)tmp;
	return (true);
}

static bool	parse_remaining(char **argv, t_config *config)
{
	if (!ft_parse_positive_field(argv[7], 0, &config->dongle_cooldown))
	{
		printf("dongle_cooldown should be a positiv int");
		return (false);
	}
	if (!ft_parse_scheduler(argv[8], &config->scheduler))
	{
		fprintf(stderr, "invalid scheduler (must be fifo or edf)");
		return (false);
	}
	return (true);
}

bool	parsing(char **argv, t_config *config)
{
	if (!parse_coders(argv, config))
		return (false);
	if (!parse_times(argv, config))
		return (false);
	if (!parse_compiles(argv, config))
		return (false);
	if (!parse_remaining(argv, config))
		return (false);
	return (true);
}
