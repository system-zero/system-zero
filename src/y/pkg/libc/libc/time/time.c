// provides: time_t sys_time (time_t *)
// requires: time/time.h
// requires: time/gettimeofday.c

time_t sys_time (time_t *t) {
  struct timeval now;

  if (sys_gettimeofday (&now, NULL) < 0)
    now.tv_sec = (time_t) -1;

  if (t)
    *t = now.tv_sec;

  return now.tv_sec;
}
