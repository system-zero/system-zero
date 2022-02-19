// provides: long sys_open (const char *, int)

#ifndef AT_FDCWD
#define AT_FDCWD       -100
#endif

long sys_open (const char *name, int flags) {
  return syscall3 (NR_openat, AT_FDCWD, (long) name, flags);
}
