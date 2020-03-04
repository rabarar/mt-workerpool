#include <pthread.h>
#include <unistd.h>
#include<stdio.h>

#include <queue>
#include <vector>
#include <string>

#include "TaskQueue.h"
#include "TaskProc.h"
#include "WorkerPool.h"
#include "Events.h"
#include "utils.h"
#include "usignal.h"

using namespace std;

int main(int argc, char *argv[]) {

		// set the signal handler
		set_sig_handler();

		// create a task q
		TaskQueue *tq_src = new TaskQueue();
		TaskQueue *tq_syn = new TaskQueue();

		// setup pool args
		PoolArgs_t poolArgs;
		poolArgs.name = "pool1";
		poolArgs.src_queue = tq_src;
		poolArgs.syn_queue = tq_syn;

		fprintf(stderr, "starting...\n");

		WorkerPool pool(500, &poolArgs);
		
		delete(tq_src);
		delete(tq_syn);
		fprintf(stderr, "ending...\n");

		usleep(SECONDS);


};
