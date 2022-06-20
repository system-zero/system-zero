// provides: long sys_fork (void)

#ifndef SIGCHLD
#define SIGCHLD 17
#endif

long sys_fork (void) {
  return syscall5 (NR_clone, SIGCHLD, 0, 0, 0, 0);
}
