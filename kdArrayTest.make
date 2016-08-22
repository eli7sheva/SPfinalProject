CC = gcc
CPP = g++
#put your object files here
OBJS =  SPPoint.o SPLogger.o SPKDArray.o sp_kdArray_unit_test.o
#The executabel filename
EXEC = kdArray_unit_test
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
sp_kdArray_unit_test.o: sp_kdArray_unit_test.c unit_test_util.h SPKDArray.h 
	$(CC) $(C_COMP_FLAG) -c $*.c
SPKDArray.o: SPKDArray.h SPKDArray.c SPPoint.h SPLogger.h SPBPriorityQueue.h
	$(CC) $(C_COMP_FLAG) -c $*.c
SPLogger.o: SPLogger.c SPLogger.h 
	$(CC) $(C_COMP_FLAG) -c $*.c
SPPoint.o: SPPoint.c SPPoint.h 
	$(CC) $(C_COMP_FLAG) -c $*.c
	
#a rule for building a simple c souorce file
#use gcc -MM SPPoint.c to see the dependencies

clean:
	rm -f $(OBJS) $(EXEC)

