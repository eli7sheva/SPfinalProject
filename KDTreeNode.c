#include "KDTreeNode.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>


#define ALLOC_ERROR_MSG "Allocation error"
#define INVALID_ARG_ERROR "Invalid arguments"
#define GENERAL_ERROR_MSG "An error occurred"
#define PARAMETER_DIM_INVALID "value of the parameter dim is invalid, must be dim >= -1"
#define PARAMETER_ARR_INVALID "value of the parameter arr is invalid, cann't be NULL"
#define PARAMETER_SIZE_INVALID "value of the parameter size is invalid, must be size > 0"
#define INIT_RETURNED_NULL "the call to Init of SPKDArray returned NULL"
#define CREATEKDTREE_RETURNED_NULL "the call to CreateKDTree returned NULL"
#define INITNODE_RETURNED_NULL "the call to InitNode returned NULL"
#define GETDIMENSIONMAXSPREAD_RETURNED_MINUS1 "the call to getDimentionMaxSpread returned -1"
#define SPLIT_RETURNED_NULL "the call to split of SPKDArray returned NULL"
#define SPLISTELEMENTCREATE_RETURNED_NULL "the call to spListElementCreate of SPListElement returned NULL"
#define SPBPQUEUEENGUEUE_RETURNED_NULL "the call to spBPQueueEnqueue of SPBPriorityqueue was not successful"


//TODO: unit testing for all functions

struct sp_KDTreeNode_t{
	int Dim; // The splitting dimension
	double Val;//  The median value of the splitting dimension
	KDTreeNode Left; // Pointer to the left subtree
	KDTreeNode Right; // Pointer to the right subtree
	SPPoint Data; // Pointer to a point (only if the current node is a leaf) otherwise this field value is NULL
};


/*
 * finds the dimension with the highest spread
 * @param KDArray a SPKDArray object
 * @return the dimension to split by if the parameter is MAX_SPREAD
 * 		   if there are several candidates returns the lowest dimension
 * 		   returns -1 if an allocation error occurred
 */
int getDimentionMaxSpread(SPKDArray KDArray){
	int d = getD(KDArray); //number of dimensions
	int n = getN(KDArray); //number of points
	double* dimension_spreads; //array where index i contains the spread value of dimension i
	int index_of_lowest; // index to the point with the lowest value of a specific dimension
	int index_of_highest; // index to the point with the highest value of a specific dimension
	double lowest_value;   //value of the point in index index_of_lowest
	double highest_value; //value of the point in index index_of_highest
	double max_spread_val;  //will store the value of max_spread
	int max_spread_index;   // will store the index corresponding to the dimension with the max_spread
	SPPoint tmp_point;      // holds copy of a point to use temporarily
	int i;

	//allocate memory for dimension_spreads
	if ( (dimension_spreads = (double*)malloc(d*sizeof(double))) == NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		free(dimension_spreads);
		return -1;
	}

	//fill dimension_spreads
	for (i=0; i<d; i++){
		index_of_lowest = getValFromMatrixOfSortedIndexes(KDArray, i, 0);
		index_of_highest = getValFromMatrixOfSortedIndexes(KDArray, i, n-1);
		tmp_point = getCopyOfPointfromArrayOfPoints(KDArray, index_of_lowest);
		lowest_value = spPointGetAxisCoor(tmp_point, i);
		spPointDestroy(tmp_point);
		tmp_point =  getCopyOfPointfromArrayOfPoints(KDArray, index_of_highest);
		highest_value = spPointGetAxisCoor(tmp_point, i);
		spPointDestroy(tmp_point);
		dimension_spreads[i] = highest_value - lowest_value;
	}

	// find highest spread from dimension_spreads
	max_spread_val = dimension_spreads[0];
	max_spread_index = 0;
	for (i=1; i<d; i++){
		if (dimension_spreads[i]>max_spread_val){
			max_spread_val = dimension_spreads[i];
			max_spread_index = i;
		}
	}
	free(dimension_spreads);
	return max_spread_index;
}

/*
 * returns the dimension to split by if the parameter is RANDOM
 * @param KDArray a SPKDArray object
 * @return a random int between 0 and KDArray->d -1
 */
int getDimentionRandom(SPKDArray KDArray){
	int rand_dimension;
	int d = getD(KDArray);
	//Initialize random number generator
	srand(time(NULL));
	//get random number between 0 and d-1
	rand_dimension = rand()%d;
	return rand_dimension;
}


/*
 * the recursive function creating the KD tree
 * @param
 * 		KDArray: a SPKDArray object
 * 		last_split_dim: the dimension that was used for split in the last recursive call
 * 		split_method: an int representing the method to split by
 * 					 0=RANDOM, 1= MAX_SPREAD,  2=INCREMENTAL
 * @return
 * 		a KDTreeNode which is the root of the tree
 * 		NULL if an error occurred while calling other functions
 */
KDTreeNode CreateKDTree(SPKDArray KDArray, int last_split_dim, int split_method){
	int split_dimension;
	KDTreeNode newNode;
	KDTreeNode left;
	KDTreeNode right;
	SPKDArray* splited_arrays;
	double split_median;
	int median_index;
	SPPoint p;
	int n = getN(KDArray); //number of point in KDArray
	int d = getD(KDArray); //number of dimensions in KDArray

	//if KDArray has only one point
	if (n==1){
		p = getCopyOfPointfromArrayOfPoints(KDArray, 0);
		newNode = InitNode(-1,INFINITY,NULL,NULL,p);
		if (newNode==NULL){
			spLoggerPrintError(GENERAL_ERROR_MSG, __FILE__, __func__, __LINE__);
			spLoggerPrintDebug(INITNODE_RETURNED_NULL, __FILE__, __func__, __LINE__);
			destroyKDArray(KDArray);
			return NULL;
		}
		destroyKDArray(KDArray);
		return newNode;
	}

	//Assign split_dimension according to value of spKDTreeSplitMethod
	if(split_method==0){   //0==RANDOM
			split_dimension = getDimentionRandom(KDArray);
		}
	else if(split_method==1){  // 1==MAX_SPREAD
		split_dimension = getDimentionMaxSpread(KDArray);
		if (split_dimension==-1){
			spLoggerPrintError(GENERAL_ERROR_MSG, __FILE__, __func__, __LINE__);
			spLoggerPrintDebug(GETDIMENSIONMAXSPREAD_RETURNED_MINUS1 , __FILE__, __func__, __LINE__);
			destroyKDArray(KDArray);
			return NULL;
		}
	}
	else if(split_method==2){   //2==INCREMENTAL){
		split_dimension = ((last_split_dim+1) % d);
	}

	//split KDArray according to split_dimension
	splited_arrays = Split(KDArray,split_dimension);
	if (splited_arrays==NULL){
		spLoggerPrintError(GENERAL_ERROR_MSG, __FILE__, __func__, __LINE__);
		spLoggerPrintDebug(SPLIT_RETURNED_NULL, __FILE__, __func__, __LINE__);
		destroyKDArray(KDArray);
		return NULL;
	}
	median_index = getN(splited_arrays[0])-1; //the last index in the left part of the split
	split_median = getValFromMatrixOfSortedIndexes(splited_arrays[0], split_dimension, median_index); //get the value of the median

	// recursive calls to left and right
	left = CreateKDTree(splited_arrays[0], split_dimension, split_method);
	if (left==NULL){
		destroyKDArray(KDArray);
		return NULL;
	}
	right = CreateKDTree(splited_arrays[1], split_dimension, split_method);
	if (right==NULL){
		destroyKDArray(KDArray);
		return NULL;
	}

	//create new node
	newNode = InitNode(split_dimension, split_median, left, right, NULL);
	if (newNode==NULL){
		spLoggerPrintError(GENERAL_ERROR_MSG, __FILE__, __func__, __LINE__);
		spLoggerPrintDebug(INITNODE_RETURNED_NULL, __FILE__, __func__, __LINE__);
		destroyKDArray(KDArray);
		return NULL;
	}

	//free unused memory
	free(splited_arrays);
	destroyKDArray(KDArray);
	return newNode;
}

KDTreeNode InitNode(int dim, double val, KDTreeNode left, KDTreeNode right, SPPoint data){
	KDTreeNode Node;
	// if the value of the dim parameter is invalid
	if (dim<-1){
		spLoggerPrintError(INVALID_ARG_ERROR, __FILE__, __func__, __LINE__);
		spLoggerPrintDebug(PARAMETER_DIM_INVALID, __FILE__, __func__, __LINE__);
		return NULL;
	}

	//allocate memory for Node
	if ( (Node = (KDTreeNode)malloc(sizeof(KDTreeNode))) == NULL ){
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		free(Node);
		return NULL;
	}

	//initialize fields according to the values given in the parameters
	Node->Dim = dim;
	Node->Val= val;
	Node->Left = left;
	Node->Right = right;
	Node->Data = data;
	return Node;
}

int KDTreeNodegetDim(KDTreeNode node){
	return node->Dim;
}

double KDTreeNodegetVal(KDTreeNode node){
	return node->Val;
}

KDTreeNode KDTreeNodegetLeft(KDTreeNode node){
	return node->Left;
}

KDTreeNode KDTreeNodegetRight(KDTreeNode node){
	return node->Right;
}

SPPoint KDTreeNodegetData(KDTreeNode node){
	if (node->Data==NULL){
		return NULL;
	}
	return spPointCopy(node->Data);
}

KDTreeNode InitTree(SPPoint* arr, int size, int split_method){
	SPKDArray KDArray;
	KDTreeNode KDTree;

	// check validation of parameter values, prints error to logger if not valid and returns NULL
	if (arr==NULL){
		spLoggerPrintError(INVALID_ARG_ERROR, __FILE__, __func__, __LINE__);
		spLoggerPrintDebug(PARAMETER_ARR_INVALID, __FILE__, __func__, __LINE__);
		return NULL;
	}
	if (size<1){
		spLoggerPrintError(INVALID_ARG_ERROR, __FILE__, __func__, __LINE__);
		spLoggerPrintDebug(PARAMETER_SIZE_INVALID, __FILE__, __func__, __LINE__);
		return NULL;
	}
	KDArray = Init(arr, size);
	if (KDArray==NULL){
		spLoggerPrintError(GENERAL_ERROR_MSG, __FILE__, __func__, __LINE__);
		spLoggerPrintDebug(INIT_RETURNED_NULL, __FILE__, __func__, __LINE__);
		return NULL;
	}
	KDTree = CreateKDTree(KDArray, -1, split_method); //parameter is -1 so that the first splitting dimension will be 0
	if (KDTree==NULL){
		spLoggerPrintError(GENERAL_ERROR_MSG, __FILE__, __func__, __LINE__);
		spLoggerPrintDebug(CREATEKDTREE_RETURNED_NULL, __FILE__, __func__, __LINE__);
		return NULL;
	}
	return KDTree;
}

void DestroyKDTreeNode(KDTreeNode node){
	if (node == NULL){
		return;
	}
	free(node->Left);
	free(node->Right);
	if (node->Data!=NULL){
		spPointDestroy(node->Data);
	}
	return;
}

void DestroyKDTree(KDTreeNode node){
	if (node == NULL){
		return;
	}
	DestroyKDTree(node->Left);
	DestroyKDTree(node->Right);
	DestroyKDTreeNode(node);
}


int kNearestNeighbors(KDTreeNode curr , SPBPQueue bpq, SPPoint P){
	SPListElement newElement;
	SP_BPQUEUE_MSG msg;
	int return_val;
	char subtree_searched;

	//reached end of tree
	if (curr==NULL){
		return 1;
	}
	//reached a leaf
	if (curr->Data==NULL){
		//create new ListElement:
		//index=index of the point that is the Data of curr. value=distance between the point of curr to P
		newElement = spListElementCreate(spPointGetIndex(curr->Data), spPointL2SquaredDistance(P,curr->Data));
		//if there was a problem creating newElement
		if (newElement==NULL){
			spLoggerPrintError(GENERAL_ERROR_MSG, __FILE__, __func__, __LINE__);
			spLoggerPrintDebug(SPLISTELEMENTCREATE_RETURNED_NULL, __FILE__, __func__, __LINE__);
			return 0;
		}
		// add newElement to bpq
		msg = spBPQueueEnqueue(bpq, newElement);
		//if here was a problem adding newElement to bpq
		if (msg!=SP_BPQUEUE_SUCCESS && msg!=SP_BPQUEUE_FULL){
			spLoggerPrintError(GENERAL_ERROR_MSG, __FILE__, __func__, __LINE__);
			spLoggerPrintDebug(SPBPQUEUEENGUEUE_RETURNED_NULL, __FILE__, __func__, __LINE__);
			return 0;
		}
	}
	//recursive call to left or right
	if (spPointGetAxisCoor(P,curr->Dim)<=curr->Val){
		//continue search on left subtree
		subtree_searched = 'L';
		return_val = kNearestNeighbors(curr->Left, bpq, P);
		//if error occurred in the recursive call
		if (return_val==0){
			return 0;
		}
	}
	else{
		//continue search on right subtree
		subtree_searched = 'R';
		return_val = kNearestNeighbors(curr->Right, bpq, P);
		//if error occurred in the recursive call
		if (return_val==0){
			return 0;
		}
	}
	//if bpq is not full or the candidate's hypersphere crosses the splitting plane
	if ( (!spBPQueueIsFull(bpq)) || ( abs(curr->Val- spPointGetAxisCoor(P,curr->Dim)) < spBPQueueMaxValue(bpq)) ){
		//search the subtree that wasn't searched yet
		if (subtree_searched=='L'){
			return_val = kNearestNeighbors(curr->Right, bpq, P);
			//if error occurred in the recursive call
			if (return_val==0){
				return 0;
			}
		}
		else{
			return_val = kNearestNeighbors(curr->Left, bpq, P);
			//if error occurred in the recursive call
			if (return_val==0){
				return 0;
			}
		}
	}
	return 1;
}


