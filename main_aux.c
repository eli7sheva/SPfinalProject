#include <stdlib.h>
#include <stdio.h>
#include "main_aux.h"
#include "SPLogger.h"

//#define ALLOC_ERROR_MSG "Allocation error" - already defined in files that are included
#define GENERAL_ERROR_MSG "An error occurred"
#define ALLOC_ERROR_MSG "An error occurred - allocation failure\n"
#define INVALID_ARGUMENT "Invalid argument"
#define PARAMETER_FEATURE_A_INVALID "value of the parameter featureA is invalid, cann't be NULL"
#define PARAMETER_ROOT_INVALID "value of the parameter root is invalid, cann't be NULL"
#define PARAMETER_SP_KNN_INVALID "value of the parameter spKNN is invalid, mast be spKNN>=1"
#define SPBP_QUEUE_CREATE_RETURNED_NULL "the call to spBPQueueCreate returned NULL"
#define KNN_RETURNED_NULL "the call to kNearestNeighbors returned NULL"
#define ERR_DEQUEUE "a problem occurred during the call to spBPQueueDequeue"

/*
* this function is called from qsort and it sorts array which each cell contains array of length two
* this function sorts in ascending order by the first element of each cell.
* if two elements are equal it sorts by the second element of each cell.
*
* this function is useful to sort array of doubles by ascending order, in case there are two elements
* that are equal, then the one with the smallest index will be the smaller one.
* to do so create a new array where it's first row is your original array and it's second row is the indexes of each element
* and sort it with this function.
*
* @param a - array of doubles of size two.
* @param b - array of doubles of size two.
* @return 1 if a is bigger than b
*         -1 if b is bigger than a
*         0 if a is equal to be (both the first and second elelments)
*

*/

int compare(const void *a, const void * b) {
    double* p1 = *(double**) a;
    double* p2 = *(double**) b;

    if (p1[0] > p2[0]) {
        return 1;
    }

    else if (p1[0] < p2[0]) {
        return -1;
    }

    else { // p1[0] == p2[0]
        if (p1[1] > p2[1]) {
            return 1;
        }

        else if (p1[1] < p2[1]) {
            return -1;
        }
        else {
            return 0;

        }
    }

}

/** given an array of doubles returns an array containing the indexes of the
 *  lowest nearestNImages numbers from the original array
  * @param arr - array of L2-Squared distances 
  * @param size - the size of array arr
  * @param nearestNImages the number of the best (closest) distances to find
  * 
  * Note: prints errors to logger
  * assumes Logger is initialized
  *
  * returns an array containing the indexes of the lowest nearestNImages numbers from the original array
  *         or NULL in case of error
  */
int* nearestImages(double* arr, int size, int nearestNImages){
    /*
    * in order to return lowest nearestNImages numbers from arr, this function sorts arr using qsort and keeping indexes order
    * arr_with_indixes - will hold the matrix of dimensions size X 2 where first row is a copy of arr and second row is the indexes of each value in arr
    *                     which will be sorted
    */
    double** arr_with_indixes;  // doc above
    int* first_minimums;        // the nearestNImages indexes that are the minimum values
    int i;
    int j;

    // if asked to find miminums more than the values in the array, return NULL
    if (nearestNImages > size) {
        spLoggerPrintError(INVALID_ARGUMENT, __FILE__, __func__, __LINE__);
        return NULL;
    }

    // allocate array of minimums (size of nearestNImages)
    first_minimums = (int*) malloc(nearestNImages*sizeof(int));
    if (first_minimums == NULL){
        spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
        fflush(NULL);
        return NULL;
    }

    if ((arr_with_indixes = (double**)malloc(size*sizeof(double*))) == NULL) {
        spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
        free(first_minimums);
        fflush(NULL);
        return NULL;
    }

    for (i = 0; i < size; i++){
        if ((arr_with_indixes[i] = (double *)malloc(2*sizeof(double))) == NULL ) {
            spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
            free(first_minimums);
            for (j=0;j<i;j++){
                free(arr_with_indixes[j]);
            }
            free(arr_with_indixes);
            fflush(NULL);
            return NULL;
        }
    }

    // initialize arr_with_indexes
    for(i = 0; i < size; i++){
        arr_with_indixes[i][0] = arr[i];    // copy value from arr
        arr_with_indixes[i][1] = i;         // set index before sorting
    }

    // sort
    qsort(arr_with_indixes, size, sizeof(double*), compare);

    // copy nearestNImages smallest values' indexes
    for (i=0; i<nearestNImages; i++) {
        first_minimums[i] = (int)arr_with_indixes[i][1];
    }

    free(arr_with_indixes);
    return first_minimums;
}

/**
 * Given a KDTreeNode containing all points (features), finds the
 * closest spKNN to a given feature (featureA). The function returns the
 * INDEXES of the IMAGES to which the closest features belong, stored in an array in ascending order
 * (Closest feature image index is first, second closest feature image index is second, etc...).
 * Assumptions:
 *   - Tie break - In case featureA has the same distance ( k-nearest-neighbor) from two features,
 *     then the feature that corresponds to the smallest image
 *     index in the database is closer.
 *
 *   - The returned result may contain duplicates in case two features belongs to the same image.
 *
 *@param spKNN             - The number of indexes to return.
 *@param featureA          - the feature which will be compared with the other descriptor
 *@param root              - A KDTreeNode representing the root of the KdTree
 * 								in which the features of the images are stored.
 *
 * Note: prints errors to logger
 * assumes Logger is initialized
 *
 * @return - NULL if either the following:
 *           	* featureA is NULL
 *           	* root is NULL
 *           	* spKNN < 1
 *           	* call to spBPQueueCreate returned NULL
 *           	* call to kNearestNeighbors failed
 *           	* call to spBPQueueDequeue failed
 *           	* allocation error occurred
 *           otherwise, an array of size spKNN is returned such that:
 *           * Given that f1, f2, ... the closest features to featureA (i.e d(featureA,f1) <= d(featureA,f2) <= ...)
 *           * i1, i2, .... are the indexes of the images to which fi belongs (i.e f1 is a SIFT descriptor of image i1,
 *             f2 is a SIFT descriptor of image i2 etc..)
 *           Then the array returned is {i1,i2,...,i_spKNN}
 */
int* getSPKNNClosestFeatures(int spKNN, SPPoint featureA, KDTreeNode root){
	int i;
	int* best_spKNN_features;   // will hold bestSPKNNFeatures images indexes
	int knnResult;              //the value returned from the call to knnResult. equals 1 iff the function ran with no problems
	SPBPQueue bpq;              // the SPBPQueue where the kNearestNeighbors function will store the image indexes
	SPListElement currElem;     //will hold the current element from bpq
	SP_BPQUEUE_MSG dequeueMsg;  // the message returned from the call to spBPQueueDequeue

	//check validation of parameters

	if (spKNN<1){
		spLoggerPrintError(INVALID_ARGUMENT, __FILE__, __func__, __LINE__);
		spLoggerPrintDebug(PARAMETER_SP_KNN_INVALID, __FILE__, __func__, __LINE__);
		return NULL;
	}
	if (featureA==NULL){
		spLoggerPrintError(INVALID_ARGUMENT, __FILE__, __func__, __LINE__);
		spLoggerPrintDebug(PARAMETER_FEATURE_A_INVALID, __FILE__, __func__, __LINE__);
		return NULL;
	}
	if (root==NULL){ //|| root==NULL || spKNN<1){
		spLoggerPrintError(INVALID_ARGUMENT, __FILE__, __func__, __LINE__);
		spLoggerPrintDebug(PARAMETER_ROOT_INVALID, __FILE__, __func__, __LINE__);
		return NULL;
	}

	//create new SPBPQueue
	bpq = spBPQueueCreate(spKNN);
	if (bpq==NULL){
		spLoggerPrintError(GENERAL_ERROR_MSG, __FILE__, __func__, __LINE__);
		spLoggerPrintDebug(SPBP_QUEUE_CREATE_RETURNED_NULL, __FILE__, __func__, __LINE__);
		return NULL;
	}

	//call kNearestNeighbors
	knnResult = kNearestNeighbors(root, bpq, featureA);
	if (knnResult!=1){
		spLoggerPrintError(GENERAL_ERROR_MSG, __FILE__, __func__, __LINE__);
		spLoggerPrintDebug(KNN_RETURNED_NULL, __FILE__, __func__, __LINE__);
		return NULL;
	}

	//allocate memory for best_spKNN_features
	if ((best_spKNN_features = (int*)malloc(spKNN*sizeof(int)))==NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		free(best_spKNN_features);
		return NULL;
	}

	//move data from bpq to best_spKNN_features
	for(i=0; i<spKNN; i++){
		currElem =  spBPQueuePeek(bpq);
		best_spKNN_features[i]=spListElementGetIndex(currElem); //the index of the element is the index of the image the feature is from
		spListElementDestroy(currElem);
		dequeueMsg = spBPQueueDequeue(bpq);
		// if a problem occurred during the call to spBPQueueDequeue
		if (dequeueMsg!=SP_BPQUEUE_SUCCESS){
			spLoggerPrintError(GENERAL_ERROR_MSG, __FILE__, __func__, __LINE__);
			spLoggerPrintDebug(ERR_DEQUEUE, __FILE__, __func__, __LINE__);
			spBPQueueDestroy(bpq);
			return NULL;
		}

	}
	spBPQueueDestroy(bpq);

	return best_spKNN_features;
}

int* getKClosestImages(int nearestKImages, int bestNFeatures, SPPoint* queryFeatures, KDTreeNode databaseFeatures, int queryNumOfFeatures, int numberOfImages, int* nFeaturesPerImage) { // todo elisheva change KD_TREE. Should SPPoint* be a KD_ARRAY?please add to #include everything needed here, and in the makefile (add it in the makefile as a dependency to main_aux.o)
    //todo: is the parameter nFeaturesPerImage needed? i don't need it for knn, but do we need for somthing else?
	int i;
    int j;
    int k;
    int* featureClosestImages; // array holds the spKNN indexes of the closest images to a feature of the  query
    double* hintsPerImage;  // array counts number of hints per image
    int* closestImages; // array holds the spNumOfSimilarImages indexes of the closest images to the query image

    // return NULL if one of the parameters is not initialized
    if (!queryFeatures || !databaseFeatures || (queryNumOfFeatures <=1) || (numberOfImages <=1) || !nFeaturesPerImage) {
        spLoggerPrintError(INVALID_ARGUMENT, __FILE__, __func__, __LINE__);
        return NULL;
    }

    if ((hintsPerImage = (double*)calloc(numberOfImages, sizeof(double))) == NULL) {
        spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
        return NULL;
    }

    // for each feature of query calculate the spBestSIFTL2SquaredDistance
    for (i=0; i< queryNumOfFeatures; i++){
    	//todo: in next line replace getNClosestImagesForFeature with getSPKNNClosestFeatures
        featureClosestImages = getSPKNNClosestFeatures(bestNFeatures, queryFeatures[i], //todo find another way to get spKNN
            databaseFeatures);

        if (featureClosestImages == NULL) {
            // free everythin
            free(hintsPerImage);
            free(featureClosestImages);
            return NULL; // error log is printed inside getNClosestImagesForFeature
        }

        // Counting hits
        for (k = 0; k < numberOfImages; k++) { // k is image index
            for (j = 0; j < bestNFeatures; j++) { // todo find other way to get spKNN
                if (featureClosestImages[j] == k) {
                    hintsPerImage[k]--;
                }
            }
        }
    }

    // find the nearest images
    closestImages = nearestImages(hintsPerImage, numberOfImages, nearestKImages); // todo find another way to get spNumOfSimilarImages
    if (closestImages == NULL) {
        // free everything
        free(hintsPerImage);
        free(featureClosestImages);
        return NULL; // error log is printed inside nearestImages

    }
    return closestImages;
}


