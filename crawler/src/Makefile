OUTBIN = sourcestats
OBJ = GameserverEntry.o GameserverInfo.o Masterquery.o Masterserver.o MasterserverManager.o SourceStats.o util.o main.o GameStats.o ThreadedRequest.o ThreadFactory.o GameInfoQuery.o DebugLog.o DBProcessor.o GameStatsResult.o

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
	g++ -I/usr/include/mysql++ -I/usr/include/mysql -c -ggdb -DDEBUG -c $<

all: ${OBJ}
	g++ -ggdb -DDEBUG -o ${OUTBIN} -pthread -lmysqlpp -lboost_system -lboost_thread *.o 

clean:
	rm -f *.o ${OUTBIN} mastertest astest 
