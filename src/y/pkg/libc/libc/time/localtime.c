// provides: struct tm *sys_localtime (time_t *, struct tm *tm)
// requires: time/time.h
// requires: time/tzset.c
// requires: time/tp2tm.c

struct tm *sys_localtime (time_t *t, struct tm *tm) {
  long timezone = sys_tzset ();
  tp2tm (tm, *t - timezone);
  return tm;
}
