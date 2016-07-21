
#define DEFAULT_CONFIG_FILENAME "spcbir.config"
#define INVALID_CMD_LINE_MSG "Invalid command line : use -c <config_filename>\n"
#define ERROR_OPENING_CONFIG_FILE_MSG "The configuration file %s couldn’t be open\n"
#define ERROR_OPENING_DEFAULT_CONFIG_FILE_MSG "The default configuration file %s couldn’t be open\n"
#define CMD_LINE_CONFIG_FILENAME_FLAG "-c"
#define ERROR_MEMORY_ALLOC_MSG "Error allocating memory\n"
#define CONFIG_FILE_PATH_SIZE      1024	 // the path of any file contains no more than 1024 characters


#define ENTER_AN_IMAGE_MSG "Please enter an image path:\n"


int main(int argc, char *argv[]) {
	char * config_filename;
	char* image_path[CONFIG_FILE_PATH_SIZE];
	SP_CONFIG_MSG msg;
	SPConfig config;
	bool extraction_mode;

	// cmd line arguments are ok if there was no arguments specified (argc ==1) or two arguments specified ( -c and filname)
	if (argc != 3 && argc != 1) {
		printf(INVALID_CMD_LINE_MSG);
		return -1;
	}

	// alloc config filename parameter
	if ((config_filename = (char*)malloc(CONFIG_FILE_PATH_SIZE*sizeof(char))) == NULL) {
		printf(ERROR_MEMORY_ALLOC_MSG);
		return -1;
	}

	if (argc == 1) {
		strcpy(config_filename, DEFAULT_CONFIG_FILENAME);
		config = spConfigCreate(config_filename, &msg);
		if (strcmp(msg, SP_CONFIG_CANNOT_OPEN_FILE) == 0)
			printf(ERROR_OPENING_DEFAULT_CONFIG_FILE_MSG, DEFAULT_CONFIG_FILENAME);

		if (msg != SP_CONFIG_SUCCESS) {
			free(config_filename);
			free(config);
			return -1;
		}
	}
	else { // argc == 3

		// check that second argument is the -c flag
		if (strcmp(argv[1], CMD_LINE_CONFIG_FILENAME_FLAG) != 0) {
			printf(INVALID_CMD_LINE_MSG);
			free(config_filename);
			return -1;
		}
	
		strcpy(config_filename, argv[2]);
		config = spConfigCreate(config_filename, &msg); 
		if (strcmp(msg, SP_CONFIG_CANNOT_OPEN_FILE) == 0) 
			printf(ERROR_OPENING_CONFIG_FILE_MSG, config_filename);

		if (msg != SP_CONFIG_SUCCESS) {
			free(config_filename);
			free(config);
			return -1;
		}
	}

	extraction_mode = spConfigIsExtractionMode(config, msg);
	if (msg != SP_CONFIG_SUCCESS) {
		//todo free and return error
	}
	if (extraction_mode)
		extractFeaturesIntoFile(config);

	initDataStructures(config);

	printf(ENTER_AN_IMAGE_MSG);
	fflush(NULL);
	scanf("%s",image_path);

	// 1. find features of image_path
	
	// 2. for each feature fi from (1)
			// find spKNN closest features (using k-nearest-neighbor(fi, config->spKNN))
			// (feature_i ----> spKNN_closest_features_i)

	//3. For each image img_i in spImagesDirectory find  number of times f_g is among the spKNN closest features (from 2):
		//for each image img_i in spImagesDirectory 	
			// for each feature f_g of img_i
				// counter_g  = 0
				// for each feature f_k of image_path
					// for each close feature f_c in spKNN_closest_features_k
						// if (features_equal (f_g, f_c))
							// counter_g ++
				// ranks[img_i][f_g] = counter_g;
			// sort(ranks[img_i])

	//4.  find images with  spNumOfSimilarImages highest ranks[img_i]
		// for each image img_i in spImagesDirectory

	//you should have spNumOfSimilarImages indexes

	free(config_filename);
	free(config);
	return 0;
}
