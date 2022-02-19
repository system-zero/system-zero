// provides: long sys_access (const char *, int)

#define AT_FDCWD       -100

long sys_access (const char *path, int mode) {
  return syscall4 (NR_faccessat, AT_FDCWD, (long) path, mode, 0);
}
