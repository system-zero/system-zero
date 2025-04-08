// provides: long sys_symlink (const char *, const char *)
// provides: long sys_symlinkat (const char *, int, const char *)
long sys_symlink (const char *target, const char *linkpath) {
  return syscall3 (NR_symlinkat, (long) target, AT_FDCWD, (long) linkpath);
}

long sys_symlinkat (const char *target, int dirfd, const char *linkpath) {
  return syscall3 (NR_symlinkat, (long) target, dirfd, (long) linkpath);
}
