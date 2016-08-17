/*
 * sp_kdTreeNode_unit_test.c
 *
 *  Created on: 9 баев 2016
 *      Author: elisheva
 */

#include "KDTreeNode.h"
#include "unit_test_util.h"
#include <stdbool.h>

static bool InitNodeBasicTest(){
	double data[3] = {123.0,70.0, 12.6};
	SPPoint p1 = spPointCreate(data, 3, 1);
	SPPoint p2;
	KDTreeNode node1;
	KDTreeNode node2;
	//create and check first node
	node1 = InitNode(2, 4.0, NULL, NULL, p1);
	ASSERT_TRUE(KDTreeNodegetDim(node1)==2);
	ASSERT_TRUE(KDTreeNodegetVal(node1)==4.0);
	ASSERT_TRUE(KDTreeNodegetLeft(node1)==NULL);
	ASSERT_TRUE(KDTreeNodegetRight(node1)==NULL);
	p2 = KDTreeNodegetData(node1);
	ASSERT_TRUE(spPointGetAxisCoor(p2,1)==2.0);
	//create and check second node
	node2 = InitNode(1, 2.0, node1, node1, NULL);
	ASSERT_TRUE(KDTreeNodegetDim(node2)==1);
	ASSERT_TRUE(KDTreeNodegetVal(node2)==2.0);
	ASSERT_TRUE(KDTreeNodegetDim(KDTreeNodegetLeft(node2))==2);  //Dim of node1
	ASSERT_TRUE(KDTreeNodegetVal(KDTreeNodegetRight(node2))==4.0); //Val of node1
	ASSERT_TRUE(KDTreeNodegetData(node2)==NULL);
	//free memory
	spPointDestroy(p1);
	spPointDestroy(p2);
	DestroyKDTreeNode(node1);
	DestroyKDTreeNode(node1);
	return true;
}

static bool InitNodeDiminvalidTest(){
	double data[3] = {123.0,70.0, 12.6};
	KDTreeNode node;
	int val = 3;
	SPPoint p = spPointCreate(data, 3, 1);
	//create and check node
	node = InitNode(-2, 23.9, NULL, NULL, p);
	ASSERT_TRUE(node==NULL);
	//free memory
	spPointDestroy(p);
	DestroyKDTreeNode(node);
	return true;
}





int main() {
	RUN_TEST(InitNodeBasicTest);
	RUN_TEST(InitNodeDiminvalidTest);
	RUN_TEST();
	RUN_TEST();
	RUN_TEST();
	RUN_TEST();
	return 0;
}
