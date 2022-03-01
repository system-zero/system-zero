// provides: static void tp2tm (struct tm *, time_t)
// requires: time/time.h

/* neatlibc implementation */
#define isleap(y)   (!(((y) % 4) && ((y) % 100)) || !((y) % 400))
#define SPD         (24 * 60 * 60)

static void tp2tm (struct tm *tm, time_t t) {
  static int dpm[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  long days = t / SPD;
  long rem = t % SPD;
  int i;
  tm->tm_sec = rem % 60;
  rem /= 60;
  tm->tm_min = rem % 60;
  tm->tm_hour = rem / 60;
  tm->tm_wday = (4 + days) % 7;

  /* calculating yday and year */
  for (i = 1970; days >= 365 + isleap(i); i++)

  days -= 365 + isleap(i);
  tm->tm_year = i - 1900;
  tm->tm_yday = days;

  /* calculating mday and mon */
  tm->tm_mday = 1;
  if (isleap (i) && days == 59)
    tm->tm_mday++;

  if (isleap (i) && days >= 59)
    days--;

  for (i = 0; i < 11 && days >= dpm[i]; i++)

  days -= dpm[i];
  tm->tm_mon = i;
  tm->tm_mday += days;
}
