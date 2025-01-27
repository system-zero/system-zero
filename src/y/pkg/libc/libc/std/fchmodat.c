// provides: long sys_fchmodat (int, char *, mode_t, int)
// requires: unistd/fcntl.h
// requires: sys/types.h

long sys_fchmodat (int dirfd, char *pathname, mode_t mode, int flags) {
  return syscall4 (NR_fchmodat, dirfd, (long) pathname, mode, flags);
}
