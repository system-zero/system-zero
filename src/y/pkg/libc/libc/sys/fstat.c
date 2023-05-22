// provides: long sys_fstat (int, struct stat *)
// requires: sys/stat.h

long sys_fstat (int fd, struct stat *st) {
#ifdef NR_fstat64
  return syscall2 (NR_fstat64, fd, (long) st);
#else
  return syscall2 (NR_fstat, fd, (long) st);
#endif
}
