// provides: long sys_pipe (int *)
// provides: long sys_pipe2 (int *, int)
// requires: unistd/fcntl.h

long sys_pipe (int *fds) {
  return syscall2 (NR_pipe2, (long) fds, 0);
}

long sys_pipe2 (int *fds, int flags) {
  return syscall2 (NR_pipe2, (long) fds, flags);
}
