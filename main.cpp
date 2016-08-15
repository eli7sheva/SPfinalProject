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
	int counter;									// helper
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
	SPPoint** features_per_image = NULL;   					 // helper - holds the features for each images
	SPPoint* all_features = NULL;   				 // holds all features of all images
	int* num_of_features; 							 // holds number of features extracted for each image
	int total_num_of_features = 0;						 // the total number of extractred features from all images
	KDTreeNode kd_tree;							 // array holds a KDTree for each image
	int* closest_images; 						 // array holds the spNumOfSimilarImages indexes of the closest images to the query image
	sp::ImageProc *improc;
	SP_CONFIG_MSG msg;
	int retval = 0;									//return value - default 0 on success
	int minGui;                                    // 1 if spMinimalGUI==true, 0 if spMinimalGUI==false
	char* Prefix;                                  // the Prefix of the images path
	char* Suffix; 									// the Suffix of the images path

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
	        printf(LOGGER_ALREADY_DEFINED, logger_filename); // todo should error or just continue(break)?
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

	if ((num_of_features = (int*)malloc(sizeof(*num_of_features) * num_of_images)) == NULL) {
		//todo add log
		retval = -1;
		goto err;
	}

	if ((features_per_image = (SPPoint**)malloc(sizeof(*features_per_image) * num_of_images)) == NULL) {
		//todo add log
		retval = -1;
		goto err;
	}


	if (extraction_mode) {
		// extract each image features and write them to file
		for (i=0; i < num_of_images; i++) {	
			// find image path
			msg = spConfigGetImagePath(current_image_path, config, i);
			if (msg != SP_CONFIG_SUCCESS) { // should not happen
				retval = -1;
				goto err; //todo print error
			}

			// extract image features
			features_per_image[i] = improc->getImageFeatures(current_image_path, i, &(num_of_features[i]));

			if (features_per_image[i] == NULL) {
				retval = -1;
				goto err; //todo print error
			}

			// write image features into file
			if (writeImageFeaturesIntoFile(config, i, features_per_image[i], num_of_features[i]) == -1) {
				retval = -1;
				goto err; //todo print error
			}

			total_num_of_features = total_num_of_features + num_of_features[i];
		}
		
	}

	else { // not extraction mode
		for (i = 0; i < num_of_images; i++)
		{
			if ((features_per_image[i] = readImageFreaturesFromFile(config, i, &(num_of_features[i]))) == NULL) {
				retval = -1;
				goto err; //todo print error
			}

			total_num_of_features = total_num_of_features + num_of_features[i];
		}
	}

	if ((all_features = (SPPoint*)malloc(sizeof(*all_features) * total_num_of_features)) == NULL) {
		//todo add log
		retval = -1;
		goto err;
	}


	// create one SPPoint array for all features images
	counter = 0;
	for (i = 0; i < num_of_images; i ++)
	{
		for (j = 0; j < num_of_features[i]; j++)
		{
			if ((all_features[counter] = spPointCopy(features_per_image[i][j])) == NULL) {
				retval = -1;
				goto err;//todo no need to print error log since it is printed inside InitTree
			}
			//set the index of each point (feature) to be the number of the image it belongs to
			spPointSetIndex(all_features[counter], i);
			counter++;
		}
	}

	if ((kd_tree = InitTree(all_features, total_num_of_features)) == NULL){
		retval = -1;
		goto err; //todo no need to print error log since it is printed inside InitTree
	}


	// get a query image from the user
	printf(ENTER_AN_IMAGE_MSG);
	fflush(NULL);
	scanf("%s",query_image);

	// get query image's features
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
	closest_images = getKClosestImages(num_of_similar_images_to_find, knn, query_features,
									   kd_tree, num_of_images, num_of_features);

	if (closest_images == NULL) { // error todo return this comment
		// todo print error log
		retval = -1;
		goto err;
	}

	// // todo elisheva - show (display) closest_images images

	//initialize minGui
	minGui = spConfigGetpMinimalGUI(config, &msg);
	if (msg != SP_CONFIG_SUCCESS) {
		// // todo print log
		retval = -1;
		goto err;
	}

	//initialize Prefix
	Prefix = spConfigGetspImagesPrefix(config, &msg);
	if (msg != SP_CONFIG_SUCCESS) {
		// // todo print log
		retval = -1;
		goto err;
	}

	//initialize Suffix
	Suffix = spConfigGetspImagesSuffix(config, &msg);
	if (msg != SP_CONFIG_SUCCESS) {
		// // todo print log
		retval = -1;
		goto err;
	}

	//need to show images
	if (minGui==1){

	}

	// i.e. minGui==0,  just need to print images path
	else{

	}


	// done - destroy logger and free everything 
	err:
		spLoggerDestroy();

		// free the kd tree
		DestroyKDTreeNode(kd_tree);

		spConfigDestroy(config);
		free(closest_images);

		// free query_features
		if (query_features != NULL) {
			for (i=0; i<query_num_of_features; i++) {
				spPointDestroy(query_features[i]);
			}
			free(query_features);
		}

		// free all_features
		if (all_features != NULL) {
			for (i=0; i<total_num_of_features; i++) {
				spPointDestroy(all_features[i]);
			}
			free(all_features);
		}

		if (features_per_image != NULL) {
			// free features_per_image
			for (i = 0; i < num_of_images; i ++) {
				if (features_per_image[i] != NULL) {
					for (j = 0; j < num_of_features[i]; j++) {
						spPointDestroy(features_per_image[i][j]);
					}
				}
			}
			free(features_per_image);
		}
		free(num_of_features); // must be freed after features_per_image

	return retval;
}
