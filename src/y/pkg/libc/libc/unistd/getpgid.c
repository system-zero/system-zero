// provides: long sys_getpgid (pid_t)
// requires: sys/pid.h

long sys_getpgid (pid_t pid) {
  return syscall1 (NR_getpgid, pid);
}
