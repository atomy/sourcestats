OUTFILE=main
SRC=$(wildcard *.cpp)
OBJ=${SRC:.cpp=.o}
OBJS=${filter-out testmain.o, ${OBJ}}
OBJS_TEST=${filter-out main.o, ${OBJ}}
HEADS=${wildcard *.h}
LD_FLAGS=-lncurses -lpthread -lcursesfrontend -ltaskforce -lmysqlpp -L../libtaskforce -L../libcursesfrontend
CC_FLAGS=-Wall -fPIC -g -ggdb -DUSE_NCURSES -I.. -I. -I../libtaskforce -I../libcursesfrontend -I/usr/include/mysql++ -I/usr/include/mysql

all: main

main: ${OBJS}
	g++ ${LD_FLAGS} $^ -o ${OUTFILE}

maintest: ${OBJS_TEST}
	g++ ${LD_FLAGS} $^ -o maintest

%.o: %.cpp ${HEADS} Makefile
	g++ ${CC_FLAGS} -c $<

clean:
	rm -rf *.o
	rm -rf ${OUTFILE}