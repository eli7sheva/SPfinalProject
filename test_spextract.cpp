#include <cstdbool.h>
#include "unit_test_util.h"
#include <ctype.h>
#include <cassert.h>
#include <cstdio.h>
#include <stdlib.h>
#include <cstring.h>
#include <cctype.h>
#include <cstdio.h>
#include <stdlib.h>
#include <cstring.h>
extern "C"{
	#include "SPLogger.h"
	#include "SPConfig.h"
	#include "SPPoint.h"
}
#include "SPImageProc.h"
/*
 * features file fomat consts
 * the format of the header is <image index>\n<number of features> (FEATURES_FILE_HEADER_FORMAT)
 * The format of the body consist of features units:
 * each unit is <feature index>\n<feature dim>\n<feature corridinates seperated with space>
 * <feature index>\n<feature dim>\n is FEATURES_FILE_UNIT_FORMAT
 * <feature corridinates seperated with space> is repeating units of FEATURES_FILE_COOR_FORMAT
 * that ends with  FEATURES_FILE_LAST_COOR_FORMAT
 *
*/
#define FEATURES_FILE_HEADER_FORMAT 	 "%d\n%d\n"
#define FEATURES_FILE_UNIT_FORMAT   	 "%d\n%d\n"     
#define FEATURES_FILE_COOR_FORMAT   	 "%f "
#define FEATURES_FILE_LAST_COOR_FORMAT   "%f\n"
#define PATH_SUFFIX_MARK     '.'
#define FEATURES_FILE_SUFFIX ".feats" // features file suffix
#define MAX_PATH_SIZE		 1024     // any file path size is of maxlen
#define MAX_LINE_SIZE		 1024     
#define STRING_NULL_TERMINATOR 		'\0'


#define ERROR_WRITING_TO_FEATURES_FILE "Error writing to features file"
#define ERROR_OPENING_FEATURES_FILE "Error while opening features file"
#define ERROR_WHILE_READING_FEATURES_FILE "Error while reading features file"
#define ERROR_READING_CONFIG_INVALID_ARG "An error occurred - invalid argument" 
#define ALLOCATION_FAILURE_MSG "An error occurred - allocation failure"

/**
 * replaces the suffix of a string
 *
 * @param input - the string which suffix will be replaces
 * @param new_suffix - the suffix to replace to
 * @param output - the string which will hold the string with the replaces suffix
 *
 * Note that if string input does not contain suffix, the new_suffix will be added to it
 * Note the output assumed to hbe allocated with enough space
 *
 */

void replaceSuffix(const char* input, char* new_suffix, char* output) {
    unsigned int i = 0;
    while ((*(input + i)) && (*(input + i) != PATH_SUFFIX_MARK)) {
		i++;
    }

    strncpy(output, input, i);
    output[i] = STRING_NULL_TERMINATOR;
	strcat(output, new_suffix);
}

/**
 * Given an index 'index' the function stores in featuresPath the full path of the
 * ith images extracted features file.
 *
 * For example:
 * Given that the value of:
 *  spImagesDirectory = "./images/"
 *  spImagesPrefix = "img"
 *  spNumOfImages = 17
 *  index = 10
 *
 * The functions stores "./images/img10.feats" to the address given by featuresPath.
 * Thus the address given by featuresPath must contain enough space to
 * store the resulting string.
 *
 * @param config - the configuration structure
 * @param index - the index of the image.
 * @param featuresPath - an address to store the result in, it must contain enough space.
 *
 * @return
 * - SP_CONFIG_INVALID_ARGUMENT - if featuresPath == NULL or config == NULL
 * - SP_CONFIG_INDEX_OUT_OF_RANGE - if index >= spNumOfImages
 * - SP_CONFIG_SUCCESS - in case of success
 */
SP_CONFIG_MSG getImageFeaturesFilePath(const SPConfig config, int index, char* featuresPath) {
	SP_CONFIG_MSG msg;
	char imagePath[MAX_PATH_SIZE];
	if (( msg = spConfigGetImagePath(imagePath, config, index)) != SP_CONFIG_SUCCESS) {
		spLoggerPrintError(ERROR_READING_CONFIG_INVALID_ARG, __FILE__, __func__, __LINE__);
		return msg;
	}
	replaceSuffix(imagePath, FEATURES_FILE_SUFFIX, featuresPath);

	return SP_CONFIG_SUCCESS;
}

/*
 * Parses coordinates line in the features file. 
 * Line format as described in writeImageFeaturesIntoFile
 * @param line - the line to parse
 * @param dim - the dimentions of the feature
 * Note: assums the line is in the right coordinates format
 *
 * @return
 * An array of the features coordinates. NULL is returned in case of an error.
 *
 */
double* parseCoorLineFormat(char* line, int dim) {
	int i;
	int cur_coor = 0;
	char coor_str[MAX_LINE_SIZE];
	
	// allocate coordinates array
	double* coordinates = (double*)malloc(dim*sizeof(double));
	if (coordinates == NULL) {
		spLoggerPrintError(ALLOCATION_FAILURE_MSG, __FILE__, __func__, __LINE__);
		return NULL;
	}

	// get all coordinate except last one
	while (*(line + i) != STRING_NULL_TERMINATOR) {
		// when reach to a space - parse the data before as a double (single coordinate)
		if (isspace(*(line + i ))) {
			strncpy(coor_str, line, i);
			coor_str[i] = STRING_NULL_TERMINATOR;
			coordinates[cur_coor] = atof(coor_str);
			cur_coor++;

			// advance the pointer to after space (skip space) and start search again to get next coordinate
			line = line + i + 1;
			i = 0;
		}
		// continue until a space is found
		else {
			i++;
		}
	}

	// get last coordinate
	strncpy(coor_str, line, i);
	coor_str[i] = STRING_NULL_TERMINATOR;
	coordinates[cur_coor] = atof(coor_str);

	return coordinates;
}

int writeImageFeaturesIntoFile(const SPConfig config, int image_index, const SPPoint* features, int num_of_features)
{
	int i;
	FILE *ofp;
	char features_filename[MAX_PATH_SIZE];
	SP_CONFIG_MSG msg;
	int dim;
	int axis;

	// find features file path
	msg = getImageFeaturesFilePath(config, image_index, features_filename);
	if (msg != SP_CONFIG_SUCCESS) { // should not happen
		//error is printed inside function
		return -1;
	}

	ofp = fopen(features_filename, "w");
    if(ofp == NULL) {
    	spLoggerPrintError(ERROR_OPENING_FEATURES_FILE, __FILE__, __func__, __LINE__);
    	return -1;
    }


    // write header (image index and number of features) to file 
	if (fprintf(ofp, FEATURES_FILE_HEADER_FORMAT, image_index, num_of_features) < 0) {
		fclose(ofp);
    	spLoggerPrintError(ERROR_WRITING_TO_FEATURES_FILE, __FILE__, __func__, __LINE__);
		return -1;
	}

	// write each feature information into the file in the right format
	for (i = 0; i < num_of_features; i++) {
		dim = spPointGetDimension(features[i]);
		// write dim and index
		if (fprintf(ofp, FEATURES_FILE_UNIT_FORMAT, spPointGetIndex(features[i]), dim) < 0) {
    		spLoggerPrintError(ERROR_WRITING_TO_FEATURES_FILE, __FILE__, __func__, __LINE__);
			fclose(ofp);
			return -1;
		}
		
		// write coordinates
		for (axis = 0 ; axis < dim -1; axis++) {
			if (fprintf(ofp, FEATURES_FILE_COOR_FORMAT, spPointGetAxisCoor(features[i], axis))< 0) {
 		   		spLoggerPrintError(ERROR_WRITING_TO_FEATURES_FILE, __FILE__, __func__, __LINE__);
				fclose(ofp);
				return -1;
			}
		}
		
		// write last coordinate
		if (fprintf(ofp, FEATURES_FILE_LAST_COOR_FORMAT, spPointGetAxisCoor(features[i], axis))< 0) {
 		   	spLoggerPrintError(ERROR_WRITING_TO_FEATURES_FILE, __FILE__, __func__, __LINE__);
			fclose(ofp);
			return -1;
		}
	}

	fclose(ofp);
	return 0;
}



/*
 * Extracts features from file
 * 
 * @param features_filename - the path to th file contains the features to extract
 * @param num_of_features - will hold the number of feature extracted
 * Note: assumes features_filename is in the right features format described in function writeImageFeaturesIntoFile
 *
 * @return
 * An array of the actual features extracted. NULL is returned in case of an error.
 *
 */
SPPoint* readImageFreaturesFromFile(const SPConfig config, int image_index, int* num_of_features) {
	int i;
	int j;
	SPPoint* features;
	// int image_index; todo need to return this?
	char line[MAX_LINE_SIZE];
	char features_filename[MAX_LINE_SIZE];
	FILE *ifp;
	int error = 0;
	// features params
	int index;
	int dim;
	double* coordinates;

	// get image's features file name and open it 
	if ((getImageFeaturesFilePath(config, image_index, features_filename)) != SP_CONFIG_SUCCESS) {
		//error is printed inside function
		return NULL;
	}

	ifp = fopen(features_filename, "r");

    if(ifp == NULL) {
		spLoggerPrintError(ERROR_OPENING_FEATURES_FILE, __FILE__, __func__, __LINE__);
		return NULL;
    }


	// extract header
	fgets(line, sizeof(line), ifp);
	// image_index = atoi(line);
	fgets(line, sizeof(line), ifp);
	*num_of_features = atoi(line);

	if ((features = (SPPoint*)malloc(sizeof(*features)*(*num_of_features))) == NULL) { // todo check this is ok
		spLoggerPrintError(ALLOCATION_FAILURE_MSG, __FILE__, __func__, __LINE__);
		fclose(ifp);
		return NULL;
	}

	// extract each features
	for(i=0; (!error) & (i<(*num_of_features)); i++) {

		index = atoi(fgets(line, sizeof(line), ifp));
		dim = atoi(fgets(line, sizeof(line), ifp));
		error = ((coordinates = parseCoorLineFormat(fgets(line, sizeof(line), ifp), dim)) == NULL);
		if (!error) {
			error = ((features[i] = spPointCreate(coordinates, dim, index)) == NULL);
		}

	}

	// on error - free allocated features and return
	free(coordinates);
	if (error) {
		for (j=0; j<i; j++) {
			spPointDestroy(features[j]);
		}
		fclose(ifp);
		free(features);
		spLoggerPrintError(ERROR_WHILE_READING_FEATURES_FILE, __FILE__, __func__, __LINE__);	
		return NULL;
	}

	fclose(ifp);
	return features;
}





bool test_replaceSuffix(){

	char * imagePath = "hello.world";
	char output[1024];
	replaceSuffix(imagePath, "reut", output);
	assert(strcmp(output, "hello.reut") == 0);
	imagePath = "hello";
	replaceSuffix(imagePath, "reut", output);
	assert(strcmp(output, "hello") == 0);
	return true;
}


bool test_getImageFeaturesFilePath(){
	char imagePath[1024];
	char * config_filename = "test_spconfig.config";
	SP_CONFIG_MSG msg ;
	SPConfig config =  spConfigCreate(config_filename, &msg);
	msg =getImageFeaturesFilePath( config, 3, imagePath);
	assert(msg == SP_CONFIG_SUCCESS);
	assert(strcmp(imagePath, "./test1images/img3.feats") == 0);	
	printf("%s\n", imagePath );
	printf("%d\n", msg);
	return true;
}



bool test_parseCoorLineFormat(){
	// * <fearure_n coordinate1> <fearure_n coordinate2>  ... <fearure_n coordinate_dn>
	char * line = "1.1 2.2 3.3";
	int dim = 3;
	double * coor = parseCoorLineFormat(line, dim);

	assert(coor[0] == 1.1);
	assert(coor[1] == 2.2);
	assert(coor[2] == 3.3);
	printf("%d\n", coor[0]);
	return true;
}

bool test_writeReadImageFeaturesIntoFile(){
	char * config_filename = "test_spconfig.config";
	SP_CONFIG_MSG msg;
	SPConfig config =  spConfigCreate(config_filename, &msg);
	int num_of_images = spConfigGetNumOfImages(config, &msg);
	char * image_path ;
	int image_index = 1;
	spConfigGetImagePath(image_path ,config, image_index);
	int i;
	int j;
	SPPoint* features_per_image;
	SPPoint* extracted;
	int num_of_features_per_image;
	// writeImageFeaturesIntoFile()
	
	features_per_image = improc->getImageFeatures(image_path, image_index, &(num_of_features_per_image));
	

    // write image features into file
    if (writeImageFeaturesIntoFile(config, image_index, features_per_image, num_of_features_per_image) == -1) {
        printf("error in here132213\n");
    
		for (i=0; i< num_of_features_per_image;i++) {
			spPointDestroy(features_per_image[i]);
		}
		free(features_per_image);
    	return false;
    }

    extracted = readImageFreaturesFromFile(const SPConfig config, int image_index, int* num_of_features) {
    
    for (i=0; i< num_of_features_per_image;i++) {
		dim = spPointGetDimension(features_per_image[i]);
		assert(dim == spPointGetDimension(extracted[i]));
		assert(spPointGetIndex(features_per_image[i]) == spPointGetIndex(extracted[i]));
		for (j=0;j<dim; j++){
			assert(spPointGetAxisCoor(features_per_image[i], j) == spPointGetAxisCoor(extracted[i], j));
		}
	}

    for (i=0; i< num_of_features_per_image;i++) {
    	spPointDestroy(features_per_image[i]);
    	spPointDestroy(extracted[i]);
    }
    free(features_per_image);
    free(extracted);
    spConfigDestroy(config);
	return true;
}




int main(){
	RUN_TEST(test_replaceSuffix);
	RUN_TEST(test_getImageFeaturesFilePath);
	RUN_TEST(test_parseCoorLineFormat);
	RUN_TEST(test_writeReadImageFeaturesIntoFile);

	return 0;
}