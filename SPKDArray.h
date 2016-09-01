/*
 * SPKDArray.h
 *
 *  Created on: 19  2016
 *      Author: elisheva
 */

#ifndef SPKDARRAY_H_
#define SPKDARRAY_H_
#include "SPPoint.h"
#include "SPLogger.h"
 
/** Type for defining SPKDArray **/
typedef struct sp_KDArray_t* SPKDArray;

/*
 * getter to a point from array_of_points
 *
 * @param
 * 		kdArray- the source SPKDArray
 * 		index- the index of the point to be returned
 * @assert kdArray != NULL
 * puts a copy of the point from kdArray->array_of_points that is in index index in the address p
 */
void getCopyOfPointfromArrayOfPoints(SPKDArray kdArray, int index, SPPoint* p);

/*
 * getter to an index from matrix_of_sorted_indexes
 * @param
 * 		kdArray- the source SPKDArray
 * 		row- the index of the row of the item to be returned
 * 		col- the index of the column of the item to be returned
 *  @assert kdArray != NULL
 *  @return the value in index [row][col] from matrix_of_sorted_indexes
 */
int getValFromMatrixOfSortedIndexes(SPKDArray kdArray, int row, int col);

/*
 * getter to n
 * @param
 * 		kdArray- the source SPKDArray
 * @assert kdArray != NULL
 * @return kdArray->n
 */
int getN(SPKDArray kdArray);

/*
 * getter to d
 * @param
 * 		kdArray- the source SPKDArray
 * @assert kdArray != NULL
 * @return kdArray->d
 */
int getD(SPKDArray kdArray);

/**
 * Initializes the kd-array with the data given by arr, and returns the kd-array
 * @param
 * 		arr: an array of points
 * 		size: the number of points in arr
 * @return: kd-array object
 * 			NULL if size<1 or arr==NULL
 * 			NULL if the call to spPointCopy returned NULL
 * 			NULL if allocation error occurred
 */
SPKDArray Init(SPPoint* arr, int size);

/**
 * @param
 * 		kdArr: a SPKDArray object
 * 		coor: the coordinate used to split kdArr by
 * 		left_array: pointer to store left kdArray - the first n/2 points with respect to the coordinate coor are left_array
 * 		right_array: pointer to store right kdArray-  the rest of the points are in left_array
 * @return
 * 		1 if completed successfully
 * 		-1 if kdArr==NULL or coor<0
 * 		-1 if allocation error occurred
 *
 */
int Split(SPKDArray kdArr, int coor, SPKDArray* left_array, SPKDArray* right_array);


/**
 * compare function to use in qsort
 * @param
 * 		elem1 and elem2 are arrays of size 2
 * @return
 * 		return value > 1: if the second value in elem1 is bigger than the second value in elem2
 * 		return value = 0: if the second value in elem1 and elem2 are equal
 * 		return value < 0: if the second value in elem1 is smaller than the second value in elem2
 */
int copmareByValue(const void* elem1, const void* elem2);

/**
 * Free all memory allocation associated with KDArray,
 * if KDArray is NULL nothing happens.
 * @param KDArray is the SPKDArray to be destroyed
 */
void destroyKDArray(SPKDArray KDArray);

#endif /* SPKDARRAY_H_ */
