// provides: long sys_nanosleep (struct timespec *, struct timespec *)
// requires: time/time.h

long sys_nanosleep (struct timespec *req, struct timespec *rem) {
  return syscall2 (NR_nanosleep, (long) req, (long) rem);
}
