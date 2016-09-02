#include <stdbool.h>
#include "unit_test_util.h"
#include <ctype.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "SPLogger.h"
#include "SPConfig.h"
#include "SPPoint.h"
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

void replaceSuffix(const char* input, const char* new_suffix, char* output) {
    int i = strlen(input) -1;
    assert(i>0);
    while ((i>=0)&&(*(input + i)) && (*(input + i) != PATH_SUFFIX_MARK)) {
		i--;
    }
    printf("%d\n", i);
    if (i<0){
    	strcpy(output, input);
    }
    else {
	    strncpy(output, input, i);
	    output[i] = STRING_NULL_TERMINATOR;
		strcat(output, new_suffix);
	}
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
	int i = 0;
	int cur_coor = 0;
	char coor_str[MAX_LINE_SIZE];
	// allocate coordinates array
	double* coordinates = (double*)malloc(dim*sizeof(double));
	if (coordinates == NULL) {
		spLoggerPrintError(ALLOCATION_FAILURE_MSG, __FILE__, __func__, __LINE__);
		return NULL;
	}
	// printf("%c\n",*(line + i));

	// get all coordinate except last one
	while ((cur_coor < dim) &&(*(line + i) != STRING_NULL_TERMINATOR)) {
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



bool test_replaceSuffix(){
	char * imagePath = "hello.world";
	char output[1024];
	
	replaceSuffix(imagePath, ".reut", output);
	printf("output is %s\n", output);
	ASSERT_TRUE(strcmp(output, "hello.reut") == 0);
	
	imagePath = "hello";
	replaceSuffix(imagePath, ".reut", output);
	ASSERT_TRUE(strcmp(output, "hello") == 0);
	
	imagePath = ".world";
	replaceSuffix(imagePath, ".reut", output);
	ASSERT_TRUE(strcmp(output, ".reut") == 0);
	
	imagePath = "hello.";
	replaceSuffix(imagePath, ".reut", output);
	ASSERT_TRUE(strcmp(output, "hello.reut") == 0);

	imagePath = "test1images/hello.world";
	replaceSuffix(imagePath, ".reut", output);
	ASSERT_TRUE(strcmp(output, "test1images/hello.reut") == 0);

	imagePath = "./test1images/hello.world";
	replaceSuffix(imagePath, ".reut", output);
	ASSERT_TRUE(strcmp(output, "./test1images/hello.reut") == 0);
	return true;
}


bool test_getImageFeaturesFilePath(){
	char imagePath[1024];
	char * config_filename = "test_spconfig.config";
	SP_CONFIG_MSG msg ;
	SPConfig config =  spConfigCreate(config_filename, &msg);
	msg =getImageFeaturesFilePath( config, 3, imagePath);
	ASSERT_TRUE(msg == SP_CONFIG_SUCCESS);
	printf("imagePath after replacing suffix%s\n",imagePath );
	ASSERT_TRUE(strcmp(imagePath, "./test1images/img3.feats") == 0);	
	printf("%s\n", imagePath );
	printf("%d\n", msg);
	spConfigDestroy(config);
	return true;
}



bool test_parseCoorLineFormat(){
	// * <fearure_n coordinate1> <fearure_n coordinate2>  ... <fearure_n coordinate_dn>
	char * line = "1.1 2.2 3.3";
	int dim = 3;
	double * coor = parseCoorLineFormat(line, dim);

	ASSERT_TRUE(coor[0] == 1.1);
	ASSERT_TRUE(coor[1] == 2.2);
	ASSERT_TRUE(coor[2] == 3.3);
	printf("%f\n", coor[0]);
	free(coor);
	return true;
}


int main(){
	printf("1\n");
	RUN_TEST(test_replaceSuffix);
	printf("2\n");
	RUN_TEST(test_getImageFeaturesFilePath);
	printf("3\n");
	RUN_TEST(test_parseCoorLineFormat);

	return 0;
}