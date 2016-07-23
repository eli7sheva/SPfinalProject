#include "SPLogger.h"
#include "SPExtractFeatures.h"
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

#define FEATURES_FILE_SUFFIX ".feats" // features file suffix
#define MAX_PATH_SIZE		 1024     // any file path size is of maxlen
#define IMAGE_PATH_FORMAT 	 "%s%s%d%s"

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
 * @param featuresPath - an address to store the result in, it must contain enough space.
 * @param config - the configuration structure
 * @param index - the index of the image.
 *
 * @return
 * - SP_CONFIG_INVALID_ARGUMENT - if featuresPath == NULL or config == NULL
 * - SP_CONFIG_INDEX_OUT_OF_RANGE - if index >= spNumOfImages
 * - SP_CONFIG_SUCCESS - in case of success
 */
SP_CONFIG_MSG spConfigGetImageFeaturesFilePath(char* featuresPath, const SPConfig config, int index) {
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
 * extraction mode 
 * extract the features of each image in the directory spImagesDirectory/spImagesPrefix<index>spImagesSuffix 
 * and then store each of these features to a file using the function writeImageFeaturesIntoFile.
 * Thr file will be located in the directory given by spImagesDirectory and will have the
 * same name as the image but the suffix will be ".feats"
 * The features are saved in a special format in the file.
 *
 * @param config - the configuration parameters
 * @return
 * An array of arrays where the ith place is the actual features extracted from the ith image.
 * NULL is returned in case of an error.
 */

SPPoint** extractFeaturesIntoFile(const SPConfig config) {
	int i;
	int j;
	int k;
	char current_image_path[MAX_PATH_SIZE];
	char features_filename[MAX_PATH_SIZE];
	SP_CONFIG_MSG msg;
	int* num_of_featues;
	SPPoint** features;
	int error = 0;

	if (config == NULL) {
		return NULL;
	}

	if ((features = (SPPoint**) malloc(sizeof(*resPoints) * config->spNumOfImages)) == NULL) {
		return NULL;
	}

	if ((num_of_featues = (int*)malloc(sizeof(int) * config->spNumOfImages)) == NULL) {
		free(features);
		return NULL;
	}

	ImageProc(config);

	// for each image - write it's features into file
	for (i=0; !error & i < config->spNumOfImages; i++) {	 
		// find image path
		msg = spConfigGetImagePath(current_image_path, config, i);
		if (msg != SP_CONFIG_SUCCESS) { // should not happen
			error = 1;
			break;
		}

		// find features file path
		msg = spConfigGetImageFeaturesFilePath(features_filename, config, i);
		if (msg != SP_CONFIG_SUCCESS) { // should not happen
			error = 1;
			break;
		}
		
		// get image features
		features[i] = getImageFeatures(current_image_path, i, &(num_of_featues[i]));
		if (features[i] == NULL) {
			error = 1;
			break;
		}

		// write all features to the features file
		if (writeImageFeaturesIntoFile(features, features_filename, i, num_of_featues[i]) == -1) {
			error = 1;
			break;
		}
	}

	// on error -free all features and return 
	if (error)
	{	
		for (j=0; j<i; j++) {
			for(k=0;k<num_of_featues[j];k++){
				spPointDestroy(features[j][k]);
			}
		}
		free(num_of_featues);
		free(features);
		return NULL;
	}

	free(num_of_featues);
	return features;
}



/*
 * Extraction mode
 * Write the given features into a file in the following format:
 *
 * <image index>
 * <number of features written to this file (mark it as n)>
 * <feature_1 index>
 * <feature_1 dim (mark as d1)>
 * <fearure_1 coordinate1> <fearure_1 coordinate2>  ... <fearure_1 coordinate_d1>
 * ...
 * <feature_n index>
 * <feature_n dim (mark as dn)>
 * <fearure_n coordinate1> <fearure_n coordinate2>  ... <fearure_n coordinate_dn>
 *
 * Note: each line in the file is of maximum length as the config file
 *
 * @param features - the features to write to the file
 * @param features_filename - the file name to write the features into
 * @param image_index - the index ofo the image which features are given in param features
 * @param num_of_featues - the number of features found in param features
 *
 * @return 0 on success
 *		   -1 on error
  */
int writeImageFeaturesIntoFile(const SPPoint* features, char* features_filename, int image_index, int num_of_featues)

{
	int i;
	FILE *ofp;
	ofp = fopen(features_filename, "w");
	int dim;
	int axis;

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
SPPoint* readImageFreaturesFromFile(char* features_filename, int num_of_featues) { // todo should return those:  int image_index, int num_of_featues?
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
	&num_of_featues = atoi(line);

	if ((features = (SPPoint*)malloc(sizeof(*features)*num_of_featues)) == NULL) { // todo check this is ok
		// *msg = SP_CONFIG_ALLOC_FAIL; todo remove
		fclose(ifp);
		return NULL;
	}

	// extract each features
	for(i=0; !error & i<num_of_featues; i++) {

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

SPPoint** extractFeaturesFromFile(const SPConfig config) {
	int i;
	int j;
	int k;
	int error = 0;
	char current_image_path[MAX_PATH_SIZE];
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
		msg = spConfigGetImageFeaturesFilePath(features_filename, config, i);
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

void freeFeatures(const SPConfig config, SPPoint** features) {
	int i;
	if ((config == NULL) || (features == NULL)) 
		return;

	for (i=0; i< config->spNumOfImages; i++)
		free(features[i]);
	free(features);
}

// KD_TREE initDataStructures(SPPoint** features) {

// 	saveAllFeaturesIntoKDTree() //todo wait for elisheva
// }

