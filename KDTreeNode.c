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
	KDTreeNode* Left; // Pointer to the left subtree
	KDTreeNode* Right; // Pointer to the right subtree
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
		return -1;
	}

	//fill dimension_spreads
	for (i=0; i<d; i++){
		index_of_lowest = getValFromMatrixOfSortedIndexes(KDArray, i, 0);
		index_of_highest = getValFromMatrixOfSortedIndexes(KDArray, i, n-1);
		getCopyOfPointfromArrayOfPoints(KDArray, index_of_lowest, &tmp_point);
		lowest_value = spPointGetAxisCoor(tmp_point, i);
		spPointDestroy(tmp_point);
		getCopyOfPointfromArrayOfPoints(KDArray, index_of_highest, &tmp_point);
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
 * 		root: an address to store the root of the created tree
 * @return todo:switch documentation if needed
 * 		the root of the KDTreeNode
 * 		NULL if an error occurred while calling other functions
 * 		NULL if KDArray=NULL or split_method is not 0,1 or 2
 */
int CreateKDTree(SPKDArray KDArray, int last_split_dim, int split_method, KDTreeNode* root){
	printf("CreateKDTree 1\n"); //todo remove this
	printf("KDArray: n=%d, d=%d \n", getN(KDArray), getD(KDArray)); //todo remove this
	int split_dimension;
	KDTreeNode leftNode = NULL;
	KDTreeNode rightNode = NULL;
	SPKDArray left_array;
	SPKDArray right_array;
	int split_result;
	int recursive_result;
	int initNode_result;
	double split_median;
	int median_index;
	SPPoint tmp_point;
	int n; //number of point in KDArray
	int d; //number of dimensions in KDArray

	//check validation of parameters
	if (KDArray==NULL){
		spLoggerPrintError(INVALID_ARG_ERROR, __FILE__, __func__, __LINE__);
		return -1;
	}
	if ( split_method!=0 && split_method!=1 && split_method!=2){
		spLoggerPrintError(INVALID_ARG_ERROR, __FILE__, __func__, __LINE__);
		return -1;
	}

	n = getN(KDArray);
	d = getD(KDArray);

	printf("CreateKDTree 2\n"); //todo remove this

	//if KDArray has only one point
	if (n==1){
		printf("CreateKDTree 2.0\n"); //todo remove this
		getCopyOfPointfromArrayOfPoints(KDArray, 0, &tmp_point);
		initNode_result = InitNode(-1,INFINITY,&leftNode,&rightNode,tmp_point,root);
		if (initNode_result==-1){
			spLoggerPrintError(GENERAL_ERROR_MSG, __FILE__, __func__, __LINE__);
			spLoggerPrintDebug(INITNODE_RETURNED_NULL, __FILE__, __func__, __LINE__);
			spPointDestroy(tmp_point);
			destroyKDArray(KDArray);
			return -1;
		}
	printf("CreateKDTree 2.1\n"); //todo remove this
	spPointDestroy(tmp_point);
	destroyKDArray(KDArray);
	printf("CreateKDTree 2.2\n"); //todo remove this
	return 1;
	}

	printf("CreateKDTree 3\n"); //todo remove this

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
			return -1;
		}
	}
	else if(split_method==2){   //2==INCREMENTAL){
		split_dimension = ((last_split_dim+1) % d);
	}

	printf("CreateKDTree 4\n"); //todo remove this
	//split KDArray according to split_dimension
	split_result = Split(KDArray,split_dimension,&left_array,&right_array);
	if (split_result==-1){
		spLoggerPrintError(GENERAL_ERROR_MSG, __FILE__, __func__, __LINE__);
		spLoggerPrintDebug(SPLIT_RETURNED_NULL, __FILE__, __func__, __LINE__);
		destroyKDArray(KDArray);
		destroyKDArray(left_array);
		destroyKDArray(right_array);
		return -1;
	}

	median_index = getN(left_array)-1; //the last index in the left part of the split
	printf("median_index = %d\n", median_index); //todo remove this
	getCopyOfPointfromArrayOfPoints(left_array, median_index, &tmp_point); //the last point from the left half
	split_median = spPointGetAxisCoor(tmp_point,split_dimension); //get the value of the median, by which the split occurred
	spPointDestroy(tmp_point);
	printf("split_median = %f\n", split_median); //todo remove this

	printf("CreateKDTree 5\n"); //todo remove this

	// recursive calls to left and right
	printf("splited_arrays[0]: n=%d, d=%d \n", getN(left_array), getD(left_array)); //todo remove this
	recursive_result = CreateKDTree(left_array, split_dimension, split_method, &leftNode);
	if (recursive_result==-1){
		destroyKDArray(left_array);
		destroyKDArray(right_array);
		destroyKDArray(KDArray);
		return -1;
	}

	printf("splited_arrays[1]: n=%d, d=%d \n", getN(right_array), getD(right_array)); //todo remove this
	recursive_result = CreateKDTree(right_array, split_dimension, split_method, &rightNode);
	if (recursive_result==-1){
		destroyKDArray(left_array);
		destroyKDArray(right_array);
		destroyKDArray(KDArray);
		//todo add destroyKDArray(splited_arrays[0])? and free(splited_arrays)?
		return -1;
	}

	printf("CreateKDTree 6\n"); //todo remove this
	//create new node
	initNode_result = InitNode(split_dimension, split_median, &leftNode, &rightNode, NULL, root);
	if (initNode_result==-1){
		spLoggerPrintError(GENERAL_ERROR_MSG, __FILE__, __func__, __LINE__);
		spLoggerPrintDebug(INITNODE_RETURNED_NULL, __FILE__, __func__, __LINE__);
		destroyKDArray(left_array);
		destroyKDArray(right_array);
		destroyKDArray(KDArray);
		return -1;
	}
	printf("node created with dim=%d, val=%f\n", split_dimension, split_median); //todo remove this

	printf("CreateKDTree 7\n"); //todo remove this
	//free unused memory
	//destroyKDArray(left_array);
	//destroyKDArray(right_array);
	destroyKDArray(KDArray);
	return 1;
}

int InitNode(int dim, double val, KDTreeNode* left, KDTreeNode* right, SPPoint data, KDTreeNode* Node){

	printf("InitNode 1\n"); //todo remove this
	// if the value of the dim parameter is invalid
	if (dim<-1){
		spLoggerPrintError(INVALID_ARG_ERROR, __FILE__, __func__, __LINE__);
		spLoggerPrintDebug(PARAMETER_DIM_INVALID, __FILE__, __func__, __LINE__);
		return -1;
	}

	printf("InitNode 2\n"); //todo remove this
	//allocate memory for Node
	if ( ((*Node) = (KDTreeNode)malloc(sizeof(**Node))) == NULL ){
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		return -1;
	}

	printf("InitNode 3\n"); //todo remove this

	//initialize fields according to the values given in the parameters
	(*Node)->Dim = dim;
	(*Node)->Val= val;

	if (*left != NULL){
		(*Node)->Left = left;
	}
	else{
		(*Node)->Left = NULL;
	}
	if (*right!=NULL){
		(*Node)->Right = right;
	}
	else{
		(*Node)->Right = NULL;
	}

	//initialize Node->Data
	if (data != NULL) {
		(*Node)->Data = spPointCopy(data);
		if((*Node)->Data==NULL){
			spLoggerPrintError(GENERAL_ERROR_MSG, __FILE__, __func__, __LINE__);
			return -1;
		}
	}
	else{
		(*Node)->Data = NULL;
	}
	printf("InitNode--end\n"); //todo remove this
	return 1;
}

int KDTreeNodegetDim(KDTreeNode node){
	if (node==NULL){
		return -2; //invalid value that is not used in the tree
	}
	return node->Dim;
}

double KDTreeNodegetVal(KDTreeNode node){
	if (node==NULL){
			return -INFINITY; //invalid value that is not used in the tree
		}
	return node->Val;
}

KDTreeNode KDTreeNodegetLeft(KDTreeNode node){
	if (node==NULL){
		return NULL;
	}
	if (node->Left==NULL){
		return NULL;
	}
	return *(node->Left);
}

KDTreeNode KDTreeNodegetRight(KDTreeNode node){
	if (node==NULL){
		return NULL;
	}
	if (node->Right==NULL){
		return NULL;
	}
	return *(node->Right);
}

SPPoint KDTreeNodegetData(KDTreeNode node){
	if (node->Data==NULL){
		return NULL;
	}
	return spPointCopy(node->Data);
}

int InitTree(SPPoint* arr, int size, int split_method, KDTreeNode* root){
	SPKDArray KDArray;
	int createTree_result;
	printf("init tree 1\n"); //todo remove this
	// check validation of parameter values, prints error to logger if not valid and returns NULL
	if (arr==NULL){
		spLoggerPrintError(INVALID_ARG_ERROR, __FILE__, __func__, __LINE__);
		spLoggerPrintDebug(PARAMETER_ARR_INVALID, __FILE__, __func__, __LINE__);
		return -1;
	}
	printf("init tree 2\n"); //todo remove this
	if (size<1){
		spLoggerPrintError(INVALID_ARG_ERROR, __FILE__, __func__, __LINE__);
		spLoggerPrintDebug(PARAMETER_SIZE_INVALID, __FILE__, __func__, __LINE__);
		return -1;
	}
	printf("init tree 3\n"); //todo remove this
	KDArray = Init(arr, size);
	printf("init tree 4\n"); //todo remove this
	if (KDArray==NULL){
		spLoggerPrintError(GENERAL_ERROR_MSG, __FILE__, __func__, __LINE__);
		spLoggerPrintDebug(INIT_RETURNED_NULL, __FILE__, __func__, __LINE__);
		return -1;
	}
	printf("init tree 5\n"); //todo remove this
	createTree_result = CreateKDTree(KDArray, -1, split_method, root); //parameter is -1 so that the first splitting dimension will be 0
	if (createTree_result==-1){
		spLoggerPrintError(GENERAL_ERROR_MSG, __FILE__, __func__, __LINE__);
		spLoggerPrintDebug(CREATEKDTREE_RETURNED_NULL, __FILE__, __func__, __LINE__);
		DestroyKDTreeNode(root);
		return -1;
	}
	printf("init tree 6 - done\n"); //todo remove this

	return 1;
}


void DestroyKDTreeNode(KDTreeNode* node){
	if (node==NULL){
		return;
	}
	if((*node)==NULL){
		return;
	}
	if ((*node)->Left!=NULL){
		DestroyKDTreeNode((*node)->Left);
	}
	if ((*node)->Right!=NULL){
		DestroyKDTreeNode((*node)->Right);
	}
	if ((*node)->Data!=NULL){
		spPointDestroy((*node)->Data);
	}
	free(*node);
	return;
}

int kNearestNeighbors(KDTreeNode curr , SPBPQueue bpq, SPPoint P){
	SPListElement newElement;
	SP_BPQUEUE_MSG msg;
	int return_val;
	char subtree_searched;
	double hypersphere;

	printf("kNearestNeighbors 1\n"); //todo remove this
	//reached end of tree
	if (curr==NULL){
		return 1;
	}
	printf("kNearestNeighbors 2\n"); //todo remove this

	//reached a leaf
	if (curr->Data!=NULL){
		printf("kNearestNeighbors 3\n"); //todo remove this
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
		printf("kNearestNeighbors 4\n"); //todo remove this
		//continue search on left subtree
		subtree_searched = 'L';
		return_val = kNearestNeighbors((*curr->Left), bpq, P);
		//if error occurred in the recursive call
		if (return_val==0){
			return 0;
		}
	}
	else{
		printf("kNearestNeighbors 5\n"); //todo remove this
		//continue search on right subtree
		subtree_searched = 'R';
		return_val = kNearestNeighbors((*curr->Right), bpq, P);
		//if error occurred in the recursive call
		if (return_val==0){
			return 0;
		}
	}
	printf("kNearestNeighbors 6\n"); //todo remove this

	//if bpq is not full or the candidate's hypersphere crosses the splitting plane
	hypersphere = curr->Val - spPointGetAxisCoor(P,curr->Dim);
	if ( (!spBPQueueIsFull(bpq)) || ((hypersphere*hypersphere) < spBPQueueMaxValue(bpq)) ){
		//search the subtree that wasn't searched yet
		printf("kNearestNeighbors 7\n"); //todo remove this
		if (subtree_searched=='L'){
			return_val = kNearestNeighbors((*curr->Right), bpq, P);
			//if error occurred in the recursive call
			if (return_val==0){
				return 0;
			}
		}
		else{
			printf("kNearestNeighbors 8\n"); //todo remove this
			return_val = kNearestNeighbors((*curr->Left), bpq, P);
			//if error occurred in the recursive call
			if (return_val==0){
				return 0;
			}
		}
	}
	return 1;
}


