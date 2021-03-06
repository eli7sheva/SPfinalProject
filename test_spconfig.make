CC = gcc
CPP = g++
#put your object files here
OBJS =  SPLogger.o   test_spconfig.o
#The executabel filename
EXEC = test_spconfig
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
test_spconfig.o: test_spconfig.c unit_test_util.h SPLogger.h 
	$(CC) $(C_COMP_FLAG) -c $*.c
SPLogger.o: SPLogger.c SPLogger.h 
	$(CC) $(C_COMP_FLAG) -c $*.c
	
#a rule for building a simple c souorce file
#use gcc -MM SPPoint.c to see the dependencies

clean:
	rm -f $(OBJS) $(EXEC)

