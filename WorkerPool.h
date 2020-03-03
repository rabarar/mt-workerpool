#pragma once

#include <queue>
#include <vector>
#include <string>
#include <pthread.h>

#include "TaskProc.h"
#include "TaskQueue.h"

using namespace std;

class WorkerPool;

typedef struct PoolArgs {
	string name;
	TaskQueue *queue;
	WorkerPool *pool;
} PoolArgs_t;

class WorkerPool {
		private:
				vector<TaskArgs *> taskArgs;
				TaskProc *dispatchProc;
				int numWorkers;
				vector<TaskProc*> procTable;
				pthread_mutex_t pm;
				pthread_cond_t cv;


		public:
				int getNumWorkers();
				void delWorker();
				WorkerPool(int numWorkers, void *(*task)(void*),void *(*dispatcher)(void*), PoolArgs_t *args);
				~WorkerPool();
				void wait(void);
				void shutdown();
				void lock();
				void unlock();
				void block();
				void block_to();
				void signal();
};


