// provides: int sys_clock_settime (clockid_t, const struct timespec *)
// provides: #define clock_settime sys_clock_settime
// requires: time/time.h

int sys_clock_settime (clockid_t clockid, const struct timespec *ts) {
  return syscall2 (NR_clock_settime, (long) clockid, (long) ts);
}
