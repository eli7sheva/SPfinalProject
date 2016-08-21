#ifndef MAIN_AUX_H_
#define MAIN_AUX_H_

#include "SPPoint.h"
#include "KDTreeNode.h"
#include "SPConfig.h"
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

/** 
  *  Initiates kd tree from a given list holds all the features per each image.
  *  The kd tree will hold all the featues of all the images
  *  returns the kd tree and prints log on error.
  *
  * @param features_per_image - array of all features per image.
  *        i.e features_per_image[i][j] holds the jth feature of the image at index i.
  * @param num_of_features_per_image - array holds the number of features described in features_per_image for each image. 
  *        i.e num_of_features_per_image[i] is the size of features_per_image[i] 
  * @param num_of_images - the number of images described in features_per_image and in num_of_features_per_image.
  * @param split_method - the split method to use in KD Tree returned
  *
  * @return the initiated kd_tree
  *         or NULL in case of error
  */
KDTreeNode initiateDataStructures(SPPoint** features_per_image, int* num_of_features_per_image, int num_of_images, int split_method);

/**
 * extract information from configuration file and stores it into the given params as described below:
 * 
 * @param config - the configuration structure
 * @param num_of_similar_images - an address to store the number of images in
 * @param knn - an address to store the number of knn in
 * @param split_method - an address to store the split method in
 * @param extraction_mode - an address to store whether extraction mode is asked 
 * @param min_gui - an address to store whether minimal gui mode is asked 
 * @param all_images_paths - an address to an array that will store all the full paths to the images described in configuration file
 *   
 * @return -1 on failure (and prints the errors) and 0 on success
 */
 int initFromConfig(const SPConfig config, int* num_of_images, int* num_of_similar_images, int* knn, int* split_method, bool* extraction_mode, bool* min_gui, char *** all_images_paths);

/**
 * saves images features into file (iuse in extraction mode)
 *
 * @param features_per_image - array of all features per image.
 *        i.e features_per_image[i][j] holds the jth feature of the image at index i.
 * @param num_of_features_per_image - array holds the number of features described in features_per_image for each image. 
 *        i.e num_of_features_per_image[i] is the size of features_per_image[i] 
 * @param num_of_images - the number of images described in features_per_image and in num_of_features_per_image.
 *
 * @return -1 on failure (and prints the errors) and 0 on success
 */
int saveToDirectory(const SPConfig config, SPPoint** features_per_image, int* num_of_features_per_image, int num_of_images);

/**
 * extract and returns images features from files (use in non-extraction mode)
 *
 * @param config - the configuration structure
 * @param num_of_features_per_image - array holds the number of features described in features_per_image for each image. 
 *        i.e num_of_features_per_image[i] is the size of features_per_image[i] 
 * @param num_of_images - the number of images described in features_per_image and in num_of_features_per_image.
 *
 * @returns array of points describing the features of the images in config file on success
 *          NULL on error (and prints log)
 */
SPPoint** extractFromFiles(const SPConfig config, int* num_of_features_per_image, int num_of_images);
#endif

/*
 * part of the show images
 * prints to std out the paths of the best images that match the query
 * @param: query_image- the path of the query image
 *         num_of_similar_images_to_find- the number of similar images (to the query) to find (from configuration file)
 *         all_images_paths- the paths to all the images
 *         closest_images- an array that holds the spNumOfSimilarImages indexes of the closest images to the query image
 *
 *
 *
 */
int PrintMinGuiFalse(char* query_image,int num_of_similar_images_to_find, char** all_images_paths,
					int* closest_images);
