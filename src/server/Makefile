TARGET  := server
SRCS    := server.cpp main.cpp client_connection.cpp
OBJS    := ${SRCS:.cpp=.o} 
DEPS    := ${SRCS:.cpp=.dep} 
XDEPS   := $(wildcard ${DEPS}) 
CCFLAGS = -Wall -ggdb -I.. -I../lib
LDFLAGS = -L.. -lnetwars -ljson -lboost_system -lboost_signals
LIBS    =
CC 	    = g++

.PHONY: all clean distclean 
all:: ${TARGET} 

ifneq (${XDEPS},) 
include ${XDEPS} 
endif 

${TARGET}: ${OBJS} 
	${CC} ${LDFLAGS} -o $@ $^ ${LIBS} 

${OBJS}: %.o: %.cpp %.dep 
	${CC} ${CCFLAGS} -o $@ -c $< 

${DEPS}: %.dep: %.cpp Makefile 
	${CC} ${CCFLAGS} -MM $< > $@ 

clean:: 
	rm -f *~ *.o ${TARGET} 

distclean:: clean
