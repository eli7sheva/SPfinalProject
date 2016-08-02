/*
 * SPKDArray.h
 *
 *  Created on: 19 αιεμ 2016
 *      Author: elisheva
 */

#ifndef SPKDARRAY_H_
#define SPKDARRAY_H_

/** Type for defining SPKDArray **/
typedef struct sp_KDArray_t* SPKDArray;

/**
 * Initializes the kd-array with the data given by arr, and returns pointer to the kd-array
 * returns NULL if the points in arr do not have the same dimension
 * returns NULL if the call to spPointCopy returned NULL
 * prints error message to logger in case of memory allocation failure
 */
SPKDArray* Init(SPPoint* arr, int size);

/** returns two kd-arrays (kdLeft,kdRight) such that the first n/2 points with
 * respect to the coordinate coor are in kdLeft , and the rest of the points are in kdRight.
 * returns dynamic array of size two, the first element is a pointer to kdLeft and the second is a pointer to kdRight
 */
SPKDArray** Split(SPKDArray kdArr, int coor);


/**
 * compare function to use in qsort
 */
int copmareByValue(void* elem1, void* elem2);

/**
 * Free all memory allocation associated with KDArray,
 * if KDArray is NULL nothing happens.
 */
void destroyKDArray(SPKDArray KDArray);

#endif /* SPKDARRAY_H_ */
