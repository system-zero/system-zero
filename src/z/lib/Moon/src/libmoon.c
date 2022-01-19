#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>

#include "moon.h"

static int time_jdn (struct tm *tim) {
  /* http://en.wikipedia.org/wiki/Julian_day
   * until 1/1/4713 B.C (should be plenty enough) */

  /* if (tim.tm_year == 1582 && lmonth == 10 &&
     (tim.tm_mday < 15 &&  tim.tm_mday > 4))
      return -1; "This happens to be a day within 4 - 15 of October of 1582"
    this needs to be a time type object, sometime in time
   */

  int jdn = 0;
  int lhour =tim->tm_hour;
  int lmonth = tim->tm_mon + 1;
  int a = (14 - lmonth) / 12;
  int newyear = (tim->tm_year + 4801 - (0 < tim->tm_year)) - a;
  int newmonth = lmonth +  (12 * a) - 3;

  if (tim->tm_year > 1582 ||
        (tim->tm_year == 1582 && (lmonth > 10 || (lmonth == 10 && tim->tm_mday > 4))))
      jdn = tim->tm_mday + ((153 * newmonth + 2) / 5) + (newyear * 365) +
          (newyear / 4) - (newyear / 100) + (newyear / 400) - 32045;
    else
      jdn = tim->tm_mday + (153 * newmonth + 2) / 5 + newyear * 365 + newyear / 4 - 32083;

    if (12 > lhour && lhour >= 0)
      jdn--;

  return jdn;
}

static int time_week_day (struct tm *tim, int jdn) {
  int a = (14 - tim->tm_mon) / 12;
  int lyear = tim->tm_year - a + (0 > tim->tm_year);
  int lmonth = (tim->tm_mon + 1) + (12 * a) - 2;

  int lday;
  if (jdn > 2299160)
    lday = (tim->tm_mday + lyear + (lyear / 4) - (lyear / 100) +  (lyear / 400)
        + (31 * lmonth) / 12) % 7;
  else
    lday = (5 + tim->tm_mday + lyear + (lyear / 4) + (31 * lmonth) / 12) % 7;

  return lday;
}

static size_t cstring_byte_cp (char *dest, const char *src, size_t nelem) {
  const char *sp = src;
  size_t len = 0;

  while (len < nelem && *sp) { // this differs in memcpy()
    dest[len] = *sp++;
    len++;
  }

  return len;
}

static size_t cstring_cp (char *dest, size_t dest_len, const char *src, size_t nelem) {
  size_t num = (nelem > (dest_len - 1) ? dest_len - 1 : nelem);
  size_t len = (NULL == src ? 0 : cstring_byte_cp (dest, src, num));
  dest[len] = '\0';
  return len;
}

#define VA_ARGS_FMT_SIZE(fmt_)                                        \
({                                                                    \
  int size = 0;                                                       \
  va_list ap; va_start(ap, fmt_);                                     \
  size = vsnprintf (NULL, size, fmt_, ap);                            \
  va_end(ap);                                                         \
  size;                                                               \
})

#define VA_ARGS_GET_FMT_STR(buf_, size_, fmt_)                        \
({                                                                    \
  va_list ap; va_start(ap, fmt_);                                     \
  vsnprintf (buf_, size_ + 1, fmt_, ap);                              \
  va_end(ap);                                                         \
  buf_;                                                               \
})

static size_t cstring_cp_fmt (char *dest, size_t dest_len, const char *fmt, ...) {
  size_t len = VA_ARGS_FMT_SIZE(fmt);
  char bytes[len + 1];
  VA_ARGS_GET_FMT_STR(bytes, len, fmt);
  return cstring_cp (dest, dest_len, bytes, len);
}

#define JDN_TO_CAL_LEN 256

static char *time_julian_day_to_cal (struct tm *tim, char *s, int jdn) {
  const char *months[] = {
      "January", "February", "March", "April", "May", "June", "July",
      "August", "September", "October", "November", "December"};

  const char *week_days[] = {
 	 "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

  float z = jdn;
  int a = z;

  if (jdn > 2299160) {
    int w = (int) (z - 1867216.25) / 36524.25;
    int x = (int) (w / 4);
    a = z + 1 + w - x;
  }

  int b = a + 1524;
  int c = (int) (b - 122.1) / 365.25;
  int d = (int) (365.25 * c);
  int e = (int) (b - d) / 30.6001;
  int f = (int) (30.6001 * e);
  int lday = b - d - f;
  int lmonth = e - 1;

  if (lmonth > 12)
    lmonth = e - 13;

  int lyear;
  if (lmonth == 1 || lmonth == 2)
    lyear = c - 4715;
  else
    lyear = c - 4716;

  //  % there is bug on the called fun, when on March? 2017
  //  % returns +2 days

  int wday = time_week_day (tim, jdn);
  cstring_cp_fmt (s, JDN_TO_CAL_LEN, "%d %s, %d, %s", lday, months[lmonth - 1], lyear, week_days[wday]);
  return s;
}

static float round2 (float x) {
  return round (100 * x) / 100.0;
}

static float normalize (float ip) {
  float i = ip - floor (ip);
  ip = ip - floor (ip);
  if (i < 0)
     i++;
  return i;
}

char *MoonPhase (struct tm *tim, char vreport[MOON_MAXLEN_REPORT]) {
 /* Thanks to
  * http://home.att.net/~srschmitt/zenosamples/zs_lunarphasecalc.html
  * for the moonphase algorithm (many years ago now) - so i do not have the
  * original sources available. (iirc) This was a program written in a
  * programming language for ms-windows, that I translate it to SLang, and
  * now translated to C. Note that I let some native language expressions around (by purpose).
  */

  char *report = vreport;

  int jdn = 0;
  int lmonth = tim->tm_mon + 1;

  if ((tim->tm_year >= 2038) && (lmonth >= 1) && (tim->tm_mday >= 19) && (tim->tm_hour >= 3)
      && (tim->tm_min >= 14) && (tim->tm_sec >= 7)) {
    cstring_cp (report, 2096, "This is the 2038 bug, happens to 32bit systems", 64);
    return report;
  }

   jdn = time_jdn (tim);
   char phase[32];

   float ip = (jdn - 2451550.1) / 29.530588853;
   float oldip = ((jdn - 1) - 2451550.1) / 29.530588853;
   float ag = normalize (ip) * 29.53;

   if      (ag <  1.84566) cstring_cp (phase, 32, "NEW",             3);
   else if (ag <  5.53699) cstring_cp (phase, 32, "Waxing crescent", 15);
   else if (ag <  9.22831) cstring_cp (phase, 32, "First quarter",   13);
   else if (ag < 12.91963) cstring_cp (phase, 32, "Waxing gibbous",  14);
   else if (ag < 16.61096) cstring_cp (phase, 32, "FULL",             4);
   else if (ag < 20.30228) cstring_cp (phase, 32, "Waning gibbous",  14);
   else if (ag < 23.99361) cstring_cp (phase, 32, "Last quarter",    12);
   else if (ag < 27.68493) cstring_cp (phase, 32, "Waning crescent", 15);
   else                    cstring_cp (phase, 32, "NEW",              3);

  float pi = 3.1415926535897932385;

  ip = ip * 2 * pi;
  oldip = oldip * 2 * pi;
                                  /* in this expression when e.g., jdn = 2458800 */
  float dp = 2 * pi * normalize ((jdn - 2451562.2) / 27.55454988);
                                  /* C returns: 262.671692
                                   * Slang    : 262.671683
                                   */
  float olddp= 2 * pi * normalize ((jdn - 1 - 2451562.2) / 27.55454988);
  float di = 60.4 - 3.3 * cos (dp) - 0.6 * cos (2 * ip - dp) - 0.5 * cos (2 * ip);
  float olddi  = 60.4 - 3.3 * cos (olddp) - 0.6 * cos (2 * oldip - olddp) - 0.5 * cos (2 * oldip);
  float np = 2 * pi * normalize ((jdn - 2451565.2 ) / 27.212220817);
  float la = 5.1 * sin (np);
  float rp = normalize ((jdn - 2451555.8) / 27.321582241);
  float lo = 360 * rp + 6.3 * sin (dp) + 1.3 * sin (2 * ip - dp) + 0.7 * sin (2 * ip);

  char zodiac[64];

  if      (lo <  33.18) cstring_cp (zodiac, 64, "Pisces - Ιχθείς",       21);
  else if (lo <  51.16) cstring_cp (zodiac, 64, "Aries - Κριός",         18);
  else if (lo <  93.44) cstring_cp (zodiac, 64, "Taurus - Ταύρος",       21);
  else if (lo < 119.48) cstring_cp (zodiac, 64, "Gemini - Διδυμος",      23);
  else if (lo < 135.30) cstring_cp (zodiac, 64, "Cancer - Καρκίνος",     25);
  else if (lo < 173.34) cstring_cp (zodiac, 64, "Leo - Λέων",            14);
  else if (lo < 224.17) cstring_cp (zodiac, 64, "Virgo - Παρθένος",      24);
  else if (lo < 242.57) cstring_cp (zodiac, 64, "Libra - Ζυγός",         18);
  else if (lo < 271.26) cstring_cp (zodiac, 64, "Scorpio - Σκορπιός",    26);
  else if (lo < 302.49) cstring_cp (zodiac, 64, "Sagittarius - Τοξότης", 28);
  else if (lo < 311.72) cstring_cp (zodiac, 64, "Capricorn - Αιγώκερος", 30);
  else if (lo < 348.58) cstring_cp (zodiac, 64, "Aquarius - Υδροχόος",   27);
  else                  cstring_cp (zodiac, 64, "Pisces - Ιχθείς",       21);

  char date[JDN_TO_CAL_LEN];
  time_julian_day_to_cal (tim, date, jdn);

  cstring_cp_fmt (report, 2048,
      "Date:          %s\n"
      "Phase:         %s\n"
      "Age:           %.2f days (%f)\n"
      "Distance:      %.2f earth radii, %s\n"
      "Latitude:      %.2f°\n"
      "Longitude:     %.2f°\n"
      "Constellation: %s",
      date, phase,
      round2 (ag), round2 (ag) / 29.530588853,
      round2 (di), olddi > di ? "descendant (κατερχόμενη)" : "ascendant (ανερχόμενη)",
      round2 (la),
      round2 (lo),
/* because of the above nentioned inconsistency, this produces a tiny difference,
 * not sure who is right */
      zodiac);
  return report;
}
