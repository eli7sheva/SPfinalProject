/*
 * sp_kdTreeNode_unit_test.c
 *
 *  Created on: 9 ���� 2016
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
	SPPoint tmp_point;
	KDTreeNode node1;
	KDTreeNode node11;
	KDTreeNode node2;
	KDTreeNode left1 = NULL;
	KDTreeNode right1 = NULL;
	KDTreeNode left2 = NULL;
	KDTreeNode right2 = NULL;
	int intNode_result;
	//create and check first node
	intNode_result = InitNode(2, 4.0, &left1, &right1, p1, &node1);
	if (intNode_result==-1){
		DestroyKDTreeNode(node1);
	}
	ASSERT_TRUE(KDTreeNodegetDim(node1)==2);
	ASSERT_TRUE(KDTreeNodegetVal(node1)==4.0);
	ASSERT_TRUE(KDTreeNodegetLeft(node1)==NULL);
	ASSERT_TRUE(KDTreeNodegetRight(node1)==NULL);
	KDTreeNodegetData(node1, &p2);
	ASSERT_TRUE(spPointGetAxisCoor(p2,1)==70.0);
	//create and check second node
	intNode_result = InitNode(2, 4.5, &left2, &right2, p1, &node11);
	if (intNode_result==-1){
		DestroyKDTreeNode(node1);
	}
	intNode_result = InitNode(1, 2.0, &node1, &node11, NULL, &node2);
	if (intNode_result==-1){
		DestroyKDTreeNode(node1);
	}
	ASSERT_TRUE(KDTreeNodegetDim(node2)==1);
	ASSERT_TRUE(KDTreeNodegetVal(node2)==2.0);
	ASSERT_TRUE(KDTreeNodegetDim(KDTreeNodegetLeft(node2))==2);  //Dim of node1
	ASSERT_TRUE(KDTreeNodegetVal(KDTreeNodegetRight(node2))==4.5); //Val of node1
	KDTreeNodegetData(node2, &tmp_point);
	ASSERT_TRUE(tmp_point==NULL);
	spPointDestroy(tmp_point);
	//free memory
	DestroyKDTreeNode(node2); //destroys all tree so node1 and node11 will also be freed
	spPointDestroy(p1);
	spPointDestroy(p2);
	return true;
}

static bool InitNodeDiminvalidTest(){
	double data[3] = {123.0,70.0, 12.6};
	KDTreeNode node;
	KDTreeNode left=NULL;
	KDTreeNode right=NULL;
	int initNode_result;
	int val = 3;
	SPPoint p = spPointCreate(data, val, 1);
	//create and check node
	initNode_result = InitNode(-2, 23.9, &left, &right, p, &node);
	ASSERT_TRUE(initNode_result==-1);
	//free memory
	spPointDestroy(p);
	return true;
}


SPPoint* getPointArray(){
	SPPoint* point_array = (SPPoint*)malloc(5*sizeof(SPPoint));
	double data1[2] = {1.0,2.0};
	double data2[2] = {123.0,70.0};
	double data3[2] = {2.0,7.0};
	double data4[2] = {9.0,11.0};
	double data5[2] = {3.0,4.0};
	point_array[0] = spPointCreate(data1, 2, 0);
	point_array[1] = spPointCreate(data2, 2, 1);
	point_array[2] = spPointCreate(data3, 2, 2);
	point_array[3] = spPointCreate(data4, 2, 3);
	point_array[4] = spPointCreate(data5, 2, 4);
	return point_array;
}


//checks initTree with incremental
static bool InitTreeIncrementalTest(){
	SPPoint* point_array = getPointArray();
	SPPoint tmp_point;
	int split_method = 2; //incremental
	int size = 5;
	int i;
	KDTreeNode root;
	int InitTree_result;
	//sons
	KDTreeNode L_of_root=NULL;
	KDTreeNode R_of_root=NULL;
	//grandsons
	KDTreeNode LL_of_root;
	KDTreeNode LR_of_root;
	KDTreeNode RL_of_root;
	KDTreeNode RR_of_root;
	//sons of grandson
	KDTreeNode LLL_of_root;
	KDTreeNode LLR_of_root;
	//root
	InitTree_result = InitTree(point_array, size, split_method, &root);
	if(InitTree_result==-1){
		DestroyKDTreeNode(root);
	}

	//check root values
	ASSERT_TRUE(KDTreeNodegetDim(root)==0);
	ASSERT_TRUE(KDTreeNodegetVal(root)==3.0);
	KDTreeNodegetData(root,&tmp_point);
	ASSERT_TRUE(tmp_point==NULL);
	spPointDestroy(tmp_point);

	//Assign L_of_root and check its values
	L_of_root = KDTreeNodegetLeft(root);
	ASSERT_TRUE(KDTreeNodegetDim(L_of_root)==1);
	ASSERT_TRUE(KDTreeNodegetVal(L_of_root)==4.0);
	KDTreeNodegetData(L_of_root,&tmp_point);
	ASSERT_TRUE(tmp_point==NULL);
	spPointDestroy(tmp_point);

	//Assign R_of_root and check its values
	R_of_root = KDTreeNodegetRight(root);
	ASSERT_TRUE(KDTreeNodegetDim(R_of_root)==1);
	ASSERT_TRUE(KDTreeNodegetVal(R_of_root)==11.0);
	KDTreeNodegetData(R_of_root,&tmp_point);
	ASSERT_TRUE(tmp_point==NULL);
	spPointDestroy(tmp_point);

	//Assign LL_of_root and check its values
	LL_of_root = KDTreeNodegetLeft(L_of_root);
	ASSERT_TRUE(KDTreeNodegetDim(LL_of_root)==0);
	ASSERT_TRUE(KDTreeNodegetVal(LL_of_root)==1.0);
	KDTreeNodegetData(LL_of_root,&tmp_point);
	ASSERT_TRUE(tmp_point==NULL);
	spPointDestroy(tmp_point);

	//Assign LR_of_root and check its values
	LR_of_root = KDTreeNodegetRight(L_of_root);
	ASSERT_TRUE(KDTreeNodegetDim(LR_of_root)==-1);
	ASSERT_TRUE(KDTreeNodegetVal(LR_of_root)==INFINITY);
	ASSERT_TRUE(KDTreeNodegetLeft(LR_of_root)==NULL);
	ASSERT_TRUE(KDTreeNodegetRight(LR_of_root)==NULL);
	KDTreeNodegetData(LR_of_root,&tmp_point);
	ASSERT_TRUE(spPointL2SquaredDistance(tmp_point, point_array[2])==0);
	spPointDestroy(tmp_point);

	//Assign RL_of_root and check its values
	RL_of_root = KDTreeNodegetLeft(R_of_root);
	ASSERT_TRUE(KDTreeNodegetDim(RL_of_root)==-1);
	ASSERT_TRUE(KDTreeNodegetVal(RL_of_root)==INFINITY);
	ASSERT_TRUE(KDTreeNodegetLeft(RL_of_root)==NULL);
	ASSERT_TRUE(KDTreeNodegetRight(RL_of_root)==NULL);
	KDTreeNodegetData(RL_of_root,&tmp_point);
	ASSERT_TRUE(spPointL2SquaredDistance(tmp_point, point_array[3])==0);
	spPointDestroy(tmp_point);

	//Assign RR_of_root and check its values
	RR_of_root = KDTreeNodegetRight(R_of_root);
	ASSERT_TRUE(KDTreeNodegetDim(RR_of_root)==-1);
	ASSERT_TRUE(KDTreeNodegetVal(RR_of_root)==INFINITY);
	ASSERT_TRUE(KDTreeNodegetLeft(RR_of_root)==NULL);
	ASSERT_TRUE(KDTreeNodegetRight(RR_of_root)==NULL);
	KDTreeNodegetData(RR_of_root,&tmp_point);
	ASSERT_TRUE(spPointL2SquaredDistance(tmp_point, point_array[1])==0);
	spPointDestroy(tmp_point);

	//Assign LLL_of_root and check its values
	LLL_of_root = KDTreeNodegetLeft(LL_of_root);
	ASSERT_TRUE(KDTreeNodegetDim(LLL_of_root)==-1);
	ASSERT_TRUE(KDTreeNodegetVal(LLL_of_root)==INFINITY);
	ASSERT_TRUE(KDTreeNodegetLeft(LLL_of_root)==NULL);
	ASSERT_TRUE(KDTreeNodegetRight(LLL_of_root)==NULL);
	KDTreeNodegetData(LLL_of_root,&tmp_point);
	ASSERT_TRUE(spPointL2SquaredDistance(tmp_point, point_array[0])==0);
	spPointDestroy(tmp_point);

	//Assign LLR_of_root and check its values
	LLR_of_root = KDTreeNodegetRight(LL_of_root);
	ASSERT_TRUE(KDTreeNodegetDim(LLR_of_root)==-1);
	ASSERT_TRUE(KDTreeNodegetVal(LLR_of_root)==INFINITY);
	ASSERT_TRUE(KDTreeNodegetLeft(LLR_of_root)==NULL);
	ASSERT_TRUE(KDTreeNodegetRight(LLR_of_root)==NULL);
	KDTreeNodegetData(LLR_of_root,&tmp_point);
	ASSERT_TRUE(spPointL2SquaredDistance(tmp_point, point_array[4])==0);
	spPointDestroy(tmp_point);

	//free memory
	for (i=0;i<5;i++){
		spPointDestroy(point_array[i]);
	}
	free(point_array);
	DestroyKDTreeNode(root);
	return true;
}


//checks initTree with max_spread
static bool InitTreeMaxSpreadTest(){
	SPPoint* point_array = getPointArray();
	SPPoint tmp_point;
	int split_method = 1; //max_spread
	int size = 5;
	int i;
	KDTreeNode root;
	int InitTree_result;
	//sons
	KDTreeNode L_of_root=NULL;
	KDTreeNode R_of_root=NULL;
	//grandsons
	KDTreeNode LL_of_root;
	KDTreeNode LR_of_root;
	KDTreeNode RL_of_root;
	KDTreeNode RR_of_root;
	//sons of grandson
	KDTreeNode LLL_of_root;
	KDTreeNode LLR_of_root;
	//root

	InitTree_result = InitTree(point_array, size, split_method, &root);
	if(InitTree_result==-1){
		DestroyKDTreeNode(root);
	}

	//check root values
	ASSERT_TRUE(KDTreeNodegetDim(root)==0);
	ASSERT_TRUE(KDTreeNodegetVal(root)==3.0);
	KDTreeNodegetData(root,&tmp_point);
	ASSERT_TRUE(tmp_point==NULL);
	spPointDestroy(tmp_point);

	//Assign L_of_root and check its values
	L_of_root = KDTreeNodegetLeft(root);
	ASSERT_TRUE(KDTreeNodegetDim(L_of_root)==1);
	ASSERT_TRUE(KDTreeNodegetVal(L_of_root)==4.0);
	KDTreeNodegetData(L_of_root,&tmp_point);
	ASSERT_TRUE(tmp_point==NULL);
	spPointDestroy(tmp_point);

	//Assign R_of_root and check its values
	R_of_root = KDTreeNodegetRight(root);
	ASSERT_TRUE(KDTreeNodegetDim(R_of_root)==0);
	ASSERT_TRUE(KDTreeNodegetVal(R_of_root)==9.0);
	KDTreeNodegetData(R_of_root,&tmp_point);
	ASSERT_TRUE(tmp_point==NULL);
	spPointDestroy(tmp_point);

	//Assign LL_of_root and check its values
	LL_of_root = KDTreeNodegetLeft(L_of_root);
	ASSERT_TRUE(KDTreeNodegetDim(LL_of_root)==0);
	ASSERT_TRUE(KDTreeNodegetVal(LL_of_root)==1.0);
	KDTreeNodegetData(LL_of_root,&tmp_point);
	ASSERT_TRUE(tmp_point==NULL);
	spPointDestroy(tmp_point);

	//Assign LR_of_root and check its values
	LR_of_root = KDTreeNodegetRight(L_of_root);
	ASSERT_TRUE(KDTreeNodegetDim(LR_of_root)==-1);
	ASSERT_TRUE(KDTreeNodegetVal(LR_of_root)==INFINITY);
	ASSERT_TRUE(KDTreeNodegetLeft(LR_of_root)==NULL);
	ASSERT_TRUE(KDTreeNodegetRight(LR_of_root)==NULL);
	KDTreeNodegetData(LR_of_root,&tmp_point);
	ASSERT_TRUE(spPointL2SquaredDistance(tmp_point, point_array[2])==0);
	spPointDestroy(tmp_point);

	//Assign RL_of_root and check its values
	RL_of_root = KDTreeNodegetLeft(R_of_root);
	ASSERT_TRUE(KDTreeNodegetDim(RL_of_root)==-1);
	ASSERT_TRUE(KDTreeNodegetVal(RL_of_root)==INFINITY);
	ASSERT_TRUE(KDTreeNodegetLeft(RL_of_root)==NULL);
	ASSERT_TRUE(KDTreeNodegetRight(RL_of_root)==NULL);
	KDTreeNodegetData(RL_of_root,&tmp_point);
	ASSERT_TRUE(spPointL2SquaredDistance(tmp_point, point_array[3])==0);
	spPointDestroy(tmp_point);

	//Assign RR_of_root and check its values
	RR_of_root = KDTreeNodegetRight(R_of_root);
	ASSERT_TRUE(KDTreeNodegetDim(RR_of_root)==-1);
	ASSERT_TRUE(KDTreeNodegetVal(RR_of_root)==INFINITY);
	ASSERT_TRUE(KDTreeNodegetLeft(RR_of_root)==NULL);
	ASSERT_TRUE(KDTreeNodegetRight(RR_of_root)==NULL);
	KDTreeNodegetData(RR_of_root,&tmp_point);
	ASSERT_TRUE(spPointL2SquaredDistance(tmp_point, point_array[1])==0);
	spPointDestroy(tmp_point);

	//Assign LLL_of_root and check its values
	LLL_of_root = KDTreeNodegetLeft(LL_of_root);
	ASSERT_TRUE(KDTreeNodegetDim(LLL_of_root)==-1);
	ASSERT_TRUE(KDTreeNodegetVal(LLL_of_root)==INFINITY);
	ASSERT_TRUE(KDTreeNodegetLeft(LLL_of_root)==NULL);
	ASSERT_TRUE(KDTreeNodegetRight(LLL_of_root)==NULL);
	KDTreeNodegetData(LLL_of_root,&tmp_point);
	ASSERT_TRUE(spPointL2SquaredDistance(tmp_point, point_array[0])==0);
	spPointDestroy(tmp_point);

	//Assign LLR_of_root and check its values
	LLR_of_root = KDTreeNodegetRight(LL_of_root);
	ASSERT_TRUE(KDTreeNodegetDim(LLR_of_root)==-1);
	ASSERT_TRUE(KDTreeNodegetVal(LLR_of_root)==INFINITY);
	ASSERT_TRUE(KDTreeNodegetLeft(LLR_of_root)==NULL);
	ASSERT_TRUE(KDTreeNodegetRight(LLR_of_root)==NULL);
	KDTreeNodegetData(LLR_of_root,&tmp_point);
	ASSERT_TRUE(spPointL2SquaredDistance(tmp_point, point_array[4])==0);
	spPointDestroy(tmp_point);

	//free memory
	for (i=0;i<5;i++){
		spPointDestroy(point_array[i]);
	}
	free(point_array);
	DestroyKDTreeNode(root);
    return true;
}


// basic check of kNearestNeighbors
static bool KNNBasicTest(){
	SPBPQueue bpq;
	SPListElement bpq_element;
	SPPoint* point_array;
	KDTreeNode root;
	SPPoint p1;
	SPPoint p2;
	double data1[2] = {3.0,4.0};
	double data2[2] = {2.4,6.0};
	int split_method = 2; //incremental
	int size = 5;
	int i;
	int initTree_result;
	int serach_result;
	int index;

	bpq = spBPQueueCreate(3);
	point_array = getPointArray();
	initTree_result = InitTree(point_array, size, split_method, &root);
	ASSERT_TRUE(initTree_result==1); //InitTree completed successfully
	p1 = spPointCreate(data1, 2, 1);
	p2 = spPointCreate(data2, 2, 2);

	//search for a point that appears in the tree
	serach_result = kNearestNeighbors(root, bpq, &p1);
	ASSERT_TRUE(serach_result==1); //search completed successfully
	bpq_element = spBPQueuePeek(bpq);
	index = spListElementGetIndex(bpq_element);
	ASSERT_TRUE(index==4);

	//free bpq and bpq_element to use again
	spBPQueueClear(bpq);
	spListElementDestroy(bpq_element);

	//search for a point that appears in the tree
	serach_result = kNearestNeighbors(root, bpq, &p2);
	ASSERT_TRUE(serach_result==1); //search completed successfully
	bpq_element = spBPQueuePeek(bpq);
	index = spListElementGetIndex(bpq_element);
	ASSERT_TRUE(index==2);

	//free all memory
	spBPQueueDestroy(bpq);
	spListElementDestroy(bpq_element);
	for (i=0;i<5;i++){
		spPointDestroy(point_array[i]);
	}
	free(point_array);
	DestroyKDTreeNode(root);
	spPointDestroy(p1);
	spPointDestroy(p2);

	return true;
}

SPPoint* get10PointArray4D(){
	SPPoint* point_array = (SPPoint*)malloc(10*sizeof(SPPoint));
	double data0[4] = {1.0,-60.0,7.0,234.0};
	double data1[4] = {28.0, 100.0, 94.0, -70.0};
	double data2[4] = {123.0, 2.0, 21.0, -97.0};
	double data3[4] = {5.0, 6.0, -5.0, 10.0};
	double data4[4] = {333.0, -2.0, 37.0, 69.0};
	double data5[4] = {-410.0, -170.0, -249.0, 172.0};
	double data6[4] = {-484.0, -34.0, 283.0, 137.0};
	double data7[4] = {14.0, 58.0, -327.0, 155.0};
	double data8[4] = {398.0, 323.0, 411.0, 228.0};
	double data9[4] = {-150.0, 456.0, -89.0, 252.0};
	point_array[0] = spPointCreate(data0, 4, 0);
	point_array[1] = spPointCreate(data1, 4, 1);
	point_array[2] = spPointCreate(data2, 4, 2);
	point_array[3] = spPointCreate(data3, 4, 3);
	point_array[4] = spPointCreate(data4, 4, 4);
	point_array[5] = spPointCreate(data5, 4, 0);
	point_array[6] = spPointCreate(data6, 4, 1);
	point_array[7] = spPointCreate(data7, 4, 2);
	point_array[8] = spPointCreate(data8, 4, 3);
	point_array[9] = spPointCreate(data9, 4, 4);
	return point_array;
}

static bool initIncrment10p4D(){
	SPPoint* point_array = get10PointArray4D();
	int split_method = 2; //INCREMENTAL
	int size = 10;
	int i;
	KDTreeNode root;
	int InitTree_result;

	InitTree_result = InitTree(point_array, size, split_method, &root);
	if(InitTree_result==-1){
		DestroyKDTreeNode(root);
	}
	//check root
	ASSERT_TRUE(KDTreeNodegetDim(root)==0);
	ASSERT_TRUE(KDTreeNodegetVal(root)==5.0);
	//free memory
	for (i=0;i<10;i++){
		spPointDestroy(point_array[i]);
	}
	free(point_array);
	DestroyKDTreeNode(root);
	return true;
}


int main() {
	RUN_TEST(InitNodeBasicTest);
	RUN_TEST(InitNodeDiminvalidTest);
	RUN_TEST(InitTreeIncrementalTest);
	RUN_TEST(InitTreeMaxSpreadTest);
	RUN_TEST(KNNBasicTest);
	RUN_TEST(initIncrment10p4D);
	return 0;
}
