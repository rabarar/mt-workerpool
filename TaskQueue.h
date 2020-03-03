#pragma once 

#include <queue>

#include "Events.h"

using namespace std;

typedef struct TaskEvent {
		void *(*handler)(void *args);
		void *args;
		Event_t type;
} TaskEvent_t;

class TaskQueue {
		private:
				pthread_mutex_t mu;
				pthread_cond_t cv;
				queue<TaskEvent_t*> taskQueue;
				void lock();
				void unlock();
				void signal();
				void block();
		public:
				TaskQueue();
				~TaskQueue();
				void enQueue(TaskEvent_t *te);
				TaskEvent_t *deQueue();
};


