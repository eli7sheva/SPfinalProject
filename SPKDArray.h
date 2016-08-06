/*
 * SPKDArray.h
 *
 *  Created on: 19 αιεμ 2016
 *      Author: elisheva
 */

#ifndef SPKDARRAY_H_
#define SPKDARRAY_H_

#include "SPPoint.h"
 
/** Type for defining SPKDArray **/
typedef struct sp_KDArray_t* SPKDArray;

/**
 * Initializes the kd-array with the data given by arr, and returns the kd-array
 * @param
 * 		arr: an array of points
 * 		size: the number of points in arr
 * @return: kd-array object
 * 			NULL if size<1 or arr==NULL
 * 			NULL if the points in arr do not have the same dimension
 * 			NULL if the call to spPointCopy returned NULL
 * 			NULL if allocation error occurred
 */
SPKDArray Init(SPPoint* arr, int size);

/**
 * @param
 * 		kdArr: a SPKDArray object
 * 		coor: the coordinate used to split kdArr by
 * @return
 * 		two kd-arrays (kdLeft,kdRight) such that the first n/2 points with
 * 		respect to the coordinate coor are in kdLeft , and the rest of the points are in kdRight.
 * 		returns dynamic array of size two, the first element is a pointer to kdLeft and the second is a pointer to kdRight
 * 		NULL if kdArr==NULL or coor<0
 * 		NULL if allocation error occurred
 *
 */
SPKDArray** Split(SPKDArray kdArr, int coor);


/**
 * compare function to use in qsort
 * @param
 * 		elem1 and elem2 are arrays of size 2
 * @return
 * 		return value > 1: if the second value in elem1 is bigger than the second value in elem2
 * 		return value = 0: if the second value in elem1 and elem2 are equal
 * 		return value < 0: if the second value in elem1 is smaller than the second value in elem2
 */
int copmareByValue(void* elem1, void* elem2);

/**
 * Free all memory allocation associated with KDArray,
 * if KDArray is NULL nothing happens.
 * @param KDArray is the SPKDArray to be destroyed
 */
void destroyKDArray(SPKDArray KDArray);

#endif /* SPKDARRAY_H_ */
