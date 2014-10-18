/*************************/
/* PRIORITY QUEUE HEADER */
/*************************/

#ifndef PQ_H
#define PQ_H

/* constants */
#define MAX_HEAP_SIZE	(256*256*16 - 1)
#define HEAP_ARRAY_SIZE (MAX_HEAP_SIZE + 1)
/* you may want to use a smaller value while testing */
#define LARGE_TEST_SIZE (MAX_HEAP_SIZE)
/*
#define LARGE_TEST_SIZE (256*256)
*/


struct heapnode {
    int priority;	/* priority - sort your heap on this */
    int val;	/* data to store within this node */
};
typedef struct heapnode node;

struct priority_queue {
    struct heapnode *heap;	/* heap as an array */
    int size;		/* number of elements in the heap */
};
/* hide implementation within typedef, only use accessor functions */
typedef struct priority_queue pq_t;


/*
	Initialise the priority queue to empty.
	Return value is a pointer to a new priority queue structure.
	This should be free'd by calling pq_destroy.
*/
pq_t *pq_create(void);

/*
	Cleanup the priority queue and free any memory within.
*/
void pq_destroy(pq_t *pq);

/*
	Insert a value with attached min.prority in the priority queue.
	priority should be non-negative (>= 0)
	returns: 1 if successful, 0 if failed (pq is full)
*/
int pq_enqueue(pq_t *pq, int val, int priority);

/*
	Return the value with the lowest priority in the queue.
	This keeps the value stored within the priority queue.
	returns: 1 if successful, 0 if failed (pq is empty)
	val and priority are returned in the pointers
*/
int pq_find(pq_t *pq, int *val, int *priority);

/*
	Returns the value with the lowest priority and removes it
	from the queue.
	This is the same as pq_find followed by pq_delete.
	returns: 1 if successful, 0 if failed (pq is empty)
	val and priority are returned in the pointers
*/
int pq_dequeue(pq_t *pq, int *val, int *priority);

/*
	Compares two pq elements with each other.
	This is needed to test the heap contained within.
	returns: <0 if a < b, >0 if a > b, 0 if a == b
	note that the root of the heap is at index 1, not zero
*/
int pq_cmp(pq_t *pq, int a, int b);

/*
	Swaps two pq elements with each other.
	This is an internal function that will help with the heap.
*/
void pq_swap(pq_t *pq, int a, int b);

/*
	Return the number of elements in the priority queue.
	This is needed to test the priority queue.
	returns: int, the size of the pq
*/
int pq_size(pq_t *pq);

/*
	You can define other functions if you wish,
	but do not change the prototypes for the functions above
*/


/*****************/
/* end PQ header */
/*****************/

#endif

