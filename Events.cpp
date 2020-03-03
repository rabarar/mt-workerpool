#include <stdio.h>

#include "Events.h"

void *eventHandler(void *args) {
		long d = (long)args;
		fprintf(stdout, "handler: args - %ld ...\n", d);
		return NULL;
}

