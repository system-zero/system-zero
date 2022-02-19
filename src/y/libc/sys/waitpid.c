// provides: long sys_waitpid (int, int *, int)
// depends: sys/pid.h

long sys_waitpid (int pid, int *status, int flags) {
  return syscall4 (NR_wait4, pid, (long) status, flags, 0);
}
