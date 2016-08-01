/*
 * SPKDArray.c
 *
 *  Created on: 19 αιεμ 2016
 *      Author: elisheva
 */
#include "SPKDArray.h"
#include <stdlib.h>

struct sp_KDArray_t{
	int** Array;
};

SPKDArray* Init(SPPoint* arr, int size){
	SPKDArray* KDArray = (SPKDArray*)malloc(sizeof(SPKDArray*));
	int n = sizeof(arr)/sizeof(arr[0]); // n = number of points
	int d = spPointGetDimension(arr[0]); // d = the dimension of the points (assuming dimension is the same for all points)
	double** index_val_arr; //double array containing n rows. each row contains the index and the value of a specific coordinate in the point of that index
	int i,j;

	//check all points have the same dimension
	for (i=0; i<n; i++){
		if (spPointGetDimension(arr[i]) != d){
			spLoggerPrintError("points don't have the same dimension", __FILE__, __func__, __LINE__);
			return NULL;
		}
	}

	//allocate memory for KDArray->Array, this will be a d*n matrix, the i'th row is the indexes of the points in arr sorted according to their i'th dimension
	if ( (KDArray->Array = (int**)malloc(d*sizeof(int*))) == NULL){
		spLoggerPrintError("ALLOCATION ERRORR", __FILE__, __func__, __LINE__);
		free(KDArray->Array);
		free(KDArray);
	}
	for (i=0; i<d; i++){
		if( (KDArray->Array[i]=(int*)malloc(n*sizeof(int))) == NULL){
			spLoggerPrintError("ALLOCATION ERRORR", __FILE__, __func__, __LINE__);
			for (j=0; j<=i; j++){
				free(KDArray->Array[j]);
			}
			free(KDArray->Array);
			free(KDArray);
		}
	}

	//allocate memory for index_val_arr: n rows, 2 columns
	if ( (index_val_arr = (double**)malloc(n*sizeof(double*))) == NULL){
		spLoggerPrintError("ALLOCATION ERRORR", __FILE__, __func__, __LINE__);
		free(index_val_arr);
	}
	for (i=0; i<2; i++){
		if( (index_val_arr[i]=(double*)malloc(2*sizeof(double))) == NULL){
			spLoggerPrintError("ALLOCATION ERRORR", __FILE__, __func__, __LINE__);
			for (j=0; j<=i; j++){
				free(index_val_arr[j]);
			}
			free(index_val_arr);
		}
	}

	// for each coordinate
	for (i=0; i<d; i++){ // i=coordinate

		// fill index and value of coordinate to val_index_arr
		for (j=0; j<n; j++){ //j=index of point
			index_val_arr[j][0] = j;
			index_val_arr[j][1] = spPointGetAxisCoor(arr[j],i);
		}

		/* sort the rows in index_val_arr by the value in the second column,
		 * meaning by the value of the coordinate of each point.
		 * the first column will be the indexes of the points sorted by the values of the coordinate
		 */
		qsort(index_val_arr, n, sizeof(double*), copmareByValue);

		// fill the sorted indexes in to KDArray->Array
		for (j=0; j<n; j++){ //j=index of point
			KDArray->Array[i][j] = index_val_arr[j][0];
		}
	}
	return &KDArray; //return the pointer to KDArray
}

//
int copmareByValue(void* elem1, void* elem2){
	double* tuple1 = (double*) elem1;
	double* tuple2 = (double*) elem2;
	return (tuple1[1] - tuple2[1]);
}

SPKDArray** Split(SPKDArray kdArr, int coor){

	SPKDArray** array_of_KDArrays; // array storing the pointers to left and right KD arrays
	int n = kdArr
	int* is_index_in_left; // array of 0's and 1's. value is 1 if the point in this index is in left half
	int i;

	//allocate memory for array_of_KDArrays
	if ( (array_of_KDArrays = (SPKDArray**)malloc(2*sizeof(SPKDArray*))) == NULL){
		spLoggerPrintError("ALLOCATION ERRORR", __FILE__, __func__, __LINE__);
		free(array_of_KDArrays);
	}

	for (i=0; i<n)


}



















