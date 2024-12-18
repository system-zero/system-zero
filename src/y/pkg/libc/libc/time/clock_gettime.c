// provides: long sys_clock_gettime (clockid_t, struct timespec *)
// requires: time/time.h

long sys_clock_gettime (clockid_t clockid, struct timespec* tp) {
  return syscall2 (NR_clock_gettime, (long) clockid, (long) tp);
}
