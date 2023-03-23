// provides: long sys_access (const char *, int)
// requires: unistd/unistd.h
// requires: unistd/fcntl.h

long sys_access (const char *path, int mode) {
  return syscall4 (NR_faccessat, AT_FDCWD, (long) path, mode, 0);
}
