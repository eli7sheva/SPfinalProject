#include <stdlib.h>
#include <stdio.h>
#include "main_aux.h"
#include "SPLogger.h"

#define ALLOC_ERROR_MSG "Allocation error"
#define GENERAL_ERROR_MSG "An error occurred"

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
// TOdo elisheve - this function is taken from ex_2 - if you also use this make sure there is not duplicated code and mive this function to a place both of s can use
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
        return NULL;
    }

    // allocate array of minimums (size of nearestNImages)
    first_minimums = (int*) malloc(nearestNImages*sizeof(int));
    if (first_minimums == NULL){
        printf(ALLOCATION_FAILURE_MSG);
        fflush(NULL);
        return NULL;
    }

    if ((arr_with_indixes = (double**)malloc(size*sizeof(double*))) == NULL) {
        printf(ALLOCATION_FAILURE_MSG);
        free(first_minimums);
        fflush(NULL);
        return NULL;
    }

    for (i = 0; i < size; i++){
        if ((arr_with_indixes[i] = (double *)malloc(2*sizeof(double))) == NULL ) {
            printf(ALLOCATION_FAILURE_MSG);
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
 * Given sift descriptors of the images in the database (databaseFeatures), finds the
 * closest bestNFeatures to a given SIFT feature (featureA). The function returns the
 * INDEXES of the images to which the closest features belong, stored in ascending order
 * (Closest feature image index is first, second closest feature image index is second, etc...).
 * Assumptions:
 *   - Tie break - In case featureA has the same distance ( k-nearest-neighbor) from two features,
 *     then the feature that corresponds to the smallest image
 *     index in the database is closer.
 *
 *   - The returned result may contain duplicates in case two features belongs to the same image.
 *
 *   - databaseFeatures is an array of two dimensional arrays, the number of elements
 *     in databaseFeatures is numberOfImages.
 *
 *   - Each entry in databaseFeatures corresponds to the features of some image in the database.
 *     The ith entry corresponds to the features of image_i in the database, and it is a two dimensional
 *     array of dimension nFeaturesPerImage[i]X128.
 *
 *   - The number of descriptors for the ith image is nFeaturesPerImage[i]
 *
 * @param bestNFeatures     - The number of indexes to return.
 * @param featureA          - the feature which will be compared with the other descriptor
 * @param databaseFeatures  - A KD_ARRAY in which the features of the images are stored.
 *                            The ith entry of the array corresponds to the features of the ith image in the database
 * @param numberOfImages    - The number of images in the database. (Number of entries in databaseFeatures)
 * @param nFeaturesPerImage - The number of features for each image. 
 * @return - NULL if either the following:
 *           * featureA is NULL
 *           * databaseFeatures is NULL
 *           * numberOfImages <= 1
 *           * nFeaturesPerImage is NULL
 *           * allocation error occurred
 *           otherwise, an array of size bestNFeatures is returned such that:
 *           * Given that f1, f2, ... the closest features to featureA (i.e d(featureA,f1) <= d(featureA,f2) <= ...)
 *           * i1, i2, .... are the indexes of the images to which fi belongs (i.e f1 is a SIFT descriptor of image i1,
 *             f2 is a SIFT descriptor of image i2 etc..)
 *           Then the array returned is {i1,i2,...,i_bestNFeatures}
 */
int* getNClosestImagesForFeature(int bestNFeatures, SPPoint* featureA, 
        KDTreeNode databaseFeatures, int numberOfImages,
        int* nFeaturesPerImage) {


    int image, feature;             // for iteration
    double **all_features_dist;     // will hold distance and image index for every feature
    double current_dist;            // will hold the current features distance
    int total_num_of_features = 0;  // amount of all features in all images
    int feature_counter = 0;        // count number of features passed
    int * best_n_features_images;   // will hold bestNFeatures images indexes
    // return NULL if one of the parameters is not initialized
    if (!featureA || !databaseFeatures || (numberOfImages <=1) || !nFeaturesPerImage) {
        return NULL;
    }

    // find total amount of features (including all images)
    for (image = 0; image < numberOfImages; image++) {
        total_num_of_features += nFeaturesPerImage[image];
    }

    // allocate dynamic memory and return NULL if allocation fails
    if ((all_features_dist = (double**)malloc(total_num_of_features*sizeof(double*))) == NULL) {
        printf(ALLOCATION_FAILURE_MSG);
        fflush(NULL);
        return NULL;
    }


    for (feature = 0; feature < total_num_of_features; feature++){
        if ((all_features_dist[feature] = (double *)malloc(2*sizeof(double))) == NULL ) {
            printf(ALLOCATION_FAILURE_MSG);
            fflush(NULL);
            free(all_features_dist);
            return NULL;
        }
    }

    if ((best_n_features_images = (int *)malloc(bestNFeatures*sizeof(int))) == NULL ){
        printf(ALLOCATION_FAILURE_MSG);
        free(all_features_dist);
        fflush(NULL);
        return NULL;
    }


    // calc distance of each feature with featureA
    for (image = 0; image < numberOfImages; image++) {
        for (feature = 0; feature < nFeaturesPerImage[image]; feature++) {

            //todo Elisheve: change kNearestNeighbor to your function (change input and output as well) and
            // iterate over the points in databaseFeatures
            current_dist = kNearestNeighbor(featureA, databaseFeatures[image][feature]); // compare between two features.
            // save distance (at index 0) and image index (at index 1) for every feature
            all_features_dist[feature_counter][0] = current_dist;
            all_features_dist[feature_counter][1] = (double)image; // image index
            feature_counter++;
        }
    }
    // now feature_counter is equal to total_num_of_features

    // sort by distances (from lowest to highest) and keep images indexes sorted too
    qsort(all_features_dist, total_num_of_features, sizeof(double*), compare);

    // take only bestNFeatures first features images indexes 
    for (feature = 0; feature < bestNFeatures; feature++) {
        best_n_features_images[feature] = (int)all_features_dist[feature][1]; // imagen index
    }

    free(all_features_dist); // free unused memory
    return best_n_features_images;
}


int* getKClosestImages(int nearestKImages, int bestNFeatures, SPPoint* queryFeatures, KDTreeNode databaseFeatures, int queryNumOfFeatures, int numberOfImages, int* nFeaturesPerImage) { // todo elisheva change KD_TREE. Should SPPoint* be a KD_ARRAY?please add to #include everything needed here, and in the makefile (add it in the makefile as a dependency to main_aux.o)
    int i;
    int j;
    int k;
    int* featureClosestImages; // array holds the spKNN indexes of the closest images to a feature of the  query
    double* hintsPerImage;  // array counts number of hints per image
    int* closestImages; // array holds the spNumOfSimilarImages indexes of the closest images to the query image

    // return NULL if one of the parameters is not initialized
    if (!queryFeatures || !databaseFeatures || (queryNumOfFeatures <=1) || (numberOfImages <=1) || !nFeaturesPerImage) {
     //todo  wrom argument error 
        return NULL;
    }

    if ((hintsPerImage = (double*)calloc(numberOfImages, sizeof(double))) == NULL) {
        //todo  allocation error 
        return NULL;
    }

    // for each feature of query calculate the spBestSIFTL2SquaredDistance
    for (i=0; i< queryNumOfFeatures; i++){
        featureClosestImages = getNClosestImagesForFeature(bestNFeatures, queryFeatures[i], //todo find another way to get spKNN
            databaseFeatures, numberOfImages, nFeaturesPerImage);

        if (featureClosestImages == NULL) {
            // free everything
            // todo error and print log
            free(hintsPerImage);
            free(featureClosestImages);
            return NULL; // todo log
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
    closestImages = nearestImages(hintsPerImage, numberOfImages, nearestKImages); // todo fund another way to get spNumOfSimilarImages
    if (closestImages == NULL) {
        //todo  allocation error 
        // free everything
        free(hintsPerImage);
        free(featureClosestImages);
        return NULL; // todo log

    } 
    return closestImages;
}


/*@param spKNN             - The number of indexes to return.
 *@param featureA          - the feature which will be compared with the other descriptor
 *@param root              - A KDTreeNode representing the root of the KdTree
 * 								in which the features of the images are stored.
 * */
//todo: finish comments on this function
int* getSPKNNClosestFeatures(int spKNN, SPPoint* featureA, KDTreeNode root){
	int i;
	int* best_spKNN_features;   // will hold bestSPKNNFeatures images indexes
	int knnResult;              //the value returned from the call to knnResult. equals 1 iff the function ran with no problems
	SPBPQueue bpq;              // the SPBPQueue where the kNearestNeighbors function will store the image indexes
	SPListElement currElem;     //will hold the current element from bpq
	SP_BPQUEUE_MSG dequeueMsg;  // the message returned from the call to spBPQueueDequeue

	//create new SPBPQueue
	bpq = spBPQueueCreate(spKNN);

	//call kNearestNeighbors
	knnResult = kNearestNeighbors(root, bpq, &featureA);
	if (knnResult!=1){
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
		best_spKNN_features[i]=currElem->index; //the index of the element is the index of the image the feature is from
		spListElementDestroy(currElem);
		dequeueMsg = spBPQueueDequeue(bpq);
		// a problem occurred during the call to spBPQueueDequeue
		if (dequeueMsg!=SP_BPQUEUE_SUCCESS){
			spLoggerPrintError(GENERAL_ERROR_MSG, __FILE__, __func__, __LINE__);
			spLoggerPrintspLoggerPrintDebug(dequeueMsg, __FILE__, __func__, __LINE__);
			spBPQueueDestroy(bpq);
			return NULL;
		}

	}
	spBPQueueDestroy(bpq);

	return best_spKNN_features;
}
