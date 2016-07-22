#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "SPList.h"
#include "SPBPriorityQueue.h"

/*
* converts SP_LIST_MSG to SP_BPQUEUEMSG
*/
SP_BPQUEUE_MSG convertListMsgToQueueMsg(SP_LIST_MSG msg);

/*
* removes the last element in the queue
TODO add doc
*/

SP_BPQUEUE_MSG spQueueRemoveLast(SPBPQueue source);
struct sp_bp_queue_t {
	int size; 		//the maximum capacity of the queue
	SPList head; 	// points to a SPList which will hold queues elements
};

SPBPQueue spBPQueueCreate(int maxSize){
	SPBPQueue queue;
	if (maxSize <= 0) {
		return NULL;
	}

	queue = (SPBPQueue)malloc(sizeof(*queue));
	if (queue == NULL) {
		return NULL;
	}

	// initializing queue arguments
	queue->size = maxSize;
	queue->head = spListCreate();

	return queue;
}


SPBPQueue spBPQueueCopy(SPBPQueue source) {
	SPBPQueue new_queue;
	if (source == NULL) {
		return NULL;
	}

	new_queue = (SPBPQueue)malloc(sizeof(*new_queue));
	if (new_queue == NULL) {
		return NULL;
	}
	new_queue->size = source->size;
	if (source->head == NULL) {
		new_queue->head = NULL;
	} else {
		new_queue->head = spListCopy(source->head);
		if (new_queue->head == NULL) {
			spBPQueueDestroy(new_queue);
		}
	}
	return new_queue;
}
void spBPQueueDestroy(SPBPQueue source){
	if (source != NULL) {
		if (source->head != NULL) {
			spListDestroy(source->head);
		}
		free(source);
	}
}
void spBPQueueClear(SPBPQueue source) {
	spListClear(source->head);
}

int spBPQueueSize(SPBPQueue source){
	if (source == NULL) {
		return -1;
	}
	return spListGetSize(source->head);
}

int spBPQueueGetMaxSize(SPBPQueue source) {
	if (source == NULL){
		return -1;
	}
	return source->size;
}



SP_BPQUEUE_MSG convertListMsgToQueueMsg(SP_LIST_MSG msg) {
	if (msg == SP_LIST_OUT_OF_MEMORY) {
		return SP_BPQUEUE_OUT_OF_MEMORY;
	}
	else if ((msg == SP_LIST_INVALID_CURRENT) ||(msg == SP_LIST_NULL_ARGUMENT)) {
		return SP_BPQUEUE_INVALID_ARGUMENT;
	}
	return SP_BPQUEUE_SUCCESS;
}

SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue source, SPListElement element) {
	double newVal; // the value of the new element to enqueue
	double maxVal; // the maximum value of the queue before inserting the new element
	double eVal;   // iterates over all values in the queue
	SP_BPQUEUE_MSG msg;
	bool success = false; // true if enqueued successfully
	SPListElement currentElement; // for iterating list

	// argument validation check
	if (element == NULL || source == NULL) {
		return SP_BPQUEUE_INVALID_ARGUMENT;	
	}
	
	// if queue is empty insert the element as first element
	if (spBPQueueIsEmpty(source)){
		msg = convertListMsgToQueueMsg(spListInsertFirst(source->head, element));
		return msg;
	} 

	newVal = spListElementGetValue(element);
	maxVal = spBPQueueMaxValue(source);
	// if new value to insert is bigger than the maximum value - insert it to the end of the queue if it's not full
	if (newVal >= maxVal) {
		// if queue is full don't insert the new element
		if (spBPQueueIsFull(source)) {
			return SP_BPQUEUE_FULL;
		}
		// if queue is not full - insert new element to the end of the queue
		else {
			msg = convertListMsgToQueueMsg(spListInsertLast(source->head, element));
			return msg;
		}
	}
	
	currentElement = spListGetFirst(source->head);

	while ((!success) && (currentElement)) {
  		eVal = spListElementGetValue(currentElement);
  		if ((newVal < eVal)) {
			msg = convertListMsgToQueueMsg(spListInsertBeforeCurrent(source->head, element));
			// return in case of failure
			// keep iterating till the end of the queue in case of success insert
			if (msg != SP_BPQUEUE_SUCCESS) {
				return msg;
			}
			else {
				success = true;
			}
		}
  		// next step
		currentElement = spListGetNext(source->head);
	}

	// at this point, iterator is at the end of the queue
	// if the queue has more elements than it's maximum size - remove last element from the queue
	if (spBPQueueSize(source) > spBPQueueGetMaxSize(source)) {
		msg = spQueueRemoveLast(source);
	}

	msg = SP_BPQUEUE_SUCCESS;
	return msg;
}

SP_BPQUEUE_MSG spQueueRemoveLast(SPBPQueue source) {
	SPListElement currentElement;
	SPListElement nextElement;
	SPListElement lastElement;
	if (source == NULL) {
		return SP_BPQUEUE_INVALID_ARGUMENT;
	}
	if (spBPQueueIsEmpty(source)) {
		return SP_BPQUEUE_INVALID_ARGUMENT;
	}

	// iterate until we get to the last element
	currentElement = spListGetFirst(source->head);
	nextElement = spListGetNext(source->head);
	while (nextElement) {
		currentElement = nextElement;
		nextElement = spListGetNext(source->head);
	}

	lastElement = currentElement; // nextElement is null and currentElement is the last element

	// iterate to one before last
	currentElement = spListGetFirst(source->head);

	nextElement = spListGetNext(source->head);
	while (nextElement != lastElement) {
		currentElement = nextElement;
		nextElement = spListGetNext(source->head);
	}

	return convertListMsgToQueueMsg(spListRemoveCurrent(source->head));

}


SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue source) {
	if (source == NULL) {
		return SP_BPQUEUE_INVALID_ARGUMENT;	
	}
	// there is nothing to dequqe if queue is empty
	if (spBPQueueIsEmpty(source)) {
		return SP_BPQUEUE_EMPTY;
	}
	// set current iterator to the first element and remove it 
	// (first element in the queue is the element with the minimum value)
	spListGetFirst(source->head);
	return convertListMsgToQueueMsg(spListRemoveCurrent(source->head));
}

SPListElement spBPQueuePeek(SPBPQueue source) {
	SPListElement currentElement;
	if (source == NULL) {
		return NULL;
	}
	currentElement = spListGetFirst(source->head);
	return spListElementCopy(currentElement);
}


SPListElement spBPQueuePeekLast(SPBPQueue source) {
	SPListElement currentElement;
	SPListElement nextElement;
	if (source == NULL) {
		return NULL;
	}
	if (spBPQueueIsEmpty(source)) {
		return NULL;
	}

	// iterate to one before last
	currentElement = spListGetFirst(source->head);
	nextElement = spListGetNext(source->head);
	while (nextElement) {
		currentElement = nextElement;
		nextElement = spListGetNext(source->head);
	}
	return spListElementCopy(currentElement);
}

double spBPQueueMinValue(SPBPQueue source) {
	assert(source!=NULL);
	return spListElementGetValue(spListGetFirst(source->head));
}


double spBPQueueMaxValue(SPBPQueue source) {
	assert(source!=NULL);
	SPListElement currentElement;
	SPListElement nextElement;
	// iterate to one before last
	currentElement = spListGetFirst(source->head);

	nextElement = spListGetNext(source->head);
	while (nextElement) {
		currentElement = nextElement;
		nextElement = spListGetNext(source->head);
	}
 
	return spListElementGetValue(currentElement);
}


bool spBPQueueIsEmpty(SPBPQueue source) {
	assert(source!=NULL);
	return spBPQueueSize(source) == 0;
}

bool spBPQueueIsFull(SPBPQueue source) {
	assert((source!=NULL) && (spBPQueueIsEmpty(source) == false));
	return spBPQueueGetMaxSize(source) == spBPQueueSize(source);
}

