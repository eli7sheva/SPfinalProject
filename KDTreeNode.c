#include "KDTreeNode.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>


#define LEFT "left"
#define RIGHT "right"
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
 * the recursive function creating the KD tree, stores it in the adress root
 * @param
 * 		KDArray: a SPKDArray object
 * 		last_split_dim: the dimension that was used for split in the last recursive call
 * 		split_method: an int representing the method to split by
 * 					 0=RANDOM, 1= MAX_SPREAD,  2=INCREMENTAL
 * 		root: an address to store the root of the created tree
 * @return
 * 		 1 if an error occurred while calling other functions
 * 		-1 if KDArray=NULL or split_method is not 0,1 or 2
 */
int CreateKDTree(SPKDArray KDArray, int last_split_dim, int split_method, KDTreeNode* root){
	int split_dimension;
	SPKDArray left_array;
	SPKDArray right_array;
	KDTreeNode leftNode=NULL;
	KDTreeNode rightNode=NULL;
	int split_result;
	int recursive_result;
	int initNode_result;
	int median_index;   	//the last index of the left half
	int index_of_mid_point; //the index from matrix_of_sorted_indexes[split_dimension][median_index]
	double split_median;	//the value of array_of_points[index_of_mid_point] of coor split_dimension
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

	//if KDArray has only one point
	if (n==1){
		getCopyOfPointfromArrayOfPoints(KDArray, 0, &tmp_point);
		//create new node
		initNode_result = InitNode(-1,INFINITY,&leftNode,&rightNode,tmp_point,root);//to create node with left and right as null we send pointer to node
		if (initNode_result==-1){
			spLoggerPrintError(GENERAL_ERROR_MSG, __FILE__, __func__, __LINE__);
			spLoggerPrintDebug(INITNODE_RETURNED_NULL, __FILE__, __func__, __LINE__);
			spPointDestroy(tmp_point);
			destroyKDArray(KDArray);
			return -1;
		}
		spPointDestroy(tmp_point);
		destroyKDArray(KDArray);
		return 1;
	}

	//Assign split_dimension according to value of spKDTreeSplitMethod
	if(split_method==0){   //0==RANDOM
			split_dimension = rand()%d;
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
	index_of_mid_point = getValFromMatrixOfSortedIndexes(KDArray, split_dimension, median_index);
	getCopyOfPointfromArrayOfPoints(KDArray, index_of_mid_point, &tmp_point); //the last point from the left half
	split_median = spPointGetAxisCoor(tmp_point,split_dimension); //get the value of the median, by which the split occurred
	spPointDestroy(tmp_point);
	
	//create new node
	initNode_result = InitNode(split_dimension, split_median, NULL, NULL, NULL, root);
	if (initNode_result==-1){
		spLoggerPrintError(GENERAL_ERROR_MSG, __FILE__, __func__, __LINE__);
		spLoggerPrintDebug(INITNODE_RETURNED_NULL, __FILE__, __func__, __LINE__);
		destroyKDArray(left_array);
		destroyKDArray(right_array);
		destroyKDArray(KDArray);
		return -1;
	}

	// recursive calls to left and right
	recursive_result = CreateKDTree(left_array, split_dimension, split_method, (*root)->Left);
	if (recursive_result==-1){
		destroyKDArray(left_array);
		destroyKDArray(right_array);
		destroyKDArray(KDArray);
		return -1;
	}
	recursive_result = CreateKDTree(right_array, split_dimension, split_method, (*root)->Right);
	if (recursive_result==-1){
		destroyKDArray(left_array);
		destroyKDArray(right_array);
		destroyKDArray(KDArray);
		return -1;
	}

	//free unused memory
	//destroyKDArray(left_array);
	//destroyKDArray(right_array);
	destroyKDArray(KDArray);
	return 1;
}

int InitNode(int dim, double val, KDTreeNode* left, KDTreeNode* right, SPPoint data, KDTreeNode* Node){

	// if the value of the dim parameter is invalid
	if (dim<-1){
		spLoggerPrintError(INVALID_ARG_ERROR, __FILE__, __func__, __LINE__);
		spLoggerPrintDebug(PARAMETER_DIM_INVALID, __FILE__, __func__, __LINE__);
		return -1;
	}

	//allocate memory for *Node
	if ( ((*Node) = (KDTreeNode)malloc(sizeof(**Node))) == NULL ){
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		return -1;
	}

	//initialize fields according to the values given in the parameters
	(*Node)->Dim = dim;
	(*Node)->Val= val;

	//initialize Node->Left
	if (left!=NULL && (*left)!=NULL){
		(*Node)->Left = left;
	}
	// if *left=NULL* -> allocate the Left of Node to be a pointer to KDTreeNode
	else if(left==NULL){
		if ( ((*Node)->Left = (KDTreeNode*)malloc(sizeof(KDTreeNode))) == NULL ){
				spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
				return -1;
			}
	}
	//if left is a POINTER TO NULL ->the Left of the Node is NULL
	else if ((*left)==NULL){
		(*Node)->Left = NULL;
	}

	//initialize Node->Right
	if (right!=NULL && (*right)!=NULL){
		(*Node)->Right = right;
	}
	// if right=NULL -> allocate the Right of Node to be a pointer to KDTreeNode
	else if(right==NULL){
		if ( ((*Node)->Right = (KDTreeNode*)malloc(sizeof(KDTreeNode))) == NULL ){
				spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
				return -1;
			}
	}
	//if right is a *pointer to NULL* ->the Right of the Node is NULL
	else if ((*right)==NULL){
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

void KDTreeNodegetData(KDTreeNode node, SPPoint* point){
	if (node->Data==NULL){
		*point = NULL;
		return;
	}
	*point = spPointCopy(node->Data);
	return;
}

int InitTree(SPPoint* arr, int size, int split_method, KDTreeNode* root){
	SPKDArray KDArray;
	int createTree_result;

	// check validation of parameter values, prints error to logger if not valid and returns NULL
	if (arr==NULL){
		spLoggerPrintError(INVALID_ARG_ERROR, __FILE__, __func__, __LINE__);
		spLoggerPrintDebug(PARAMETER_ARR_INVALID, __FILE__, __func__, __LINE__);
		return -1;
	}

	if (size<1){
		spLoggerPrintError(INVALID_ARG_ERROR, __FILE__, __func__, __LINE__);
		spLoggerPrintDebug(PARAMETER_SIZE_INVALID, __FILE__, __func__, __LINE__);
		return -1;
	}
	KDArray = Init(arr, size);

	if (KDArray==NULL){
		spLoggerPrintError(GENERAL_ERROR_MSG, __FILE__, __func__, __LINE__);
		spLoggerPrintDebug(INIT_RETURNED_NULL, __FILE__, __func__, __LINE__);
		return -1;
	}
	createTree_result = CreateKDTree(KDArray, -1, split_method, root); //parameter is -1 so that the first splitting dimension will be 0
	if (createTree_result==-1){
		spLoggerPrintError(GENERAL_ERROR_MSG, __FILE__, __func__, __LINE__);
		spLoggerPrintDebug(CREATEKDTREE_RETURNED_NULL, __FILE__, __func__, __LINE__);
		DestroyKDTreeNode(*root);
		return -1;
	}

	return 1;
}

void DestroyKDTreeNode(KDTreeNode node){
	if (node==NULL){
		return;
	}
	if (node->Left!=NULL){
		DestroyKDTreeNode(*(node->Left));
		free(node->Left);
	}
	if (node->Right!=NULL){
		DestroyKDTreeNode(*(node->Right));
		free(node->Right);
	}
	if (node->Data!=NULL){
		spPointDestroy(node->Data);
	}
	free(node);
}

int kNearestNeighbors(KDTreeNode curr , SPBPQueue bpq, SPPoint* P){
	//parameters needed if curr is leaf:
	SPPoint data; 			//holds the point that belongs to the Node curr
	int index;   			// holds the index of data
	double distance; 		//L2Square distance between data and p
	SPListElement newElement;

	//parameters needed if curr is NOT leaf:
	double P_dim; 			//holds P[dim]
	char* subtree_searched; // equals either LEFT or RIGHT and indicates the subtree that was searches already
	int return_val;         // holds the result of the recursive call
	SP_BPQUEUE_MSG msg;


	double hypersphere;


	//check parameter validation:
	if (bpq==NULL || P==NULL){
		spLoggerPrintError(INVALID_ARG_ERROR, __FILE__, __func__, __LINE__);
		return -1;
	}

	//reached end of tree
	if (curr==NULL){
		return 1;
	}
	
	//reached a leaf
	if (curr->Dim==-1){
		KDTreeNodegetData(curr, &data); //get the point that belongs to the leaf
		//create new ListElement:
		//index=index of the point that is the Data of curr. value=distance between the point of curr to P
		index = spPointGetIndex(data);
		distance = spPointL2SquaredDistance(*P,data);
		newElement = spListElementCreate(index, distance);
		//if there was a problem creating newElement
		if (newElement==NULL){
			spLoggerPrintError(GENERAL_ERROR_MSG, __FILE__, __func__, __LINE__);
			spLoggerPrintDebug(SPLISTELEMENTCREATE_RETURNED_NULL, __FILE__, __func__, __LINE__);
			return -1;
		}
		// add newElement to bpq
		msg = spBPQueueEnqueue(bpq, newElement);
		//if here was a problem adding newElement to bpq
		if (msg!=SP_BPQUEUE_SUCCESS && msg!=SP_BPQUEUE_FULL){
			spLoggerPrintError(GENERAL_ERROR_MSG, __FILE__, __func__, __LINE__);
			spLoggerPrintDebug(SPBPQUEUEENGUEUE_RETURNED_NULL, __FILE__, __func__, __LINE__);
			spListElementDestroy(newElement);
			spPointDestroy(data);
			return -1;
		}
		spPointDestroy(data);
		spListElementDestroy(newElement);
		return 1;
	}

	//if curr is not a leaf: recursive call to left or right
	//check validation of curr->Val
	if (curr->Val==INFINITY){
		spLoggerPrintError(GENERAL_ERROR_MSG, __FILE__, __func__, __LINE__);
		return -1;
	}
	P_dim = spPointGetAxisCoor(*P,curr->Dim);
	if (P_dim <= curr->Val){
		//continue search on left subtree
		subtree_searched = LEFT;
		return_val = kNearestNeighbors(*(curr->Left), bpq, P);
		//if error occurred in the recursive call
		if (return_val==-1){
			return -1;
		}
	}
	else{
		//continue search on right subtree
		subtree_searched = RIGHT;
		return_val = kNearestNeighbors(*(curr->Right), bpq, P);
		//if error occurred in the recursive call
		if (return_val==-1){
			return -1;
		}
	}

	//if bpq is not full or the candidate's hyper sphere crosses the splitting plane
	hypersphere = curr->Val - P_dim;
	if ( (!spBPQueueIsFull(bpq)) || (((hypersphere*hypersphere) < spBPQueueMaxValue(bpq))) ){
		//search the subtree that wasn't searched yet
		if (strcmp(subtree_searched, LEFT)==0){
			return_val = kNearestNeighbors(*(curr->Right), bpq, P);
			//if error occurred in the recursive call
			if (return_val==-1){
				return -1;
			}
		}
		else{ //(subtree_searched==RIGHT)
			return_val = kNearestNeighbors(*(curr->Left), bpq, P);
			//if error occurred in the recursive call
			if (return_val==-1){
				return -1;
			}
		}
	}
	return 1;
}


