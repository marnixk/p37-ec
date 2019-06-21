CFLAGS=-Wall
TARGET=p37ec
OBJECTS=p37ec.o

all: p37ec

${TARGET}: ${OBJECTS}
	gcc ${CFLAGS} p37ec.c -lm -o p37ec

clean: 
	rm -f ${TARGET} ${OBJECTS}