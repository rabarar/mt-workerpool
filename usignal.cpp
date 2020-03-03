#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>

#include "usignal.h"

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

