#ifndef MAIN_AUX_H_
#define MAIN_AUX_H_

#include "SPPoint.h"
#include "KDTreeNode.h"

/*

 * Given all features of the images in the database (databaseFeatures), finds the
 * closest nearestKImages images to a given image's features (queryFeatures). The function returns the
 * INDEXES of the images to which the closest features belong, stored in ascending order
 * (Closest feature image index is first, second closest feature image index is second, etc...).
 *
 * @param nearestKImages the number of the best (closest) distances to find
 * @param bestNFeatures     - The number of indexes to return.
 * @param queryFeatures    - the features which will be compared with the other descriptor
 * @param databaseFeatures  - An KD_TREE in which all the features of all the images are stored.
 / @param queryNumOfFeatures - number of features in param queryFeatures
 * @param numberOfImages    - The number of images in the database. (Number of entries in databaseFeatures)
 * @param nFeaturesPerImage - The number of features for each image. 
 *
 * Note: prints errors to logger
 * assumes Logger is initialized
 *
 * @return - NULL if either the following:
 *           * queryFeatures is NULL
 *           * databaseFeatures is NULL
 *           * numberOfImages <= 1
 *           * queryNumOfFeatures <= 1
 *           * nFeaturesPerImage is NULL
 *           * allocation error occurred
 *           otherwise, an array of size bestNFeatures is returned such that:
 *           * Given that f1, f2, ... the closest features to featureA (i.e d(featureA,f1) <= d(featureA,f2) <= ...)
 *           * i1, i2, .... are the indexes of the images to which fi belongs (i.e f1 is a SIFT descriptor of image i1,
 *             f2 is a SIFT descriptor of image i2 etc..)
 *           Then the array returned is {i1,i2,...,i_bestNFeatures}
 */
int* getKClosestImages(int nearestKImages, int bestNFeatures, SPPoint* queryFeatures, KDTreeNode databaseFeatures, int queryNumOfFeatures, int numberOfImages, int* nFeaturesPerImage); // todo elisheva change KD_TREE. Should SPPoint* be a KD_ARRAY?please add to #include everything needed here, and in the makefile (add it in the makefile as a dependency to main_aux.o)


// todo add doc
int initiateLoggerByConfig(const SPConfig config);
char** getAllImagesPaths(const SPConfig config, int* num_of_images);
int getConfigParameters(const SPConfig config, int* num_of_similar_images, int* knn, int* split_method, bool* extraction_mode, bool* min_gui);
#endif
