#
# Makefile
# Computer Networking Programing Assignments
#
#  Created by Phillip Romig on 4/3/12.
#  Copyright 2012 Colorado School of Mines. All rights reserved.
#

CXX = g++
LD = g++
CXXFLAGS = -g -std=c++11 -DBOOST_LOG_DYN_LINK
LDFLAGS = -g

#
# You should be able to add object files here without changing anything else
#
TARGET = web_server
OBJ_FILES = ${TARGET}.o connection.o request.o response.o
INC_FILES = includes.h connection.h request.h response.h

#
# Any libraries we might need.
#
LIBRARIES = -lboost_log_setup -lboost_log -lboost_thread -lboost_system  -lpthread


${TARGET}: ${OBJ_FILES}
	${LD} ${LDFLAGS} ${OBJ_FILES} -o $@ ${LIBRARIES}

%.o : %.cpp ${INC_FILES}
	${CXX} -c ${CXXFLAGS} -o $@ $<

#
# Please remember not to submit objects or binarys.
#
clean:
	rm -f core ${TARGET} ${OBJ_FILES}
	rm -r -f ./Submission

#
# This might work to create the submission tarball in the formal I asked for.
#
submit:
	rm -f core project2 ${OBJ_FILES}
	mkdir `whoami`
	mkdir Submission
	cp Makefile README.txt *.h *.cpp `whoami`
	tar cf ./Submission/`whoami`.tar `whoami`
	rm -r -f `whoami`
