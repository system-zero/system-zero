// provides: long sys_unlink (const char *)
// provides: long sys_unlinkat (int, const char *, int)

long sys_unlink (const char *file) {
  return syscall3 (NR_unlinkat, AT_FDCWD, (long) file, 0);
}

long sys_unlinkat (int dirfd, const char *file, int flags) {
  return syscall3 (NR_unlinkat, dirfd, (long) file, flags);
}
