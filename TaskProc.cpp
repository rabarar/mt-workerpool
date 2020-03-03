#include <pthread.h>

#include "TaskProc.h"

TaskProc::
TaskProc(string name, void *(*handler)(void*), TaskArgs_t *args)
{
		this->name = name;
		this->handler = handler;

		fprintf(stderr, "creating thread\n");
		
		int rc = pthread_create(&thread, NULL, handler, (void *)args);
		if (rc != 0) {
				fprintf(stderr, "failed to create thread: %d\n", rc);
				exit(-1);
		}
};

void
TaskProc::
join()
{
		fprintf(stderr, "joining thread [%s]\n", name.c_str());
		pthread_join(thread, &exit_status);
}


void
TaskProc::
terminate(void)
{
		pthread_cancel(thread);
}

TaskProc::
~TaskProc()
{
}
