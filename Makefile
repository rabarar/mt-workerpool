all: fun

fun: fun.o TaskQueue.o TaskProc.o WorkerPool.o
	g++ fun.o TaskQueue.o TaskProc.o WorkerPool.o -o fun

TaskQueue.o: TaskQueue.cpp TaskQueue.h
	g++ -g -std=c++17 -o TaskQueue.o -c TaskQueue.cpp

TaskProc.o: TaskProc.cpp TaskProc.h
	g++ -g -std=c++17 -o TaskProc.o -c TaskProc.cpp

WorkerPool.o: WorkerPool.cpp WorkerPool.h
	g++ -g -std=c++17 -o WorkerPool.o -c WorkerPool.cpp

clean:
	@rm fun *.o


