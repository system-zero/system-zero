// provides: long sys_gettimeofday(struct timeval *,  struct timezone *)

long sys_gettimeofday (struct timeval *tv,  struct timezone *tz) {
  return syscall2 (NR_gettimeofday, (long) tv, (long) tz);
}
