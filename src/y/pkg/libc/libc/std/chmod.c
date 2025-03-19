// provides: long sys_chmod (char *, mode_t)
// requires: unistd/fcntl.h
// requires: sys/types.h

long sys_chmod (char *pathname, mode_t mode) {
  return syscall4 (NR_fchmodat, AT_FDCWD, (long) pathname, mode, 0);
}
