// provides: long sys_setpgid (pid_t, pid_t)
// requires: sys/pid.h

long sys_setpgid (pid_t pid, pid_t pgid) {
  return syscall2 (NR_setpgid, pid, pgid);
}
