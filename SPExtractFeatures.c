#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SPLogger.h"
#include "SPExtractFeatures.h"

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

#define FEATURES_FILE_SUFFIX ".feats" // features file suffix
#define MAX_PATH_SIZE		 1024     // any file path size is of maxlen
#define MAX_LINE_SIZE		 1024     
#define IMAGE_PATH_FORMAT 	 "%s%s%d%s"
#define STRING_NULL_TERMINATOR 		'\0'



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

int writeImageFeaturesIntoFile(const SPConfig config, int image_index, const SPPoint* features, int num_of_featue)
{
	int i;
	FILE *ofp;
	char features_filename[MAX_PATH_SIZE];
	SP_CONFIG_MSG msg;
	int dim;
	int axis;

	// find features file path
	msg = spConfigGetImageFeaturesFilePath(config, image_index, features_filename);
	if (msg != SP_CONFIG_SUCCESS) { // should not happen
		return -1;
	}

	ofp = fopen(features_filename, "w");
    if(ofp == NULL) {
    	return -1;
    }


    // write header (image index and number of features) to file 
	if (fprintf(ofp, FEATURES_FILE_HEADER_FORMAT, image_index, num_of_featues) < 0) {
		fclose(ofp);
		return -1;
	}

	// write each feature information into the file in the right format
	for (i = 0; i < num_of_featues; i++) {
		dim = spPointGetDimension(features[i]);
		// write dim and index
		if (fprintf(ofp, FEATURES_FILE_UNIT_FORMAT, spPointGetIndex(features[i]), dim, *features[i]->coordinates) < 0) {
			fclose(ofp);
			return -1;
		}
		
		// write coordinates
		for (axis = 0 ; axis < dim -1; axis++) {
			if (fprintf(ofp, FEATURES_FILE_COOR_FORMAT, spPointGetAxisCoor(features[i], axis))< 0) {
				fclose(ofp);
				return -1;
			}
		}
		
		// write last coordinate
		if (fprintf(ofp, FEATURES_FILE_LAST_COOR_FORMAT, spPointGetAxisCoor(features[i], axis))< 0) {
			fclose(ofp);
			return -1;
		}
	}

	fclose(ofp);
	return 0;
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
SP_CONFIG_MSG spConfigGetImageFeaturesFilePath(const SPConfig config, int index, char* featuresPath,) {
	int n; //todo need?
	if ((featuresPath == NULL) || (config == NULL)) {
		return SP_CONFIG_INVALID_ARGUMENT;
	}

	if (index >= config->spNumOfImages) {
		return SP_CONFIG_INDEX_OUT_OF_RANGE;
	}
	if ((n = sprintf(featuresPath, IMAGE_PATH_FORMAT, config->spImagesDirectory, config->spImagesPrefix, index, FEATURES_FILE_SUFFIX)) < 0) {
		// todo what to return on error here? now I can define it
	}
	return SP_CONFIG_SUCCESS;
}


/*
 * Extracts features from file
 * 
 * @param features_filename - the path to th file contains the features to extract
 * @param num_of_featues - will hold the number of feature extracted
 * Note: assumes features_filename is in the right features format described in function writeImageFeaturesIntoFile
 *
 * @return
 * An array of the actual features extracted. NULL is returned in case of an error.
 *
 */
SPPoint* readImageFreaturesFromFile(char* features_filename, int* num_of_featues) { // todo should return those:  int image_index, int num_of_featues?
	int i;
	int j;
	SPPoint* features;
	int image_index;
	char line[MAX_LINE_SIZE];
	FILE *ifp;
	int error = 0;
	// features params
	int index;
	int dim;
	double* coordinates;

	if (features_filename == NULL) {
		return NULL;
	}

	ifp = fopen(features_filename, "r");

    if(ifp == NULL) {
       // *msg = SP_CONFIG_CANNOT_OPEN_FILE; // todo keep msg?
       return NULL;
    }


	// extract header
	fgets(line, sizeof(line), ifp);
	image_index = atoi(line);
	fgets(line, sizeof(line), ifp);
	*num_of_featues = atoi(line);

	if ((features = (SPPoint*)malloc(sizeof(*features)*(*num_of_featues))) == NULL) { // todo check this is ok
		// *msg = SP_CONFIG_ALLOC_FAIL; todo remove
		fclose(ifp);
		return NULL;
	}

	// extract each features
	for(i=0; !error & i<*num_of_featues; i++) {

		index = atoi(fgets(line, sizeof(line), ifp));
		dim = atoi(fgets(line, sizeof(line), ifp));
		error = ((coordinates = parseCoorLineFormat(fgets(line, sizeof(line), ifp), dim)) == NULL);
		if (!error) {
			error = ((features[i] = spPointCreate(coordinates, dim, index)) == NULL);
		}

	}

	// on error - free allocated features and return
	if (error) {
		for (j=0; j<i; j++) {
			spPointDestroy(features[j]);
		}
		fclose(ifp);
		free(features);
		return NULL;
	}

	fclose(ifp);
	return features;
}

SPPoint** extractFeaturesFromFile(const SPConfig config) {
	int i;
	int j;
	int k;
	int error = 0;
	char features_filename[MAX_PATH_SIZE];
	SPPoint** resPoints;
	int* num_of_featues;

	if ((resPoints = (SPPoint**) malloc(sizeof(*resPoints) * config->spNumOfImages)) == NULL) {
		return NULL;
	}

	if ((num_of_featues = (int*) malloc(sizeof(int) * config->spNumOfImages)) == NULL ) {
		free(resPoints);
		return NULL;
	}

	for (i=0; !error & i<config->spNumOfImages; i++) {	 
		msg = spConfigGetImageFeaturesFilePath(config, i, features_filename);
		error = (msg == SP_CONFIG_SUCCESS);
		
		if (!error) {
			error = ((resPoints[i] = readImageFreaturesFromFile(features_filename, &(num_of_featues[i])))==NULL);
		}
	}

	// on error - free all allocated features and return 
	if (error) {
		for(j=0; j<i; j++) {
			for (k=0; k<num_of_featues[j]; k++) {
				spPointDestroy(resPoints[j][k]);
			}
		}
		free(num_of_featues);
		free(resPoints);
		return NULL;
	}
	
	free(num_of_featues);
	return resPoints;
}


// todo dont remove this
// KD_TREE initDataStructures(SPPoint** features) {

// 	saveAllFeaturesIntoKDTree() //todo wait for elisheva
// }

