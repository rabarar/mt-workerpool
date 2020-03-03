#include <sys/time.h>

#include "WorkerPool.h"

WorkerPool::
WorkerPool(int numWorkers, void *(*task)(void*), void *(*dispatcher)(void *), PoolArgs_t *args)
{

		this->numWorkers = numWorkers;
		pthread_mutex_init(&pm, NULL);
		pthread_cond_init(&cv, NULL);


		TaskArgs_t * dispatchArgs = new TaskArgs_t();

		args->pool = this;
	   	dispatchArgs->args = args;
		dispatchArgs->name = "dispatcher";

		// start the workers
		for (int p=0; p<numWorkers; p++) {
			TaskArgs_t *targs = new TaskArgs_t();
			
			// keep track so we can delete later
			taskArgs.push_back(targs);

			targs->args = args;

			string name = "proc: ";
			TaskProc* proc = new  TaskProc(name.append(to_string(p)), task, targs);
			fprintf(stderr, "adding proc to table\n");

			// keep track so we can delete later
			procTable.push_back(proc);
		}

		// start the dispatcher
		TaskProc* dispatcherProc = new  TaskProc("dispatcher", dispatcher, dispatchArgs);
		this->dispatchProc = dispatcherProc;
		// wait for threads...
		wait();
};

WorkerPool::
~WorkerPool()
{
		// delete taskargs
		for (auto t : taskArgs) {
				delete(t);
		}
};

void 
WorkerPool::
wait(void)
{
		while (!procTable.empty()) {
					TaskProc *p = procTable.front();
					p->join();
					procTable.erase(procTable.begin());
		}
		fprintf(stderr, "all threads waited for ...\n");
};


int
WorkerPool::
getNumWorkers()
{
		return numWorkers;
}

void
WorkerPool::
delWorker()
{
		lock();
		numWorkers--;
		unlock();
}

void
WorkerPool::
lock()
{
		pthread_mutex_lock(&pm);
}

void
WorkerPool::
unlock()
{
		pthread_mutex_unlock(&pm);
}

void
WorkerPool::
signal()
{
		pthread_cond_signal(&cv);
}


void
WorkerPool::
block()
{
		pthread_cond_wait(&cv, &pm);
}

void
WorkerPool::
block_to()
{
		struct timeval tv;
		struct timespec ts;
		gettimeofday(&tv, NULL);
		ts.tv_sec = tv.tv_sec + 2;
		ts.tv_nsec = 0;

		int rc = pthread_cond_timedwait(&cv, &pm, &ts);
}

