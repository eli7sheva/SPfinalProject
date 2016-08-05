#ifndef SPEXTRACTFEATUREA_H_
#define SPEXTRACTFEATUREA_H_
#include "SPPoint.h"
#include "SPConfig.h"



/*
 * Extraction mode
 * Write the given features into a file in a special format.
 * The file will be located in the directory given by spImagesDirectory and will have the
 * same name as the image but the suffix will be ".feats"
 * The features are saved in a special format in the file.
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
 * @param config - the configuration paramethers
 * @param image_index - the index ofo the image which features are given in param features
 * @param features - the features to write to the file
 * @param num_of_features - the number of features found in param features
 *
 * @return 0 on success
 *		   -1 on error	
 */
int writeImageFeaturesIntoFile(const SPConfig config, int image_index, const SPPoint* features, int num_of_features);



/**
 * Extract the features of an image  from the features file that we generated in extraction mode.
 * Filename starts with spImagesPrefix and ends with ".feats"
 *
 * @param config - the configuration parameters
 * @param image_index - the index ofo the image which features are given in param features
 * @param num_of_features - will hold the number of feature extracted
 *
 * Note: each image path is of maximun length CONFIG_MAX_LINE_SIZE
 *
 * @return
 * An array of the actual features extracted.
 * NULL is returned in case of an error.
 *
 */
SPPoint* readImageFreaturesFromFile(const SPConfig config, int image_index, int* num_of_features);


#endif
