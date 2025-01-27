// provides: long sys_link (const char *, const char *)

long sys_link (const char *oldpath, const char *newpath) {
  return syscall5 (NR_linkat, AT_FDCWD, (long) oldpath, AT_FDCWD, (long) newpath, 0);
}
