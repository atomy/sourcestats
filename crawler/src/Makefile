OUTBIN = sourcestats
OBJ = GameInfoRequest.o GameserverEntry.o GameserverInfo.o GameserverManager.o Masterquery.o MasterqueryManager.o Masterserver.o MasterserverManager.o SourceStats.o util.o main.o GameStats.o ThreadedRequest.o

default: all

mastertest: ${OBJ}
	rm -f SourceStats.o
	rm -f asquerytest.o
	g++ -ggdb -DDEBUG -o mastertest -lboost_system -lboost_thread *.o 

astest: ${OBJ}
	rm -f SourceStats.o
	rm -f masterquery.o
	g++ -ggdb -DDEBUG -o astest -lboost_system -lboost_thread *.o 

%.o: %.cpp
	g++ -c -ggdb -DDEBUG -c $<

all: ${OBJ}
	g++ -ggdb -DDEBUG -o ${OUTBIN} -pthread -lboost_system -lboost_thread *.o 

clean:
	rm -f *.o ${OUTBIN} mastertest astest 
