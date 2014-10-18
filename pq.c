/******************/
/* PRIORITY QUEUE */
/******************/

#include <stdio.h>
#include <stdlib.h>
#include "pq.h"

#define SENTINEL -1

/*
    Initialise the priority queue to empty.
    Return value is a pointer to a new priority queue structure.
    This should be free'd by calling pq_destroy.
*/
struct priority_queue *pq_create()
{
    pq_t * pq = (pq_t *) malloc(sizeof(pq_t));
    pq->heap = (node *) malloc(HEAP_ARRAY_SIZE * sizeof(node));
    pq->size = 0;

    pq->heap[0].priority = SENTINEL;
    pq->heap[0].val = SENTINEL;

    return pq;
}

/*
    Cleanup the priority queue and free any memory within.
*/
void pq_destroy(struct priority_queue *pq)
{
    if (pq != NULL) {
        free(pq->heap);
        free(pq);
    }
}

/*
    Return the number of elements in the priority queue.
    This is needed to test the priority queue.
    returns: int, the size of the pq
*/
int pq_size(struct priority_queue *pq) {
    if (pq == NULL) return 0;

    return pq->size;
}

/*
    Insert a value with attached min.prority in the priority queue.
    priority should be non-negative (>= 0)
    returns: 1 if successful, 0 if failed (pq is full)
*/
int pq_enqueue(struct priority_queue *pq, int val, int priority)
{
    if (pq == NULL) return 0;

    if (pq->size == MAX_HEAP_SIZE) return 0;

    if (priority < 0) return 0;

    int i;

    for (i = ++pq->size; priority < pq->heap[i / 2].priority; i /= 2)
        pq_swap(pq, i, i / 2);

    pq->heap[i].priority = priority;
    pq->heap[i].val = val;

    return 1;
}

/*
    Return the value with the lowest priority in the queue.
    This keeps the value stored within the priority queue.
    returns: 1 if successful, 0 if failed (pq is empty)
    val and priority are returned in the pointers
*/
int pq_find(struct priority_queue *pq, int *val, int *priority)
{
    if (pq == NULL) return 0;
    *val = pq->heap[1].val;
    *priority = pq->heap[1].priority;

    return 1;
}

/*
    Removes the lowest priority object from the priority queue
    returns: 1 if successful, 0 if failed (pq is empty)

    note: may remove this from the spec (do cleanup)
*/
int pq_delete(struct priority_queue *pq)
{
    if (pq == NULL) return 0;

    int i, c;
    node last = pq->heap[pq->size--];

    for (i = 1; i * 2 <= pq->size; i = c) {
        c = i * 2;

        if (c < pq->size && pq->heap[c].priority > pq->heap[c+1].priority) c++;

        if (last.priority < pq->heap[c].priority) break;
        else pq_swap(pq, i, c);
    }

    pq->heap[i].priority = last.priority;
    pq->heap[i].val = last.val;

    return 1;
}

/*
    Returns the value with the lowest priority and removes it
    from the queue.
    This is the same as pq_find followed by pq_delete.
    returns: 1 if successful, 0 if failed (pq is empty)
    val and priority are returned in the pointers
*/
int pq_dequeue(struct priority_queue *pq, int *val, int *priority)
{
    if (pq == NULL) return 0;

    *val = pq->heap[1].val;
    *priority = pq->heap[1].priority;

    return pq_delete(pq);
}

/*
    Compares two pq elements with each other.
    This is needed to test the heap contained within.
    returns: <0 if a < b, >0 if a > b, 0 if a == b
    note that the root of the heap is at index 1, not zero
*/
int pq_cmp(struct priority_queue *pq, int a, int b)
{
    int pri_a = pq->heap[a].priority,
        pri_b = pq->heap[b].priority;

    if (pri_a > pri_b) return 1;
    if (pri_a < pri_b) return -1;

    return 0;
}

/*
    Swaps two pq elements with each other.
    This is an internal function that will help with the heap.
*/
void pq_swap(struct priority_queue *pq, int a, int b)
{
    int temp_v = pq->heap[a].val,
        temp_p = pq->heap[a].priority;

    pq->heap[a].val = pq->heap[b].val;
    pq->heap[a].priority = pq->heap[b].priority;

    pq->heap[b].val = temp_v;
    pq->heap[b].priority = temp_p;
}
