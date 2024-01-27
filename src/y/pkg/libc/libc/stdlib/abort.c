// provides: void sys_abort (void)
// requires: stdlib/_exit.c
// requires: signal/sigemptyset.c
// requires: signal/sigaddset.c
// requires: signal/sigprocmask.c
// requires: signal/raise.c
// requires: signal/signal.h

void sys_abort (void) {
  sigset_t set;
  sys_sigemptyset (&set);
  sys_sigaddset (&set, SIGABRT);
  sys_sigprocmask (SIG_UNBLOCK, &set, NULL);
  sys_raise (SIGABRT);
  _exit (255);
}
