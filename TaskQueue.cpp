#include <queue>
#include <pthread.h>

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
		TaskEvent_t *te;
		lock();
		while(taskQueue.empty()) {
				block();
		}
		te = taskQueue.front();
		taskQueue.pop();
		signal();
		unlock();

		return te;
}
