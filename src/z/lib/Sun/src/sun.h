#ifndef SUN_HDR
#define SUN_HDR

#define SUN_OK 0
#define SUN_NONE 255

#define SUN_IS_ABOVE_HORIZON  1
#define SUN_IS_BELOW_HORIZON -1
#define SUN_IS_ABOVE_HORIZON_STRING "sun is above horizon"
#define SUN_IS_BELOW_HORIZON_STRING "sun is below horizon"

#define SUN_NEVER_DARKER_THAN_CIVIL_TWILIGHT   1
#define SUN_NEVER_AS_BRIGHT_AS_CIVIL_TWILIGHT -1
#define SUN_NEVER_DARKER_THAN_CIVIL_TWILIGHT_STRING "Never darker than civil twilight"
#define SUN_NEVER_AS_BRIGHT_AS_CIVIL_TWILIGHT_STRING "Never as bright as civil twilight"

#define SUN_NEVER_DARKER_THAN_NAUTICAL_TWILIGHT   1
#define SUN_NEVER_AS_BRIGHT_AS_NAUTICAL_TWILIGHT -1
#define SUN_NEVER_DARKER_THAN_NAUTICAL_TWILIGHT_STRING "Never darker than nautical twilight"
#define SUN_NEVER_AS_BRIGHT_AS_NAUTICAL_TWILIGHT_STRING "Never as bright as nautical twilight"

#define SUN_NEVER_DARKER_THAN_ASTRONOMICAL_TWILIGHT   1
#define SUN_NEVER_AS_BRIGHT_AS_ASTRONOMICAL_TWILIGHT -1
#define SUN_NEVER_DARKER_THAN_ASTRONOMICAL_TWILIGHT_STRING "Never darker than astronomical twilight"
#define SUN_NEVER_AS_BRIGHT_AS_ASTRONOMICAL_TWILIGHT_STRING "Never as bright as astronomical twilight"

#define SUN_BUF_LEN 1023

typedef struct sun_t {
  double
    lon,
    lat,
    set,
    rise,
    day_len,
    civil_len,
    civil_start,
    civil_end,
    nautical_len,
    nautical_start,
    nautical_end,
    astronomical_len,
    astronomical_start,
    astronomical_end,
    sun_at_south;

  int
    utc,
    day,
    year,
    month,
    sun_pos,
    civil_twilight,
    nautical_twilight,
    astronomical_twilight;

  struct tm *tm;

  char buf[SUN_BUF_LEN];

} sun_t;

sun_t *SunInit (sun_t *);
sun_t *SunCompute (sun_t *);
char  *SunToString (sun_t *);

#endif /* SUN_HDR */
