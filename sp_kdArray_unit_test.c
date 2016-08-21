/*
 * sp_kdArray_unit_test.c
 *
 *  Created on: 9 баев 2016
 *      Author: elisheva
 */

#include "SPKDArray.h"
#include "unit_test_util.h"
#include <stdlib.h>
#include <stdbool.h>

//creates an array of points to use for tests
SPPoint* getPointArray(){
	SPPoint* point_array = (SPPoint*)malloc(5*sizeof(SPPoint));
	double data1[2] = {1.0,2.0};
	double data2[2] = {123.0,70.0};
	double data3[2] = {2.0,7.0};
	double data4[2] = {9.0,11.0};
	double data5[2] = {3.0,4.0};
	point_array[0] = spPointCreate((double *)data1, 2, 1);
	point_array[1] = spPointCreate((double *)data2, 2, 1);
	point_array[2] = spPointCreate((double *)data3, 2, 1);
	point_array[3] = spPointCreate((double *)data4, 2, 1);
	point_array[4] = spPointCreate((double *)data5, 2, 1);
	return point_array;
}

/*
 * basic test for Init, when all parameters are valid
 * in this test after Init this is what the values of kdArray should be:
 * kdArray->array_of_points = [(1.0,2.0),(123.0,70.0),(2.0,7.0),(9.0,11.0),(3.0,4.0)]
 * kdArray->matrix_of_sorted_indexes = [[0,2,4,3,1],
 * 										[0,4,2,3,1]]
 * kdArray->n = 5
 * kdArray->d = 2
 */
static bool InitBasicTest(){
	int i;
	SPPoint* point_array = getPointArray();
	SPKDArray kdArray = Init(point_array,5);
	SPPoint p0 = getCopyOfPointfromArrayOfPoints(kdArray,0);
	SPPoint p2 = getCopyOfPointfromArrayOfPoints(kdArray,2);
	SPPoint p4 = getCopyOfPointfromArrayOfPoints(kdArray,4);
	ASSERT_TRUE(spPointGetAxisCoor(p0, 0)==1.0);
	ASSERT_TRUE(spPointGetAxisCoor(p2, 1)==7.0);
	ASSERT_TRUE(spPointGetAxisCoor(p4, 0)==3.0);
	ASSERT_TRUE(spPointGetAxisCoor(p4, 1)==4.0);
	ASSERT_TRUE(getN(kdArray) == 5);
	ASSERT_TRUE(getD(kdArray) == 2);
	ASSERT_TRUE(getValFromMatrixOfSortedIndexes(kdArray,0,0) == 0);
	ASSERT_TRUE(getValFromMatrixOfSortedIndexes(kdArray,1,2) == 2);
	ASSERT_TRUE(getValFromMatrixOfSortedIndexes(kdArray,0,4) == 1);
	ASSERT_TRUE(getValFromMatrixOfSortedIndexes(kdArray,0,2) == 4);
	for (i=0;i<5;i++){
		spPointDestroy(point_array[i]);
	}
	free(point_array);
	destroyKDArray(kdArray);
	free(p0);
	free(p2);
	free(p4);
	return true;
}

//check Init when the parameter size<1
static bool InitInvalidParameterSize(){
	int i;
	SPPoint* point_array = getPointArray();
	SPKDArray kdArray = Init(point_array,-5);// -5 is an invalid size
	ASSERT_TRUE(kdArray==NULL);
	for (i=0;i<5;i++){
		spPointDestroy(point_array[i]);
	}
	free(point_array);
	destroyKDArray(kdArray);
	return true;
}

//check Init when the parameter arr==NULL
static bool InitInvalidParameterArr(){
	SPPoint* point_array = NULL;
	SPKDArray kdArray = Init(point_array,5);
	ASSERT_TRUE(kdArray==NULL);
	destroyKDArray(kdArray);
	return true;
}

/*
 * basic test for Split, when all parameters are valid
 *
 * in this test after Split this is what the values of *left* kdArray should be:
 * kdArray->array_of_points = [(1.0,2.0),(2.0,7.0),(3.0,4.0)]
 * kdArray->matrix_of_sorted_indexes = [[0,1,2],
 * 										[0,2,1]]
 * kdArray->n = 3
 * kdArray->d = 2
 *
 * in this test after Split this is what the values of *right* kdArray should be:
 * kdArray->array_of_points = [((123.0,70.0),(9.0,11.0)]
 * kdArray->matrix_of_sorted_indexes = [[1,0],
 * 										[1,0]]
 * kdArray->n = 2
 * kdArray->d = 2
 */
static bool SplitBasicTest(){
	int i;
	SPPoint* point_array = getPointArray();
	SPKDArray kdArray = Init(point_array,5);
	SPKDArray* array_of_kdArrays = Split(kdArray, 0);
	SPPoint p00 = getCopyOfPointfromArrayOfPoints(array_of_kdArrays[0],0);
	SPPoint p02 = getCopyOfPointfromArrayOfPoints(array_of_kdArrays[0],2);
	SPPoint p11 = getCopyOfPointfromArrayOfPoints(array_of_kdArrays[1],1);
	ASSERT_TRUE(getN(array_of_kdArrays[0]) == 3);
	ASSERT_TRUE(getD(array_of_kdArrays[0]) == 2);
	ASSERT_TRUE(getN(array_of_kdArrays[1]) == 2);
	ASSERT_TRUE(getD(array_of_kdArrays[1]) == 2);
	ASSERT_TRUE(spPointGetAxisCoor(p00, 1)==2.0);
	ASSERT_TRUE(spPointGetAxisCoor(p02, 0)==3.0);
	ASSERT_TRUE(spPointGetAxisCoor(p11, 1)==11.0);
	ASSERT_TRUE(getValFromMatrixOfSortedIndexes(array_of_kdArrays[0],0,0) == 0);
	ASSERT_TRUE(getValFromMatrixOfSortedIndexes(array_of_kdArrays[0],0,2) == 2);
	ASSERT_TRUE(getValFromMatrixOfSortedIndexes(array_of_kdArrays[0],1,1) == 2);
	ASSERT_TRUE(getValFromMatrixOfSortedIndexes(array_of_kdArrays[1],0,1) == 0);
	ASSERT_TRUE(getValFromMatrixOfSortedIndexes(array_of_kdArrays[1],1,0) == 1);
	for (i=0;i<5;i++){
		spPointDestroy(point_array[i]);
	}
	free(point_array);
	destroyKDArray(kdArray);
	destroyKDArray(array_of_kdArrays[0]);
	destroyKDArray(array_of_kdArrays[1]);
	free(array_of_kdArrays);
	free(p00);
	free(p02);
	free(p11);
	return true;
}

//check split when the parameter coor<0
static bool SplitInvalidParameterCoor(){
	int i;
	SPPoint* point_array = getPointArray();
	SPKDArray kdArray = Init(point_array,5);
	SPKDArray* array_of_kdArrays = Split(kdArray, -1); // -1 is an invalid coordinate
	ASSERT_TRUE(array_of_kdArrays==NULL);
	for (i=0;i<5;i++){
		spPointDestroy(point_array[i]);
	}
	free(point_array);
	destroyKDArray(kdArray);
	destroyKDArray(array_of_kdArrays[0]);
	destroyKDArray(array_of_kdArrays[1]);
	free(array_of_kdArrays);
	return true;
}

//check split when the parameter kdArr==NULL
static bool SplitInvalidParameterKdArr(){
	SPKDArray kdArray=NULL;
	SPKDArray* array_of_kdArrays = Split(kdArray, 0);
	ASSERT_TRUE(array_of_kdArrays==NULL);
	return true;
}

//check basic case of getCopyOfPointfromArrayOfPoints
static bool getCopyOfPointfromArrayOfPointsTest(){
	int i;
	SPPoint p;
	SPPoint* point_array = getPointArray();
	SPKDArray kdArray = Init(point_array,5);
	p = getCopyOfPointfromArrayOfPoints(kdArray, 2);
	ASSERT_TRUE(spPointGetAxisCoor(p, 0) == 2.0);
	ASSERT_TRUE(spPointGetAxisCoor(p, 1) == 7.0);
	ASSERT_TRUE(spPointGetIndex(p) == 1);
	ASSERT_TRUE(spPointGetDimension(p) == 2);
	spPointDestroy(p);
	for (i=0;i<5;i++){
		spPointDestroy(point_array[i]);
	}
	free(point_array);
	destroyKDArray(kdArray);
	return true;
}

int main() {
	RUN_TEST(InitBasicTest);
	RUN_TEST(InitInvalidParameterSize);
	RUN_TEST(InitInvalidParameterArr);
	RUN_TEST(SplitBasicTest);
	RUN_TEST(SplitInvalidParameterCoor);
	RUN_TEST(SplitInvalidParameterKdArr);
	RUN_TEST(getCopyOfPointfromArrayOfPointsTest);
	return 0;
}
