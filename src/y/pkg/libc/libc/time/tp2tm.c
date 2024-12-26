// provides: static struct tm *tp2tm (struct tm *, time_t)
// requires: time/time.h

/* neatlibc implementation */
#define isleap(y)   (!(((y) % 4) && ((y) % 100)) || !((y) % 400))
#define SPD         (24 * 60 * 60)

static struct tm *tp2tm (struct tm *tm, time_t t) {
  static int dpm[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  long days = t / (24 * 60 * 60);
  long rem = t % (24 * 60 * 60);

  int i;

  tm->tm_sec = rem % 60;
  rem /= 60;
  tm->tm_min = rem % 60;
  tm->tm_hour = rem / 60;
  tm->tm_wday = (4 + days) % 7;

  for (i = 1970; days >= 365 + (!(((i) % 4) && ((i) % 100)) || !((i) % 400)); i++)

  days -= 365 + (!(((i) % 4) && ((i) % 100)) || !((i) % 400));

  tm->tm_year = i - 1900;
  tm->tm_yday = days;
  tm->tm_mday = 1;

  if ((!(((i) % 4) && ((i) % 100)) || !((i) % 400)) && days == 59)
    tm->tm_mday++;

  if ((!(((i) % 4) && ((i) % 100)) || !((i) % 400)) && days >= 59)
    days--;

  for (i = 0; i < 11 && days >= dpm[i]; i++)

  days -= dpm[i];
  tm->tm_mon = i;
  tm->tm_mday += days;

  return tm;
}
