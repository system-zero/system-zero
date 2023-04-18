// provides: struct tm *sys_gmtime (time_t *t, struct tm *tm)
// requires: time/tp2tm.c

struct tm *sys_gmtime (time_t *t, struct tm *tm) {
  tp2tm (tm, *t);
  return tm;
}
