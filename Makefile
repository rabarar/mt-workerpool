all: fun

fun: fun.o TaskQueue.o TaskProc.o WorkerPool.o Events.o usignal.o
	g++ fun.o TaskQueue.o TaskProc.o WorkerPool.o Events.o usignal.o -o fun

TaskQueue.o: TaskQueue.cpp TaskQueue.h
	g++ -g -std=c++17 -o TaskQueue.o -c TaskQueue.cpp

TaskProc.o: TaskProc.cpp TaskProc.h
	g++ -g -std=c++17 -o TaskProc.o -c TaskProc.cpp

WorkerPool.o: WorkerPool.cpp WorkerPool.h
	g++ -g -std=c++17 -o WorkerPool.o -c WorkerPool.cpp

Events.o: Events.cpp Events.h
	g++ -g -std=c++17 -o Events.o -c Events.cpp

usignal.o: usignal.cpp usignal.h
	g++ -g -std=c++17 -o usignal.o -c usignal.cpp

clean:
	@rm fun *.o


