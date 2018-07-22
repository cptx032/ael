CC = g++
CFLAGS = -pedantic -std=c++11 -lpthread

all: ael

ael: ael.o
	${CC} ael.o ${CFLAGS} -o ael

ael.o: ael.cpp
	${CC} ael.cpp ${CFLAGS} -c -o ael.o

clear:
	rm -rf *.o
