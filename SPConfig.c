#define DEFUALT_PCA_DIM 				20
#define DEFAULT_PCA_FILENAME 			"pca.yml"
#define DEFAULT_NUM_OF_FEATURES 		100
#define DEFAULT_EXTRACTION_MODE 		true
#define DEFAULT_NUM_OF_SIMILAR_IMAGES   1
#define DEFAULT_KD_TREE_SPLIT 			MAX_SPREAD
#define DEFAULT_KNN 					1
#define DEFAULT_MINIMAL_GUI 			false
#define DEFAULT_LOGGER_LEVEL 			3
#define DEFAULT_LOGGER_FILENAME 		"stdout"

#define SP_IMAGES_DIRECTORY_STR 	 "spImagesDirectory"
#define SP_IMAGES_PREFIX_STR	 	 "spImagesPrefix"
#define SP_IMAGES_SUFFIX_STR 		 "spImagesSuffix"
#define SP_NUM_OF_IMAGES_STR 		 "spNumOfImages"
#define SP_PCA_DIMENSION_STR 		 "spPCADimension"
#define SP_PCA_FILENAME_STR			 "spPCAFilename"
#define SP_NUM_OF_FEATURES_STR		 "spNumOfFeatures"
#define SP_EXTRACTION_MODE_STR 			 "spExtractionMode"
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
#define CONFIG_FILE_PATH_SIZE      1024	 // the path of any file contains no more than 1024 characters
#define CONFIG_START_COMMENT_MARK '#'    // comment in config file mast start with #
#define CONFIG_ASSIGNMENT_MARK    '='

typedef enum { false, true } boolean;
typedef enum {RANDOM, MAX_SPREAD, INCREMENTAL} tree_split;
const char* OPTIONAL_SUFFIX[] =  { ".jpg" , ".png" , ".bmp" , ".gif"};
#define PCA_DIM_LOW_LIMIT 10
#define PCA_DIM_HIGH_LIMIT 28

struct sp_config_t{
	char* spImagesDirectory; //hasNoWhiteSpace
	char* spImagesPrefix; // hasNoWhiteSpace
	char* spImagesSuffix; //validSuffix
	int spNumOfImages; //isPositiveInteger
	int spPCADimension; // isInRange(spPCADimension, PCA_DIM_LOW_LIMIT, PCA_DIM_HIGH_LIMIT)
	char* spPCAFilename; //hasNoWhiteSpace
	int spNumOfFeatures; //isPositiveInteger
	boolean spExtractionMode;
	int spNumOfSimilarImages; //spNumOfSimilarImages>0
	tree_split spKDTreeSplitMethod;
	int spKNN; // spKNN > 0
	boolean spMinimalGUI;
	SP_LOGGER_LEVEL spLoggerLevel; // todo ask if using SP_LOGGER_LEVEL is ok.. thats not the ORAOY
	char * spLoggerFilename; //hasNoWhiteSpace
};

ERROR_INVALID_LINE_MSG = 
/*File: <the configuration filename>
Line: <the number of the invalid line>
Message: Invalid configuration line”
*/

ERROR_INVALID_CONSTRAINT_MSG = 
/*
“File: <the configuration filename>
Line: <the number of the invalid value>
Message: Invalid value - constraint not met”
*/

ERROR_PARMETER_NOT_SET_MSG = 
/*
“File: <the configuration filename>
Line: <the number of lines in the configuration file>
Message: Parameter <parameter name> is not set”
*/


int parseConfigFile(char * config_filename) {
	FILE *fp
    char line[MAX_LINE_SIZE];
    int line_number = 0;
    int parameter_found_index;
    char parameter_found[CONFIG_PARAMETERS_COUNT -1] = {0};
    /* opening file for reading */
    fp = fopen(config_filename , "r");
    if(fp == NULL) 
    {
       perror("Error opening file"); // TODO print to log and to screen
       return(-1);
    }

	while (fgets (line, sizeof(line), fp)) {
		line_number++;
		parameter_found_index = parseLine(config_filename, line, line_number);
		if (parameter_found_index == -1) {
			// todo this is an error
			return error
		}
		else if (parameter_found_index >= 0) {
			parameter_found[parameter_found_index] = 1;
		}
	}
	
    fclose(fp);
   
	if (checkMissingAndSetDefaults(config_filename, line_number, parameter_found) == -1) { 
		// todo this is an error
		return error
	}
    return(0);
 }


int parseLine(char * config_filename, char * line, int line_number, SPConfig config) {
 	char right[CONFIG_MAX_LINE_SIZE]; // todo might need to set to CONFIG_MAX_LINE_SIZE -1 because of trim - check it 
 	char left[CONFIG_MAX_LINE_SIZE];
 	splitEqual(line, right, CONFIG_MAX_LINE_SIZE, left, CONFIG_MAX_LINE_SIZE);

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
			ERROR_INVALID_CONSTRAINT_MSG(config_filename, line_number);
			return -1;
		}
		else {
			config->spImagesDirectory = right; // todo malloc and strcpy for every char * (not only this parameter)
 			return SP_IMAGES_DIRECTORY_INDEX;
 		}
 	}


	else if (strcmp(left, SP_IMAGES_PREFIX_STR) == 0) {
		if (!hasNoWhiteSpace(right))
			ERROR_INVALID_CONSTRAINT_MSG(config_filename, line_number);
			return -1;
		}
		else {
			config->spImagesPrefix = right;
 			return SP_IMAGES_PREFIX_INDEX;
 		}
	}

	else if (strcmp(left, SP_IMAGES_SUFFIX_STR) == 0) {
		if (!validSuffix(right)) {
			ERROR_INVALID_CONSTRAINT_MSG(config_filename, line_number);
			return -1;
		}
		else {
			config->spImagesSuffix = right;
			return SP_IMAGES_SUFFIX_INDEX;
		}
	}

	else if (strcmp(left, SP_NUM_OF_IMAGES_STR) == 0) {
		if (!isPositiveInteger(right)) {
			ERROR_INVALID_CONSTRAINT_MSG(config_filename, line_number);
			return -1;
		}
		else {
			config->spNumOfImages = atoi(right);
			return SP_NUM_OF_IMAGES_INDEX;
		}
	}

	else if (strcmp(left, SP_PCA_DIMENSION_STR) == 0) {
		if (!isInRange(right, PCA_DIM_LOW_LIMIT, PCA_DIM_HIGH_LIMIT) {
			ERROR_INVALID_CONSTRAINT_MSG(config_filename, line_number);
			return -1;
		}
		else {
			config->spPCADimension = atoi(right);
			return SP_PCA_DIMENSION_INDEX;
		}
	}

	else if (strcmp(left, SP_PCA_FILENAME_STR) == 0) {
		if (!hasNoWhiteSpace(right)) {
			ERROR_INVALID_CONSTRAINT_MSG(config_filename, line_number);
			return -1;
		}
		else {
			config->spPCAFilename = right;
			return SP_PCA_FILENAME_INDEX;
		}
	}

	else if (strcmp(left, SP_NUM_OF_FEATURES_STR) == 0) {
		if (!isPositiveInteger(right)) {
			ERROR_INVALID_CONSTRAINT_MSG(config_filename, line_number);
			return -1;
		}
		else {
			config->spNumOfFeatures = atoi(right);
			return SP_NUM_OF_FEATURES_INDEX;
		}
	}

	//todo finish this one
	else if (strcmp(left, SP_EXTRACTION_MODE_STR) == 0) {
		if (!check boolean))) {
			ERROR_INVALID_CONSTRAINT_MSG(config_filename, line_number);
			return -1;
		}
		else {
			config->spExtractionMode = right;
			return SP_EXTRACTION_MODE_INDEX;
		}
	}

	else if (strcmp(left, SP_NUM_OF_SIMILAR_IMAGES_STR) == 0) {
		if (!isNotNegativeInteger(right)) {
			ERROR_INVALID_CONSTRAINT_MSG(config_filename, line_number);
			return -1;
		}
		else {
			config->spNumOfSimilarImages = atoi(right);
			return SP_NUM_OF_SIMILAR_IMAGES_INDEX;
		}
	}

	// todo finish this one
	else if (strcmp(left, SP_KD_TREE_SPLIT_METHOD_STR) == 0) {
		if (!check tree_split) {
			ERROR_INVALID_CONSTRAINT_MSG(config_filename, line_number);
			return -1;
		}
		else {
			config->spKDTreeSplitMethod = right;
			return SP_KD_TREE_SPLIT_METHOD_INDEX;
		}
	}

	else if (strcmp(left, SP_KNN_STR) == 0) {
		if (!isNotNegativeInteger(right)) {
			ERROR_INVALID_CONSTRAINT_MSG(config_filename, line_number);
			return -1;
		}
		else {
			config->spKNN = atoi(right);
			return SP_KNN_INDEX;
		}
	}

	//todo finish this one
	else if (strcmp(left, SP_MINIMAL_GUI_STR) == 0) {
		if (! check boolean) {
			ERROR_INVALID_CONSTRAINT_MSG(config_filename, line_number);
			return -1;
		}
		else {
			config->spMinimalGUI = right;
			return SP_MINIMAL_GUI_INDEX;
		}
	}			

	//todo finish this one
	else if (strcmp(left, SP_LOGGER_LEVEL_STR) == 0) {
		if (! check SP_LOGGER_LEVEL) {
			ERROR_INVALID_CONSTRAINT_MSG(config_filename, line_number);
			return -1;
		}
		else {
			config->spLoggerLevel = right;
			return SP_LOGGER_LEVEL_INDEX;
		}
	}			

	else if (strcmp(left, SP_LOGGER_FILNAME_STR) == 0) {
		if (!hasNoWhiteSpace(right) ) {
			ERROR_INVALID_CONSTRAINT_MSG(config_filename, line_number);
			return -1;
		}
		else {
			config->spLoggerFilename = right;
			return SP_LOGGER_FILNAME_INDEX;
		}
	}			

 	else {
 		ERROR_INVALID_LINE_MSG(config_filename, line_number, line);
 		return -1;
 	}
 }

/* todo doc and check config is actually set that way of passing
return -1 on error
return 0 on success
*/
int checkMissingAndSetDefaults(char * config_filename, int num_of_lines, char * set_in_config, SPConfig config) {
	if (!set_in_config[SP_IMAGES_PREFIX_INDEX]) {
		ERROR_PARMETER_NOT_SET(config_filename, num_of_lines, SP_IMAGES_PREFIX_STR);
		return -1;
 	}
 	
 	if (!set_in_config[SP_IMAGES_SUFFIX_INDEX]) {
		ERROR_PARMETER_NOT_SET(config_filename, num_of_lines, SP_IMAGES_SUFFIX_STR);
		return -1;
 	}
 	
 	if (!set_in_config[SP_NUM_OF_IMAGES_INDEX]) {
		ERROR_PARMETER_NOT_SET(config_filename, num_of_lines, SP_NUM_OF_IMAGES_STR);
		return -1;
 	}

	if (!set_in_config[SP_IMAGES_DIRECTORY_INDEX) {
		ERROR_PARMETER_NOT_SET(config_filename, num_of_lines, SP_IMAGES_DIRECTORY_STR);
		return -1;
	}			

 	if (!set_in_config[SP_PCA_DIMENSION_INDEX]) {
		config->spPCADimension = DEFUALT_PCA_DIM;
 	}

	if (!set_in_config[SP_PCA_FILENAME_INDEX]) {
		config->spPCAFilename = DEFAULT_PCA_FILENAME;
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
		config->spLoggerFilename = DEFAULT_LOGGER_FILENAME;
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

// todo doc
int numbersOnly(const char *s)
{
    while (*s) {
        if (isdigit(*s++) == 0) return 0;
    }

    return 1;
}



// todo doc
// Stores the trimmed input string into the given output buffer, which must be
// large enough to store the result.  If it is too small, the output is
// truncated.
// @param buffer_size the length of string out
// @param out the new string - str trimmed
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


//todo doc
void splitEqual(const char *s, char * left, int left_size, char * right, int right_size)
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

//todo doc
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


SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg) {
	assert(msg != NULL);
}