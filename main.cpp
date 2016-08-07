#include <cstring>
#include <cstdlib>
#include <cstdio>
extern "C"{
#include "SPConfig.h"
#include "SPPoint.h"
#include "SPLogger.h"
#include "SPExtractFeatures.h"
#include "main_aux.h"
#include "KDTreeNode.h"
}
#include "SPImageProc.h"


#define DEFAULT_CONFIG_FILENAME "spcbir.config"
#define INVALID_CMD_LINE_MSG "Invalid command line : use -c <config_filename>\n"
#define ERROR_OPENING_CONFIG_FILE_MSG "The configuration file %s couldn’t be open\n"
#define ERROR_OPENING_DEFAULT_CONFIG_FILE_MSG "The default configuration file %s couldn’t be open\n"
#define ERROR_OPENING_LOGGER_FILE_MSG "The logger file file %s couldn’t be open\n"
#define ALLOCATION_FAILURE_MSG "An error occurred - allocation failure\n"
#define LOGGER_ALREADY_DEFINED "logger file %s is already defined.\n"
#define CMD_LINE_CONFIG_FILENAME_FLAG "-c"
#define ERROR_MEMORY_ALLOC_MSG "Error allocating memory\n"
#define CONFIG_FILE_PATH_SIZE      1024	 // the path of any file contains no more than 1024 characters
#define ENTER_AN_IMAGE_MSG "Please enter an image path:\n"




int main(int argc, char *argv[]) {
	int i;
	int j;
	int k;
	char config_filename[CONFIG_FILE_PATH_SIZE];     // the configuration file name
	char query_image[CONFIG_FILE_PATH_SIZE];         // the query image 
	char logger_filename[CONFIG_FILE_PATH_SIZE];	 // the logger filename in configuration file
	SP_LOGGER_LEVEL logger_level;                    // the logger level in configuration file
	int knn;										 // the number of similar features in each image to find (spKNN from configuration file)
	int num_of_similar_images_to_find;               // the number of similar images (to the query) to find (from configuration file)
	int query_num_of_features;					 // number of features in query image
	SPPoint* query_features; 					 // all query features
	SPConfig config;								 // hold configuration parameters
	bool extraction_mode;							 //indicates if extraction mode on or off
	char current_image_path[CONFIG_FILE_PATH_SIZE];  // the path to the current image
	int num_of_images;   						     // number of images in the directory given by the user in the configuration file
	SPPoint* features = NULL;   					 // helper - holds the features of an images
	int* num_of_features; 							 // holds number of features extracted for each image
	int last_num_of_features;						 // helper that holds the number of features of the last succesfully extracted image
	KDTreeNode* kd_trees;							 // array holds a KDTree for each image
	int* closest_images; 						 // array holds the spNumOfSimilarImages indexes of the closest images to the query image
	sp::ImageProc *improc;
	SP_CONFIG_MSG msg;
	int error = 0;
	int retval = 0;									//return value - default 0 on success

	// validate command line arguments:
	// cmd line arguments are ok if there was no arguments specified (argc == 1) or two arguments specified ( -c and filname)
	if (argc != 3 && argc != 1) {
		printf(INVALID_CMD_LINE_MSG);
		return -1;
	}

	if (argc == 1) {
		strcpy(config_filename, DEFAULT_CONFIG_FILENAME);
		config = spConfigCreate(config_filename, &msg);
		if (msg == SP_CONFIG_CANNOT_OPEN_FILE) {
			printf(ERROR_OPENING_DEFAULT_CONFIG_FILE_MSG, DEFAULT_CONFIG_FILENAME);
		}

		if (msg != SP_CONFIG_SUCCESS) {
			//todo add log
			retval = -1;
			goto err;
		}
	}
	else { // argc == 3

		// check that second argument is the -c flag
		if (strcmp(argv[1], CMD_LINE_CONFIG_FILENAME_FLAG) != 0) {
			printf(INVALID_CMD_LINE_MSG);
			//todo add log
			retval = -1;
			goto err;
		}
	
		strcpy(config_filename, argv[2]);
		config = spConfigCreate(config_filename, &msg);
		
		if (msg == SP_CONFIG_CANNOT_OPEN_FILE) {
			printf(ERROR_OPENING_CONFIG_FILE_MSG, config_filename);
		}

		if (msg != SP_CONFIG_SUCCESS) {
			//todo add log
			retval = -1;
			goto err;
		}
	}
	
	// initiate logger
	logger_level = spConfigGetLoggerLevel(config, &msg);
	if ((msg != SP_CONFIG_SUCCESS) || (spConfigGetLoggerFileName(logger_filename, config) != SP_CONFIG_SUCCESS)) {
		// spLoggerPrintError(NUM_OF_IMAGES_ERROR, __FILE__, __func__, __LINE__);
		//todo add log
		retval = -1;
		goto err;
	}

	switch(spLoggerCreate(logger_filename, logger_level)) {
	   case SP_LOGGER_DEFINED:
	        printf(LOGGER_ALREADY_DEFINED, logger_filename);
	        break;
		
	   case SP_LOGGER_OUT_OF_MEMORY:
		    //todo change to  log
		    printf(ALLOCATION_FAILURE_MSG);
			retval = -1;
			goto err;
	  
	  	case SP_LOGGER_CANNOT_OPEN_FILE:
			//todo change to log
	  		printf(ERROR_OPENING_LOGGER_FILE_MSG, logger_filename);
			retval = -1;
			goto err;
		
		default: 
			break;
	}

	// get number of images from configuration
	num_of_images = spConfigGetNumOfImages(config, &msg);
	if (msg != SP_CONFIG_SUCCESS) {
		// spLoggerPrintError(NUM_OF_IMAGES_ERROR, __FILE__, __func__, __LINE__);
		//todo add log
		retval = -1;
		goto err;
	}

	extraction_mode = spConfigIsExtractionMode(config, &msg);
	if (msg != SP_CONFIG_SUCCESS) {
		//todo add log
		retval = -1;
		goto err;
	}

	improc = new sp::ImageProc(config);

	// allocate memory for the array of kd trees
	if ((kd_trees = (KDTreeNode*)malloc(sizeof(*kd_trees) * num_of_images)) == NULL) {
		//todo add log
		retval = -1;
		goto err;
	}

	if ((num_of_features = (int*)malloc(sizeof(int) * num_of_images)) == NULL) {
		//todo add log
		retval = -1;
		goto err;
	}

	last_num_of_features = -1;
	if (extraction_mode) {
		// extract each image features and write them to file
		for (i=0; (!error) & (i < num_of_images); i++) {	
			// find image path
			msg = spConfigGetImagePath(current_image_path, config, i);
			if (msg != SP_CONFIG_SUCCESS) { // should not happen
				error = 1;
				break;//todo print error
			}

			// extract image features
			features = improc->getImageFeatures(current_image_path, i, &(num_of_features[i]));
			last_num_of_features = num_of_features[i];

			if (features == NULL) {
				error = 1;
				break;//todo print error
			}

			// write image features into file
			if (writeImageFeaturesIntoFile(config, i, features, num_of_features[i]) == -1) {
				error = 1;
				break;//todo print error
			}

			if ((kd_trees[i] = InitTree(features, num_of_features[i])) == NULL){
				error = 1;
				break;//no need to print error log since it is printed inside InitTree
			}

			// free the current image features
			for (k=0; k<last_num_of_features; k++) {
				spPointDestroy(features[k]);
			}
		}
		
	}

	else { // not extraction mode
		for (i = 0; (!error) & (i < num_of_images); i++)
		{
			if ((features = readImageFreaturesFromFile(config, i, &(num_of_features[i]))) == NULL) {
				error = 1;
				break; //todo print error
			}
			last_num_of_features = num_of_features[i];
			
			if ((kd_trees[i] = InitTree(features, num_of_features[i])) == NULL){
				error = 1;
				break; //todo no need to print error log since it is printed inside InitTree
			}

			// free the current image features
			for (k=0; k<last_num_of_features; k++) {
				spPointDestroy(features[k]);
			}
		}
	}

	// if an error occured in extraction mode or non-extraction mode - free everything needed
	if (error)	{	
		// free features of the current extracted image
		for (k=0; k<last_num_of_features; k++) {
			spPointDestroy(features[k]);
		}
		//print error  todo
		retval = -1;
		goto err;
	}

	// get a query image from the user
	printf(ENTER_AN_IMAGE_MSG);
	fflush(NULL);
	scanf("%s",query_image);

	// get query image's deatures
	num_of_similar_images_to_find = spConfigGetNumOfSimilarImages(config, &msg);
	if (msg != SP_CONFIG_SUCCESS) { // todo handle error differently
		// // todo print log
		retval = -1;
		goto err;
	}

	knn = spConfigGetKNN(config, &msg);
	if (msg != SP_CONFIG_SUCCESS) {
		// // todo print log
		retval = -1;
		goto err;
	}



	query_features = improc->getImageFeatures(query_image, num_of_images, &query_num_of_features); // todo which index to give?
	
	// find closest images to the query image
	closest_images = getKClosestImages(num_of_similar_images_to_find, knn, query_features[i],
									   kd_trees, num_of_images, num_of_features); todo return this!!

	if (closest_images == NULL) { // error todo return this comment
		// todo print error log
		retval = -1;
		goto err;
	}

	// // todo elisheva - show (display) closest_images images



	// done - destroy logger and free everything 
	err:
		spLoggerDestroy();

		// free the kd tree
		if (kd_trees != NULL) {
			for (j=0; j<num_of_images; j++) {
				DestroyKDTreeNode(kd_trees[j]);
			}
			free(kd_trees);
		}

		// free query features
		if (query_features != NULL) {
			for (k=0; k<query_num_of_features; k++) {
				spPointDestroy(query_features[k]);
			}
			free(query_features);
		}

		free(closest_images);
		free(features);
		free(num_of_features);
		spConfigDestroy(config);

	return retval;

}
