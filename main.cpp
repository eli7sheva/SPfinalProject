#include <cstring>
#include <cstdlib>
#include <cstdio>
extern "C"{
#include "SPConfig.h"
#include "SPPoint.h"
#include "SPLogger.h"
#include "SPExtractFeatures.h"
#include "main_aux.h"
}
#include "SPImageProc.h"


#define DEFAULT_CONFIG_FILENAME "spcbir.config"
#define INVALID_CMD_LINE_MSG "Invalid command line : use -c <config_filename>\n"
#define ERROR_OPENING_CONFIG_FILE_MSG "The configuration file %s couldn’t be open\n"
#define ERROR_OPENING_DEFAULT_CONFIG_FILE_MSG "The default configuration file %s couldn’t be open\n"
#define CMD_LINE_CONFIG_FILENAME_FLAG "-c"
#define ERROR_MEMORY_ALLOC_MSG "Error allocating memory\n"
#define CONFIG_FILE_PATH_SIZE      1024	 // the path of any file contains no more than 1024 characters
#define ENTER_AN_IMAGE_MSG "Please enter an image path:\n"




int main(int argc, char *argv[]) {
	int i;
	int j;
	int k;
	char config_filename[CONFIG_FILE_PATH_SIZE];
	char query_image[CONFIG_FILE_PATH_SIZE];
	int query_num_of_features;						// number of features in query image
	SPPoint* query_features; 						// all query features
	SPConfig config;								// hold configuration parameters
	bool extraction_mode;							//indicates if extraction mode on or off
	char current_image_path[CONFIG_FILE_PATH_SIZE]; // the path to the current image
	int* num_of_features; 							// num of features extracted from each image
	int num_of_images;   						    // number of images in the directory given by the user in the configuration file
	SPPoint** features;   							// the features of the images in the  directory given by the user in the configuration file
	KD_TREE all_features_tree; 						// KDTree that will hold all features of all images todo elisheve change KD_TREE to the right name
	int* closest_images; 							// array holds the spNumOfSimilarImages indexes of the closest images to the query image
	sp::ImageProc *improc;
	SP_CONFIG_MSG msg;
	int error = 0;

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
			free(config);
		}

		if (msg != SP_CONFIG_SUCCESS) {
			free(config);
			return -1;
		}
	}
	else { // argc == 3

		// check that second argument is the -c flag
		if (strcmp(argv[1], CMD_LINE_CONFIG_FILENAME_FLAG) != 0) {
			printf(INVALID_CMD_LINE_MSG);
			return -1;
		}
	
		strcpy(config_filename, argv[2]);
		config = spConfigCreate(config_filename, &msg);
		
		if (msg == SP_CONFIG_CANNOT_OPEN_FILE) {
			printf(ERROR_OPENING_CONFIG_FILE_MSG, config_filename);
			free(config);
		}

		if (msg != SP_CONFIG_SUCCESS) {
			free(config);
			return -1;
		}
	}

	// todo elisheva initiate logger

	// get number of images from configuration
	num_of_images = spConfigGetNumOfImages(config, &msg);
	if (msg != SP_CONFIG_SUCCESS) {
		// spLoggerPrintError(NUM_OF_IMAGES_ERROR, __FILE__, __func__, __LINE__);
		free(config);
		return -1; //todo print error msg
	}

	extraction_mode = spConfigIsExtractionMode(config, &msg);
	if (msg != SP_CONFIG_SUCCESS) {
		free(config);
		return -1;
	}

	improc = new sp::ImageProc(config);

	if (extraction_mode) {


		if ((features = (SPPoint**)malloc(sizeof(*features) * num_of_images)) == NULL) {
			free(config);
			return -1; //todo print error msg
		}
		
		if ((num_of_features = (int*)malloc(sizeof(int) * num_of_images)) == NULL) {
			free(features);
			free(config);
			return -1; //todo print error msg
		}
		
		// extract each image features and write them to file
		for (i=0; (!error) & (i < num_of_images); i++) {	
			// find image path
			msg = spConfigGetImagePath(current_image_path, config, i);
			if (msg != SP_CONFIG_SUCCESS) { // should not happen
				error = 1;
				break;
			}

			// extract image features
			features[i] = improc->getImageFeatures(current_image_path, i, &(num_of_features[i]));
			if (features[i] == NULL) {
				error = 1;
				break;
			}

			// write image features into file
			if (writeImageFeaturesIntoFile(config, i, features[i], num_of_features[i]) == -1) {
				error = 1;
				break;
			}
		}

		if (error)
		{	
			for (j=0; j<i; j++) {
				for(k=0;k<num_of_features[j];k++){
					spPointDestroy(features[j][k]);
				}
			}
			free(features);
			free(num_of_features);
			free(config);
			//print error  todo
			return -1;
		}
		
	}

	else { // not extraction mode
		features = extractFeaturesFromFile(config);
		if (features == NULL) {
			free(config);
			return -1;
		}
	}

	all_features_tree = initDataStructures(features); // todo initiate tree from images features

	// free features
	for (j=0; j<num_of_images; j++) {
		for(k=0;k<num_of_features[j];k++){
			spPointDestroy(features[j][k]);
		}
	}
	free(features); // // done with that



	if (all_features_tree == NULL) { // todo initDataStructures error
		free(config);
		return -1;
	}

	// get a query image from the user
	printf(ENTER_AN_IMAGE_MSG);
	fflush(NULL);
	scanf("%s",query_image);

	// get query image's deatures
	query_features = improc->getImageFeatures(query_image, num_of_images, &query_num_of_features); // todo which index to give?

	// find closest images to the query image
	closest_images = getKClosestImages(config->spNumOfSimilarImages, config->spKNN, query_features[i], //todo find another way to get spKNN
									   all_features_tree, num_of_images, num_of_features);
	free(num_of_features); // done with that

	if (closest_images == NULL) { // error
		// todo print error log
		// free everything
		for(k=0;k<query_num_of_features;k++){
			spPointDestroy(query_features[k]);
		}
		free(query_features);
		free(all_features_tree); // todo elisheva: is that the way to free a KDTree?
		free(config);
		return -1;
	}

	// todo elisheva - show (display) closest_images images



	// done - free everything 
	// todo elisheva destroy logger
	for(k=0;k<query_num_of_features;k++){
		spPointDestroy(query_features[k]);
	}
	free(query_features);
	free(closest_images);
	free(all_features_tree); // todo elisheva: is that the way to free a KDTree?
	free(config);
	return 0;

}
