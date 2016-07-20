#include <stdio.h>
#include <assert.h>
#include <stdlib.h>


#define DEFUALT_PCA_DIM 				20
#define DEFAULT_PCA_FILENAME 			"pca.yml"
#define DEFAULT_NUM_OF_FEATURES 		100
#define DEFAULT_EXTRACTION_MODE 		true
#define DEFAULT_NUM_OF_SIMILAR_IMAGES   1
#define DEFAULT_KD_TREE_SPLIT 			MAX_SPREAD
#define DEFAULT_KNN 					1
#define DEFAULT_MINIMAL_GUI 			false
#define DEFAULT_LOGGER_LEVEL 			3
#define DEFAULT_LOGGER_FILENAME 		"stdout" // todo do we need this? stdout as a string name? . remember to handle it differently

#define SP_IMAGES_DIRECTORY_STR 	 "spImagesDirectory"
#define SP_IMAGES_PREFIX_STR	 	 "spImagesPrefix"
#define SP_IMAGES_SUFFIX_STR 		 "spImagesSuffix"
#define SP_NUM_OF_IMAGES_STR 		 "spNumOfImages"
#define SP_PCA_DIMENSION_STR 		 "spPCADimension"
#define SP_PCA_FILENAME_STR			 "spPCAFilename"
#define SP_NUM_OF_FEATURES_STR		 "spNumOfFeatures"
#define SP_EXTRACTION_MODE_STR 		 "spExtractionMode"
#define SP_NUM_OF_SIMILAR_IMAGES_STR "spNumOfSimilarImages"
#define SP_KD_TREE_SPLIT_METHOD_STR  "spKDTreeSplitMethod"
#define SP_KNN_STR 					 "spKNN"
#define SP_MINIMAL_GUI_STR 			 "spMinimalGUI"
#define SP_LOGGER_LEVEL_STR 		 "spLoggerLevel"
#define SP_LOGGER_FILNAME_STR 		 "spLoggerFilename"

#define SP_IMAGES_DIRECTORY_INDEX 	   0
#define SP_IMAGES_PREFIX_INDEX	 	   1
#define SP_IMAGES_SUFFIX_INDEX		   2
#define SP_NUM_OF_IMAGES_INDEX 		   3
#define SP_PCA_DIMENSION_INDEX 		   4
#define SP_PCA_FILENAME_INDEX		   5
#define SP_NUM_OF_FEATURES_INDEX	   6
#define SP_EXTRACTION_MODE_INDEX       7
#define SP_NUM_OF_SIMILAR_IMAGES_INDEX 8
#define SP_KD_TREE_SPLIT_METHOD_INDEX  9
#define SP_KNN_INDEX				   10
#define SP_MINIMAL_GUI_INDEX		   11
#define SP_LOGGER_LEVEL_INDEX 		   12
#define SP_LOGGER_FILNAME_INDEX		   13

#define CONFIG_PARAMETERS_COUNT    14    // number of parameters
#define CONFIG_MAX_LINE_SIZE       1024  // each line in the configuration file contains no more than 1024 characters
#define CONFIG_START_COMMENT_MARK '#'    // comment in config file mast start with #
#define CONFIG_ASSIGNMENT_MARK    '='

typedef enum {RANDOM, MAX_SPREAD, INCREMENTAL, SPLIT_METHOD_COUNT} tree_split_method;

#define SPLIT_METHOD_STR(method)                 \
    (RANDOM      == method ? "RANDOM"       :    \
    (MAX_SPREAD  == method ? "MAX_SPREAD"   :    \
    (INCREMENTAL == method ? "INCREMENTAL"  :  "unknown"))))   

typedef int boolean;
#define true 1
#define false 0

// todo remove this
// typedef enum { false, true, BOOL_COUNTER } boolean;
// #define BOOLEAN_STR(bool)          \
//     (false == bool ? "false"  :    \
//     (true  == bool ? "true"   :    "unknown"))))   



const char* OPTIONAL_SUFFIX[] =  { ".jpg" , ".png" , ".bmp" , ".gif"};
#define PCA_DIM_LOW_LIMIT  10
#define PCA_DIM_HIGH_LIMIT 28
#define LOW_LOGGER_LEVEL   1
#define HIGH_LOGGER_LEVEL  4


#define ERROR_INVALID_LINE_MSG       "File: %s\nLine: %d\nMessage: Invalid configuration line\n"
#define ERROR_INVALID_CONSTRAINT_MSG "File: %s\nLine: %d\nMessage: Invalid value - constraint not met\n"
#define ERROR_PARMETER_NOT_SET_MSG   "File: %s\nLine: %d\nMessage: Parameter %s is not set\n"


struct sp_config_t{
	char* spImagesDirectory;
	char* spImagesPrefix;
	char* spImagesSuffix; 
	int spNumOfImages; 
	int spPCADimension; 
	char* spPCAFilename; 
	int spNumOfFeatures; 
	boolean spExtractionMode;
	int spNumOfSimilarImages;
	tree_split_method spKDTreeSplitMethod;
	int spKNN; 
	boolean spMinimalGUI;
	int spLoggerLevel; // todo ask if using SP_LOGGER_LEVEL is ok.. thats not the ORAOY
	char * spLoggerFilename; 
};


/*
 * Parses a line from the configuration file. if a valid configuration file parameter is found
 * in the line - it is added to the config struct.
 *
 * 
 * @param config_filename - the configuration file name
 * @param line - a line from the configuration file
 * @param line_number - the line number in configuration file of the param line given 
 * @param config - a pointer to a struct which holds system configuration.
 * @assert msg != NULL	
 * @param msg - pointer in which the msg returned by the function is stored
 *
 * @return -2 in case an empty line or comment line is given
 *		   -1 on any error (error explanation will be stored in msg)
 *          configuration parameter index of the parameter found in the given line
 * 
 * The resulting value stored in msg is as follow:
 * - SP_CONFIG_ALLOC_FAIL - if an allocation failure occurred
 * - SP_CONFIG_INVALID_INTEGER - if a line in the config file contains invalid integer
 * - SP_CONFIG_INVALID_STRING - if a line in the config file contains invalid string
 * - SP_CONFIG_SUCCESS - in case of success
 */
int parseLine(char * config_filename, char * line, int line_number, SPConfig config, SP_CONFIG_MSG* msg)) {
 	char right[CONFIG_MAX_LINE_SIZE]; // todo might need to set to CONFIG_MAX_LINE_SIZE -1 because of trim - check it 
 	char left[CONFIG_MAX_LINE_SIZE];
 	splitEqualAndTrim(line, right, CONFIG_MAX_LINE_SIZE, left, CONFIG_MAX_LINE_SIZE);

 	assert(msg != NULL);
 	*msg = SP_CONFIG_SUCCESS; // default is success

 	// if line is a comment - ignore
 	if (*left == CONFIG_START_COMMENT_MARK) {
 		return -2;
 	}
 	// ignore empty lines
 	if ((*left == '\0') && (*right  == '\0')) {
 		return -2;
 	}


 	else if (strcmp(left, SP_IMAGES_DIRECTORY_STR) == 0) {
		if (!hasNoWhiteSpace(right)) {
			printf(ERROR_INVALID_CONSTRAINT_MSG, config_filename, line_number);
			*msg = SP_CONFIG_INVALID_STRING;
			return -1;
		}
		else {
			if ((config->spImagesDirectory = (char*)malloc(CONFIG_MAX_LINE_SIZE*sizeof(char))) == NULL) {
				*msg = SP_CONFIG_ALLOC_FAIL;
				return -1; 
			}

			strcpy(config->spImagesDirectory, right);
 			return SP_IMAGES_DIRECTORY_INDEX;
 		}
 	}


	else if (strcmp(left, SP_IMAGES_PREFIX_STR) == 0) {
		if (!hasNoWhiteSpace(right))
			printf(ERROR_INVALID_CONSTRAINT_MSG, config_filename, line_number);
			*msg = SP_CONFIG_INVALID_STRING;
			return -1;
		}
		else {
			if ((config->spImagesPrefix = (char*)malloc(CONFIG_MAX_LINE_SIZE*sizeof(char))) == NULL) {
				*msg = SP_CONFIG_ALLOC_FAIL;
				return -1;
			}

			strcpy(config->spImagesPrefix, right);
 			return SP_IMAGES_PREFIX_INDEX;
 		}
	}

	else if (strcmp(left, SP_IMAGES_SUFFIX_STR) == 0) {
		if (!validSuffix(right)) {
			printf(ERROR_INVALID_CONSTRAINT_MSG, config_filename, line_number);
			*msg = SP_CONFIG_INVALID_STRING;
			return -1;
		}
		else {
			if ((config->spImagesSuffix = (char*)malloc(CONFIG_MAX_LINE_SIZE*sizeof(char))) == NULL) {
				*msg = SP_CONFIG_ALLOC_FAIL;
				return -1;

			}

			strcpy(config->spImagesSuffix, right);
			return SP_IMAGES_SUFFIX_INDEX;
		}
	}

	else if (strcmp(left, SP_NUM_OF_IMAGES_STR) == 0) {
		if (!isPositiveInteger(right)) {
			printf(ERROR_INVALID_CONSTRAINT_MSG, config_filename, line_number);
			*msg = SP_CONFIG_INVALID_INTEGER;
			return -1;
		}
		else {
			config->spNumOfImages = atoi(right);
			return SP_NUM_OF_IMAGES_INDEX;
		}
	}

	else if (strcmp(left, SP_PCA_DIMENSION_STR) == 0) {
		if (!isInRange(right, PCA_DIM_LOW_LIMIT, PCA_DIM_HIGH_LIMIT) {
			printf(ERROR_INVALID_CONSTRAINT_MSG, config_filename, line_number);
			*msg = SP_CONFIG_INVALID_INTEGER;
			return -1;
		}
		else {
			config->spPCADimension = atoi(right);
			return SP_PCA_DIMENSION_INDEX;
		}
	}

	else if (strcmp(left, SP_PCA_FILENAME_STR) == 0) {
		if (!hasNoWhiteSpace(right)) {
			printf(ERROR_INVALID_CONSTRAINT_MSG, config_filename, line_number);
			*msg = SP_CONFIG_INVALID_STRING;
			return -1;
		}
		else {
			if ((config->spPCAFilename = (char*)malloc(CONFIG_MAX_LINE_SIZE*sizeof(char))) == NULL) {
				*msg = SP_CONFIG_ALLOC_FAIL;
				return -1;
			}

			strcpy(config->spPCAFilename, right);
			return SP_PCA_FILENAME_INDEX;
		}
	}

	else if (strcmp(left, SP_NUM_OF_FEATURES_STR) == 0) {
		if (!isPositiveInteger(right)) {
			printf(ERROR_INVALID_CONSTRAINT_MSG, config_filename, line_number);
			*msg = SP_CONFIG_INVALID_INTEGER;
			return -1;
		}
		else {
			config->spNumOfFeatures = atoi(right);
			return SP_NUM_OF_FEATURES_INDEX;
		}
	}

	else if (strcmp(left, SP_EXTRACTION_MODE_STR) == 0) {
		if ((config->spExtractionMode = getBoolean(right)) == -1) {
			printf(ERROR_INVALID_CONSTRAINT_MSG, config_filename, line_number);
			*msg = //todo ask
			return -1;
		}
		else {
			return SP_EXTRACTION_MODE_INDEX;
		}
	}

	else if (strcmp(left, SP_NUM_OF_SIMILAR_IMAGES_STR) == 0) {
		if (!isNotNegativeInteger(right)) {
			printf(ERROR_INVALID_CONSTRAINT_MSG, config_filename, line_number);
			*msg = SP_CONFIG_INVALID_INTEGER;
			return -1;
		}
		else {
			config->spNumOfSimilarImages = atoi(right);
			return SP_NUM_OF_SIMILAR_IMAGES_INDEX;
		}
	}

	else if (strcmp(left, SP_KD_TREE_SPLIT_METHOD_STR) == 0) {
		if ((config->spKDTreeSplitMethod = getTreeSplitMethod(right)) == -1) {
			printf(ERROR_INVALID_CONSTRAINT_MSG, config_filename, line_number);
			*msg = // todo ask 
			return -1;
		}
		else {
			return SP_KD_TREE_SPLIT_METHOD_INDEX;
		}
	}

	else if (strcmp(left, SP_KNN_STR) == 0) {
		if (!isNotNegativeInteger(right)) {
			printf(ERROR_INVALID_CONSTRAINT_MSG, config_filename, line_number);
			*msg = SP_CONFIG_INVALID_INTEGER;
			return -1;
		}
		else {
			config->spKNN = atoi(right);
			return SP_KNN_INDEX;
		}
	}

	else if (strcmp(left, SP_MINIMAL_GUI_STR) == 0) {
		if ((config->spMinimalGUI = getBoolean(right)) == -1) {
			printf(ERROR_INVALID_CONSTRAINT_MSG, config_filename, line_number);
			*msg = // TODO ask
			return -1;
		}
		else {
			return SP_MINIMAL_GUI_INDEX;
		}
	}			

	else if (strcmp(left, SP_LOGGER_LEVEL_STR) == 0) {
		if (!isLoggerLevel(right)) {
			printf(ERROR_INVALID_CONSTRAINT_MSG, config_filename, line_number);
			*msg = SP_CONFIG_INVALID_INTEGER;
			return -1;
		}
		else {
			config->spLoggerLevel = atoi(right);
			return SP_LOGGER_LEVEL_INDEX;
		}
	}			

	else if (strcmp(left, SP_LOGGER_FILNAME_STR) == 0) {
		if (!hasNoWhiteSpace(right) ) {
			printf(ERROR_INVALID_CONSTRAINT_MSG, config_filename, line_number);
			*msg = SP_CONFIG_INVALID_STRING;
			return -1;
		}
		else {
			if ((config->spLoggerFilename = (char*)malloc(CONFIG_MAX_LINE_SIZE*sizeof(char))) == NULL) {
				*msg = SP_CONFIG_ALLOC_FAIL;
				return -1;
			}

			strcpy(config->spLoggerFilename, right);
			return SP_LOGGER_FILNAME_INDEX;
		}
	}			

 	else {
 		printf(ERROR_INVALID_LINE_MSG, config_filename, line_number);
 		*msg = // todo 
 		return -1;
 	}
 }


/*
 *
 * Checks which parameters in config struct are not set. If a parameter is not set - set it to its default 
 * value if exists. otherwise return error.
 * 
 * @param config_filename - the configuration file name
 * @param num_of_lines - the number of lines in the configuration file
 * @param config - a pointer to a struct which holds system configuration.
 * @param set_in_config - an array holding 0 or 1. if set_in_config[i] == 1 then configuration paramener
 *						  of index i is set in config struct. else it is not set.
 * @assert msg != NULL	
 * @param msg - pointer in which the msg returned by the function is stored
 *
 * @return -1 on any error (error explanation will be stored in msg)
 *          configuration parameter index of the parameter found in the given line
 * 
 * The resulting value stored in msg is as follow:
 * - SP_CONFIG_MISSING_DIR - if spImagesDirectory is missing
 * - SP_CONFIG_MISSING_PREFIX - if spImagesPrefix is missing
 * - SP_CONFIG_MISSING_SUFFIX - if spImagesSuffix is missing 
 * - SP_CONFIG_MISSING_NUM_IMAGES - if spNumOfImages is missing
 * - SP_CONFIG_SUCCESS - in case of success
 */

int checkMissingAndSetDefaults(char * config_filename, int num_of_lines, SPConfig config, char * set_in_config, SP_CONFIG_MSG* msg) {
	
 	assert(msg != NULL);
 	*msg = SP_CONFIG_SUCCESS; // default is success

	if (!set_in_config[SP_IMAGES_PREFIX_INDEX]) {
		printf(ERROR_PARMETER_NOT_SET_MSG, config_filename, num_of_lines, SP_IMAGES_PREFIX_STR);
		*msg = SP_CONFIG_MISSING_PREFIX;
		return -1;
 	}
 	
 	if (!set_in_config[SP_IMAGES_SUFFIX_INDEX]) {
		printf(ERROR_PARMETER_NOT_SET_MSG, config_filename, num_of_lines, SP_IMAGES_SUFFIX_STR);
		*msg = SP_CONFIG_MISSING_SUFFIX;
		return -1;
 	}
 	
 	if (!set_in_config[SP_NUM_OF_IMAGES_INDEX]) {
		printf(ERROR_PARMETER_NOT_SET_MSG, config_filename, num_of_lines, SP_NUM_OF_IMAGES_STR);
		*msg = SP_CONFIG_MISSING_NUM_IMAGES;
		return -1;
 	}

	if (!set_in_config[SP_IMAGES_DIRECTORY_INDEX) {
		printf(ERROR_PARMETER_NOT_SET_MSG, config_filename, num_of_lines, SP_IMAGES_DIRECTORY_STR);
		*msg = SP_CONFIG_MISSING_DIR;
		return -1;
	}			

 	if (!set_in_config[SP_PCA_DIMENSION_INDEX]) {
		config->spPCADimension = DEFUALT_PCA_DIM;
 	}

	if (!set_in_config[SP_PCA_FILENAME_INDEX]) {
		if ((config->spPCAFilename = (char*)malloc(CONFIG_MAX_LINE_SIZE*sizeof(char))) == NULL) {
			*msg = SP_CONFIG_ALLOC_FAIL;
			return -1;
		}
		strcpy(config->spPCAFilename, DEFAULT_PCA_FILENAME);
 	}

 	if (!set_in_config[SP_NUM_OF_FEATURES_INDEX]) {
		config->spNumOfFeatures = DEFAULT_NUM_OF_FEATURES;
 	}

 	if (!set_in_config[SP_EXTRACTION_MODE_INDEX]) {
		config->spExtractionMode = DEFAULT_EXTRACTION_MODE;
 	}
 	
	if (!set_in_config[SP_MINIMAL_GUI_INDEX) {
		config->spMinimalGUI = DEFAULT_MINIMAL_GUI;
	}			

	if (!set_in_config[SP_NUM_OF_SIMILAR_IMAGES_INDEX) {
		config->spNumOfSimilarImages = DEFAULT_NUM_OF_SIMILAR_IMAGES;
	}

	if (!set_in_config[SP_KNN_INDEX) {
		config->spKNN = DEFAULT_KNN;
	}

	if (!set_in_config[SP_KD_TREE_SPLIT_METHOD_INDEX) {
		config->spKDTreeSplitMethod = DEFAULT_KD_TREE_SPLIT;
	}

	if (!set_in_config[SP_LOGGER_LEVEL_INDEX) {
		config->spLoggerLevel = DEFAULT_LOGGER_LEVEL;
	}			

	if (!set_in_config[SP_LOGGER_FILNAME_INDEX) {
		// todo do we need this? stdout as a string name?
		if ((config->spLoggerFilename = (char*)malloc(CONFIG_MAX_LINE_SIZE*sizeof(char))) == NULL) {
			*msg = SP_CONFIG_ALLOC_FAIL;
			return -1;
		}
		strcpy(config->spLoggerFilename, DEFAULT_LOGGER_FILENAME);
 	}

	return 0;		
}

/**
 * check if string contains white space in it
 * @param s - string to check
 * @return 0 if string has at least one white space
 *         1 if string has no white space
 */
int hasNoWhiteSpace(const char *s) {
  while (*s != '\0') {
    if (isspace(*s))
      return 0; // False - string has white space
    s++;
  }
  return 1; // True - string has no white space
}



/**
 * checks if a string is a valid suffix
 * a string is a valid suffix if its value is in the OPTIONAL_SUFFIX set
 * @param suffix - the string to check
 * @return 1 if string is valid
 *         0 otherwise
 */
int validSuffix(const char * suffix) {
	int i;
	for (i = 0 ; i < sizeof(OPTIONAL_SUFFIX)/sizeof(*OPTIONAL_SUFFIX); i++) {
	    if (strcmp(OPTIONAL_SUFFIX[i], suffix) == 0) {
	        return 1;
	    }
	}
	return 0;
}

/**
 * checks if string contains only numbers (not - or + signs)
 * @param s - the string to check
 * @return 0 if string s contains only numbers (not - or + signs)
 *          -1 otherwise
 */
int numbersOnly(const char *s)
{
    while (*s) {
        if (isdigit(*s++) == 0) return 0;
    }

    return 1;
}


/**
 * Trim white spaces from the begining and end of the given string
 * Stores the trimmed input string into the given output buffer, which must be
 * large enough to store the result.  If it is too small, the output is truncated.
 * @param out - the string that will hold the output string (whichout whitespaces)
 * @param buffer_size - the size of param out string
 * @param str - the string to trim spaces off
 * @returns the size of the output string (out). 
 *          0 if buffer_size is 0
 *          1 if the output string is the empty string ('\0')
 */
size_t trimWhitespace(char *out, size_t buffer_size, const char *str)
{
  if(buffer_size == 0)
    return 0;

  const char *end;
  size_t out_size;

  // Trim leading space
  while(isspace(*str)) str++;

  if(*str == '\0')  // All spaces?
  {
    *out = '\0';
    return 1;
  }

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace(*end)) end--;
  end++;

  // Set output size to minimum of trimmed string length and buffer size minus 1
  out_size = (end - str) < buffer_size-1 ? (end - str) : buffer_size-1;

  // Copy trimmed string and add null terminator
  memcpy(out, str, out_size);
  out[out_size] = '\0';

  return out_size;
}


/**
 * Splits a string at the first occurence of the '=' char, into two strings: left and right.
 * Trims white spaces from the begining and end of the two new trings left and right.
 * If there is no '=' mark in the given string - the right string will be empty and the left string will 
 * be the same as the given string but without spaces at the begining and the end.
 *
 * @param s - the string to split and trim
 * @param left - the string that will hold the left part after splitting and trimming
 * @param left_size  - the size of the left string
 * @param right - the string that will hold the right part after splitting and trimming
 * @param right_size  - the size of the right string
 *
 */
void splitEqualAndTrim(const char *s, char * left, int left_size, char * right, int right_size)
{
    int i = 0;
    char left_temp [left_size];
    char right_temp [right_size];
    
    while ((*(s + i)) && (*(s + i) != CONFIG_ASSIGNMENT_MARK)) {
		i++;
    }

    // handle left part of equation
    if (i==0) {
		*left = '\0';
	}
	else {
		strncpy(left_temp, s, i);
		left_temp[i] = '\0';
		trimWhitespace(left, left_size, left_temp);
	}

	// handle right part of equation
    if (i == strlen(s)) {
        *right = '\0';
    }
    else {
        strcpy(right_temp, s + i + 1);
        trimWhitespace(right, right_size, right_temp);
    }

}


/**
 * checks if a string is positive integer
 * @param n - the string to check
 * @return 1 if n is positive
 *         0 otherwise
 */
int isPositiveInteger(char * n) {
	int num ;
	if (numbersOnly(n)) {
		num = atoi(n);
		return num >= 0 ? 1 : 0;
	}
	return 0;
}

/**
 * checks if a string represent an int which is not negative (larger than 0)
 * @param n - the string to check
 * @returns 0 -  if string n represent an int which is not negative (larger than 0)
 *          1  - otherwise
 */
int isNotNegativeInteger(char * n) {
	int num ;
	if (numbersOnly(n)) {
		num = atoi(n);
		return num > 0 ? 1 : 0;
	}
	return 0;
}


/**
 * checks if a atring  is  an integer in a range (between low and high limits including them)
 * @param n - the atring to check
 * @param low - the bottom boundary
 * @param high - the top boundary
 * @return 1 if n is in the range
 *         0 otherwise
 */
int isInRange(char * n, int low, int high) {
	int num;
	if (numbersOnly(n)) {
		num = atoi(n);
		return ((num >= low) && (num <= high)) ? 1 : 0;
	}
	return 0;
}


/*
 * returns the tree_split_method enum integer value of a string ( if it is a valid enum)
 * @param c - the string to convert
 * @return the enum value of c  (0 if c is "RANDOM"
 *						    	 1 if c is "MAX_SPREAD"
 *				                 2 if c is "INCREMENTAL")
 * 		  -1 is c is not a valid tree_split_method enum
 */
int getTreeSplitMethod(char * c) {
	int i;
	for (i=0; i < SPLIT_METHOD_COUNT; i++) {
		if (strcmp(c, SPLIT_METHOD_STR(i)) == 0)
			return i;
	}
	return -1;
}


/*
 * returns the boolean value of string (if it is a valid boolean)
 * @param c - the string to check
 * @return true (1) if c is "true"
 * @return false (0) is c is "false"
 * @return -1 if c is not a valid boolean
 */
int getBoolean(char * c) {
	if (strcmp(c, "true") == 0)
		return true;

	if (strcmp(c, "false") == 0)
		return false;

	else
		return -1;
}


/*
 * checks if string is a SP_LOGGER_LEVEL
 * @param c - the string to check
 * @return 1 if c is a valid logger level
 * @return 0 otherwise
 */
int isLoggerLevel(char * c) {
	return isInRange(LOW_LOGGER_LEVEL, HIGH_LOGGER_LEVEL);
}


SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg) {
	SPConfig config;
	FILE *fp
    char line[MAX_LINE_SIZE];
    int line_number = 0;
    int parameter_found_index;
    char parameter_found[CONFIG_PARAMETERS_COUNT -1] = {0};
	
 	assert(msg != NULL);
 	*msg = SP_CONFIG_SUCCESS; // default is success

	if (filename == NULL) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return NULL;
	}

    
    /* opening file for reading */
    fp = fopen(config_filename , "r");
    if(fp == NULL) {
       *msg = SP_CONFIG_CANNOT_OPEN_FILE;
       return NULL;
    }

    if ((config = (SPConfig)malloc(sizeof(*config))) == NULL) {
    	fclose(fp);
    	*msg = SP_CONFIG_ALLOC_FAIL;
        return NULL;
    }

	while (fgets (line, sizeof(line), fp)) {
		line_number++;
		parameter_found_index = parseLine(config_filename, line, line_number, config, msg);
		if (parameter_found_index == -1) {
    		fclose(fp);	
    		spConfigDestroy(); //free config
			return NULL;
		}
		else if (parameter_found_index >= 0) {
			parameter_found[parameter_found_index] = 1;
		}
	}
	
    fclose(fp);

	if (checkMissingAndSetDefaults(config_filename, line_number, parameter_found, msg) == -1 ) {
		spConfigDestroy(); // free config
		return NULL;
	}

	*msg = SP_CONFIG_SUCCESS;
	return config;
 }

// todo remove this doc (its in the header)
/*
 * Returns true if spExtractionMode = true, false otherwise.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return true if spExtractionMode = true, false otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
bool spConfigIsExtractionMode(const SPConfig config, SP_CONFIG_MSG* msg){
	config->spExtractionMode == true
	// bool? todo finish this

}

// todo remove this doc (its in the header)
/*
 * Returns true if spMinimalGUI = true, false otherwise.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return true if spExtractionMode = true, false otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
bool spConfigMinimalGui(const SPConfig config, SP_CONFIG_MSG* msg){
	// bool? todo finish this
}


int spConfigGetNumOfImages(const SPConfig config, SP_CONFIG_MSG* msg) {
	if (config == NULL) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}

	assert(msg != NULL);
 	*msg = SP_CONFIG_SUCCESS; // default is success

 	return config->spNumOfImages;

}

int spConfigGetNumOfFeatures(const SPConfig config, SP_CONFIG_MSG* msg){
	if (config == NULL) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}

	assert(msg != NULL);
 	*msg = SP_CONFIG_SUCCESS; // default is success

 	return config->spNumOfFeatures;
}

int spConfigGetPCADim(const SPConfig config, SP_CONFIG_MSG* msg){
	if (config == NULL) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}

	assert(msg != NULL);
 	*msg = SP_CONFIG_SUCCESS; // default is success

 	return config->spPCADimension;
}

SP_CONFIG_MSG spConfigGetImagePath(char* imagePath, const SPConfig config, int index) {
	int n; //todo need?
	if ((imagePath == NULL) || (config == NULL)) {
		return SP_CONFIG_INVALID_ARGUMENT;
	}

	if (index >= config->spNumOfImages) {
		return SP_CONFIG_INDEX_OUT_OF_RANGE;
	}
	if ((n = sprintf(imagePath, "%s%s%d%s", config->spImagesDirectory, config->spImagesPrefix, index, config->spImagesSuffix)) < 0) {
		// todo what to return on error here?
	}
	return SP_CONFIG_SUCCESS;
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
	if ((n = sprintf(featuresPath, "%s%s%d%s", config->spImagesDirectory, config->spImagesPrefix, index, FEATURES_FILE_SUFFIX)) < 0) {
		// todo what to return on error here? now I can define it
	}
	return SP_CONFIG_SUCCESS;
}


SP_CONFIG_MSG spConfigGetPCAPath(char* pcaPath, const SPConfig config) {
	int n; //todo need?
	if ((pcaPath == NULL) || (config == NULL)) {
		return SP_CONFIG_INVALID_ARGUMENT;
	}
	
	if ((n = sprintf(pcaPath, "%s%s", config->spImagesDirectory, config->spPCAFilename)) < 0) {
		// todo what to return on error here? maybe just ignore?
	}
	
	return SP_CONFIG_SUCCESS;
}

// todo check this
void spConfigDestroy(SPConfig config) {
	if (config != NULL) {
		free(config->spImagesDirectory);
		free(config->spImagesPrefix);
		free(config->spImagesSuffix);
		free(config->spPCAFilename);
		free(config->spLoggerFilename);
		free(config);
	}
}


/* todo find real place in code */

#define FEATURES_FILE_SUFFIX ".feats"


/*extraction mode 
 * todo doc
 * @return SP_CONFIG_INVALID_ARGUMENT if config == NULL
 */
extractFeaturesIntoFile(const SPConfig config) {
	int i;
	char current_image_path[CONFIG_MAX_LINE_SIZE];
	char features_filename[CONFIG_MAX_LINE_SIZE];
	SP_CONFIG_MSG msg;
	int num_of_featues;
	SPPoint* features;

	if (config == NULL) {
		return SP_CONFIG_INVALID_ARGUMENT;
	}


	ImageProc(config);

	// for each image - write it's features into file
	for (i=0; i < config->spNumOfImages; i++) {	 
		msg = spConfigGetImagePath(current_image_path, config, i);
		if (msg != SP_CONFIG_SUCCESS) {
			//todo return error
		}

		msg = spConfigGetImageFeaturesFilePath(features_filename, config, i);
		if (msg != SP_CONFIG_SUCCESS) {
			//todo return error
		}
		
		features = getImageFeatures(current_image_path, i, &num_of_featues); // todo handle error

		writeImageFeaturesToFile(features, features_filename, i, num_of_featues); //todo handle error and free
	}

}

// file format - doc it
// _______
// image index (int)
// number of features (int) (n)
// feature_1 index (int)
// feature_1 dim (int)
// feature_1 coordinates (double)
// feature_2 index (int)
// feature_2 dim (int)
// feature2 coordinates (double)
// ...
// feature_n index (int)
// feature_n dim (int)
// feature_n coordinates (double)
// each line if of  maximum length as the config file)

#define FEATURES_FILE_HEADER_FORMAT "%d\n%d\n"     // the format of the header is <image index>\n<number of features>
// after the header for each feature (unit) the format is <index>\n<dim>\n<corridinates seperated with space>
#define FEATURES_FILE_UNIT_FORMAT   "%d\n%d\n"     
#define FEATURES_FILE_COOR_FORMAT   "%f "
#define FEATURES_FILE_LAST_COOR_FORMAT   "%f\n"

// todo add doc =- writes given features of specific image (of index image_index) into file
void writeImageFeaturesToFile (const SPPoint* features, char* features_filename, int image_index, int num_of_featues)

{
	int i;
	FILE *ofp;
	ofp = fopen(features_filename, "w");
	int dim;
	int axis;

    if(ofp == NULL) {
       *msg = SP_CONFIG_CANNOT_OPEN_FILE;
       return NULL;
       //todo return -1
    }

    // write header (image index and number of features) to file 
	if (fprintf(ofp, FEATURES_FILE_HEADER_FORMAT, image_index, num_of_featues) < 0) {
			//todo failure
	}

	// write each feature information into the file in the right format
	for (i = 0; i < num_of_featues; i++) {
		dim = spPointGetDimension(features[i]);
		// write dim and index
		if (fprintf(ofp, FEATURES_FILE_UNIT_FORMAT, spPointGetIndex(features[i]), dim, *features[i]->coordinates) < 0) {
			//todo failure
		}
		
		// write coordinates
		for (axis = 0 ; axis < dim -1; axis++) {
			if (fprintf(ofp, FEATURES_FILE_COOR_FORMAT, spPointGetAxisCoor(features[i], axis))< 0) {
				//todo failure
			}
		}
		
		// write last coordinate
		if (fprintf(ofp, FEATURES_FILE_LAST_COOR_FORMAT, spPointGetAxisCoor(features[i], axis))< 0) {
			//todo failure
		}
	}

	fclose(ofp);
}

//todo doc
// assume features_filename is of the format we set
//todo could features_filename be NULL?
SPPoint* extractImageFreaturesFromFile(char* features_filename ) { // todo should return those:  int image_index, int num_of_featues?

{
	SPPoint* features;
	int image_index;
	int num_of_featues;
	char line[MAX_LINE_SIZE];
	int i;
	FILE *ifp;

	// features params
	int index;
	int dim;
	double* coordinates;

	ifp = fopen(features_filename,"r");

    if(ifp == NULL) {
       *msg = SP_CONFIG_CANNOT_OPEN_FILE;
       return NULL;
       //todo return -1
    }


	// extract header
	fgets(line, sizeof(line), ifp);
	image_index = atoi(line);
	fgets(line, sizeof(line), ifp);
	num_of_featues = atoi(line);

	if ((features = (SPPoint*)malloc(sizeof(*features)*num_of_featues)) == NULL) { // todo check this is ok
		//todo error allocation
	}

	// extract each features
	for(i=0; i< num_of_featues; i++) {

		index = atoi(fgets(line, sizeof(line), ifp));
		dim = atoi(fgets(line, sizeof(line), ifp));
		coordinates = parseCoorLineFormat(fgets(line, sizeof(line), ifp), dim);//todo handle error
		features[i] = spPointCreate(coordinates, dim, index); // todo handle error

	}

	// todo free everything on all failures
	fclose(ifp);
	return features;
}

// todo doc each coordinate no more than line size (1024)
double* parseCoorLineFormat(char* line, int dim) {
	int i;
	int cur_coor = 0;
	char coor_str[MAX_LINE_SIZE];
	

	double* coordinates = (double*)malloc(dim*sizeof(double));
	if (coordinates == NULL) {
		// todo return erroe and free
	}

	// get all coordinate except last one
	while (*(line + i) != '\0') {
		// when reach to a space - parse the data before as a double
		if (isspace(*(line + i ))) {
			strncpy(coor_str, line, i);
			coor_str[i] = '\0';
			coordinates[cur_coor] = atof(coor_str);
			cur_coor++;

			// advance the pointer to after space (skip space) and start search again
			line = line + i + 1;
			i = 0;
		}
		else {
			i++;
		}
	}

	// get last coordinate
	strncpy(coor_str, line, i);
	coor_str[i] = '\0';
	coordinates[cur_coor] = atof(coor_str);

	return coordinates;
}

// todo doc
readFeaturesFromFile(const SPConfig config) {

	int i;
	char current_image_path[CONFIG_MAX_LINE_SIZE];
	SPPoint* resPoints = (SPPoint*) malloc(sizeof(*resPoints) * config->spNumOfImages);
	if (resPoints == NULL) {
		// todo handle error and free
	}

	for (i=0; i < config->spNumOfImages; i++) {	 
		msg = spConfigGetImageFeaturesFilePath(features_filename, config, i);
		if (msg != SP_CONFIG_SUCCESS) {
			//todo return error
		}
		resPoints[i] = extractImageFreaturesFromFile(features_filename);// todo handle error
	}

}