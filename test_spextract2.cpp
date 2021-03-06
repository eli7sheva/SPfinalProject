#include <stdbool.h>
#include "unit_test_util.h"
#include <ctype.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
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
	printf("image path is%s\n", imagePath);
	replaceSuffix(imagePath, FEATURES_FILE_SUFFIX, featuresPath);
	printf("after replacing %s\n", featuresPath);

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
	printf("writing to file %s\n", features_filename);
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
	printf("reading from file %s\n", features_filename);
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

	if ((features = (SPPoint*)malloc(sizeof(*features)*(*num_of_features))) == NULL) {
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
		free(coordinates);

	}

	// on error - free allocated features and return
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
bool fequal(double a, double b)
{
    return fabs(a-b) < 0.0004;
}

int adjust_num(double num) {
    double low_bound = 1e7;
    double high_bound = low_bound*10;
    double adjusted = num;
    int is_negative = (num < 0);
    if(num == 0) {
        return 0;
    }
    if(is_negative) {
        adjusted *= -1;
    }
    while(adjusted < low_bound) {
        adjusted *= 10;
    }
    while(adjusted >= high_bound) {
        adjusted /= 10;
    }
    if(is_negative) {
        adjusted *= -1;
    }
    //define int round(double) to be a function which rounds
    //correctly for your domain application.
    return round(adjusted);
}

bool test_writeReadImageFeaturesIntoFile(){
	const char * config_filename = "test_spconfig.config";
	SP_CONFIG_MSG msg;
	SPConfig config =  spConfigCreate(config_filename, &msg);
	char  image_path [1024];
	int image_index = 1;
	sp::ImageProc *improc = new sp::ImageProc(config);
	spConfigGetImagePath(image_path ,config, image_index);
	printf("using pic at%s\n", image_path);
	int i;
	int j;
	double coor1;
	double coor2;
	int dim;
	SPPoint* features_per_image;
	SPPoint* extracted;
	int num_of_features_per_image;
	int num_of_features_per_image2;
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

    printf("read from file\n");
    extracted = readImageFreaturesFromFile(config,  image_index, &num_of_features_per_image2);
    ASSERT_TRUE(num_of_features_per_image2 == num_of_features_per_image);
    printf("compare dim and index\n");
    for (i=0; i< num_of_features_per_image;i++) {
		dim = spPointGetDimension(features_per_image[i]);
		ASSERT_TRUE(dim == spPointGetDimension(extracted[i]));
		ASSERT_TRUE(spPointGetIndex(features_per_image[i]) == spPointGetIndex(extracted[i]));
	}
	printf("compare coor\n");
    for (i=0; i< num_of_features_per_image;i++) {
		dim = spPointGetDimension(features_per_image[i]);
		for (j=0;j<dim; j++){
			coor1 = spPointGetAxisCoor(features_per_image[i], j) ;
			coor2 =  spPointGetAxisCoor(extracted[i], j);
			// printf("coors %f %f\n",coor2, coor1 );
			ASSERT_TRUE((fequal(coor2, coor1)) ||(adjust_num(coor2) == adjust_num(coor1)));
			// ASSERT_TRUE(spPointGetAxisCoor(features_per_image[i], j) == spPointGetAxisCoor(extracted[i], j));
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
	RUN_TEST(test_writeReadImageFeaturesIntoFile);

	return 0;
}