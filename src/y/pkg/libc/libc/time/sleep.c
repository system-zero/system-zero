// provides: long sys_sleep (ulong)
// requires: time/nanosleep.c
// requires: time/time.h

long sys_sleep (ulong sec) {
  struct timespec req;
  struct timespec rem;
  req.tv_sec = sec;
  req.tv_nsec = 0;

  long r;
  while (1) {
    r = sys_nanosleep (&req, &rem);
    ifnot (r) return r;

    if (-1 is r) {
      if (sys_errno is EINTR) {
        req = rem;
        rem.tv_sec = 0;
        rem.tv_nsec = 0;
        continue;
      }
    }

    break;
  }

  return r;
}
