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
	//int n;
	SPPoint p0;
	SPPoint p2;
	SPPoint p4;
	SPPoint* point_array = getPointArray();
	SPKDArray kdArray = Init(point_array,5);
	getCopyOfPointfromArrayOfPoints(kdArray,0, &p0);
	getCopyOfPointfromArrayOfPoints(kdArray,2,&p2);
	getCopyOfPointfromArrayOfPoints(kdArray,4,&p4);
	ASSERT_TRUE(spPointGetAxisCoor(p0, 0)==1.0);
	ASSERT_TRUE(spPointGetAxisCoor(p2, 1)==7.0);
	ASSERT_TRUE(spPointGetAxisCoor(p4, 0)==3.0);
	ASSERT_TRUE(spPointGetAxisCoor(p4, 1)==4.0);
	ASSERT_TRUE(getN(kdArray) == 5);
	ASSERT_TRUE(getD(kdArray) == 2);
	ASSERT_TRUE(getValFromMatrixOfSortedIndexes(kdArray,0,0) == 0);
	ASSERT_TRUE(getValFromMatrixOfSortedIndexes(kdArray,0,4) == 1);
	ASSERT_TRUE(getValFromMatrixOfSortedIndexes(kdArray,0,2) == 4);
	ASSERT_TRUE(getValFromMatrixOfSortedIndexes(kdArray,1,2) == 2);
	for (i=0;i<5;i++){
		spPointDestroy(point_array[i]);
	}
	free(point_array);
	destroyKDArray(kdArray);
	spPointDestroy(p0);
	spPointDestroy(p2);
	spPointDestroy(p4);
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
	SPKDArray left;
	SPKDArray right;
	SPKDArray left1;
	SPKDArray right1;
	int split_result;
	SPPoint p00;
	SPPoint p02;
	SPPoint p11;
	SPPoint* point_array = getPointArray();

	SPKDArray kdArray = Init(point_array,5);

	split_result = Split(kdArray, 0, &left, &right);
	ASSERT_TRUE(split_result==1);

	getCopyOfPointfromArrayOfPoints(left,0,&p00);
	getCopyOfPointfromArrayOfPoints(left,2,&p02);
	getCopyOfPointfromArrayOfPoints(right,1,&p11);

	ASSERT_TRUE(getN(left) == 3);
	ASSERT_TRUE(getD(left) == 2);
	ASSERT_TRUE(getN(right) == 2);
	ASSERT_TRUE(getD(right) == 2);
	ASSERT_TRUE(spPointGetAxisCoor(p00, 1)==2.0);
	ASSERT_TRUE(spPointGetAxisCoor(p02, 0)==3.0);
	ASSERT_TRUE(spPointGetAxisCoor(p11, 1)==11.0);
	ASSERT_TRUE(getValFromMatrixOfSortedIndexes(left,0,0) == 0);
	ASSERT_TRUE(getValFromMatrixOfSortedIndexes(left,0,1) == 1);
	ASSERT_TRUE(getValFromMatrixOfSortedIndexes(left,0,2) == 2);
	ASSERT_TRUE(getValFromMatrixOfSortedIndexes(left,1,0) == 0);
	ASSERT_TRUE(getValFromMatrixOfSortedIndexes(left,1,1) == 2);
	ASSERT_TRUE(getValFromMatrixOfSortedIndexes(left,1,2) == 1);
	ASSERT_TRUE(getValFromMatrixOfSortedIndexes(right,0,0) == 1);
	ASSERT_TRUE(getValFromMatrixOfSortedIndexes(right,0,1) == 0);
	ASSERT_TRUE(getValFromMatrixOfSortedIndexes(right,1,0) == 1);
	ASSERT_TRUE(getValFromMatrixOfSortedIndexes(right,1,1) == 0);

	//split left half again
	split_result = Split(left, 1, &left1, &right1);

	/*res_split1[0]->matrix_of_sorted_indexes = [[0,1],
	 * 									         [0,1]]
	 */
	ASSERT_TRUE(getValFromMatrixOfSortedIndexes(left1,0,0) == 0);
	ASSERT_TRUE(getValFromMatrixOfSortedIndexes(left1,0,1) == 1);
	ASSERT_TRUE(getValFromMatrixOfSortedIndexes(left1,1,0) == 0);
	ASSERT_TRUE(getValFromMatrixOfSortedIndexes(left1,1,1) == 1);

	for (i=0;i<5;i++){
		spPointDestroy(point_array[i]);
	}
	free(point_array);
	destroyKDArray(kdArray);
	destroyKDArray(left);
	destroyKDArray(right);
	destroyKDArray(left1);
	destroyKDArray(right1);
	spPointDestroy(p00);
	spPointDestroy(p02);
	spPointDestroy(p11);
	return true;
}

//check split when the parameter coor<0
static bool SplitInvalidParameterCoor(){
	int i;
	SPPoint* point_array = getPointArray();
	SPKDArray kdArray = Init(point_array,5);
	SPKDArray left;
	SPKDArray right;
	int split_result;
	split_result = Split(kdArray, -1, &left, &right); // -1 is an invalid coordinate
	ASSERT_TRUE(split_result==-1);
	for (i=0;i<5;i++){
		spPointDestroy(point_array[i]);
	}
	free(point_array);
	destroyKDArray(kdArray);
	return true;
}

//check split when the parameter kdArr==NULL
static bool SplitInvalidParameterKdArr(){
	SPKDArray kdArray=NULL;
	SPKDArray left;
	SPKDArray right;
	int split_result;
	split_result = Split(kdArray, 0, &left, &right);
	ASSERT_TRUE(split_result==-1);
	return true;
}

//check basic case of getCopyOfPointfromArrayOfPoints
static bool getCopyOfPointfromArrayOfPointsTest(){
	int i;
	SPPoint p;
	SPPoint* point_array = getPointArray();
	SPKDArray kdArray = Init(point_array,5);
	getCopyOfPointfromArrayOfPoints(kdArray, 2, &p);
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
