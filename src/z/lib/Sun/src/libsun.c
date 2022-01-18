/* +++Date last modified: 05-Jul-1997 */
/* Updated comments, 05-Aug-2013 */

/*

SUNRISET.C - computes Sun rise/set times, start/end of twilight, and
             the length of the day at any date and latitude

Written as DAYLEN.C, 1989-08-16

Modified to SUNRISET.C, 1992-12-01
Split to a header file, 2017-12-10, by Joachim Nilsson

(c) Paul Schlyter, 1989, 1992

Released to the public domain by Paul Schlyter, December 1992

*/

#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdarg.h>

#include "sunriset.h"

#include "sun.h"

/* The "workhorse" function for sun rise/set times */

static int __sunriset__( int year, int month, int day, double lon, double lat,
                  double altit, int upper_limb, double *trise, double *tset )
/***************************************************************************/
/* Note: year,month,date = calendar date, 1801-2099 only.             */
/*       Eastern longitude positive, Western longitude negative       */
/*       Northern latitude positive, Southern latitude negative       */
/*       The longitude value IS critical in this function!            */
/*       altit = the altitude which the Sun should cross              */
/*               Set to -35/60 degrees for rise/set, -6 degrees       */
/*               for civil, -12 degrees for nautical and -18          */
/*               degrees for astronomical twilight.                   */
/*         upper_limb: non-zero -> upper limb, zero -> center         */
/*               Set to non-zero (e.g. 1) when computing rise/set     */
/*               times, and to zero when computing start/end of       */
/*               twilight.                                            */
/*        *rise = where to store the rise time                        */
/*        *set  = where to store the set  time                        */
/*                Both times are relative to the specified altitude,  */
/*                and thus this function can be used to compute       */
/*                various twilight times, as well as rise/set times   */
/* Return value:  0 = sun rises/sets this day, times stored at        */
/*                    *trise and *tset.                               */
/*               +1 = sun above the specified "horizon" 24 hours.     */
/*                    *trise set to time when the sun is at south,    */
/*                    minus 12 hours while *tset is set to the south  */
/*                    time plus 12 hours. "Day" length = 24 hours     */
/*               -1 = sun is below the specified "horizon" 24 hours   */
/*                    "Day" length = 0 hours, *trise and *tset are    */
/*                    both set to the time when the sun is at south.  */
/*                                                                    */
/**********************************************************************/
{
      double  d,  /* Days since 2000 Jan 0.0 (negative before) */
      sr,         /* Solar distance, astronomical units */
      sRA,        /* Sun's Right Ascension */
      sdec,       /* Sun's declination */
      sradius,    /* Sun's apparent radius */
      t,          /* Diurnal arc */
      tsouth,     /* Time when Sun is at south */
      sidtime;    /* Local sidereal time */

      int rc = 0; /* Return cde from function - usually 0 */

      /* Compute d of 12h local mean solar time */
      d = days_since_2000_Jan_0(year,month,day) + 0.5 - lon/360.0;

      /* Compute the local sidereal time of this moment */
      sidtime = revolution( GMST0(d) + 180.0 + lon );

      /* Compute Sun's RA, Decl and distance at this moment */
      sun_RA_dec( d, &sRA, &sdec, &sr );

      /* Compute time when Sun is at south - in hours UT */
      tsouth = 12.0 - rev180(sidtime - sRA)/15.0;

      /* Compute the Sun's apparent radius in degrees */
      sradius = 0.2666 / sr;

      /* Do correction to upper limb, if necessary */
      if ( upper_limb )
            altit -= sradius;

      /* Compute the diurnal arc that the Sun traverses to reach */
      /* the specified altitude altit: */
      {
            double cost;
            cost = ( sind(altit) - sind(lat) * sind(sdec) ) /
                  ( cosd(lat) * cosd(sdec) );
            if ( cost >= 1.0 )
                  rc = -1, t = 0.0;       /* Sun always below altit */
            else if ( cost <= -1.0 )
                  rc = +1, t = 12.0;      /* Sun always above altit */
            else
                  t = acosd(cost)/15.0;   /* The diurnal arc, hours */
      }

      /* Store rise and set times - in hours UT */
      *trise = tsouth - t;
      *tset  = tsouth + t;

      return rc;
}  /* __sunriset__ */



/* The "workhorse" function */


static double __daylen__( int year, int month, int day, double lon, double lat,
                   double altit, int upper_limb )
/**********************************************************************/
/* Note: year,month,date = calendar date, 1801-2099 only.             */
/*       Eastern longitude positive, Western longitude negative       */
/*       Northern latitude positive, Southern latitude negative       */
/*       The longitude value is not critical. Set it to the correct   */
/*       longitude if you're picky, otherwise set to to, say, 0.0     */
/*       The latitude however IS critical - be sure to get it correct */
/*       altit = the altitude which the Sun should cross              */
/*               Set to -35/60 degrees for rise/set, -6 degrees       */
/*               for civil, -12 degrees for nautical and -18          */
/*               degrees for astronomical twilight.                   */
/*         upper_limb: non-zero -> upper limb, zero -> center         */
/*               Set to non-zero (e.g. 1) when computing day length   */
/*               and to zero when computing day+twilight length.      */
/**********************************************************************/
{
      double  d,  /* Days since 2000 Jan 0.0 (negative before) */
      obl_ecl,    /* Obliquity (inclination) of Earth's axis */
      sr,         /* Solar distance, astronomical units */
      slon,       /* True solar longitude */
      sin_sdecl,  /* Sine of Sun's declination */
      cos_sdecl,  /* Cosine of Sun's declination */
      sradius,    /* Sun's apparent radius */
      t;          /* Diurnal arc */

      /* Compute d of 12h local mean solar time */
      d = days_since_2000_Jan_0(year,month,day) + 0.5 - lon/360.0;

      /* Compute obliquity of ecliptic (inclination of Earth's axis) */
      obl_ecl = 23.4393 - 3.563E-7 * d;

      /* Compute Sun's ecliptic longitude and distance */
      sunpos( d, &slon, &sr );

      /* Compute sine and cosine of Sun's declination */
      sin_sdecl = sind(obl_ecl) * sind(slon);
      cos_sdecl = sqrt( 1.0 - sin_sdecl * sin_sdecl );

      /* Compute the Sun's apparent radius, degrees */
      sradius = 0.2666 / sr;

      /* Do correction to upper limb, if necessary */
      if ( upper_limb )
            altit -= sradius;

      /* Compute the diurnal arc that the Sun traverses to reach */
      /* the specified altitude altit: */
      {
            double cost;
            cost = ( sind(altit) - sind(lat) * sin_sdecl ) /
                  ( cosd(lat) * cos_sdecl );
            if ( cost >= 1.0 )
                  t = 0.0;                      /* Sun always below altit */
            else if ( cost <= -1.0 )
                  t = 24.0;                     /* Sun always above altit */
            else  t = (2.0/15.0) * acosd(cost); /* The diurnal arc, hours */
      }
      return t;
}  /* __daylen__ */


/* This function computes the Sun's position at any instant */

static void sunpos( double d, double *lon, double *r )
/******************************************************/
/* Computes the Sun's ecliptic longitude and distance */
/* at an instant given in d, number of days since     */
/* 2000 Jan 0.0.  The Sun's ecliptic latitude is not  */
/* computed, since it's always very near 0.           */
/******************************************************/
{
      double M,         /* Mean anomaly of the Sun */
             w,         /* Mean longitude of perihelion */
                        /* Note: Sun's mean longitude = M + w */
             e,         /* Eccentricity of Earth's orbit */
             E,         /* Eccentric anomaly */
             x, y,      /* x, y coordinates in orbit */
             v;         /* True anomaly */

      /* Compute mean elements */
      M = revolution( 356.0470 + 0.9856002585 * d );
      w = 282.9404 + 4.70935E-5 * d;
      e = 0.016709 - 1.151E-9 * d;

      /* Compute true longitude and radius vector */
      E = M + e * RADEG * sind(M) * ( 1.0 + e * cosd(M) );
            x = cosd(E) - e;
      y = sqrt( 1.0 - e*e ) * sind(E);
      *r = sqrt( x*x + y*y );              /* Solar distance */
      v = atan2d( y, x );                  /* True anomaly */
      *lon = v + w;                        /* True solar longitude */
      if ( *lon >= 360.0 )
            *lon -= 360.0;                   /* Make it 0..360 degrees */
}

static void sun_RA_dec( double d, double *RA, double *dec, double *r )
/******************************************************/
/* Computes the Sun's equatorial coordinates RA, Decl */
/* and also its distance, at an instant given in d,   */
/* the number of days since 2000 Jan 0.0.             */
/******************************************************/
{
      double lon, obl_ecl, x, y, z;

      /* Compute Sun's ecliptical coordinates */
      sunpos( d, &lon, r );

      /* Compute ecliptic rectangular coordinates (z=0) */
      x = *r * cosd(lon);
      y = *r * sind(lon);

      /* Compute obliquity of ecliptic (inclination of Earth's axis) */
      obl_ecl = 23.4393 - 3.563E-7 * d;

      /* Convert to equatorial rectangular coordinates - x is unchanged */
      z = y * sind(obl_ecl);
      y = y * cosd(obl_ecl);

      /* Convert to spherical coordinates */
      *RA = atan2d( y, x );
      *dec = atan2d( z, sqrt(x*x + y*y) );

}  /* sun_RA_dec */


/******************************************************************/
/* This function reduces any angle to within the first revolution */
/* by subtracting or adding even multiples of 360.0 until the     */
/* result is >= 0.0 and < 360.0                                   */
/******************************************************************/

#define INV360    ( 1.0 / 360.0 )

static double revolution( double x )
/*****************************************/
/* Reduce angle to within 0..360 degrees */
/*****************************************/
{
      return( x - 360.0 * floor( x * INV360 ) );
}  /* revolution */

static double rev180( double x )
/*********************************************/
/* Reduce angle to within +180..+180 degrees */
/*********************************************/
{
      return( x - 360.0 * floor( x * INV360 + 0.5 ) );
}  /* revolution */


/*******************************************************************/
/* This function computes GMST0, the Greenwich Mean Sidereal Time  */
/* at 0h UT (i.e. the sidereal time at the Greenwich meridian at   */
/* 0h UT).  GMST is then the sidereal time at Greenwich at any     */
/* time of the day.  I've generalized GMST0 as well, and define it */
/* as:  GMST0 = GMST - UT  --  this allows GMST0 to be computed at */
/* other times than 0h UT as well.  While this sounds somewhat     */
/* contradictory, it is very practical:  instead of computing      */
/* GMST like:                                                      */
/*                                                                 */
/*  GMST = (GMST0) + UT * (366.2422/365.2422)                      */
/*                                                                 */
/* where (GMST0) is the GMST last time UT was 0 hours, one simply  */
/* computes:                                                       */
/*                                                                 */
/*  GMST = GMST0 + UT                                              */
/*                                                                 */
/* where GMST0 is the GMST "at 0h UT" but at the current moment!   */
/* Defined in this way, GMST0 will increase with about 4 min a     */
/* day.  It also happens that GMST0 (in degrees, 1 hr = 15 degr)   */
/* is equal to the Sun's mean longitude plus/minus 180 degrees!    */
/* (if we neglect aberration, which amounts to 20 seconds of arc   */
/* or 1.33 seconds of time)                                        */
/*                                                                 */
/*******************************************************************/

static double GMST0( double d )
{
      double sidtim0;
      /* Sidtime at 0h UT = L (Sun's mean longitude) + 180.0 degr  */
      /* L = M + w, as defined in sunpos().  Since I'm too lazy to */
      /* add these numbers, I'll let the C compiler do it for me.  */
      /* Any decent C compiler will add the constants at compile   */
      /* time, imposing no runtime or code overhead.               */
      sidtim0 = revolution( ( 180.0 + 356.0470 + 282.9404 ) +
                          ( 0.9856002585 + 4.70935E-5 ) * d );
      return sidtim0;
}  /* GMST0 */

static void convert (struct tm *tm, double ut, int *h, int *m) {
  *h = (int) floor (ut);
  *m = (int) (60 * (ut - floor (ut)));

  *m += (tm->tm_gmtoff % 3600) / 60;
  *h +=  tm->tm_gmtoff / 3600;
}

static char *lctime_r (struct tm *tm, double ut, char *buf, size_t len) {
  int h, m;

  convert (tm, ut, &h, &m);
  snprintf(buf, len, "%02d:%02d", h, m);

  return buf;
}

static char *lctime (struct tm *tm, double ut) {
  static char buf[10];

  return lctime_r(tm, ut, buf, sizeof(buf));
}

sun_t *SunCompute (sun_t *sun) {
  sun->day_len  =
    day_length (sun->year, sun->month, sun->day, sun->lon, sun->lat);
  sun->civil_len  =
    day_civil_twilight_length (sun->year, sun->month, sun->day, sun->lon, sun->lat);
  sun->nautical_len =
    day_nautical_twilight_length (sun->year, sun->month, sun->day, sun->lon, sun->lat);
  sun->astronomical_len =
    day_astronomical_twilight_length (sun->year, sun->month, sun->day, sun->lon, sun->lat);

  sun->sun_pos =
    sun_rise_set (sun->year, sun->month, sun->day, sun->lon, sun->lat, &sun->rise, &sun->set);

  sun->sun_at_south = (sun->rise + sun->set) / 2.0;

  sun->civil_twilight =
    civil_twilight (sun->year, sun->month, sun->day, sun->lon, sun->lat, &sun->civil_start, &sun->civil_end);

  sun->nautical_twilight =
    nautical_twilight (sun->year, sun->month, sun->day, sun->lon, sun->lat, &sun->nautical_start, &sun->nautical_end);

  sun->astronomical_twilight =
    astronomical_twilight (sun->year, sun->month, sun->day, sun->lon, sun->lat, &sun->astronomical_start, &sun->astronomical_end);

  return sun;
}

sun_t *SunInit (sun_t *sun) {
  time_t now = time (NULL);
  if (sun->utc > 0)
    sun->tm = gmtime (&now);
  else
    sun->tm = localtime (&now);

  if (sun->year < 0)  sun->year  = sun->tm->tm_year + 1900;
  if (sun->month < 0) sun->month = sun->tm->tm_mon + 1;
  if (sun->day < 0)   sun->day   = sun->tm->tm_mday;

  sun->rise = -1.0;
  sun->set  = -1.0;

  sun->sun_pos = sun->civil_len = sun->day_len =
  sun->nautical_len = sun->astronomical_len =
  sun->civil_twilight = sun->nautical_twilight =
  sun->astronomical_twilight = SUN_NONE;

  return sun;
}

static size_t cstring_byte_cp (char *dest, const char *src, size_t nelem) {
  const char *sp = src;
  size_t len = 0;

  while (len < nelem && *sp) {
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

char *SunToString (sun_t *sun) {
  char bufr[16], bufs[16];
  size_t len = SUN_BUF_LEN;
  size_t offset = 0;
  size_t num = 0;

  num = cstring_cp_fmt (sun->buf + offset, len,
    "Sun Rise Set at %d/%d/%d %s (%.0f°/%.0f°)\n",
      sun->day, sun->month, sun->year, sun->tm->tm_zone, sun->lon, sun->lat);
  len -= num;
  offset += num;

  num = 0;

  switch (sun->sun_pos) {
    case SUN_NONE:
      break;

    case SUN_OK:
      num = cstring_cp_fmt (sun->buf + offset, len,
        "Sun rises at %s, sets at %s %s\n",
          lctime_r (sun->tm, sun->rise, bufs, sizeof (bufs)),
          lctime_r (sun->tm, sun->set,  bufr, sizeof (bufr)),
          sun->tm->tm_zone);
      break;

    case SUN_IS_ABOVE_HORIZON:
      num = cstring_cp_fmt (sun->buf + offset, len, SUN_IS_ABOVE_HORIZON_STRING "\n");
      break;

    case SUN_IS_BELOW_HORIZON:
      num = cstring_cp_fmt (sun->buf + offset, len, SUN_IS_BELOW_HORIZON_STRING "\n");
      break;
  }

  len -= num;
  offset += num;
  num = 0;

  if (sun->day_len != SUN_NONE) {
    num = cstring_cp_fmt (sun->buf + offset, len,
      "Day length:                 %5.2f hours\n", sun->day_len);
    len -= num;
    offset += num;
  }

  if (sun->civil_len != SUN_NONE) {
    num = cstring_cp_fmt (sun->buf + offset, len,
      "With civil twilight         %5.2f hours\n", sun->civil_len);
    len -= num;
    offset += num;
  }

  if (sun->nautical_len != SUN_NONE) {
    num = cstring_cp_fmt (sun->buf + offset, len,
      "With nautical twilight      %5.2f hours\n", sun->nautical_len);
    len -= num;
    offset += num;
  }

  if (sun->astronomical_len != SUN_NONE) {
    num = cstring_cp_fmt (sun->buf + offset, len,
      "With astronomical twilight  %5.2f hours\n", sun->astronomical_len);
    len -= num;
    offset += num;
  }

  if (sun->day_len != SUN_NONE &&
     (sun->civil_len != SUN_NONE ||
      sun->nautical_len != SUN_NONE ||
      sun->astronomical_len != SUN_NONE)) {
    num = cstring_cp_fmt (sun->buf + offset, len, "Length of twilight:\n");
    len -= num;
    offset += num;
  }

  if (sun->civil_len != SUN_NONE && sun->day_len != SUN_NONE) {
    num = cstring_cp_fmt (sun->buf + offset, len,
      "                     civil  %5.2f hours\n", (sun->civil_len - sun->day_len) / 2.0);
    len -= num;
    offset += num;
  }

  if (sun->nautical_len != SUN_NONE && sun->day_len != SUN_NONE) {
    num = cstring_cp_fmt (sun->buf + offset, len,
      "                  nautical  %5.2f hours\n", (sun->nautical_len - sun->day_len) / 2.0);
    len -= num;
    offset += num;
  }

  if (sun->astronomical_len != SUN_NONE && sun->day_len != SUN_NONE) {
    num = cstring_cp_fmt (sun->buf + offset, len,
      "              astronomical  %5.2f hours\n", (sun->astronomical_len - sun->day_len) / 2.0);
    len -= num;
    offset += num;
  }

  if (sun->rise > 0 && sun->set > 0) {
    num = cstring_cp_fmt (sun->buf + offset, len,
      "Sun at south %s %s\n", lctime (sun->tm, (sun->rise + sun->set) / 2.0), sun->tm->tm_zone);
    len -= num;
    offset += num;
  }

  num = 0;

  switch (sun->civil_twilight) {
    case SUN_NONE:
      break;

    case SUN_OK:
      num = cstring_cp_fmt (sun->buf + offset, len,
        "       Civil twilight starts at %s, ends at %s %s\n",
          lctime_r (sun->tm, sun->civil_start, bufr, sizeof (bufr)),
          lctime_r (sun->tm, sun->civil_end,   bufs, sizeof (bufs)),
          sun->tm->tm_zone);
      break;

    case SUN_NEVER_DARKER_THAN_CIVIL_TWILIGHT:
      num = cstring_cp_fmt (sun->buf + offset, len, SUN_NEVER_DARKER_THAN_CIVIL_TWILIGHT_STRING "\n");
      break;

    case SUN_NEVER_AS_BRIGHT_AS_CIVIL_TWILIGHT:
      num = cstring_cp_fmt (sun->buf + offset, len,
        SUN_NEVER_AS_BRIGHT_AS_CIVIL_TWILIGHT_STRING "\n");
      break;
  }

  offset += num;
  len -= num;
  num = 0;

  switch (sun->nautical_twilight) {
    case SUN_NONE:
      break;

    case SUN_OK:
      num = cstring_cp_fmt (sun->buf + offset, len,
        "    Nautical twilight starts at %s, ends at %s %s\n",
          lctime_r (sun->tm, sun->nautical_start, bufr, sizeof (bufr)),
          lctime_r (sun->tm, sun->nautical_end,   bufs, sizeof (bufs)),
          sun->tm->tm_zone);
      break;

    case SUN_NEVER_DARKER_THAN_NAUTICAL_TWILIGHT:
      num = cstring_cp_fmt (sun->buf + offset, len, SUN_NEVER_DARKER_THAN_NAUTICAL_TWILIGHT_STRING "\n");
      break;

    case SUN_NEVER_AS_BRIGHT_AS_NAUTICAL_TWILIGHT :
      num = cstring_cp_fmt (sun->buf + offset, len, SUN_NEVER_AS_BRIGHT_AS_NAUTICAL_TWILIGHT_STRING "\n");
      break;
  }

  len -= num;
  offset += num;
  num = 0;

  switch (sun->astronomical_twilight) {
    case SUN_NONE:
      break;

    case SUN_OK:
      num = cstring_cp_fmt (sun->buf + offset, len,
        "Astronomical twilight starts at %s, ends at %s %s\n",
          lctime_r (sun->tm, sun->astronomical_start, bufr, sizeof (bufr)),
          lctime_r (sun->tm, sun->astronomical_end,   bufs, sizeof (bufs)),
          sun->tm->tm_zone);
      break;

    case SUN_NEVER_DARKER_THAN_ASTRONOMICAL_TWILIGHT:
      num = cstring_cp_fmt (sun->buf + offset, len, SUN_NEVER_DARKER_THAN_ASTRONOMICAL_TWILIGHT_STRING "\n");
      break;

    case SUN_NEVER_AS_BRIGHT_AS_ASTRONOMICAL_TWILIGHT:
      num = cstring_cp_fmt (sun->buf + offset, len, SUN_NEVER_AS_BRIGHT_AS_ASTRONOMICAL_TWILIGHT_STRING "\n");
      break;
  }

  return sun->buf;
}
