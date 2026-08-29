/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_help.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zdaouari <zdaouari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 20:34:28 by zdaouari          #+#    #+#             */
/*   Updated: 2026/08/26 20:35:33 by zdaouari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	heap_swap(t_request *a, t_request *b)
{
	t_request	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

t_request	peek_the_min(t_heap *heap)
{
	t_request	empty;

	if (heap->size == 0)
	{
		empty.coder_id = -1;
		empty.key = LONG_MAX;
		empty.seq = LONG_MAX;
		return (empty);
	}
	return (heap->items[0]);
}
