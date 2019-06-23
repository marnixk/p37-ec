CFLAGS=-Wall
TARGET=p37ec
OBJECTS=p37ec.o main.o

all: p37ec

${TARGET}: ${OBJECTS}
	gcc ${CFLAGS} ${OBJECTS} -lm -o p37ec

main.o: main.c
	gcc ${CFLAGS} main.c -lm -c 

p37ec.o: p37ec.c
	gcc ${CFLAGS} p37ec.c -lm -c


install: ${TARGET}
	sudo cp ${TARGET} /usr/local/bin

clean: 
	rm -f ${TARGET} ${OBJECTS}