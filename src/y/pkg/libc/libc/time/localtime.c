// provides: struct tm *localtime (time_t *)
// requires: time/time.h
// requires: time/tzset.c
// requires: time/tp2tm.c

struct tm *localtime (time_t *t) {
  static struct tm tm;
  tzset ();
  tp2tm (&tm, *t - timezone);
  return &tm;
}
