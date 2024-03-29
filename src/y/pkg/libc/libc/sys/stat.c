// provides: long sys_stat (const char *, struct stat *)
// requires: sys/stat.h

long sys_stat (const char *file, struct stat *st) {
  return syscall2 (NR_stat, (long) file, (long) st);
}
