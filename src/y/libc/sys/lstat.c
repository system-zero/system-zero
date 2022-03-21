// provides: long sys_lstat (const char *, struct stat *)
// provides: #define lstat sys_lstat
// requires: sys/stat.h

long sys_lstat (const char *file, struct stat *st) {
  return syscall2 (NR_lstat, (long) file, (long) st);
}
