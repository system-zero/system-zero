// provides: long fork (void)
// requires: signal/signal.h

long fork (void) {
  return syscall5 (NR_clone, SIGCHLD, 0, 0, 0, 0);
}
