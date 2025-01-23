// provides: long sys_rmdir (const char *)
// requires: unistd/fcntl.h

long sys_rmdir (const char *dir) {
  return syscall3 (NR_unlinkat, AT_FDCWD, (long) dir, AT_REMOVEDIR);
}
