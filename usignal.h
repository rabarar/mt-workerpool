#pragma once

extern sig_atomic_t volatile done;
void sig_handler(int signo, siginfo_t *info, void *extra);
void set_sig_handler(void);
