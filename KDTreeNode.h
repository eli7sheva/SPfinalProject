/*
 * KDTreeNode.h
 *
 *  Created on: 3 баев 2016
 *      Author: elisheva
 */

#ifndef KDTREENODE_H_
#define KDTREENODE_H_
#include "SPKDArray.h"
 
typedef struct sp_KDTreeNode_t* KDTreeNode;

/*
 * initialize a new KDTreeNode
 * @param
 * 		dim: the splitting dimension
 * 		val: the median value of the splitting dimension
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
 * creates the KD Tree by calling the recursive function CreateKDTree
 * this is an envelope function for CreateKDTree
 * @param
 * 		arr: an array of points (each point represents a feature)
 * 		size: the number of points in arr
 * @return
 * 		a KDTreeNode which is the root of the tree
 *		NULL if arr==NULL, size<1 or call to other function returned NULL.
 *		 	 the relevant error message will be sent to Logger
 */
KDTreeNode InitTree(SPPoint* arr, int size);


/*
 * the recursive function creating the KD tree
 * @param
 * 		KDArray: a SPKDArray object
 * 		last_split_dim: the dimension that was used for split in the last recursive call
 * @return
 * 		a KDTreeNode which is the root of the tree
 */
KDTreeNode CreateKDTree(SPKDArray KDArray,int last_split_dim);

/*
 * finds the dimension with the highest spread
 * @param KDArray a SPKDArray object
 * @return the dimension to split by if the parameter is MAX_SPREAD
 * 		   if there are several candidates returns the lowest dimension
 * 		   returns -1 if an allocation error occurred
 */
int getDimentionMaxSpread(SPKDArray KDArray);

/*
 * returns the dimension to split by if the parameter is RANDOM
 * @param KDArray a SPKDArray object
 * @return a random int between 0 and KDArray->d -1
 */
int getDimentionRandom(SPKDArray KDArray);

/*
 *
 * free all memory of a KDTreeNode object
 * @param node: a node which is the root of the tree to be destroyed
 */
void DestroyKDTreeNode(KDTreeNode node);

#endif /* KDTREENODE_H_ */
