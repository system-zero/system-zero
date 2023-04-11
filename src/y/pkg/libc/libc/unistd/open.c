// provides: long sys_open  (const char *, int)
// provides: long sys_open3 (const char *, int, int)
// requires: unistd/fcntl.h

long sys_open (const char *name, int flags) {
  return syscall3 (NR_openat, AT_FDCWD, (long) name, flags);
}

long sys_open3 (const char *name, int flags, int mode) {
  return syscall4 (NR_openat, AT_FDCWD, (long) name, flags, mode);
}
