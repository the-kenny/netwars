TARGET  := server
SRCS    := server.cpp main.cpp client_connection.cpp
OBJS    := ${SRCS:.cpp=.o} 
DEPS    := ${SRCS:.cpp=.dep} 
XDEPS   := $(wildcard ${DEPS}) 
ARCH 	= -arch i386
CCFLAGS =  ${ARCH} -Wall -ggdb -I.. -I../lib
LDFLAGS =  ${ARCH} -L.. -lnetwars -ltinyxml -ljson -lboost_system -lboost_filesystem -lmd5 -lbase64
LIBS    =
CC 	    = g++
DEFINES = -DTIXML_USE_TICPP=1 -DTIXML_USE_STL=1

.PHONY: all clean distclean 
all:: ${TARGET} 

ifneq (${XDEPS},) 
include ${XDEPS} 
endif 

${TARGET}: ${OBJS} 
	${CC} ${DEFINES} ${LDFLAGS} -o $@ $^ ${LIBS} 

${OBJS}: %.o: %.cpp %.dep 
	${CC} ${DEFINES} ${CCFLAGS} -o $@ -c $< 

${DEPS}: %.dep: %.cpp Makefile 
	${CC} ${DEFINES} ${CCFLAGS}  $< > $@ 

clean:: 
	rm -f *~ *.o ${TARGET} 

distclean:: clean
