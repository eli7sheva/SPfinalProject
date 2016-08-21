/*
 * SPPoint.c
 *
 *  Created on: 22 במאי 2016
 *      Author: elisheva
 */
#include "SPPoint.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h> ////TODO remove this with all the prints



struct sp_point_t{
	int index;
	int dim;
	double* coordinates;
};

	SPPoint spPointCreate(double* data, int dim, int index){

		SPPoint point;
		int i;
		printf("spPointCreate 1\n"); //todo remove this
		// check validation of parameters
		if (data == NULL || dim <= 0 || index < 0) {
			return NULL;
		}
		printf("spPointCreate 2\n"); //todo remove this
		point = (SPPoint)malloc(sizeof(*point));
		printf("spPointCreate 3\n"); //todo remove this
		if (point == NULL){
			printf("spPointCreate 4\n"); //todo remove this
			spPointDestroy(point);
			return NULL;
		}
		printf("spPointCreate 5\n"); //todo remove this
		// allocate memory for point->coordinates
		point->coordinates = (double*)malloc(dim*sizeof(double));
		printf("spPointCreate 6\n"); //todo remove this
		if (point->coordinates == NULL){ //Allocation failure - need to free(point)
			printf("spPointCreate 7\n"); //todo remove this
			spPointDestroy(point);
			return NULL;
		}
		printf("spPointCreate 8\n"); //todo remove this
		//get data
		for (i=0; i<dim; i++){
			point->coordinates[i] = data[i];
		}

		printf("spPointCreate 9\n"); //todo remove this
		// initializing the rest of point arguments
		point->index = index;
		point->dim = dim;

		printf("spPointCreate 10\n"); //todo remove this
		return point;
		}

	SPPoint spPointCopy(SPPoint source){
		printf("spPointCopy 1\n"); //todo remove this
		SPPoint copiedPoint;
		printf("spPointCopy 2\n"); //todo remove this
		// assertions
		assert (source != NULL);
		printf("spPointCopy 3\n"); //todo remove this

		copiedPoint = spPointCreate(source->coordinates, source->dim, source->index);
		printf("spPointCopy 4\n"); //todo remove this
		if (copiedPoint==NULL){
			printf("spPointCopy 4-null\n"); //todo remove this
			return NULL;
		}
		printf("spPointCopy 5\n"); //todo remove this
		return copiedPoint;
	}

	void spPointDestroy(SPPoint point){
		//if point is NULL nothing happens
		if (point==NULL){
			return;
		}
		if (point->coordinates!=NULL){
			free(point->coordinates);
		}
		free(point);
	}

	int spPointGetDimension(SPPoint point){
		assert (point != NULL);
		return point->dim;
	}

	int spPointGetIndex(SPPoint point){
		assert (point != NULL);
		return point->index;
	}

	void spPointSetIndex(SPPoint point, int index_val){
		assert (point != NULL);
		point->index = index_val;
		return;
	}

	double spPointGetAxisCoor(SPPoint point, int axis){
		assert(point!=NULL && axis < point->dim);
		return point->coordinates[axis];
	}

	double spPointL2SquaredDistance(SPPoint p, SPPoint q){
		double sum = 0.0;   // Holds temporary sum
		double pVal;  // holds current p_i value
		double qVal;  // holds current q_i value
		double pqSub; // holds current p_i-q_i value
		int dim;      // dim of p and q
		int i;

		assert(p!=NULL && q!=NULL && p->dim == q->dim);

		dim = p->dim;
		for (i=0; i<dim; i++){
			pVal = spPointGetAxisCoor(p, i); // get current p_i value
			qVal = spPointGetAxisCoor(q, i); // get current q_i value
			pqSub = pVal-qVal;
			sum += (pqSub*pqSub);
		}
		return sum;
	}















