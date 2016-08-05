#include "KDTreeNode.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct sp_KDTreeNode_t{
	int Dim; // The splitting dimension
	double Val;//  The median value of the splitting dimension
	KDTreeNode Left; // Pointer to the left subtree
	KDTreeNode Right; // Pointer to the right subtree
	SPPoint Data; // Pointer to a point (only if the current node is a leaf) otherwise this field value is NULL
};

KDTreeNode InitNode(int dim, double val, KDTreeNode left, KDTreeNode right, SPPoint data){
	KDTreeNode Node = (KDTreeNode)malloc(sizeof(KDTreeNode));
	Node->Dim = dim;
	Node->Val= val;
	Node->Left = left;
	Node->Right = right;
	Node->Data = data;
}

KDTreeNode InitTree(SPPoint* arr, int size){
	SPKDArray KDArray = Init(arr, size);
	KDTreeNode KDTree = CreateKDTree(KDArray, -1);
	return KDTree;
}

KDTreeNode CreateKDTree(SPKDArray KDArray, int last_split_dim){
	int split_dimension;
	KDTreeNode newNode;
	//if KDArray has only one point
	if (KDArray->n==1){
		newNode = InitNode(-1,INFINITY,NULL,NULL,spPointCopy(KDArray.array_of_points[0]));
		destroyKDArray(KDArray);
		return newNode;
	}
	//Assign split_dimension according to value of spKDTreeSplitMethod
	if(MAX_SPREAD){ //TODO: how to check this?
		split_dimension = getDimentionMaxSpread(KDArray);
	}
	else if(RANDOM){
		split_dimension = getDimentionRandom(KDArray);
	}
	else if(INCREMENTAL){
		split_dimension = (last_split_dim+1)%KDArray->d;
	}

//TODO: implement
}

int getDimentionMaxSpread(SPKDArray KDArray){
	int d = KDArray->d; //number of dimensions
	int n = KDArray->n; //number of points
	double* dimension_spreads; //array where index i contains the spread value of dimension i
	int index_of_lowest; // index to the point with the lowest value of a specific dimension
	int index_of_highest; // index to the point with the highest value of a specific dimension
	double lowest_value;   //value of the point in index index_of_lowest
	double highest_value; //value of the point in index index_of_highest
	double max_spread_val;  //will store the value of max_spread
	int max_spread_index;   // will store the index corresponding to the dimension with the max_spread
	int i;

	//allocate memory for dimension_spreads
	if ( (dimension_spreads = (double*)malloc(d*sizeof(double))) == NULL){
		spLoggerPrintError("ALLOCATION ERRORR", __FILE__, __func__, __LINE__);
		free(dimension_spreads);
	}

	//fill dimension_spreads
	for (i=0; i<d; i++){
		index_of_lowest = KDArray.matrix_of_sorted_indexes[i][0];
		index_of_highest = KDArray.matrix_of_sorted_indexes[i][n-1];
		lowest_value = spPointGetAxisCoor(KDArray.array_of_points[index_of_lowest], i);
		highest_value = spPointGetAxisCoor(KDArray.array_of_points[index_of_highest], i);
		dimension_spreads[i] = highest_value - lowest_value;
	}

	// find highest spread from dimension_spreads
	max_spread_val = dimension_spreads[0];
	max_spread_index = 0;
	for (i=1; i<d; i++){
		if (max_spread_index[i]>max_spread_val){
			max_spread_val = dimension_spreads[i];
			max_spread_index = i;
		}
	}
	return max_spread_index;
}

int getDimentionRandom(SPKDArray KDArray){
	int t;
	int rand_dimension;
	int d = KDArray->d;
	//Initialize random number generator
	srand((unsigned) time(&t));
	//get random number between 0 and d-1
	rand_dimension = rand()%d;
	return rand_dimension;
}


