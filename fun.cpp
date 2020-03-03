#include <pthread.h>
#include <unistd.h>
#include<stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>

#include <queue>
#include <vector>
#include <string>

#include "TaskQueue.h"
#include "TaskProc.h"
#include "WorkerPool.h"
#include "Events.h"

#define SECONDS     1000000
#define MSECS       (SECONDS/1000)
#define MICSECS       (MSECS/1000)
#define DELAY       (MSECS * 500)

using namespace std;

sig_atomic_t volatile done = 0;
void sig_handler(int signo, siginfo_t *info, void *extra)
{
		fprintf(stdout, "signal handler: ...\n");
		done = 1;
}

void set_sig_handler(void)
{
        struct sigaction action;


        action.sa_flags = SA_SIGINFO;
        action.sa_sigaction = sig_handler;

        if (sigaction(SIGINT, &action, NULL) == -1) {
			fprintf(stdout, "err: signal handler\n");
            exit(1);
        }

}

void *eventHandler(void *args) {
		long d = (long)args;
		fprintf(stdout, "handler: args - %ld ...\n", d);
		return NULL;
}

void *task(void *args) {
		TaskArgs* targs = (TaskArgs*)args;
		PoolArgs* pargs = (PoolArgs *)(targs->args);
		TaskQueue *tqueue = pargs->queue;
		
		WorkerPool *pool = pargs->pool;

		while (1) {

				if (done) {
						fprintf(stderr, "interrupt worker task terinating - no more workers\n");
						pthread_exit(NULL);
				}

				TaskEvent_t *tv = tqueue->deQueue();
				if (tv == NULL || tv->type == STOP) {
						fprintf(stdout, "received STOP event on pool - %s \n", targs->name.c_str());
						pool->delWorker();
						pthread_exit(NULL);

				} else {
						fprintf(stdout, "received event on pool - %s \n", targs->name.c_str());
						tv->handler(tv->args);
						delete(tv);
				}
				

				usleep(MSECS*500);
		}
};

void *dispatcher(void *args) {
		TaskArgs_t *targs = (TaskArgs_t *)args;
		PoolArgs* pargs = (PoolArgs *)(targs->args);
		TaskQueue *tqueue = pargs->queue;
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
						tqueue->enQueue(te);
				}
				pool->unlock();

				usleep(MSECS*500);
		}
};

int main(int argc, char *argv[]) {

		// set the signal handler
		set_sig_handler();

		// create a task q
		TaskQueue *tq = new TaskQueue();

		// setup pool args
		PoolArgs_t poolArgs;
		poolArgs.name = "pool1";
		poolArgs.queue = tq;

		fprintf(stderr, "starting...\n");

		WorkerPool pool(5, task, dispatcher,&poolArgs);
		
		delete(tq);
		fprintf(stderr, "ending...\n");

		usleep(SECONDS);


};
