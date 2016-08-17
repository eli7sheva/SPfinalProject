/*
 * KDTreeNode.h
 *
 *  Created on: 3  2016
 *      Author: elisheva
 */

#ifndef KDTREENODE_H_
#define KDTREENODE_H_

#include "SPKDArray.h"
#include "SPBPriorityQueue.h"


typedef struct sp_KDTreeNode_t* KDTreeNode;

/*
 * initialize a new KDTreeNode
 * @param
 * 		dim: the splitting dimension, -1 represents invalid value
 * 		val: the median value of the splitting dimension. infinity represents invalid value
 *		left: a pointer to the left subtree
 *		right: a pointer to the right subtree
 *		data: a pointer to a point (only if the current node is a leaf) otherwise this field value is NULL
 * @return
 *		a new KDTreeNode with it's field values set according to the parameters
 *		Null if dim<0 or if allocation error accrued.
 *			 the relevant error message will be sent to Logger
 */
KDTreeNode InitNode(int dim, double val, KDTreeNode left, KDTreeNode right, SPPoint data);

/*
 * getter for Dim
 * @param node: KDTreeNode to get the Dim of
 * @return node->Dim
 */
int KDTreeNodegetDim(KDTreeNode node);

/*
 * getter for Val
 * @param node: KDTreeNode to get the Val of
 * @return node->Val
 */
double KDTreeNodegetVal(KDTreeNode node);

/*
 * getter for Left
 * @param node: KDTreeNode to get the Left of
 * @return pointer to node->Left
 */
KDTreeNode KDTreeNodegetLeft(KDTreeNode node);

/*
 * getter for Right
 * @param node: KDTreeNode to get the Right of
 * @return pointer to node->Right
 */
KDTreeNode KDTreeNodegetRight(KDTreeNode node);

/*
 * returns a SPPoint copy of node->Data
 * @param node: KDTreeNode to get the Data of
 * @return NULL if node->Data==NULL,
 *			copy of node->Data otherwise.
 */
SPPoint KDTreeNodegetData(KDTreeNode node);

/*
 * creates the KD Tree by calling the recursive function CreateKDTree
 * this is an envelope function for CreateKDTree
 * @param
 * 		arr: an array of points (each point represents a feature)
 * 		size: the number of points in arr
 * 		split_method: an int representing the method to split by
 * 					 0=RANDOM, 1= MAX_SPREAD,  2=INCREMENTAL
 * @return
 * 		a KDTreeNode which is the root of the tree
 *		NULL if arr==NULL, size<1 or call to other function returned NULL.
 *		 	 the relevant error message will be sent to Logger
 */
KDTreeNode InitTree(SPPoint* arr, int size, int split_method);

/*
 * free all memory of a KDTree including all its nodes
 * @param node: a node which is the root of the tree to be destroyed
 * 				if node is NULL nothing will happen
 */
void DestroyKDTree(KDTreeNode node);

/*
 * free all memory of a KDTreeNode object
 * @param node: a node to be destroyed
 * 				if node is NULL nothing will happen
 */
void DestroyKDTreeNode(KDTreeNode node);

/*
 * searches a KDtree for the K closest points to a given point
 * @param
 * 		curr: a KDTreeNode that is the root of the KDTree
 * 		bpq: a SPBPQueue to store the closest points, K is the size of the bpq
 * 		P: the given point
 * @return
 * 		1 if the search was successful
 * 		0 if an error occurred during the search (such as an error while calling another function)
 */
int kNearestNeighbors(KDTreeNode curr , SPBPQueue bpq, SPPoint P);

#endif /* KDTREENODE_H_ */
