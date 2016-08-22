CC = gcc
CPP = g++
#put your object files here
OBJS = SPListElement.o SPList.o SPBPriorityQueue.o SPLogger.o SPPoint.o SPKDArray.o KDTreeNode.o sp_kdTreeNode_unit_test.o
#The executabel filename
EXEC = kdTreeTest
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
sp_kdTreeNode_unit_test.o: unit_test_util.h KDTreeNode.h
	$(CC) $(C_COMP_FLAG) -c $*.c
KDTreeNode.o: KDTreeNode.h KDTreeNode.c SPKDArray.h 
	$(CC) $(C_COMP_FLAG) -c $*.c
SPKDArray.o: SPKDArray.h SPKDArray.c SPPoint.h SPLogger.h SPBPriorityQueue.h
	$(CC) $(C_COMP_FLAG) -c $*.c
SPLogger.o: SPLogger.c SPLogger.h 
	$(CC) $(C_COMP_FLAG) -c $*.c
SPPoint.o: SPPoint.c SPPoint.h 
	$(CC) $(C_COMP_FLAG) -c $*.c
SPBPriorityQueue.o: SPBPriorityQueue.c SPBPriorityQueue.h SPList.h SPListElement.h
	$(CC) $(C_COMP_FLAG) -c $*.c
SPList.o: SPList.c SPList.h SPListElement.h
	$(CC) $(C_COMP_FLAG) -c $*.c
SPListElement.o: SPListElement.c SPListElement.h
	$(CC) $(C_COMP_FLAG) -c $*.c
main_aux.o: main_aux.h main_aux.c SPLogger.h KDTreeNode.h SPExtractFeatures.h SPConfig.h SPPoint.h
	$(CC) $(C_COMP_FLAG) -I$(INCLUDEPATH) -c $*.c
	


#a rule for building a simple c souorce file
#use gcc -MM SPPoint.c to see the dependencies

clean:
	rm -f $(OBJS) $(EXEC)

