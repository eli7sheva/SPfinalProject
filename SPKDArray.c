/*
 * SPKDArray.c
 *
 *  Created on: 19 αιεμ 2016
 *      Author: elisheva
 */
#include "SPKDArray.h"
#include <stdlib.h>
#include <assert.h>

struct sp_KDArray_t{
	SPPoint* array_of_points;
	int** matrix_of_sorted_indexes;
	int n; // number of points in array_of_points, and number of columns in matrix_of_sorted_indexes
	int d; // number of dimensions of each point in array_of_points, and number of rows in matrix_of_sorted_indexes
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

	// initialize n and d
	KDArray->n = n;
	KDArray->d = d;

	// allocate memory for KDArray->array_of_points
	if ( (KDArray->array_of_points = (SPPoint*)malloc(n*sizeof(SPPoint))) == NULL){
		spLoggerPrintError("ALLOCATION ERRORR", __FILE__, __func__, __LINE__);
		free(KDArray->array_of_points);
		free(KDArray);
	}

	//copy each point from arr to KDArray->array_of_points
	for (i=0; i<n; i++){
		KDArray->array_of_points[i] = spPointCopy(arr[i]); //spPointCopy returns NULL if an allocation error occurred
		if (KDArray->array_of_points[i] == NULL){
			spLoggerPrintError("spPointCopy returned NULL", __FILE__, __func__, __LINE__);
			for (j=0; j<=i; j++){
				spPointDestroy(KDArray->array_of_points[j]);
			}
			free(KDArray->array_of_points);
			free(KDArray);
			return NULL;
		}
	}

	//allocate memory for KDArray->matrix_of_sorted_indexes, this will be a d*n matrix, the i'th row is the indexes of the points in arr sorted according to their i'th dimension
	if ( (KDArray->matrix_of_sorted_indexes = (int**)malloc(d*sizeof(int*))) == NULL){
		spLoggerPrintError("ALLOCATION ERRORR", __FILE__, __func__, __LINE__);
		free(KDArray->matrix_of_sorted_indexes);
		free(KDArray->array_of_points);
		free(KDArray);
	}
	for (i=0; i<d; i++){
		if( (KDArray->matrix_of_sorted_indexes[i]=(int*)malloc(n*sizeof(int))) == NULL){
			spLoggerPrintError("ALLOCATION ERRORR", __FILE__, __func__, __LINE__);
			for (j=0; j<=i; j++){
				free(KDArray->matrix_of_sorted_indexes[j]);
			}
			free(KDArray->matrix_of_sorted_indexes);
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
			KDArray->matrix_of_sorted_indexes[i][j] = index_val_arr[j][0];
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
	int n = kdArr.n;
	int num_of_left_points;
	int num_of_right_points;
	int* is_index_in_left; // array of 0's and 1's. value is 1 if the point in this index is in left half
	SPPoint* left_points;
	SPPoint* right_points;
	int i, j, k;

	//allocate memory for array_of_KDArrays
	if ( (array_of_KDArrays = (SPKDArray**)malloc(2*sizeof(SPKDArray*))) == NULL){
		spLoggerPrintError("ALLOCATION ERRORR", __FILE__, __func__, __LINE__);
		free(array_of_KDArrays);
	}

	//allocate memory for is_index_in_left
	if ( (is_index_in_left = (int*)malloc(n*sizeof(int))) == NULL){
		spLoggerPrintError("ALLOCATION ERRORR", __FILE__, __func__, __LINE__);
		free(is_index_in_left);
	}

	//initialize num_of_leftt_points and num_of_right_points
	if (n%2==0){
		num_of_left_points = n/2;
	}
	else{
		num_of_left_points = (n/2) +1;
	}
	num_of_right_points = n - num_of_left_points;

	// fill is_index_in_left
	for (i=0; i<num_of_left_points; i++){
		is_index_in_left[kdArr.matrix_of_sorted_indexes[coor][i]] = 1;
	}
	for (i=num_of_left_points; i<n; i++){
		is_index_in_left[kdArr.matrix_of_sorted_indexes[coor][i]] = 0;
	}

	//allocate memory for left_points
	if ( (left_points = (SPPoint*)malloc(num_of_left_points*sizeof(SPPoint))) == NULL){
		spLoggerPrintError("ALLOCATION ERRORR", __FILE__, __func__, __LINE__);
		free(left_points);
	}

	//allocate memory for right_points
	if ( (right_points = (SPPoint*)malloc((num_of_right_points)*sizeof(SPPoint))) == NULL){
			spLoggerPrintError("ALLOCATION ERRORR", __FILE__, __func__, __LINE__);
			free(right_points);
		}
	j=0; //index counter for left_points
	k=0; //index counter for right_points

	// fill left_points and right_points
	for (int i=0; i<n; i++){ // i= index counter for is_index_in_left
		if (is_index_in_left[i]==1){
			left_points[j]= spPointCopy(kdArr.array_of_points[i]);
			j++;
		}
		else{
			right_points[k]= spPointCopy(kdArr.array_of_points[i]);
			k++;
		}
	}
	//TODO: i copied the points do we need the original or to erase it?
	//TODO: in general can we clear kdArr in the end?

	// assertions on j and k after previous for loop
	assert (j==num_of_left_points);
	assert (k==num_of_right_points);
}



















