#pragma once

#include <queue>
#include <vector>
#include <string>
#include <pthread.h>

#include "TaskProc.h"
#include "TaskQueue.h"
#include "usignal.h"

using namespace std;

class WorkerPool;

typedef struct PoolArgs {
	string name;
	TaskQueue *src_queue;
	TaskQueue *syn_queue;
	WorkerPool *pool;
} PoolArgs_t;

static void *dispatcher(void*); 
static void *task(void*);


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
				WorkerPool(int numWorkers,PoolArgs_t *args);
				~WorkerPool();
				void wait(void);
				void shutdown();
				void lock();
				void unlock();
				void block();
				void block_to();
				void signal();
};


