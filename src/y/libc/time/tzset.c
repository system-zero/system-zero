// provides: void tzset (void)
// provides: long timezone
// requires: time/time.h
// requires: time/gettimeofday.c

void tzset (void) {
  struct timezone tz;
  gettimeofday(0, &tz);
  timezone = tz.tz_minuteswest * 60;
}
