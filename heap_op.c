#include "codexion.h"

bool heap_less(t_request *a, t_request *b)
{
    if (a->key != b->key)
        return (a->key < b->key);
    return  (a->seq < b ->seq);
}

void    heap_push(t_heap *heap, t_request a)
{
    int i;

    int parent;
    
    fprintf(stderr, "heap->size : %d ", heap->size);
    heap->items[heap->size] = a;
    i = heap->size;
    heap->size++;

    while(i > 0)
    {
        parent = (i - 1) / 2;
        if (!(heap_less(&heap->items[i], &heap->items[parent])))
            break;
        heap_swap(&heap->items[i], &heap->items[parent]);
        i = parent;
    }
}
void    heap_sift_down(t_heap *heap, int i)
{
    int    left;
    int    right;
    int smallest;
    
    while (true)
    {
        left = (2 * i) + 1;
        right = (2 * 2) + 2;
        smallest = i;
        if (left < heap->size && heap_less(&heap->items[left], &heap->items[i]))
            smallest = left;
        if (right < heap->size && heap_less(&heap->items[right], &heap->items[i]))
            smallest = right;
        if (smallest == i)
            break;
        heap_swap(&heap->items[i], &heap->items[smallest]);
        i = smallest;
    }
}

// return the min of the heap and also delet it and swift_down
t_request    heap_exxtract_min(t_heap *heap)
{
    int i ;
    t_request min;

    i = 0;
    min = peek_the_min(heap);
    heap_remove(heap, min.coder_id);
    return (min);
}

void heap_remove(t_heap *heap, int  coder_id)
{
    int i;
    int idx;

    idx = -1;
    i = heap->size;

    while (i > 0)
    {
        if (heap->items[i].coder_id == coder_id)
            break;
        i++;
    }
    if (idx == -1)
        return ;
    // overright the target with the last element so you can start swipping down till
    // you make it in it s place
    heap->size--;
    heap->items[idx] = heap->items[heap->size];
    heap_sift_down(heap, idx);
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

// see the min
t_request peek_the_min(t_heap *heap)
{
    return (heap->items[0]);
}