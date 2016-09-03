/*
 * SPKDArray.c
 *
 *  Created on: 19  2016
 *      Author: elisheva
 */
#include "SPKDArray.h"
#include <stdlib.h>
#include <assert.h>

#define ALLOC_ERROR_MSG "Allocation error"
#define INVALID_ARG_ERROR "Invalid arguments"
#define GENERAL_ERROR_MSG "An error occurred"
#define PARAMETER_SIZE_INVALID "value of the parameter size is invalid, must be size > 0"
#define PARAMETER_ARR_INVALID "value of the parameter arr is invalid, cann't be NULL"
#define SPPOINTCOPY_RETURNED_NULL "spPointCopy returned NULL"
#define PARAMETER_COOR_INVALID "value of the parameter coor is invalid, must be coor>=0"
#define PARAMETER_KDARR_INVALID "value of the parameter kdArr is invalid,cann't be NULL"


struct sp_KDArray_t{
	SPPoint* array_of_points;
	int** matrix_of_sorted_indexes;
	int n; // number of points in array_of_points, and number of columns in matrix_of_sorted_indexes
	int d; // number of dimensions of each point in array_of_points, and number of rows in matrix_of_sorted_indexes
};

void getCopyOfPointfromArrayOfPoints(SPKDArray kdArray, int index, SPPoint* p){
	assert (kdArray != NULL);
	if (kdArray->array_of_points[index]==NULL){
		*p = NULL;
		return;
	}
	*p = spPointCopy(kdArray->array_of_points[index]);
}

int getValFromMatrixOfSortedIndexes(SPKDArray kdArray, int row, int col){
	assert (kdArray != NULL);
	return kdArray->matrix_of_sorted_indexes[row][col];
}

int getN(SPKDArray kdArray){
	assert (kdArray != NULL);
	return kdArray->n;
}

int getD(SPKDArray kdArray){
	assert (kdArray != NULL);
	return kdArray->d;
}

SPKDArray Init(SPPoint* arr, int size){
	SPKDArray KDArray;
	int d;                  // d = the dimension of the points (assuming dimension is the same for all points)
	double** index_val_arr; //double array containing n rows. each row contains the index and the value of a specific coordinate in the point of that index
	int i,j;

	//check validation of the parameters
	if (size<1){
		spLoggerPrintError(INVALID_ARG_ERROR, __FILE__, __func__, __LINE__);
		spLoggerPrintDebug(PARAMETER_SIZE_INVALID, __FILE__, __func__, __LINE__);
		return NULL;
	}
	if (arr==NULL){
		spLoggerPrintError(INVALID_ARG_ERROR, __FILE__, __func__, __LINE__);
		spLoggerPrintDebug(PARAMETER_ARR_INVALID, __FILE__, __func__, __LINE__);
		return NULL;
	}

	//initialize d
	d = spPointGetDimension(arr[0]);

	//allocate memory for KDArray
	if ((KDArray = (SPKDArray)malloc(sizeof(*KDArray)))==NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		return NULL;
	}

	// allocate memory for KDArray->array_of_points
	if ( (KDArray->array_of_points = (SPPoint*)malloc(size*sizeof(SPPoint))) == NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		free(KDArray);
		return NULL;
	}

	//copy each point from arr to KDArray->array_of_points
	for (i=0; i<size; i++){
		KDArray->array_of_points[i] = spPointCopy(arr[i]); //spPointCopy returns NULL if an allocation error occurred
		if (KDArray->array_of_points[i] == NULL){
			spLoggerPrintError(GENERAL_ERROR_MSG, __FILE__, __func__, __LINE__);
			spLoggerPrintDebug(SPPOINTCOPY_RETURNED_NULL, __FILE__, __func__, __LINE__);
			for (j=0; j<i; j++){
				spPointDestroy(KDArray->array_of_points[j]);
			}
			free(KDArray->array_of_points);
			free(KDArray);
			return NULL;
		}
	}

	//allocate memory for KDArray->matrix_of_sorted_indexes, this will be a d*n matrix, the i'th row is the indexes of the points in arr sorted according to their i'th dimension
	if ( (KDArray->matrix_of_sorted_indexes = (int**)malloc(d*sizeof(int*))) == NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		for (i=0; i<size; i++){
			spPointDestroy(KDArray->array_of_points[i]);
		}
		free(KDArray->array_of_points);
		free(KDArray);
		return NULL;
	}
	for (i=0; i<d; i++){
		if( (KDArray->matrix_of_sorted_indexes[i]=(int*)malloc(size*sizeof(int))) == NULL){
			spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
			for (j=0; j<i; j++){
				free(KDArray->matrix_of_sorted_indexes[j]);
			}
			free(KDArray->matrix_of_sorted_indexes);
			
			for (i=0; i<size; i++){
				spPointDestroy(KDArray->array_of_points[j]);
			}
			free(KDArray->array_of_points);
			free(KDArray);


			return NULL;
		}
	}

	//allocate memory for index_val_arr: n rows, 2 columns
	if ( (index_val_arr = (double**)malloc(size*sizeof(double*))) == NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		for (j=0; j<d; j++){
			free(KDArray->matrix_of_sorted_indexes[j]);
		}
		free(KDArray->matrix_of_sorted_indexes);
		
		for (i=0; i<size; i++){
			spPointDestroy(KDArray->array_of_points[j]);
		}
		free(KDArray->array_of_points);
		free(KDArray);
		return NULL;
	}

	for (i=0; i<size; i++){
		if( (index_val_arr[i]=(double*)malloc(2*sizeof(double))) == NULL){
			spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
			for (j=0; j<i; j++){
				free(index_val_arr[j]);
			}
			free(index_val_arr);

			for (j=0; j<d; j++){
				free(KDArray->matrix_of_sorted_indexes[j]);
			}
			free(KDArray->matrix_of_sorted_indexes);
			
			for (i=0; i<size; i++){
				spPointDestroy(KDArray->array_of_points[j]);
			}
			free(KDArray->array_of_points);
			free(KDArray);

			return NULL;
		}
	}
	// for each coordinate
	for (i=0; i<d; i++){ // i=coordinate

		// fill index and value of coordinate to val_index_arr
		for (j=0; j<size; j++){ //j=index of point
			index_val_arr[j][0] = (double)j;
			index_val_arr[j][1] = spPointGetAxisCoor(arr[j],i);
		}

		/* sort the rows in index_val_arr by the value in the second column,
		 * meaning by the value of the coordinate of each point.
		 * the first column will be the indexes of the points sorted by the values of the coordinate
		 */
		qsort(index_val_arr, size, sizeof(index_val_arr[0]), copmareByValue);

		// fill the sorted indexes in to KDArray->Array
		for (j=0; j<size; j++){ //j=index of point
			KDArray->matrix_of_sorted_indexes[i][j] = (int)index_val_arr[j][0];
		}
	}

	// initialize n and d
	KDArray->n = size;
	KDArray->d = d;

	for (j=0; j<size; j++){
		free(index_val_arr[j]);
	}
	free(index_val_arr);

	return KDArray;
}

int copmareByValue(const void* elem1, const void* elem2){
	double* tuple1 = *(double**) elem1;
	double* tuple2 = *(double**) elem2;
	return ( tuple1[1] - tuple2[1] );
}

int Split(SPKDArray kdArr, int coor, SPKDArray* left_array, SPKDArray* right_array){
	int n;                  		   // number of points in kdArr
	int d;                  	       // number of dimensions of each point in kdArr
	int num_of_left_points;            // number of points that will be in the left half
	int num_of_right_points;           // number of points that will be in the right half
	int* is_index_in_left;              // array of 0's and 1's. value is 1 if the point in this index is in left half
	int* map_indexes;                  // mapping from the indexes of the points in kdArr to the indexes in left or right half
	int index;
	int i, j, k, l;


	//check validation of arguments
	if (coor<0){
		spLoggerPrintError(INVALID_ARG_ERROR, __FILE__, __func__, __LINE__);
		spLoggerPrintDebug(PARAMETER_COOR_INVALID, __FILE__, __func__, __LINE__);
		return -1;
	}

	if (kdArr==NULL){
		spLoggerPrintError(INVALID_ARG_ERROR, __FILE__, __func__, __LINE__);
		spLoggerPrintDebug(PARAMETER_KDARR_INVALID, __FILE__, __func__, __LINE__);
		return -1;
	}

	//initialize n and d
	n = kdArr->n;
	d = kdArr->d;

	//initialize num_of_left_points and num_of_right_points
	if (n%2==0){
		num_of_left_points = n/2;
	}
	else{
		num_of_left_points = (n+1)/2;
	}
	num_of_right_points = n - num_of_left_points;

	//allocate memory for (*left array)= (is of type SPKDArray)
	if (((*left_array) = (SPKDArray)malloc(sizeof(**left_array)))==NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		free(left_array);
		return -1;
	}

	//allocate memory for left array->array_of_points
	if ( ((*left_array)->array_of_points=(SPPoint*)malloc(num_of_left_points*sizeof(SPPoint))) == NULL ){
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		free((*left_array));
		free(left_array);
		return -1;
	}
	//Initialize left_array->matrix_of_sorted_indexes
	if ( ((*left_array)->matrix_of_sorted_indexes=(int**)malloc(d*sizeof(int*)))==NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		free((*left_array)->array_of_points);
		free((*left_array));
		free(left_array);
		return -1;
	}
	for (i=0; i<d; i++){
		if (((*left_array)->matrix_of_sorted_indexes[i]=(int*)malloc(num_of_left_points*sizeof(int)))==NULL){
			for (j=0; j<i;j++){
				free((*left_array)->matrix_of_sorted_indexes[j]);
			}
			free((*left_array)->matrix_of_sorted_indexes);
			free((*left_array)->array_of_points);
			free((*left_array));
			free(left_array);
			return -1;
		}
	}

	//allocate memory for (*right array)= (is of type SPKDArray)
	if (((*right_array) = (SPKDArray)malloc(sizeof(**right_array)))==NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		free(right_array);
		for (j=0; j<num_of_left_points;j++){
			free((*left_array)->matrix_of_sorted_indexes[j]);
		}
		free((*left_array)->matrix_of_sorted_indexes);
		free((*left_array)->array_of_points);
		free((*left_array));
		free(left_array);
		return -1;
	}
	//allocate memory for right array->array_of_points
	if ( ((*right_array)->array_of_points=(SPPoint*)malloc(num_of_right_points*sizeof(SPPoint))) == NULL ){
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		free((*right_array));
		free(right_array);
		for (j=0; j<num_of_left_points;j++){
			free((*left_array)->matrix_of_sorted_indexes[j]);
		}
		free((*left_array)->matrix_of_sorted_indexes);
		free((*left_array)->array_of_points);
		free((*left_array));
		free(left_array);
		return -1;
	}
	//Initialize right_array->matrix_of_sorted_indexes
	if (((*right_array)->matrix_of_sorted_indexes=(int**)malloc(d*sizeof(int*)))==NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		free((*right_array)->array_of_points);
		free((*right_array));
		free(right_array);
		for (j=0; j<num_of_left_points;j++){
			free((*left_array)->matrix_of_sorted_indexes[j]);
		}
		free((*left_array)->matrix_of_sorted_indexes);
		free((*left_array)->array_of_points);
		free((*left_array));
		free(left_array);
		return -1;
	}
	for (i=0; i<d; i++){
		if (((*right_array)->matrix_of_sorted_indexes[i]=(int*)malloc(num_of_left_points*sizeof(int)))==NULL){
			for (j=0; j<i;j++){
				free((*right_array)->matrix_of_sorted_indexes[j]);
			}
			free((*right_array)->array_of_points);
			free((*left_array));
			free(right_array);
			for (j=0; j<num_of_left_points;j++){
				free((*left_array)->matrix_of_sorted_indexes[j]);
			}
			free((*left_array)->matrix_of_sorted_indexes);
			free((*left_array)->array_of_points);
			free((*left_array));
			free(left_array);
			return -1;
		}
	}

	//initialize n and d for left_array and right_array
	(*left_array)->d = d;
	(*left_array)->n = num_of_left_points;
	(*right_array)->d = d;
	(*right_array)->n = num_of_right_points;

	//allocate memory for is_index_in_left
	if ( (is_index_in_left = (int*)malloc(n*sizeof(int))) == NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		for (j=0; j<num_of_right_points;j++){
			free((*right_array)->matrix_of_sorted_indexes[j]);
		}
		free((*right_array)->array_of_points);
		free((*left_array));
		free(right_array);
		for (j=0; j<num_of_left_points;j++){
			free((*left_array)->matrix_of_sorted_indexes[j]);
		}
		free((*left_array)->matrix_of_sorted_indexes);
		free((*left_array)->array_of_points);
		free((*left_array));
		free(left_array);
		return -1;
	}


	// fill is_index_in_left
	for (i=0; i<num_of_left_points; i++){
		is_index_in_left[kdArr->matrix_of_sorted_indexes[coor][i]] = 1;
	}

	for (i=num_of_left_points; i<n; i++){
		is_index_in_left[kdArr->matrix_of_sorted_indexes[coor][i]] = 0;
	}

	j=0; //index counter for left_points
	k=0; //index counter for right_points

	// fill left->array_of_points and right->array_of_points
	for (i=0; i<n; i++){ // i= index counter for is_index_in_left
		if (is_index_in_left[i]==1){
			(*left_array)->array_of_points[j]= spPointCopy(kdArr->array_of_points[i]);
			j++;
		}
		else{ //is_index_in_left[i]==0
			(*right_array)->array_of_points[k]= spPointCopy(kdArr->array_of_points[i]);
			k++;
		}
	}

	// assertions on j and k after previous for loop
	assert(j==num_of_left_points);
	assert(k==num_of_right_points);

	//allocate memory for map_indexes
	if ( (map_indexes= (int*)malloc(n*sizeof(int)))==NULL ){
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		free(is_index_in_left);
		for (j=0; j<num_of_right_points;j++){
			free((*right_array)->matrix_of_sorted_indexes[j]);
		}
		free((*right_array)->array_of_points);
		free((*left_array));
		free(right_array);
		for (j=0; j<num_of_left_points;j++){
			free((*left_array)->matrix_of_sorted_indexes[j]);
		}
		free((*left_array)->matrix_of_sorted_indexes);
		free((*left_array)->array_of_points);
		free((*left_array));
		free(left_array);
		return -1;
	}

	//fill map_indexes
	j=0; //counter for left
	k=0; //counter for right
	for (i=0; i<n; i++){
		if (is_index_in_left[i]==1){ //i is an index that belongs to left
			map_indexes[i] = j;
			j++;
		}
		else{ //i is an index that belongs to right
			map_indexes[i] = k;
			k++;
		}
	}

	// assertions on j and k after previous for loop
	assert(j==num_of_left_points);
	assert(k==num_of_right_points);

	k=0; //counter for right
	l=0; //counter for left
	//fill right_sorted_indexes and left_sorted_indexes
	for (i=0; i<d; i++){
		for(j=0; j<n; j++){
			index = kdArr->matrix_of_sorted_indexes[i][j];
			if (is_index_in_left[index]==1){ //the index to map belongs to left
				(*left_array)->matrix_of_sorted_indexes[i][l] = map_indexes[index];
				l++;
			}
			else{ //the index to map belongs to right
				(*right_array)->matrix_of_sorted_indexes[i][k] = map_indexes[index];
				k++;
			}
		}
		// assertions on j and l after previous for loop
		assert(l==num_of_left_points);
		assert(k==num_of_right_points);
		//reset counters
		k=0;
		l=0;
	}

	//free memory and return
	free(map_indexes);
	free(is_index_in_left);
	return 1;
}

void destroyKDArray(SPKDArray KDArray){
	int i;
	int n; //number of points
	int d; //number of dimensions

	//if KDArray==NULL returns
	if (KDArray==NULL){
		return;
	}

	n = KDArray->n;
	d = KDArray->d;

	if (KDArray->array_of_points!=NULL){
		for (i=0; i<n; i++){
			spPointDestroy(KDArray->array_of_points[i]);
		}
		free(KDArray->array_of_points);
	}

	if (KDArray->matrix_of_sorted_indexes!=NULL){
		for(i=0; i<d;i++){
			if (KDArray->matrix_of_sorted_indexes[i]!=NULL){
				free(KDArray->matrix_of_sorted_indexes[i]);
			}
		}
		free(KDArray->matrix_of_sorted_indexes);
	}
	free(KDArray);
}















