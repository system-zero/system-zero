// provides: long sys_flock (int, int)

long sys_flock (int fd, int op) {
  return syscall2 (NR_flock, fd, op);
}
