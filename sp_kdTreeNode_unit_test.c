/*
 * sp_kdTreeNode_unit_test.c
 *
 *  Created on: 9 баев 2016
 *      Author: elisheva
 */

#include "KDTreeNode.h"
#include "unit_test_util.h"
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>

static bool InitNodeBasicTest(){
	double data[3] = {123.0,70.0, 12.6};
	SPPoint p1 = spPointCreate(data, 3, 1);
	SPPoint p2;
	KDTreeNode node1;
	KDTreeNode node11;
	KDTreeNode node2;
	//create and check first node
	node1 = InitNode(2, 4.0, NULL, NULL, p1);
	printf("InitNodeBasicTest 1\n"); //todo remove this
	ASSERT_TRUE(KDTreeNodegetDim(node1)==2);
	ASSERT_TRUE(KDTreeNodegetVal(node1)==4.0);
	ASSERT_TRUE(KDTreeNodegetLeft(node1)==NULL);
	ASSERT_TRUE(KDTreeNodegetRight(node1)==NULL);
	p2 = KDTreeNodegetData(node1);
	ASSERT_TRUE(spPointGetAxisCoor(p2,1)==70.0);
	//create and check second node
	node11 = InitNode(2, 4.0, NULL, NULL, p1);
	node2 = InitNode(1, 2.0, node1, node11, NULL);
	printf("InitNodeBasicTest 2\n"); //todo remove this
	ASSERT_TRUE(KDTreeNodegetDim(node2)==1);
	ASSERT_TRUE(KDTreeNodegetVal(node2)==2.0);
	ASSERT_TRUE(KDTreeNodegetDim(KDTreeNodegetLeft(node2))==2);  //Dim of node1
	ASSERT_TRUE(KDTreeNodegetVal(KDTreeNodegetRight(node2))==4.0); //Val of node1
	ASSERT_TRUE(KDTreeNodegetData(node2)==NULL);
	//free memory
	DestroyKDTreeNode(node2); //destroys all tree so node1 and node11 will also be freed
	spPointDestroy(p1);
	spPointDestroy(p2);
	printf("InitNodeBasicTest 3\n"); //todo remove this
	return true;
}

static bool InitNodeDiminvalidTest(){
	double data[3] = {123.0,70.0, 12.6};
	KDTreeNode node;
	int val = 3;
	SPPoint p = spPointCreate(data, val, 1);
	//create and check node
	node = InitNode(-2, 23.9, NULL, NULL, p);
	ASSERT_TRUE(node==NULL);
	//free memory
	spPointDestroy(p);
	DestroyKDTreeNode(node);
	return true;
}

//creates an array of points to use for tests- todo-this is duplicate from kdArray test
SPPoint* getPointArray(){
	printf("getPointArray 1\n"); //todo remove this
	SPPoint* point_array = (SPPoint*)malloc(5*sizeof(SPPoint));
	printf("getPointArray 2\n"); //todo remove this
	double data1[2] = {1.0,2.0};
	double data2[2] = {123.0,70.0};
	double data3[2] = {2.0,7.0};
	double data4[2] = {9.0,11.0};
	double data5[2] = {3.0,4.0};
	printf("getPointArray 3.1\n"); //todo remove this
	point_array[0] = spPointCreate((double *)data1, 2, 1);
	printf("getPointArray 3.2\n"); //todo remove this
	point_array[1] = spPointCreate((double *)data2, 2, 1);
	printf("getPointArray 3.3\n"); //todo remove this
	point_array[2] = spPointCreate((double *)data3, 2, 1);
	printf("getPointArray 3.4\n"); //todo remove this
	point_array[3] = spPointCreate((double *)data4, 2, 1);
	printf("getPointArray 3.5\n"); //todo remove this
	point_array[4] = spPointCreate((double *)data5, 2, 1);
	printf("getPointArray 4\n"); //todo remove this
	return point_array;
}


//checks initTree with incremental
static bool InitTreeBasicTest(){
	printf("InitTreeBasicTest 1\n"); //todo remove this
	SPPoint* point_array = getPointArray();
	printf("InitTreeBasicTest 2\n"); //todo remove this
	int split_method = 2; //incremental
	int size = 5;
	int i;
	//sons
	KDTreeNode L_of_root;
	KDTreeNode R_of_root;
	//grandsons
	KDTreeNode LL_of_root;
	KDTreeNode LR_of_root;
	KDTreeNode RL_of_root;
	KDTreeNode RR_of_root;
	//sons of grandson
	KDTreeNode LLL_of_root;
	KDTreeNode LLR_of_root;
	//root
	KDTreeNode root = InitTree(point_array, size, split_method);

	printf("InitTreeBasicTest 3\n"); //todo remove this

	//check root values
	ASSERT_TRUE(KDTreeNodegetDim(root)==0);
	ASSERT_TRUE(KDTreeNodegetVal(root)==3.0);
	ASSERT_TRUE(KDTreeNodegetData(root)==NULL);

	//Assign L_of_root and check its values
	L_of_root = KDTreeNodegetLeft(root);
	ASSERT_TRUE(KDTreeNodegetDim(L_of_root)==1);
	ASSERT_TRUE(KDTreeNodegetVal(L_of_root)==4.0);
	ASSERT_TRUE(KDTreeNodegetData(L_of_root)==NULL);

	//Assign R_of_root and check its values
	R_of_root = KDTreeNodegetRight(root);
	ASSERT_TRUE(KDTreeNodegetDim(R_of_root)==1);
	ASSERT_TRUE(KDTreeNodegetVal(R_of_root)==11.0);
	ASSERT_TRUE(KDTreeNodegetData(R_of_root)==NULL);

	//Assign LL_of_root and check its values
	LL_of_root = KDTreeNodegetLeft(L_of_root);
	ASSERT_TRUE(KDTreeNodegetDim(LL_of_root)==0);
	ASSERT_TRUE(KDTreeNodegetVal(LL_of_root)==2.0);
	ASSERT_TRUE(KDTreeNodegetData(LL_of_root)==NULL);

	//Assign LR_of_root and check its values
	LR_of_root = KDTreeNodegetRight(L_of_root);
	ASSERT_TRUE(KDTreeNodegetDim(LR_of_root)==-1);
	ASSERT_TRUE(KDTreeNodegetVal(LR_of_root)==INFINITY);
	ASSERT_TRUE(KDTreeNodegetLeft(LR_of_root)==NULL);
	ASSERT_TRUE(KDTreeNodegetRight(LR_of_root)==NULL);
	ASSERT_TRUE(spPointL2SquaredDistance(KDTreeNodegetData(LR_of_root), point_array[2])==0);

	//Assign RL_of_root and check its values
	RL_of_root = KDTreeNodegetLeft(R_of_root);
	ASSERT_TRUE(KDTreeNodegetDim(RL_of_root)==-1);
	ASSERT_TRUE(KDTreeNodegetVal(RL_of_root)==INFINITY);
	ASSERT_TRUE(KDTreeNodegetLeft(RL_of_root)==NULL);
	ASSERT_TRUE(KDTreeNodegetRight(RL_of_root)==NULL);
	ASSERT_TRUE(spPointL2SquaredDistance(KDTreeNodegetData(RL_of_root), point_array[3])==0);

	//Assign RR_of_root and check its values
	RR_of_root = KDTreeNodegetRight(R_of_root);
	ASSERT_TRUE(KDTreeNodegetDim(RR_of_root)==-1);
	ASSERT_TRUE(KDTreeNodegetVal(RR_of_root)==INFINITY);
	ASSERT_TRUE(KDTreeNodegetLeft(RR_of_root)==NULL);
	ASSERT_TRUE(KDTreeNodegetRight(RR_of_root)==NULL);
	ASSERT_TRUE(spPointL2SquaredDistance(KDTreeNodegetData(RR_of_root), point_array[1])==0);

	//Assign LLL_of_root and check its values
	LLL_of_root = KDTreeNodegetLeft(LL_of_root);
	ASSERT_TRUE(KDTreeNodegetDim(LLL_of_root)==-1);
	ASSERT_TRUE(KDTreeNodegetVal(LLL_of_root)==INFINITY);
	ASSERT_TRUE(KDTreeNodegetLeft(LLL_of_root)==NULL);
	ASSERT_TRUE(KDTreeNodegetRight(LLL_of_root)==NULL);
	ASSERT_TRUE(spPointL2SquaredDistance(KDTreeNodegetData(LLL_of_root), point_array[0])==0);

	//Assign LLL_of_root and check its values
	LLR_of_root = KDTreeNodegetRight(LL_of_root);
	ASSERT_TRUE(KDTreeNodegetDim(LLR_of_root)==-1);
	ASSERT_TRUE(KDTreeNodegetVal(LLR_of_root)==INFINITY);
	ASSERT_TRUE(KDTreeNodegetLeft(LLR_of_root)==NULL);
	ASSERT_TRUE(KDTreeNodegetRight(LLR_of_root)==NULL);
	ASSERT_TRUE(spPointL2SquaredDistance(KDTreeNodegetData(LLR_of_root), point_array[4])==0);

	//free memory
	for (i=0;i<5;i++){
		spPointDestroy(point_array[i]);
	}
	free(point_array);
	DestroyKDTreeNode(root);
	return true;
}



int main() {
	RUN_TEST(InitNodeBasicTest);
	RUN_TEST(InitNodeDiminvalidTest);
	RUN_TEST(InitTreeBasicTest);
	return 0;
}
