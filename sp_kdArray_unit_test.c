/*
 * sp_kdArray_unit_test.c
 *
 *  Created on: 9 баев 2016
 *      Author: elisheva
 */

#include "SPKDArray.h"
#include "unit_test_util.h"
#include <stdbool.h>

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

//check Init when size<1
static bool InitInvalidParameterSize(){
	int i;
	SPPoint* point_array = getPointArray();
	SPKDArray kdArray = Init(point_array,-5);
	ASSERT_TRUE(kdArray==NULL);
	for (i=0;i<5;i++){
		spPointDestroy(point_array[i]);
	}
	free(point_array);
	destroyKDArray(kdArray);
	return true;
}

//check Init when arr==NULL
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

}


int main() { //todo: how to run these tests if not with main?
	RUN_TEST(InitBasicTest);
	RUN_TEST(InitInvalidParameterSize);
	RUN_TEST(InitInvalidParameterArr);
	return 0;
}
