#ifndef SPBPRIORITYQUEUE_H_
#define SPBPRIORITYQUEUE_H_
#include "SPListElement.h"
#include <stdbool.h>
/**
 * SP Bounded Priority Queue summary
 *
 * TODO Complete documentation
 */


/** type used to define Bounded priority queue **/
typedef struct sp_bp_queue_t* SPBPQueue;

/** type for error reporting **/
typedef enum sp_bp_queue_msg_t {
	SP_BPQUEUE_OUT_OF_MEMORY,
	SP_BPQUEUE_FULL,
	SP_BPQUEUE_EMPTY,
	SP_BPQUEUE_INVALID_ARGUMENT,
	SP_BPQUEUE_SUCCESS
} SP_BPQUEUE_MSG;

/**
 * Allocates a new queue.
 *
 * This function creates a new empty queue.
 * @param maxSize - the maximun capacity of the queue to be created
 * @return
 * 	NULL - If allocations failed or if maxSize if smaller than 0.
 * 	A new empty queue in case of success.
 */
SPBPQueue spBPQueueCreate(int maxSize);

/**
 * Creates a copy of target queue.
 *
 * The new copy will contain all the elements from the source queue in the same
 * order.
 * @param source The target queue to copy
 * @return
 * NULL if a NULL was sent or a memory allocation failed.
 * A queue containing the same elements with same order as source otherwise.
 */
SPBPQueue spBPQueueCopy(SPBPQueue source);

/**
 * TODO Complete documentation
 */
void spBPQueueDestroy(SPBPQueue source);

/**
 * removes all the elements in the queue
 * @param source The queue  to remove all element from
 *
 */
void spBPQueueClear(SPBPQueue source);


/**
 * returns the number of elements in the queue (queue's current size)
 * @param source The queue which current size was requested
 * @return
 *		 -1 if a NULL pointer was sent 
 *		 Otherwise, the number of elements in the Queue
 */int spBPQueueSize(SPBPQueue source);

/**
 * returns the maximum size a queue can be
 * @param source The queue which max size was requested
 * @return
 *		 -1 if a NULL pointer was sent 
 *		 Otherwise, the maximum size of the queue
 */
int spBPQueueGetMaxSize(SPBPQueue source);


/*
* inserts a new copy of the element to the right place in the queue
* 
* in case the queue is full:
*	will not insert the new element if it's value is bigger than the maximum value in the queue and will return SP_BPQUEUE_FULL
*   otherwise, will insert it in the right place in the queue and will remove the last element (with the maximum value) from the queue
*
* in case the queue is empty:
*	will insert the new element to the first place in the queue
*
* in other cases will insert the element to the right place in the queue
*
* @param source The queue to insert the element to
* @param element The element to insert a copy of to the queue
* @return
* SP_BPQUEUE_INVALID_ARGUMENT if NULL pointer was sent as queue or element, or if the queues iterator is in an invalid state
* SP_BPQUEUE_OUT_OF_MEMORY if an allocation error has occured
* SP_BPQUEUE_FULL if queue is full and element to insert is bigger than maximum value in the queue
* SP_BPQUEUE_SUCCESS on successfull insert
*/
SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue source, SPListElement element);

/*
* removes the element with the lowest value from the queue
* 
* in case the queue is empty:
*	will not remove any element from the queue and will return SP_BPQUEUE_EMPTY
*
* in other cases will remove the first element (the element with the minimum value) from the queue
*
* @param source The queue to dequeue from
* @return
* SP_BPQUEUE_INVALID_ARGUMENT if null pointer was sent
* SP_BPQUEUE_OUT_OF_MEMORY if an allocation error has occured
* SP_BPQUEUE_EMPTY in case source queue is empty
* SP_BPQUEUE_SUCCESS on successfull insert
*/
SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue source);


/**
 * creates a new copy of the element with the lowest value in queue
 * @param source The queue which peek will be returned
 * @retun
 *		 NULL if queue is empty, if a NULL pointer was sent or if there was an allocation error
 *		 Otherwise, the new copy of the element with the highest value in queue
 */
 SPListElement spBPQueuePeek(SPBPQueue source);

/**
 * creates a new copy of the element with the highest value in queue
 * @param source The queue which last peek will be returned
 * @return
 *		 NULL if queue is empty, if a NULL pointer was sent or if there was an allocation error
 *		 Otherwise, the new copy of the element with the highest value in queue
 */
SPListElement spBPQueuePeekLast(SPBPQueue source);


/**
 * returns the minimum value in the queue
 * @param source The queue to return it's minimum value
 * @assert source!=NULL
 * @return -1 if queue is empty
 * 			Otherwize, the minimum value in the queue 	
 */double spBPQueueMinValue(SPBPQueue source);

/**
 * returns the maximum value in the queue
 * @param source The queue to return it's maximum value
 * @assert source!=NULL
 * @return -1 if the queue is empty
 * 			Otherwise, the maximum value in the queue 
 */
double spBPQueueMaxValue(SPBPQueue source);


/**
 * checks if the queue is empty
 * @param source The target queue which is requested to check
 * @assert source!=NULL
 * @return
 *	 true - if queue is empty
 *	 false - if queue is not empty
 */bool spBPQueueIsEmpty(SPBPQueue source);

/**
 * checks if the queue is full
 * @param source  The target queue which is requested to check
 * @assert source!=NULL AND spBPQueueIsEmpty(source)==false
 * @return
 *	 true - if queue is full
 *	 false - if queue is not full
 */
bool spBPQueueIsFull(SPBPQueue source);

#endif
