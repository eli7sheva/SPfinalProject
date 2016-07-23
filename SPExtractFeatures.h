#include "SPLogger.h"
#include "SPPoint.h"

#define KD_TREE char* //#todo remove this
/*
 * extraction mode 
 * extract the features of each image in the directory spImagesDirectory/spImagesPrefix<index>spImagesSuffix 
 * and then store each of these features to a file using the function writeImageFeaturesToFile.
 * Thr file will be located in the directory given by spImagesDirectory and will have the
 * same name as the image but the suffix will be ".feats"
 * The features are saved in a special format in the file.
 *
 * @param config - the configuration parameters
 * @return
 * An array of arrays where the ith place is the actual features extracted from the ith image.
 * NULL is returned in case of an error.
 */

SPPoint** extractFeaturesIntoFile(const SPConfig config);

/**
 * Extract the features of the images from the features files that we generated in extraction mode.
 * The features of the spNumOfImages images will be extracted from the features files located in spImagesDirectory.
 * Filename starts with spImagesPrefix and ends with ".feats"
 *
 * @param config - the configuration parameters
 *
 * Note: each image path is of maximun length CONFIG_MAX_LINE_SIZE
 *
 * @return
 * An array of arrays where the ith place is the actual features extracted from the ith image.
 * NULL is returned in case of an error.
 *
 */
SPPoint** extractFeaturesFromFile(const SPConfig config);


// todo doc return NULL on error
KD_TREE initDataStructures(SPPoint** features) ;	


/*
 * Frees all features
 * @param config - the configuration parameters
 * @param features - an array of arrays of features that belongs to all images of config
 *
 *	Note: if config or featuires are NULL - doesnt free anything
 */
void freeFeatures(const SPConfig config, SPPoint** features);