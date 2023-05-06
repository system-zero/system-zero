// provides: long sys_mkdir (const char *, mode_t)
// requires: unistd/fcntl.h
// requires: sys/types.h

long sys_mkdir (const char *dir, mode_t mode) {
  return syscall3 (NR_mkdirat, AT_FDCWD, (long) dir, mode);
}
