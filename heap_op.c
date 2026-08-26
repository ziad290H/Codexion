/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_op.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zdaouari <zdaouari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 14:30:34 by zdaouari          #+#    #+#             */
/*   Updated: 2026/08/26 20:33:17 by zdaouari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

bool	heap_less(t_request *child, t_request *parent)
{
	if (child->key != parent->key)
		return (child->key <= parent->key);
	return (child->seq < parent ->seq);
}

void	heap_push(t_heap *heap, t_request a)
{
	int	i;	
	int	parent;

	heap->items[heap->size] = a;
	i = heap->size;
	heap->size++;
	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (!(heap_less(&heap->items[i], &heap->items[parent])))
		{
			break ;
		}
		heap_swap(&heap->items[i], &heap->items[parent]);
		i = parent;
	}
}

void	heap_sift_down(t_heap *heap, int i)
{
	int		left;
	int		right;
	int		smallest;

	while (true)
	{
		left = (2 * i) + 1;
		right = (2 * 2) + 2;
		smallest = i;
		if (left < heap->size
			&& heap_less(&heap->items[left], &heap->items[i]))
			smallest = left;
		if (right < heap->size
			&& heap_less(&heap->items[right], &heap->items[i]))
			smallest = right;
		if (smallest == i)
			break ;
		heap_swap(&heap->items[i], &heap->items[smallest]);
		i = smallest;
	}
}

t_request	heap_extract_min(t_heap *heap)
{
	t_request	min;

	min = peek_the_min(heap);
	heap_remove(heap, min.coder_id);
	return (min);
}

void	heap_remove(t_heap *heap, int coder_id)
{
	int	i;
	int	idx;

	idx = -1;
	i = 0;
	while (i < heap->size)
	{
		if (heap->items[i].coder_id == coder_id)
		{
			idx = i;
			break ;
		}
		i++;
	}
	if (idx == -1)
		return ;
	heap->size--;
	heap->items[idx] = heap->items[heap->size];
	if (idx < heap->size)
		heap_sift_down(heap, idx);
}
