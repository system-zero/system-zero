// provides: long sys_pipe (int *)

long sys_pipe (int *fds) {
  return syscall2 (NR_pipe2, (long) fds, 0);
}
