// provides: int system (const char *)
// requires: sys/waitpid.c
// requires: std/environ.h
// requires: signal/sigset.h
// requires: signal/sigaction.h
// requires: signal/sigaction.c
// requires: signal/sigprocmask.c
// requires: signal/sigemptyset.c
// requires: signal/sigaddset.c
// requires: signal/signal.h
// requires: unistd/fork.c
// requires: unistd/execve.c
// requires: stdlib/_exit.c

/* diet-libc implementation */

int system (const char *line) {
  struct sigaction sa, intr, quit;
  sigset_t block, omask;

  int save, pid, ret = -1;

  if (NULL == line)
    return system ("exit 0") == 0;

  sa.sa_handler = SIG_IGN;
  sa.sa_flags = 0;
  sys_sigemptyset (&sa.sa_mask);

  if (sys_sigaction (SIGINT,  &sa, &intr) < 0)
    return -1;

  if (sys_sigaction (SIGQUIT, &sa, &quit) < 0) {
    save = sys_errno;

undo:
    sys_sigaction (SIGINT, &intr, (struct sigaction *) 0);
    sys_errno = save;
    return -1;
  }

  sys_sigemptyset (&block);
  sys_sigaddset (&block, SIGCHLD);

  if (sys_sigprocmask (SIG_BLOCK, &block, &omask) < 0) {
    save = sys_errno;
    sys_sigaction (SIGQUIT, &quit, (struct sigaction *) 0);
    goto undo;
  }

  pid = sys_fork ();

  if (pid > 0) { /* parent */
    int n;

    do
      n = sys_waitpid (pid, &ret, 0);
    while ((n == -1) && (sys_errno == EINTR));

    if (n != pid)
      ret = -1;

  } else if (0 == pid) { /* child */
    const char *nargs[4];
    nargs[0] = "/bin/sh";
    nargs[1] = "-c";
    nargs[2] = line;
    nargs[3] = 0;

    sys_sigaction (SIGINT,  &intr, (struct sigaction *) 0);
    sys_sigaction (SIGQUIT, &quit, (struct sigaction *) 0);
    sys_sigprocmask (SIG_SETMASK, &omask, 0);

    sys_execve ("/bin/sh", (char *const *) nargs, environ);
    _exit (127);
  }

  save = sys_errno;
  sys_sigaction (SIGINT,  &intr, (struct sigaction *) 0);
  sys_sigaction (SIGQUIT, &quit, (struct sigaction *) 0);
  sys_sigprocmask (SIG_SETMASK, &omask, 0);
  sys_errno = save;
  return ret;
}
