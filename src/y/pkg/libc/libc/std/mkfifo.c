// provides: long sys_mkfifo (const char *, mode_t)
// requires: std/mknod.c

long sys_mkfifo (const char *pathname, mode_t mode) {
  return sys_mknod (pathname, (mode & ~S_IFMT) | S_IFIFO, (dev_t) 0);
}
