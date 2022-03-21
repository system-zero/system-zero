// provides: long sys_open (const char *, int)
// provides: #define open sys_open
// requires: unistd/fcntl.h

long sys_open (const char *name, int flags) {
  return syscall3 (NR_openat, AT_FDCWD, (long) name, flags);
}
