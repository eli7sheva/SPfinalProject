
#define DEFAULT_CONFIG_FILENAME "spcbir.config"
#define INVALID_CMD_LINE_MSG "Invalid command line : use -c <config_filename>\n"
#define ERROR_OPENING_CONFIG_FILE_MSG "The configuration file %s couldn’t be open\n"
#define ERROR_OPENING_DEFAULT_CONFIG_FILE_MSG "The default configuration file %s couldn’t be open\n"
#define CMD_LINE_CONFIG_FILENAME_FLAG "-c"
#define ERROR_MEMORY_ALLOC_MSG "Error allocating memory\n"
#define CONFIG_FILE_PATH_SIZE      1024	 // the path of any file contains no more than 1024 characters

printf(ERROR_OPENING_DEFAULT_CONFIG_FILE_MSG, DEFAULT_CONFIG_FILENAME);
printf(ERROR_OPENING_CONFIG_FILE_MSG, config_filename);

int main(int argc, char *argv[]) {
	char * config_filename;
	SP_CONFIG_MSG msg;
	SPConfig config;

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
			printf(ERROR_OPENING_DEFAULT_CONFIG_FILE_MSG);

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
			printf(ERROR_OPENING_CONFIG_FILE_MSG);

		if (msg != SP_CONFIG_SUCCESS) {
			free(config_filename);
			free(config);
			return -1;
		}
	}


	free(config_filename);
	free(config);
	return 0;

}
