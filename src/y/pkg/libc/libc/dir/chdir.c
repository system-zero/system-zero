// provides: long sys_chdir (const char *)

long sys_chdir (const char *path) {
  return syscall1 (NR_chdir, (long) path);
}
