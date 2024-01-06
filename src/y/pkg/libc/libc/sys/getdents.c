// provides: long sys_getdents (int, void *, int)

long sys_getdents (int fd, void *dp, int count) {
  return syscall3 (NR_getdents64, fd, (long) dp, count);
}
