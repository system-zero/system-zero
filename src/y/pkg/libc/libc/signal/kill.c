// provides: long sys_kill (int, int)

long sys_kill (int pid, int sig) {
  return syscall2 (NR_kill, pid, sig);
}
