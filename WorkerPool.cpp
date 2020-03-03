
#include "WorkerPool.h"

WorkerPool::
WorkerPool(int numWorkers, void *(*task)(void*), void *(*dispatcher)(void *), PoolArgs_t *args)
{

		this->numWorkers = numWorkers;

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
		int n;
		pthread_mutex_lock(&pm);
		n = numWorkers;
		pthread_mutex_unlock(&pm);

		return n;
}

void
WorkerPool::
delWorker()
{
		pthread_mutex_lock(&pm);
		numWorkers--;
		pthread_mutex_unlock(&pm);
}
