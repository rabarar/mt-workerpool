#include <sys/time.h>
#include <unistd.h>


#include "WorkerPool.h"
#include "Events.h"
#include "utils.h"

WorkerPool::
WorkerPool(int numWorkers, PoolArgs_t *args)
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

static void *task(void *args)
{
		TaskArgs* targs = (TaskArgs*)args;
		PoolArgs* pargs = (PoolArgs *)(targs->args);
		
		WorkerPool *pool = pargs->pool;

		while (1) {

				if (done) {
						fprintf(stderr, "interrupt worker task terinating - no more workers\n");
						pthread_exit(NULL);
				}

				TaskEvent_t *tv = pargs->src_queue->deQueue();
				if (tv == NULL || tv->type == STOP) {
						fprintf(stdout, "received STOP event on pool - %s \n", targs->name.c_str());
						pool->delWorker();
						pthread_exit(NULL);

				} else {
						fprintf(stdout, "received event on pool - %s \n", targs->name.c_str());
						tv->handler(tv->args);
						delete(tv);
				}
				

				usleep(MICSECS*50);
		}
}

static void *dispatcher(void *args)
{
		TaskArgs_t *targs = (TaskArgs_t *)args;
		PoolArgs* pargs = (PoolArgs *)(targs->args);

		static long evNo = 0;

		WorkerPool *pool = pargs->pool;

		while (1) {

				if (done) {
						fprintf(stderr, "interrupt dispatcher terinating - no more workers\n");
						pthread_exit(NULL);
				}

				pool->lock();
				int numWorkers = pool->getNumWorkers();
				pool->unlock();

				if (numWorkers == 0) {
						fprintf(stderr, "dispatcher terinating - no more workers\n");
						pthread_exit(NULL);
				}

				TaskEvent *te = new TaskEvent();
				if (evNo >= 10) {
						te->handler = NULL;
						te->args = NULL;
						te->type = STOP;

						
				} else {
						te->handler = eventHandler;
						te->args = (void *)(long(evNo));
						te->type = GENERIC;
						}
				evNo++;

				// only add if there are more than 0 workers 
				pool->lock();
				numWorkers = pool->getNumWorkers();
				if  (numWorkers > 0) {
						fprintf(stdout, "%d workers: add event to pool - %s \n", numWorkers, targs->name.c_str());
						pargs->src_queue->enQueue(te);
				}
				pool->unlock();
				usleep(MICSECS*50);
		}
}

