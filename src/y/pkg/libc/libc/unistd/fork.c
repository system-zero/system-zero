// provides: long sys_fork (void)
// requires: signal/signal.h

long sys_fork (void) {
  return syscall5 (NR_clone, SIGCHLD, 0, 0, 0, 0);
}
