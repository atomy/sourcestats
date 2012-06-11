OUT=libcursesfrontend.so
TESTOUT=main
SRC=${wildcard *.cpp}
OBJ=${SRC:.cpp=.o}
LIBOBJ=${filter-out main.o, ${OBJ}}
HEAD=${wildcard *.h}
LD_FLAGS=-lncurses -lpthread -L. -L../
CC_FLAGS=-Wall -Werror -fPIC -g -ggdb -I. -I.. -DUSE_NCURSES

all: lib test

lib: ${LIBOBJ}
	g++ ${LD_FLAGS} -g -ggdb -shared $^ -o ${OUT}

%.o: %.cpp ${HEAD} Makefile
	g++ ${CC_FLAGS} -c $<

test: main.o lib
	g++ ${LD_FLAGS} -lcursesfrontend $< -o ${TESTOUT}

clean:
	rm -rf *.o
	rm -rf ${OUT}
	rm -rf ${TESTOUT}
