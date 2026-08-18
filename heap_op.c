#include "codexion.h"

bool heap_less()
{

}

void    heap_push(t_heap *heap, t_request *a)
{
    int i;

    i = heap->size;
    heap->size++;
    while(i > 0)
    {
    }
}





void heap_swap(t_request *a, t_request *b)
{
    t_request   tmp;

    tmp = *a;
    *a   = *b;
    *b = tmp;
}

bool heap_is_empty(t_heap *heap)
{
    return (heap->size == 0);
}

t_request pick_the_min(t_heap *heap)
{
    return (heap->items[0]);
}