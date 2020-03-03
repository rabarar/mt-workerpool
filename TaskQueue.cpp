#include <queue>
#include <pthread.h>
#include <sys/time.h>
#include <sys/errno.h>

#include "TaskQueue.h"


TaskQueue::
TaskQueue()
{
		pthread_mutex_init(&mu, NULL);
		pthread_cond_init(&cv, NULL);
}

TaskQueue::
~TaskQueue()
{
		pthread_mutex_destroy(&mu);
		pthread_cond_destroy(&cv);
}

void
TaskQueue::
lock()
{
		pthread_mutex_lock(&mu);
}

void
TaskQueue::
unlock()
{
		pthread_mutex_unlock(&mu);
}

void
TaskQueue::
signal()
{
		pthread_cond_signal(&cv);
}

void
TaskQueue::
block()
{
		pthread_cond_wait(&cv, &mu);
}

int
TaskQueue::
block_to()
{
		struct timeval tv;
		struct timespec ts;
		gettimeofday(&tv, NULL);
		ts.tv_sec = tv.tv_sec + 2;
		ts.tv_nsec = 0;

		return pthread_cond_timedwait(&cv, &mu, &ts);
}

void
TaskQueue::
enQueue(TaskEvent_t *te)
{
		lock();
		taskQueue.push(te);
		signal();
		unlock();
}

TaskEvent_t *
TaskQueue::
deQueue()
{
		TaskEvent_t *te = NULL;
		lock();
		bool timedout = false;
		while(taskQueue.empty()) {
				int rc = block_to();
				if (rc == ETIMEDOUT) {
						fprintf(stderr, "wait block: timedout\n");
						timedout = true;
						break;
				} else if (rc == EINVAL) {
						fprintf(stderr, "EINVAL on conditional block with timeout, aborting\n");
						exit(-1);
				}
		}

		if (!timedout)  {
				te = taskQueue.front();
				taskQueue.pop();
				signal();
		}
		unlock();

		return te;
}
