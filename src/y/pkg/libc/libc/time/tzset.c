// provides: long sys_tzset (void)
// requires: time/time.h
// requires: time/gettimeofday.c


long sys_tzset (void) {
  static long time_zone = -1;

  if (time_zone != -1)
    return time_zone;

  struct timezone tz;
  sys_gettimeofday (0, &tz);
  return tz.tz_minuteswest * 60;
}
