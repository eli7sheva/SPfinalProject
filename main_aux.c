#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "main_aux.h"
#include "SPLogger.h"
#include "SPExtractFeatures.h"
#include "KDTreeNode.h"

//#define ALLOC_ERROR_MSG "Allocation error" - already defined in files that are included
#define GENERAL_ERROR_MSG               "An error occurred"
#define ALLOC_ERROR_MSG                 "An error occurred - allocation failure"
#define INVALID_ARGUMENT                "Invalid argument"
#define PARAMETER_FEATURE_A_INVALID     "value of the parameter featureA is invalid, cann't be NULL"
#define PARAMETER_ROOT_INVALID          "value of the parameter root is invalid, cann't be NULL"
#define PARAMETER_SP_KNN_INVALID        "value of the parameter spKNN is invalid, must be spKNN>=1"
#define SPBP_QUEUE_CREATE_RETURNED_NULL "the call to spBPQueueCreate returned NULL"
#define KNN_RETURNED_NULL               "the call to kNearestNeighbors returned NULL"
#define ERR_DEQUEUE                     "a problem occurred during the call to spBPQueueDequeue"
#define BEST_CADIDATES 					"Best candidates for - %s -are:\n"
#define FILE_PATH_SIZE_PLUS				1060 //1024 for path plus room for more words in the string

#define EXTRACT_CONFIG_PARAM_LOG                "Extrating parameters from configuration file..."
#define INITIALIZING_LOGGER_INFO_MSG            "Initializing logger, reading logger parameters from configuration\n"
#define CONFIG_FILE_PATH_SIZE                   1024   // the maximum length  of path to any file
#define ERROR_READING_CONFIG_INVALID_ARG_MSG    "While reading configuration parameter - invalid argument\n"
#define LOGGER_ALREADY_DEFINED                  "logger file %s is already defined.\n"
#define ERROR_OPENING_LOGGER_FILE_MSG           "The logger file file %s couldn't be open\n"
#define ALLOCATION_FAILURE_MSG                  "An error occurred - allocation failure\n"
#define ALLOCATION_FAILURE_LOG                  "An error occurred - allocation failure"
#define ERROR_READING_CONFIG_INVALID_ARG_LOG    "While reading configuration parameter - invalid argument"

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

    for (i = 0; i < size; i++){
    	free(arr_with_indixes[i]);
    }
    free(arr_with_indixes);
    return first_minimums;
}

KDTreeNode initiateDataStructures(SPPoint** features_per_image, int* num_of_features_per_image, int num_of_images, int split_method) {
    int total_num_of_features = 0; // the total number of extractred features from all images
    SPPoint* all_features = NULL;  // holds all features of all images    
    KDTreeNode kd_tree = NULL;            // the tree to return
    int counter = 0;               // helper
    int initTree_results;          // holds the result from call to InitTree
    int i;      
    int j;

    for (i=0; i < num_of_images; i++) { 
        total_num_of_features = total_num_of_features + num_of_features_per_image[i];
    }
    
    if ((all_features = (SPPoint*)malloc(sizeof(*all_features) * total_num_of_features)) == NULL) {
        spLoggerPrintError(ALLOCATION_FAILURE_LOG, __FILE__, __func__, __LINE__);
        return NULL;
    }

    // create one SPPoint array for all features images
    for (i = 0; i < num_of_images; i ++)
    {
        for (j = 0; j < num_of_features_per_image[i]; j++)
        {
            if ((all_features[counter] = spPointCopy(features_per_image[i][j])) == NULL) {
                spLoggerPrintError(ALLOCATION_FAILURE_LOG, __FILE__, __func__, __LINE__);
                goto err;
            }
            //set the index of each point (feature) to be the number of the image it belongs to
            spPointSetIndex(all_features[counter], i);
            counter++;
        }
    }
    // initiate kd tree with all features of all images
    if ((initTree_results = InitTree(all_features, total_num_of_features, split_method, &kd_tree)) == -1){
        kd_tree = NULL;
        goto err; // error log is printed inside InitTree
    }

    err:
        // free all_features
        if (all_features != NULL) {
            for (i=0; i<counter; i++) {
                spPointDestroy(all_features[i]);
            }
            free(all_features);
        }

    return kd_tree; // on error kd_tree is NULL
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
	knnResult = kNearestNeighbors(root, bpq, &featureA);
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

int* getKClosestImages(int nearestKImages, int bestNFeatures, SPPoint* queryFeatures, KDTreeNode databaseFeatures, int queryNumOfFeatures, int numberOfImages) {
	int i;
    int j;
    int k;
    int* featureClosestImages; // array holds the spKNN indexes of the closest images to a feature of the  query
    double* hintsPerImage;  // array counts number of hints per image
    int* closestImages; // array holds the spNumOfSimilarImages indexes of the closest images to the query image

    // return NULL if one of the parameters is not initialized
    if (!queryFeatures || !databaseFeatures || (queryNumOfFeatures <=1) || (numberOfImages <=1)) {
        spLoggerPrintError(INVALID_ARGUMENT, __FILE__, __func__, __LINE__);
        return NULL;
    }

    if ((hintsPerImage = (double*)calloc(numberOfImages, sizeof(double))) == NULL) {
        spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
        return NULL;
    }

    // for each feature of query calculate the spBestSIFTL2SquaredDistance
    for (i=0; i< queryNumOfFeatures; i++){
        featureClosestImages = getSPKNNClosestFeatures(bestNFeatures, queryFeatures[i],
            databaseFeatures);

        if (featureClosestImages == NULL) {
            // free everything
            free(hintsPerImage);
            free(featureClosestImages);
            return NULL; // error log is printed inside getNClosestImagesForFeature
        }

        // Counting hits
        for (k = 0; k < numberOfImages; k++) { // k is image index
            for (j = 0; j < bestNFeatures; j++) {
                if (featureClosestImages[j] == k) {
                    hintsPerImage[k]--;
                }
            }
        }
        free(featureClosestImages);
    }

    // find the nearest images
    closestImages = nearestImages(hintsPerImage, numberOfImages, nearestKImages);
    if (closestImages == NULL) {
        // free everything
        free(hintsPerImage);
        free(featureClosestImages);
        return NULL; // error log is printed inside nearestImages

    }
    free(hintsPerImage);
    return closestImages;
}

/**
 * initiates the logger by the parameters specified in the cvonfiguration file
 *
 * @param config - the configuration structure
 *
 * @return -1 on failure (and prints the errors) and 0 on success
 */
int initiateLoggerByConfig(const SPConfig config) {
    SP_CONFIG_MSG msg;
    SP_LOGGER_MSG logmsg;
    SP_LOGGER_LEVEL logger_level;                   // the logger level in configuration file
    char logger_filename[CONFIG_FILE_PATH_SIZE];    // the logger filename in configuration file
    
    logger_level = spConfigGetLoggerLevel(config, &msg);
    if (msg != SP_CONFIG_SUCCESS) {
        printf(ERROR_READING_CONFIG_INVALID_ARG_MSG);
        return -1;
    }

    if (spConfigGetLoggerFileName(logger_filename, config) != SP_CONFIG_SUCCESS) {
        printf(ERROR_READING_CONFIG_INVALID_ARG_MSG);
        return -1;
    }

    if (strcmp(logger_filename, DEFAULT_LOGGER_FILENAME) == 0) {
        logmsg = spLoggerCreate(NULL, logger_level);
    }   
    else {
        logmsg = spLoggerCreate(logger_filename, logger_level);
    }
        

    switch(logmsg) {
       case SP_LOGGER_DEFINED:
            printf(LOGGER_ALREADY_DEFINED, logger_filename);
            break;
        
       case SP_LOGGER_OUT_OF_MEMORY:
            printf(ALLOCATION_FAILURE_MSG);
            return -1;
      
        case SP_LOGGER_CANNOT_OPEN_FILE:
            printf(ERROR_OPENING_LOGGER_FILE_MSG, logger_filename);
            return -1;
        
        default: 
            break;
    }

    return 0;
}


/**
 * extract information from configuration file and stores it into the given params as described below:
 * 
 * @param config - the configuration structure
 * @param num_of_similar_images - an address to store the number of images in
 * @param knn - an address to store the number of knn in
 * @param split_method - an address to store the split method in
 * @param extraction_mode - an address to store whether extraction mode is asked 
 * @param min_gui - an address to store whether minimal gui mode is asked 
 * 
 * Note -assumes that logger is initiated
 *  
 * @return -1 on failure (and prints the errors) and 0 on success
 */
int getConfigParameters(const SPConfig config, int* num_of_similar_images, int* knn, int* split_method, bool* extraction_mode, bool* min_gui) {
    SP_CONFIG_MSG msg;

    *extraction_mode = spConfigIsExtractionMode(config, &msg);
    if (msg != SP_CONFIG_SUCCESS) {
        spLoggerPrintError(ERROR_READING_CONFIG_INVALID_ARG_LOG, __FILE__, __func__, __LINE__);
        return -1;
    }

    *num_of_similar_images = spConfigGetNumOfSimilarImages(config, &msg);
    if (msg != SP_CONFIG_SUCCESS) { 
        spLoggerPrintError(ERROR_READING_CONFIG_INVALID_ARG_LOG, __FILE__, __func__, __LINE__);
        return -1;
    }

    *knn = spConfigGetKNN(config, &msg);
    if (msg != SP_CONFIG_SUCCESS) {
        spLoggerPrintError(ERROR_READING_CONFIG_INVALID_ARG_LOG, __FILE__, __func__, __LINE__);
        return -1;
    }


    *split_method = spConfigGetKDTreeSplitMethod(config, &msg);
    if (msg != SP_CONFIG_SUCCESS) {
        spLoggerPrintError(ERROR_READING_CONFIG_INVALID_ARG_LOG, __FILE__, __func__, __LINE__);
        return -1;
    }

    *min_gui = spConfigMinimalGui(config, &msg);
    if (msg != SP_CONFIG_SUCCESS) {
        spLoggerPrintError(ERROR_READING_CONFIG_INVALID_ARG_LOG, __FILE__, __func__, __LINE__);
        return -1;
    }

    return 0;
}



/*
 * returns list of the full path of all images paths specified in configuration file
 * and stores in num_of_images the number of images specified in the configuration file
 *
 * For example:
 * Given that the value of:
 *  spImagesDirectory = "./images/"
 *  spImagesPrefix = "img"
 *  spImagesSuffix = ".png"
 *  spNumOfImages = 17
 *  index = 10
 *
 * The functions returns ["./images/img1.png",...,"./images/img17.png"]
 * and stores in num_of_images 17
 *
 * @param config - the configuration structure
 * @param num_of_images - an address to store the number of images in
 *
 * Note - assumes spLogger is initiated
 *
 * @return a list of all images path on success
 *         NULL on error
 */
char** getAllImagesPaths(const SPConfig config, int* num_of_images) {
    char** images_paths;
    SP_CONFIG_MSG msg;
    int i;
    int j;
    int error = 0;

    *num_of_images = spConfigGetNumOfImages(config, &msg);
    if (msg != SP_CONFIG_SUCCESS) {
        spLoggerPrintError(ERROR_READING_CONFIG_INVALID_ARG_LOG, __FILE__, __func__, __LINE__);
        return NULL;
    }

    if ((images_paths = (char**)malloc(sizeof(*images_paths) * (*num_of_images))) == NULL) {
        spLoggerPrintError(ALLOCATION_FAILURE_LOG, __FILE__, __func__, __LINE__);
        return NULL;
    }

    for(i=0; i< (*num_of_images); i++) {
        if ((images_paths[i] = (char*)malloc(sizeof(*(images_paths[i])) * CONFIG_FILE_PATH_SIZE)) == NULL) {
            spLoggerPrintError(ALLOCATION_FAILURE_LOG, __FILE__, __func__, __LINE__);
            for (j=0; j<i; j++) {
                free(images_paths[j]);
            }
            free(images_paths);
            return NULL;
        }
    }
    
    for(i=0; (!error) & (i< (*num_of_images)); i++) {
        msg = spConfigGetImagePath(images_paths[i], config, i);
        error = (msg != SP_CONFIG_SUCCESS);
    }

    if (error) {
        spLoggerPrintError(ERROR_READING_CONFIG_INVALID_ARG_LOG, __FILE__, __func__, __LINE__);
        for (j=0; j<(*num_of_images); j++) {
            free(images_paths[j]);
        }
        free(images_paths);
        return NULL;
    }

    return images_paths;
}

int initFromConfig(const SPConfig config, int* num_of_images, int* num_of_similar_images, int* knn, int* split_method, bool* extraction_mode, bool* min_gui, char *** all_images_paths) {
    // initiate logger
    printf(INITIALIZING_LOGGER_INFO_MSG);
    if (initiateLoggerByConfig(config) == -1) {
        return -1; // error is printed inside  initiateLoggerByConfig
    }

    // extract parameters from configuration file 
    spLoggerPrintInfo(EXTRACT_CONFIG_PARAM_LOG);
    if (getConfigParameters(config, num_of_similar_images, knn, split_method, extraction_mode, min_gui) == -1) {
        return -1;// error is printed inside getConfigParameters
    }

    // get all images paths
    if ((*all_images_paths =  getAllImagesPaths(config, num_of_images)) == NULL) {
        return -1; // error is printed inside getAllImagesPaths
    }
    return 0;
}


int saveToDirectory(const SPConfig config, SPPoint** features_per_image, int* num_of_features_per_image, int num_of_images) {
    int i;
    // write image features into file
    for (i=0; i < num_of_images; i++) { 
        if (writeImageFeaturesIntoFile(config, i, features_per_image[i], num_of_features_per_image[i]) == -1) {
            return -1; // errors are printed inside  writeImageFeaturesIntoFile
        }
    }
    return 0;
}


SPPoint** extractFromFiles(const SPConfig config, int* num_of_features_per_image, int num_of_images){
    SPPoint** features_per_image;
    int i;
    int j;

    if ((features_per_image = (SPPoint**)malloc(sizeof(*features_per_image) * num_of_images)) == NULL) {
        spLoggerPrintError(ALLOCATION_FAILURE_LOG, __FILE__, __func__, __LINE__);
        return NULL;
    }

    for (i = 0; i < num_of_images; i++) {
        if ((features_per_image[i] = readImageFreaturesFromFile(config, i, &(num_of_features_per_image[i]))) == NULL) {
            for (j=0; j<i; j++) {  
                free(features_per_image[j]);
            }
            free(features_per_image);
            return NULL; // error is printed inside readImageFreaturesFromFile
        }
    }
    return features_per_image;
}

int PrintMinGuiFalse(char* query_image,int num_of_similar_images_to_find, char** all_images_paths,
					int* closest_images){
	int i;
	int n;
	char best_candidate_msg[FILE_PATH_SIZE_PLUS];   // holds the string "Best candidates for - <query image path> - are:\n"

	// initialize best_candidate_msg
	if ((n = sprintf(best_candidate_msg,BEST_CADIDATES,query_image)) < 0) {
		spLoggerPrintError(GENERAL_ERROR_MSG, __FILE__, __func__, __LINE__);
		return 0;
	}
	//print best_candidate_msg
	printf("%s", best_candidate_msg);
	fflush(NULL);
	//print the candidates paths, first path is the closest image
	for (i=0; i<num_of_similar_images_to_find; i++){
		//get file path of the images by the indexes in closest_images
		printf("%s\n", all_images_paths[closest_images[i]]);
		fflush(NULL);
	}
	return 1;
}

