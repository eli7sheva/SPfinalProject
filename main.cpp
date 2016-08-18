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


#define DEFAULT_CONFIG_FILENAME 	  			"spcbir.config"  // default configuration file name
#define CMD_LINE_CONFIG_FILENAME_FLAG 			"-c"   // the commang ine flag
#define CONFIG_FILE_PATH_SIZE      	  			1024   // the maximum length  of path to any file

// messaged to print to stdout
#define INVALID_CMD_LINE_MSG 					"Invalid command line : use -c <config_filename>\n"
#define ERROR_OPENING_CONFIG_FILE_MSG 			"The configuration file %s couldn't be open\n"
#define ERROR_OPENING_DEFAULT_CONFIG_FILE_MSG 	"The default configuration file %s couldn't be open\n"
#define ERROR_OPENING_LOGGER_FILE_MSG			"The logger file file %s couldn't be open\n"
#define ALLOCATION_FAILURE_MSG 					"An error occurred - allocation failure\n"
#define LOGGER_ALREADY_DEFINED 					"logger file %s is already defined.\n"
#define ERROR_MEMORY_ALLOC_MSG 					"Error allocating memory\n"
#define ENTER_AN_IMAGE_MSG 						"Please enter an image path:\n"
#define ERROR_READING_CONFIG_INVALID_ARG_MSG 	"While reading configuration parameter - invalid argument\n"
#define DONE_MSG 								"Done.\n"


// logger messages (no new line at the end since it is added automatically by the logger)
#define NUM_OF_SIMILAR_IMAGES_DEBUG_LOG 		"Number of similar images to find is %d, knn is %d"
#define GENERAL_ERROR_MSG 						"An error occured - internal error"
#define NUM_OF_EXTRACTED_FEATURES_DEBUG_LOG 	"Extracted %d features from query image"
#define SEARCING_SIMILAR_IMAGES_MSG 			"Searching for %d closest images..."
#define ERROR_READING_CONFIG_INVALID_ARG_LOG 	"While reading configuration parameter - invalid argument"
#define INITIALIZING_LOGGER_INFO_LOG 			"Initializing logger, reading logger parameters from configuration"
#define CHECK_EXTRACTION_MODE_INFO_LOG 			"Checking if extraction mode is set..."
#define USE_EXTRACTION_MODE_LOG 				"Extraction mode is set"
#define USE_NOT_EXTRACTION_MODE_LOG 			"Extraction mode is not set"
#define EXTRACT_IMAGES_FEATURES_INFO_LOG 		"Extracting the features of each image and storing them into files..."
#define READ_FEATURES_FROM_FILE_LOG 			"Reading extracted images' features from features files.."
#define STORE_FEATURES_INTO_KD_TREE_LOG 		"Storing all extracted features (%d) into kd tree..."
#define EXTRACT_QUERY_IMAGE_FEATURES_LOG 		"Extracting query image features..."

int main(int argc, char *argv[]) {
	int i;
	int j;
	int n;
	int counter;									// helper
	char config_filename[CONFIG_FILE_PATH_SIZE];    // the configuration file name
	char query_image[CONFIG_FILE_PATH_SIZE];        // the query image 
	char logger_filename[CONFIG_FILE_PATH_SIZE];	// the logger filename in configuration file
	SP_LOGGER_LEVEL logger_level;                   // the logger level in configuration file
	int knn;										// the number of similar features in each image to find (spKNN from configuration file)
	int num_of_similar_images_to_find;              // the number of similar images (to the query) to find (from configuration file)
	int query_num_of_features;					    // number of features in query image
	SPPoint* query_features; 					    // all query features
	SPConfig config;								// hold configuration parameters
	bool extraction_mode;							// indicates if extraction mode on or off
	char current_image_path[CONFIG_FILE_PATH_SIZE]; // the path to the current image
	int num_of_images;   						    // number of images in the directory given by the user in the configuration file
	SPPoint** features_per_image = NULL;   			// helper - holds the features for each images
	SPPoint* all_features = NULL;   				// holds all features of all images
	int* num_of_features; 							// holds number of features extracted for each image
	int total_num_of_features = 0;					// the total number of extractred features from all images
	KDTreeNode kd_tree;							    // array holds a KDTree for each image
	int* closest_images; 						    // array holds the spNumOfSimilarImages indexes of the closest images to the query image
	int retval = 0;									// return value - default 0 on success
	bool minGui;                                     // value of the system variable MinimalGui
	char best_candidate_msg[CONFIG_FILE_PATH_SIZE+35];   //CR: todo elisheva - what is 35? if it is needed it should be in define...                    // holds the string "Best candidates for - <query image path> - are:\n"
	char string_holder[CONFIG_FILE_PATH_SIZE];       // helper to hold strings
	int split_method;                                // holds an int representing the split method: 0=RANDOM, 1= MAX_SPREAD,  2=INCREMENTAL
	sp::ImageProc *improc;
	SP_CONFIG_MSG msg;

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
			retval = -1;
			goto err; // error is printed inside spConfigCreate
		}
	}
	else { // argc == 3

		// check that second argument is the -c flag
		if (strcmp(argv[1], CMD_LINE_CONFIG_FILENAME_FLAG) != 0) {
			printf(INVALID_CMD_LINE_MSG);
			retval = -1;
			goto err;
		}
	
		strcpy(config_filename, argv[2]);
		config = spConfigCreate(config_filename, &msg);
		
		if (msg == SP_CONFIG_CANNOT_OPEN_FILE) {
			printf(ERROR_OPENING_CONFIG_FILE_MSG, config_filename);
		}

		if (msg != SP_CONFIG_SUCCESS) {
			retval = -1;
			goto err; // error is printed inside spConfigCreate
		}
	}
	
	// initiate logger
	spLoggerPrintInfo(INITIALIZING_LOGGER_INFO_LOG);
	logger_level = spConfigGetLoggerLevel(config, &msg);
	if ((msg != SP_CONFIG_SUCCESS) || (spConfigGetLoggerFileName(logger_filename, config) != SP_CONFIG_SUCCESS)) {
		printf(ERROR_READING_CONFIG_INVALID_ARG_MSG);
		retval = -1;
		goto err;
	}

	switch(spLoggerCreate(logger_filename, logger_level)) {
	   case SP_LOGGER_DEFINED:
	        printf(LOGGER_ALREADY_DEFINED, logger_filename); // todo reut should error or just continue(break)?
	        break;
		
	   case SP_LOGGER_OUT_OF_MEMORY:
		    printf(ALLOCATION_FAILURE_MSG);
			retval = -1;
			goto err;
	  
	  	case SP_LOGGER_CANNOT_OPEN_FILE:
	  		printf(ERROR_OPENING_LOGGER_FILE_MSG, logger_filename);
			retval = -1;
			goto err;
		
		default: 
			break;
	}

	// get number of images from configuration
	num_of_images = spConfigGetNumOfImages(config, &msg);
	if (msg != SP_CONFIG_SUCCESS) {
		spLoggerPrintError(ERROR_READING_CONFIG_INVALID_ARG_LOG, __FILE__, __func__, __LINE__);
		retval = -1;
		goto err;
	}

	spLoggerPrintInfo(CHECK_EXTRACTION_MODE_INFO_LOG);
	extraction_mode = spConfigIsExtractionMode(config, &msg);
	if (msg != SP_CONFIG_SUCCESS) {
		spLoggerPrintError(ERROR_READING_CONFIG_INVALID_ARG_LOG, __FILE__, __func__, __LINE__);
		retval = -1;
		goto err;
	}

	improc = new sp::ImageProc(config);

	if ((num_of_features = (int*)malloc(sizeof(*num_of_features) * num_of_images)) == NULL) {
		spLoggerPrintError(ALLOCATION_FAILURE_MSG, __FILE__, __func__, __LINE__);
		retval = -1;
		goto err;
	}

	if ((features_per_image = (SPPoint**)malloc(sizeof(*features_per_image) * num_of_images)) == NULL) {
		spLoggerPrintError(ALLOCATION_FAILURE_MSG, __FILE__, __func__, __LINE__);
		retval = -1;
		goto err;
	}


	if (extraction_mode) {
		spLoggerPrintMsg(USE_EXTRACTION_MODE_LOG);
		spLoggerPrintInfo(EXTRACT_IMAGES_FEATURES_INFO_LOG);
		// extract each image features and write them to file
		for (i=0; i < num_of_images; i++) {	
			// find image path
			msg = spConfigGetImagePath(current_image_path, config, i);
			if (msg != SP_CONFIG_SUCCESS) { // should not happen
				spLoggerPrintError(ERROR_READING_CONFIG_INVALID_ARG_LOG, __FILE__, __func__, __LINE__);
				retval = -1;
				goto err;
			}

			// extract image features
			features_per_image[i] = improc->getImageFeatures(current_image_path, i, &(num_of_features[i]));

			if (features_per_image[i] == NULL) {
				retval = -1;
				goto err; // error is printed inside  getImageFeatures
			}

			// write image features into file
			if (writeImageFeaturesIntoFile(config, i, features_per_image[i], num_of_features[i]) == -1) {
				retval = -1;
				goto err; // error is printed inside writeImageFeaturesIntoFile
			}

			total_num_of_features = total_num_of_features + num_of_features[i];
		}
		
	}

	else { // not extraction mode
		spLoggerPrintMsg(USE_NOT_EXTRACTION_MODE_LOG);
		spLoggerPrintInfo(READ_FEATURES_FROM_FILE_LOG);
		for (i = 0; i < num_of_images; i++)
		{
			if ((features_per_image[i] = readImageFreaturesFromFile(config, i, &(num_of_features[i]))) == NULL) {
				retval = -1;
				goto err; // error is printed inside readImageFreaturesFromFile
			}

			total_num_of_features = total_num_of_features + num_of_features[i];
		}
	}
	

	//  print debug log
	if ((n = sprintf(string_holder, STORE_FEATURES_INTO_KD_TREE_LOG, total_num_of_features)) < 0) {
		spLoggerPrintError(GENERAL_ERROR_MSG, __FILE__, __func__, __LINE__);
		retval = -1;
		goto err;
	}
	spLoggerPrintDebug(string_holder, __FILE__, __func__, __LINE__);

	// hold all features
	if ((all_features = (SPPoint*)malloc(sizeof(*all_features) * total_num_of_features)) == NULL) {
		spLoggerPrintError(ALLOCATION_FAILURE_MSG, __FILE__, __func__, __LINE__);
		retval = -1;
		goto err;
	}

	//get split method value
	split_method = spConfigGetKDTreeSplitMethod(config, &msg);

	// create one SPPoint array for all features images
	counter = 0;
	for (i = 0; i < num_of_images; i ++)
	{
		for (j = 0; j < num_of_features[i]; j++)
		{
			if ((all_features[counter] = spPointCopy(features_per_image[i][j])) == NULL) {
				spLoggerPrintError(ALLOCATION_FAILURE_MSG, __FILE__, __func__, __LINE__);
				retval = -1;
				goto err; // error log is printed inside InitTree
			}
			//set the index of each point (feature) to be the number of the image it belongs to
			spPointSetIndex(all_features[counter], i);
			counter++;
		}
	}

	// initiate kd tree with all features of all images
	if ((kd_tree = InitTree(all_features, total_num_of_features,split_method)) == NULL){
		retval = -1;
		goto err; // error log is printed inside InitTree
	}


	// get a query image from the user
	printf(ENTER_AN_IMAGE_MSG);
	fflush(NULL);
	scanf("%s",query_image);

	// get query image's features
	num_of_similar_images_to_find = spConfigGetNumOfSimilarImages(config, &msg);
	if (msg != SP_CONFIG_SUCCESS) { 
		spLoggerPrintError(ERROR_READING_CONFIG_INVALID_ARG_LOG, __FILE__, __func__, __LINE__);
		retval = -1;
		goto err;
	}

	knn = spConfigGetKNN(config, &msg);
	if (msg != SP_CONFIG_SUCCESS) {
		spLoggerPrintError(ERROR_READING_CONFIG_INVALID_ARG_LOG, __FILE__, __func__, __LINE__);
		retval = -1;
		goto err;
	}

	// print debug log
	if ((n = sprintf(string_holder, NUM_OF_SIMILAR_IMAGES_DEBUG_LOG, num_of_similar_images_to_find, knn)) < 0) {
		spLoggerPrintError(GENERAL_ERROR_MSG, __FILE__, __func__, __LINE__);
		retval = -1;
		goto err;
	}
	spLoggerPrintDebug(string_holder, __FILE__, __func__, __LINE__); 


	// extract query image features
	spLoggerPrintMsg(EXTRACT_QUERY_IMAGE_FEATURES_LOG);
	if ((query_features = improc->getImageFeatures(query_image, num_of_images, &query_num_of_features)) == NULL) {
		retval = -1;
		goto err; // error log is printed inside getImageFeatures	
	}
	
	// print debug log
	if ((n = sprintf(string_holder, NUM_OF_EXTRACTED_FEATURES_DEBUG_LOG, query_num_of_features)) < 0) {
		spLoggerPrintError(GENERAL_ERROR_MSG, __FILE__, __func__, __LINE__);
		retval = -1;
		goto err;
	}
	spLoggerPrintDebug(string_holder, __FILE__, __func__, __LINE__);
	
	//  print log message
	if ((n = sprintf(string_holder, SEARCING_SIMILAR_IMAGES_MSG, num_of_similar_images_to_find)) < 0) {
		spLoggerPrintError(GENERAL_ERROR_MSG, __FILE__, __func__, __LINE__);
		retval = -1;
		goto err;
	}
	spLoggerPrintMsg(string_holder);

	// find closest images to the query image
	closest_images = getKClosestImages(num_of_similar_images_to_find, knn, query_features,
									   kd_tree, query_num_of_features, num_of_images, num_of_features);


	if (closest_images == NULL) { 
		// todo elisheva print error log or dont print if its printed inside getKClosestImages
		retval = -1;
		goto err;
	}

	// show (display) closest_images images
	//initialize minGui
	minGui = spConfigMinimalGui(config, &msg);
	if (msg != SP_CONFIG_SUCCESS) {
		// // todo elisheva print log
		retval = -1;
		goto err;
	}

	//need to show images
	if (minGui==true){ 
		for (i=0; i<num_of_similar_images_to_find; i++){
			//get file path of the images by the indexes in closest_images
			msg = spConfigGetImagePath(current_image_path, config, closest_images[i]);
			if (msg != SP_CONFIG_SUCCESS) { // should not happen
				spLoggerPrintError(ERROR_READING_CONFIG_INVALID_ARG_LOG, __FILE__, __func__, __LINE__);
				retval = -1;
				goto err;
			}
			improc->showImage(current_image_path);
		}
	}

	// i.e. minGui==false,  just need to print images path
	else{
		// initialize best_candidate_msg
		//todo: find better way to print next line
		//CR: todo elisheva "Best candidates for.." should be in define.. (do the same as i did in line 314)
		if ((n = sprintf(best_candidate_msg,"Best candidates for - %s -are:\n",query_image)) < 0) {
			// // todo print log 
			// CR: todo elisheva you can print GENERAL_ERROR_MSG
			retval = -1;
			goto err;
		}
		//print best_candidate_msg
		printf("%s", best_candidate_msg);
		fflush(NULL);
		//print the candidates paths, first path is the closest image
		for (i=0; i<num_of_similar_images_to_find; i++){
			//get file path of the images by the indexes in closest_images
			msg = spConfigGetImagePath(current_image_path, config, closest_images[i]);
			if (msg != SP_CONFIG_SUCCESS) { // should not happen
				spLoggerPrintError(ERROR_READING_CONFIG_INVALID_ARG_LOG, __FILE__, __func__, __LINE__);
				retval = -1;
				goto err;
			}
			printf("%s", current_image_path);
			fflush(NULL);
		}
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

	printf(DONE_MSG);
	return retval;
}
