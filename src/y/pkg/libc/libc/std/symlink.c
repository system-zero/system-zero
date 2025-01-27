// provides: long sys_symlink (const char *, const char *)

long sys_symlink (const char *target, const char *linkpath) {
  return syscall3 (NR_symlinkat, (long)target, AT_FDCWD, (long) linkpath);
}
