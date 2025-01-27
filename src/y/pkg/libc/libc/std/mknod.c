// provides: long sys_mknod (const char *, mode_t, dev_t)
// requires: unistd/fcntl.h

long sys_mknod (const char *pathname, mode_t mode, dev_t dev) {
  return syscall4 (NR_mknodat, AT_FDCWD, (long) pathname, mode, dev);
}
