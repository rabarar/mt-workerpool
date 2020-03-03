#pragma once

#include <pthread.h>
#include <string>

#include "Events.h"

/*
 *  Thread Process Object
 */

using namespace std;

typedef struct TaskArgs {
		void *args;
		string name;
} TaskArgs_t;

class TaskProc {
		private:
			string name;
			pthread_t thread;
			void *(*handler)(void*);
			void *exit_status;
		public:
			TaskProc(string name, void *(*handler)(void*), TaskArgs_t *args);
			~TaskProc();
			int terminate(void *status);
			void join();
};


