CC = gcc
CPP = g++
#put your object files here
OBJS = SPLogger.o SPPoint.o SPConfig.o  SPImageProc.o SPExtractFeatures.o SPKDArray.o KDTreeNode.o main_aux.o main.o
#The executabel filename
EXEC = SPCBIR
INCLUDEPATH=/usr/local/lib/opencv-3.1.0/include/
LIBPATH=/usr/local/lib/opencv-3.1.0/lib/
LIBS=-lopencv_xfeatures2d -lopencv_features2d \
-lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_core


CPP_COMP_FLAG = -std=c++11 -Wall -Wextra \
-Werror -pedantic-errors -DNDEBUG

C_COMP_FLAG = -std=c99 -Wall -Wextra \
-Werror -pedantic-errors -DNDEBUG

$(EXEC): $(OBJS)
	$(CPP) $(OBJS) -L$(LIBPATH) $(LIBS) -o $@
main.o: main.cpp SPPoint.h SPLogger.h SPConfig.h SPExtractFeatures.h SPImageProc.h main_aux.h KDTreeNode.h #put dependencies here!
	$(CPP) $(CPP_COMP_FLAG) -I$(INCLUDEPATH) -c $*.cpp
SPImageProc.o: SPImageProc.cpp SPImageProc.h SPConfig.h SPPoint.h SPLogger.h
	$(CPP) $(CPP_COMP_FLAG) -I$(INCLUDEPATH) -c $*.cpp
SPConfig.o: SPConfig.c SPConfig.h SPLogger.h
	$(CC) $(C_COMP_FLAG) -c $*.c	
SPExtractFeatures.o: SPExtractFeatures.c SPExtractFeatures.h SPLogger.h SPPoint.h SPConfig.h
	$(CC) $(C_COMP_FLAG) -c $*.c
KDTreeNode.o: KDTreeNode.h KDTreeNode.c SPKDArray.h 
	$(CC) $(C_COMP_FLAG) -c $*.c
SPKDArray.o: SPKDArray.h SPKDArray.c SPPoint.h
	$(CC) $(C_COMP_FLAG) -c $*.c
SPLogger.o: SPLogger.c SPLogger.h 
	$(CC) $(C_COMP_FLAG) -c $*.c
SPPoint.o: SPPoint.c SPPoint.h 
	$(CC) $(C_COMP_FLAG) -c $*.c
main_aux.o: main_aux.h main_aux.c SPLogger.h KDTreeNode.h #put dependencies here!
	$(CC) $(C_COMP_FLAG) -I$(INCLUDEPATH) -c $*.c


#a rule for building a simple c souorce file
#use gcc -MM SPPoint.c to see the dependencies

#add here you .o files

## todo should add if use queue should also add in more places..
# SPBPriorityQueue.o: SPBPriorityQueue.c SPBPriorityQueue.h SPList.h SPListElement.h
# 	$(CC) $(COMP_FLAG) -c $*.c
# SPList.o: SPList.c SPList.h SPListElement.h
# 	$(CC) $(COMP_FLAG) -c $*.c
# SPListElement.o: SPListElement.c SPListElement.h
# 	$(CC) $(COMP_FLAG) -c $*.c

clean:
	rm -f $(OBJS) $(EXEC)

