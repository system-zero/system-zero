// provides: long sys_gettimeofday(struct timeval *,  struct timezone *)
// provides: #define gettimeofday sys_gettimeofday

long sys_gettimeofday (struct timeval *tv,  struct timezone *tz) {
  return syscall2 (NR_gettimeofday, (long) tv, (long) tz);
}
