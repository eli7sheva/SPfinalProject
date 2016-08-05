/*
 * KDTreeNode.h
 *
 *  Created on: 3 баев 2016
 *      Author: elisheva
 */

#ifndef KDTREENODE_H_
#define KDTREENODE_H_

typedef struct sp_KDTreeNode_t* KDTreeNode;

/*
 * creates the KD Tree by calling the recursive function CreateKDTree
 * this is an envelope function for CreateKDTree
 */
KDTreeNode InitTree(SPPoint* arr, int size);

/*
 * the recursive function creating the KD tree
 */
KDTreeNode CreateKDTree(SPKDArray KDArray);

/*
 * returns the dimension to split by if the parameter is MAX_SPREAD
 * finds the dimension with the highest spread
 * if there are several candidates returns the lowest dimension
 */
int getDimentionMaxSpread(SPKDArray KDArray);

/*
 * returns the dimension to split by if the parameter is Random
 * returns a random number between 0 and KDArray->d -1
 */
int getDimentionRandom(SPKDArray KDArray);


#endif /* KDTREENODE_H_ */
