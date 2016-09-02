#include "SPConfig.h"
#include <stdlib.h>
#include <stdbool.h>
#include "unit_test_util.h"

bool test_hasNoWhiteSpace() {
	 assert (0== hasNoWhiteSpace(" D"));
	 assert (0== hasNoWhiteSpace("d D"));
	 assert (0== hasNoWhiteSpace("D "));
	 assert (0== hasNoWhiteSpace(" "));
	 assert (1== hasNoWhiteSpace("dD"));
	 assert (1== hasNoWhiteSpace(""));
	 return true;
}


bool test_validSuffix() {
	assert(validSuffix(".gif") == 1);
	assert(validSuffix(".jpg") == 1);
	assert(validSuffix(".png") == 1);
	assert(validSuffix(".bmp") == 1);
	assert(validSuffix(".bmpf") == 0);
	assert(validSuffix("f.bmp") == 0);
	assert(validSuffix("") == 0);
	return true;
}


bool test_isInteger(){
	assert(1== isInteger("1"));
	assert(1== isInteger("+1"));
	assert(1== isInteger("-1"));
	assert(0== isInteger("1213+675"));
	assert(0== isInteger("1."));
	assert(0== isInteger("1as"));
	assert(0== isInteger("+s"));
	assert(0== isInteger("1.0"));
	return true;
}

bool test_isPositiveInteger() {
	assert(1== isPositiveInteger("12"));
	assert(1== isPositiveInteger("0"));
	assert(1== isPositiveInteger("+3"));
	assert(0== isPositiveInteger("+"));
	assert(0== isPositiveInteger("-"));
	assert(0== isPositiveInteger("-2"));
	assert(0== isPositiveInteger("1.2"));
	assert(0== isPositiveInteger("1.0"));
	assert(0== isPositiveInteger("s"));
	return true;
}


bool tewst_isNotNegativeInteger() {
	assert(1== isNotNegativeInteger("12"));
	assert(0== isNotNegativeInteger("0"));
	assert(1== isNotNegativeInteger("+3"));
	assert(0== isNotNegativeInteger("+"));
	assert(0== isNotNegativeInteger("-"));
	assert(0== isNotNegativeInteger("-2"));
	assert(0== isNotNegativeInteger("1.2"));
	assert(0== isNotNegativeInteger("s"));
	return true;
}


bool test_isInRange() {
	assert(1==isInRange("1",0,2));
	assert(1==isInRange("1",1,1));
	assert(1==isInRange("1",1,1));
	assert(1==isInRange("-1",-1,1));
	assert(0==isInRange("1.1",0,2));
	assert(0==isInRange("-1",0,2));
	assert(0==isInRange("+3",0,2));
	assert(0==isInRange("1.0",0,2));
	return true;
}

bool test_getTreeSplitMethod() {
	assert(0==getTreeSplitMethod("RANDOM"));
	assert(1==getTreeSplitMethod("MAX_SPREAD"));
	assert(2==getTreeSplitMethod("INCREMENTAL"));
	assert(-1==getTreeSplitMethod("MAX_SPREAd"));
	assert(-1==getTreeSplitMethod("MAX_SPR"));
	assert(-1==getTreeSplitMethod("SPLIT_METHOD_COUNT"));
	return true;
}

bool test_isBoolean() {
	assert(1==isBoolean("true"));
	assert(0==isBoolean("false"));
	assert(-1==isBoolean("fdalse"));
	assert(-1==isBoolean(""));
	return true;
}

bool test_getBoolean() {
	assert(true == getBoolean("true"));
	assert(false == getBoolean("false"));
	return true;
}

bool isLoggerLevel(){
	assert(isLoggerLevel("1")==1);
	assert(isLoggerLevel("2")==1);
	assert(isLoggerLevel("4")==1);
	assert(isLoggerLevel("3")==1);
	assert(isLoggerLevel("ERRORR")==0);
	assert(isLoggerLevel("5")==0);
	return true;
}



bool test_trimWhitespace(){
	char* left_temp  = " aa";
    trimWhitespace(left, 1024, left_temp);
	assert(strcmp(left, "aa") == 0);
    left_temp = " ";
	assert(strcmp(left, "") == 0);
    left_temp = " s s ";
	assert(strcmp(left, "s s") == 0);
    left_temp = "ss";
	assert(strcmp(left, "ss") == 0);
    left_temp = "";
	assert(strcmp(left, "") == 0);
	return true;
}

bool test_splitEqualAndTrim(){
	char right[1024];
    char left[1024];
    char * line;
    line = "asd";
    splitEqualAndTrim(line, left, 1024, right, 1024);
    assert(strcmp(left, "asd")==0);
    assert(strcmp(right, "")==0);
    

    line = " asd=";
    splitEqualAndTrim(line, left, 1024, right, 1024);
    assert(strcmp(left, "asd")==0);
    assert(strcmp(right, "")==0);
    
    line = "= asd";
    splitEqualAndTrim(line, left, 1024, right, 1024);
    assert(strcmp(left, "")==0);
    assert(strcmp(right, "asd")==0);
    
    line = "as= d";
    splitEqualAndTrim(line, left, 1024, right, 1024);
    assert(strcmp(left, "as")==0);
    assert(strcmp(right, "d")==0);
    
    line = "=";
    splitEqualAndTrim(line, left, 1024, right, 1024);
    assert(strcmp(left, "")==0);
    assert(strcmp(right, "")==0);
    
    line = " = ";
    splitEqualAndTrim(line, left, 1024, right, 1024);
    assert(strcmp(left, "")==0);
    assert(strcmp(right, "")==0);
    
	return true;
}


bool test_parseLine_and_checkMissingAndSetDefaults(){
	SP_CONFIG_MSG msg;
	SPConfig config;
	char parameter_found[CONFIG_PARAMETERS_COUNT -1] = {0};
    if ((config = (SPConfig)malloc(sizeof(*config))) == NULL) {
    	msg = SP_CONFIG_ALLOC_FAIL;
    	printf("malloc error\n");
    	return NULL;
    }


 	assert(-1 == checkMissingAndSetDefaults("somefilename", 14, config, parameter_found, &msg));
	assert(msg == SP_CONFIG_MISSING_DIR);

    assert(-2==parseLine("somefilename", "", 1, config, &msg));
    assert(msg == SP_CONFIG_SUCCESS);
    assert(-2==parseLine("somefilename", "asdf", 1, config, &msg));
    assert(msg == SP_CONFIG_SUCCESS);
    assert(-2==parseLine("somefilename", " ", 1, config, &msg));
    assert(msg == SP_CONFIG_SUCCESS);
    assert(-2==parseLine("somefilename", " #rer", 1, config, &msg));
    assert(msg == SP_CONFIG_SUCCESS);
    assert(-2==parseLine("somefilename", "#", 1, config, &msg));
    assert(msg == SP_CONFIG_SUCCESS);
    
    assert(-1==parseLine("somefilename", "spImagesDirectory", 1, config, &msg));
    assert(msg == SP_CONFIG_INVALID_STRING);
    assert(-1==parseLine("somefilename", "spImagesDirectory =", 1, config, &msg));
    assert(msg == SP_CONFIG_INVALID_STRING);
    assert(-1==parseLine("somefilename", "spImagesDirectory =as d", 1, config, &msg));
    assert(msg == SP_CONFIG_INVALID_STRING);
    assert(SP_IMAGES_DIRECTORY_INDEX==parseLine("somefilename", "spImagesDirectory= /sads", 1, config, &msg));
    assert(msg == SP_CONFIG_SUCCESS);
    parameter_found[SP_IMAGES_DIRECTORY_INDEX] = 1;

	assert(-1 == checkMissingAndSetDefaults("somefilename", 14, config, parameter_found, &msg));
	assert(msg == SP_CONFIG_MISSING_PREFIX);
	    
	assert(-1==parseLine("somefilename", "spImagesPrefix", 1, config, &msg));
    assert(msg == SP_CONFIG_INVALID_STRING);
    assert(-1==parseLine("somefilename", " spImagesPrefix =", 1, config, &msg));
    assert(msg == SP_CONFIG_INVALID_STRING);
    assert(SP_IMAGES_PREFIX_INDEX==parseLine("somefilename", "spImagesPrefix= reut", 1, config, &msg));
    assert(msg == SP_CONFIG_SUCCESS);
    assert(-1==parseLine("somefilename", " spImagesPrefix =d d", 1, config, &msg));
    assert(msg == SP_CONFIG_INVALID_STRING);
    assert(SP_IMAGES_PREFIX_INDEX==parseLine("somefilename", "spImagesPrefix= reut ", 1, config, &msg));
    assert(msg == SP_CONFIG_SUCCESS);
    parameter_found[SP_IMAGES_PREFIX_INDEX] = 1;

	assert(-1 == checkMissingAndSetDefaults("somefilename", 14, config, parameter_found, &msg));
	assert(msg == SP_CONFIG_MISSING_SUFFIX);
	
    assert(-1==parseLine("somefilename", "spImagesSuffix= reut", 1, config, &msg));
    assert(msg == SP_CONFIG_INVALID_STRING);
    assert(-1==parseLine("somefilename", " spImagesSuffix =dd", 1, config, &msg));
    assert(msg == SP_CONFIG_INVALID_STRING);
    assert(SP_IMAGES_SUFFIX_INDEX==parseLine("somefilename", "spImagesSuffix= .png ", 1, config, &msg));
    assert(msg == SP_CONFIG_SUCCESS);
    parameter_found[SP_IMAGES_SUFFIX_INDEX] = 1;

	assert(-1 == checkMissingAndSetDefaults("somefilename", 14, config, parameter_found, &msg));
	assert(msg == SP_CONFIG_MISSING_NUM_IMAGES);

    assert(-1==parseLine("somefilename", "spNumOfImages= -2", 1, config, &msg));
    assert(msg == SP_CONFIG_INVALID_INTEGER);
    assert(-1==parseLine("somefilename", " spNumOfImages =2.0", 1, config, &msg));
    assert(msg == SP_CONFIG_INVALID_INTEGER);
    assert(SP_NUM_OF_IMAGES_INDEX==parseLine("somefilename", "spNumOfImages= 2 ", 1, config, &msg));
    assert(msg == SP_CONFIG_SUCCESS);
    assert(SP_NUM_OF_IMAGES_INDEX==parseLine("somefilename", "spNumOfImages=+2 ", 1, config, &msg));
    assert(msg == SP_CONFIG_SUCCESS);
    parameter_found[SP_NUM_OF_IMAGES_INDEX] = 1;

	assert(0 == checkMissingAndSetDefaults("somefilename", 14, config, parameter_found, &msg));
	assert(msg == SP_CONFIG_SUCCESS);

    assert(-1==parseLine("somefilename", "spPCADimension= -2", 1, config, &msg));
    assert(msg == SP_CONFIG_INVALID_INTEGER);
    assert(-1==parseLine("somefilename", " spPCADimension =29", 1, config, &msg));
    assert(msg == SP_CONFIG_INVALID_INTEGER);
    assert(SP_PCA_DIMENSION_INDEX==parseLine("somefilename", "spPCADimension= 12 ", 1, config, &msg));
    assert(msg == SP_CONFIG_SUCCESS);
    assert(SP_PCA_DIMENSION_INDEX==parseLine("somefilename", "spPCADimension=+28 ", 1, config, &msg));
    assert(msg == SP_CONFIG_SUCCESS);
    parameter_found[SP_PCA_DIMENSION_INDEX] = 1;

	assert(0 == checkMissingAndSetDefaults("somefilename", 14, config, parameter_found, &msg));
	assert(msg == SP_CONFIG_SUCCESS);

    assert(-1==parseLine("somefilename", "spPCAFilename:= sd", 1, config, &msg));
    assert(msg == SP_CONFIG_INVALID_STRING);
    assert(-1==parseLine("somefilename", " spPCAFilename =s d", 1, config, &msg));
    assert(msg == SP_CONFIG_INVALID_STRING);
    assert(SP_PCA_FILENAME_INDEX==parseLine("somefilename", "spPCAFilename= s ", 1, config, &msg));
    assert(msg == SP_CONFIG_SUCCESS);
    assert(SP_PCA_FILENAME_INDEX==parseLine("somefilename", "spPCAFilename=d8 ", 1, config, &msg));
    assert(msg == SP_CONFIG_SUCCESS);
    parameter_found[SP_PCA_FILENAME_INDEX] = 1;

	assert(0 == checkMissingAndSetDefaults("somefilename", 14, config, parameter_found, &msg));
	assert(msg == SP_CONFIG_SUCCESS);

    assert(-1==parseLine("somefilename", "spNumOfFeatures= 1.2", 1, config, &msg));
    assert(msg == SP_CONFIG_INVALID_INTEGER);
    assert(-1==parseLine("somefilename", " spNumOfFeatures =d", 1, config, &msg));
    assert(msg == SP_CONFIG_INVALID_INTEGER);
    assert(-1==parseLine("somefilename", " spNumOfFeatures =-1", 1, config, &msg));
    assert(msg == SP_CONFIG_INVALID_INTEGER);
    assert(SP_NUM_OF_FEATURES_INDEX==parseLine("somefilename", "spNumOfFeatures= +1 ", 1, config, &msg));
    assert(msg == SP_CONFIG_SUCCESS);
    assert(SP_NUM_OF_FEATURES_INDEX==parseLine("somefilename", "spNumOfFeatures=8 ", 1, config, &msg));
    assert(msg == SP_CONFIG_SUCCESS);
    parameter_found[SP_NUM_OF_FEATURES_INDEX] = 1;

	assert(0 == checkMissingAndSetDefaults("somefilename", 14, config, parameter_found, &msg));
	assert(msg == SP_CONFIG_SUCCESS);

    assert(-1==parseLine("somefilename", "spExtractionMode= 1.2", 1, config, &msg));
    assert(msg == SP_CONFIG_INVALID_STRING);
    assert(-1==parseLine("somefilename", " spExtractionMode =d", 1, config, &msg));
    assert(msg == SP_CONFIG_INVALID_STRING);
    assert(-1==parseLine("somefilename", " spExtractionMode =-1", 1, config, &msg));
    assert(msg == SP_CONFIG_INVALID_STRING );
    assert(SP_EXTRACTION_MODE_INDEX==parseLine("somefilename", "spExtractionMode= true ", 1, config, &msg));
    assert(msg == SP_CONFIG_SUCCESS);
    assert(SP_EXTRACTION_MODE_INDEX==parseLine("somefilename", "spExtractionMode=false", 1, config, &msg));
    assert(msg == SP_CONFIG_SUCCESS);
    parameter_found[SP_EXTRACTION_MODE_INDEX] = 1;

	assert(0 == checkMissingAndSetDefaults("somefilename", 14, config, parameter_found, &msg));
	assert(msg == SP_CONFIG_SUCCESS);

    assert(-1==parseLine("somefilename", "spNumOfSimilarImages= -1.2", 1, config, &msg));
    assert(msg == SP_CONFIG_INVALID_INTEGER);
    assert(-1==parseLine("somefilename", " spNumOfSimilarImages =0", 1, config, &msg));
    assert(msg == SP_CONFIG_INVALID_INTEGER);
    assert(-1==parseLine("somefilename", " spNumOfSimilarImages =d1", 1, config, &msg));
    assert(msg == SP_CONFIG_INVALID_INTEGER);
    assert(SP_NUM_OF_SIMILAR_IMAGES_INDEX==parseLine("somefilename", "spNumOfSimilarImages= 1 ", 1, config, &msg));
    assert(msg == SP_CONFIG_SUCCESS);
    assert(SP_NUM_OF_SIMILAR_IMAGES_INDEX==parseLine("somefilename", "spNumOfSimilarImages=2023 ", 1, config, &msg));
    assert(msg == SP_CONFIG_SUCCESS);
    parameter_found[SP_NUM_OF_SIMILAR_IMAGES_INDEX] = 1;

	assert(0 == checkMissingAndSetDefaults("somefilename", 14, config, parameter_found, &msg));
	assert(msg == SP_CONFIG_SUCCESS);

    assert(-1==parseLine("somefilename", "spKDTreeSplitMethod= 1.2", 1, config, &msg));
    assert(msg == SP_CONFIG_INVALID_STRING);
    assert(-1==parseLine("somefilename", " spKDTreeSplitMethod =d", 1, config, &msg));
    assert(msg == SP_CONFIG_INVALID_STRING);
    assert(-1==parseLine("somefilename", " spKDTreeSplitMethod =-1", 1, config, &msg));
    assert(msg == SP_CONFIG_INVALID_STRING );
    assert(SP_KD_TREE_SPLIT_METHOD_INDEX==parseLine("somefilename", "spKDTreeSplitMethod= RANDOM ", 1, config, &msg));
    assert(msg == SP_CONFIG_SUCCESS);
    assert(SP_KD_TREE_SPLIT_METHOD_INDEX==parseLine("somefilename", "spKDTreeSplitMethod=MAX_SPREAD", 1, config, &msg));
    assert(msg == SP_CONFIG_SUCCESS);
    assert(SP_KD_TREE_SPLIT_METHOD_INDEX==parseLine("somefilename", "spKDTreeSplitMethod=INCREMENTAL", 1, config, &msg));
    assert(msg == SP_CONFIG_SUCCESS);
    parameter_found[SP_KD_TREE_SPLIT_METHOD_INDEX] = 1;

	assert(0 == checkMissingAndSetDefaults("somefilename", 14, config, parameter_found, &msg));
	assert(msg == SP_CONFIG_SUCCESS);

    assert(-1==parseLine("somefilename", "spKNN= -1.2", 1, config, &msg));
    assert(msg == SP_CONFIG_INVALID_INTEGER);
    assert(-1==parseLine("somefilename", " spKNN =0", 1, config, &msg));
    assert(msg == SP_CONFIG_INVALID_INTEGER);
    assert(-1==parseLine("somefilename", " spKNN =d1", 1, config, &msg));
    assert(msg == SP_CONFIG_INVALID_INTEGER);
    assert(SP_KNN_INDEX==parseLine("somefilename", "spKNN= 1 ", 1, config, &msg));
    assert(msg == SP_CONFIG_SUCCESS);
    assert(SP_KNN_INDEX==parseLine("somefilename", "spKNN=2023 ", 1, config, &msg));
    assert(msg == SP_CONFIG_SUCCESS);
    parameter_found[SP_KNN_INDEX] = 1;

	assert(0 == checkMissingAndSetDefaults("somefilename", 14, config, parameter_found, &msg));
	assert(msg == SP_CONFIG_SUCCESS);

    assert(-1==parseLine("somefilename", "spMinimalGUI= 1.2", 1, config, &msg));
    assert(msg == SP_CONFIG_INVALID_STRING);
    assert(-1==parseLine("somefilename", " spMinimalGUI =d", 1, config, &msg));
    assert(msg == SP_CONFIG_INVALID_STRING);
    assert(-1==parseLine("somefilename", " spMinimalGUI =-1", 1, config, &msg));
    assert(msg == SP_CONFIG_INVALID_STRING );
    assert(SP_MINIMAL_GUI_INDEX==parseLine("somefilename", "spMinimalGUI= true ", 1, config, &msg));
    assert(msg == SP_CONFIG_SUCCESS);
    assert(SP_MINIMAL_GUI_INDEX==parseLine("somefilename", "spMinimalGUI=false", 1, config, &msg));
    assert(msg == SP_CONFIG_SUCCESS);
    parameter_found[SP_MINIMAL_GUI_INDEX] = 1;

	assert(0 == checkMissingAndSetDefaults("somefilename", 14, config, parameter_found, &msg));
	assert(msg == SP_CONFIG_SUCCESS);

    assert(-1==parseLine("somefilename", "spLoggerFilename= sd", 1, config, &msg));
    assert(msg == SP_CONFIG_INVALID_STRING);
    assert(-1==parseLine("somefilename", " spLoggerFilename =s d", 1, config, &msg));
    assert(msg == SP_CONFIG_INVALID_STRING);
    assert(SP_LOGGER_FILNAME_INDEX==parseLine("somefilename", "spLoggerFilename= s ", 1, config, &msg));
    assert(msg == SP_CONFIG_SUCCESS);
    assert(SP_LOGGER_FILNAME_INDEX==parseLine("somefilename", "spLoggerFilename=d8 ", 1, config, &msg));
    assert(msg == SP_CONFIG_SUCCESS);
    parameter_found[SP_LOGGER_FILNAME_INDEX] = 1;

	assert(0 == checkMissingAndSetDefaults("somefilename", 14, config, parameter_found, &msg));
	assert(msg == SP_CONFIG_SUCCESS);

    assert(-1==parseLine("somefilename", "spLoggerLevel= -2", 1, config, &msg));
    assert(msg == SP_CONFIG_INVALID_INTEGER);
    assert(-1==parseLine("somefilename", " spLoggerLevel =2.0", 1, config, &msg));
    assert(msg == SP_CONFIG_INVALID_INTEGER);
    assert(SP_LOGGER_LEVEL_INDEX==parseLine("somefilename", "spLoggerLevel=2 ", 1, config, &msg));
    assert(msg == SP_CONFIG_SUCCESS);
    assert(SP_LOGGER_LEVEL_INDEX==parseLine("somefilename", "spLoggerLevel= 3 ", 1, config, &msg));
    assert(msg == SP_CONFIG_SUCCESS);
    assert(SP_LOGGER_LEVEL_INDEX==parseLine("somefilename", "spLoggerLevel=4 ", 1, config, &msg));
    assert(msg == SP_CONFIG_SUCCESS);
    assert(SP_LOGGER_LEVEL_INDEX==parseLine("somefilename", "spLoggerLevel=1 ", 1, config, &msg));
    assert(msg == SP_CONFIG_SUCCESS);
    parameter_found[SP_LOGGER_LEVEL_INDEX] = 1;

	assert(0 == checkMissingAndSetDefaults("somefilename", 14, config, parameter_found, &msg));
	assert(msg == SP_CONFIG_SUCCESS);

	spConfigDestroy(config);
	return true;
}


bool test_spConfigCreate(){
	char imagePath[1024];
	char * config_filename = "test_spconfig.config";
	SP_CONFIG_MSG msg;
	SPConfig config =  spConfigCreate(config_filename, &msg);

	assert(spConfigIsExtractionMode(config, &msg) == true);
	assert(msg == SP_CONFIG_SUCCESS);	

	assert(spConfigMinimalGui(config, &msg) == false);
	assert(msg == SP_CONFIG_SUCCESS);

	assert(spConfigGetNumOfImages(config, &msg) == 100);
	assert(msg == SP_CONFIG_SUCCESS);	

	assert(spConfigGetNumOfFeatures(config, &msg) == 101);
	assert(msg == SP_CONFIG_SUCCESS);	

	assert(spConfigGetPCADim(config, &msg) == 28);
	assert(msg == SP_CONFIG_SUCCESS);	

	assert(spConfigGetImagePath(imagePath, config, 3) == SP_CONFIG_SUCCESS);
	assert(strcmp(imagePath, "./test1images/img3.jpg") == 0);	

	assert(spConfigGetImagePath(imagePath, config, 30) == SP_CONFIG_SUCCESS);
	assert(strcmp(imagePath, "./test1images/img30.jpg") == 0);	

	assert(spConfigGetPCAPath(imagePath, config) == SP_CONFIG_SUCCESS);
	assert(strcmp(imagePath, "./test1images/pca.yml") == 0);	// default filename


	assert(spConfigGetLoggerLevel(imagePath, config) == DEFAULT_LOGGER_LEVEL);
	assert(msg == SP_CONFIG_SUCCESS);	

	assert(spConfigGetKNN(imagePath, config) == 10);
	assert(msg == SP_CONFIG_SUCCESS);	


	assert(spConfigGetKDTreeSplitMethod(imagePath, config) == 0);
	assert(msg == SP_CONFIG_SUCCESS);	
	
	assert(spConfigGetNumOfSimilarImages(imagePath, config) == 5);
	assert(msg == SP_CONFIG_SUCCESS);	
	
	assert(spConfigGetLoggerFileName(imagePath, config) == SP_CONFIG_SUCCESS);
	assert(strcmp(imagePath, "test1.log") == 0);

	spConfigDestroy(config);
	return true;

}

int main() {

	RUN_TEST(test_hasNoWhiteSpace);
	RUN_TEST(test_validSuffix);
	RUN_TEST(test_isInteger);
	RUN_TEST(test_isPositiveInteger);
	RUN_TEST(tewst_isNotNegativeInteger);
	RUN_TEST(test_isInRange);
	RUN_TEST(test_getTreeSplitMethod);
	RUN_TEST(test_isBoolean);
	RUN_TEST(test_getBoolean);
	RUN_TEST(isLoggerLevel);
	RUN_TEST(test_trimWhitespace);
	RUN_TEST(test_splitEqualAndTrim);
	RUN_TEST(test_parseLine_and_checkMissingAndSetDefaults);
	RUN_TEST(test_spConfigCreate);
return 0;
}


