typedef long time_t;
typedef long clock_t;
typedef long suseconds_t;

struct timespec {
  time_t tv_sec;
  long tv_nsec;
};

struct timeval {
  time_t tv_sec;
  suseconds_t tv_usec;
};

struct tm {
  int tm_sec;
  int tm_min;
  int tm_hour;
  int tm_mday;
  int tm_mon;
  int tm_year;
  int tm_wday;
  int tm_yday;
  int tm_isdst;
  long tm_gmtoff;
  char *tm_zone;
};

struct timezone {
  int tz_minuteswest;
  int tz_dsttime;
};

#define CLOCK_REALTIME           0
#define CLOCK_MONOTONIC          1
#define CLOCK_PROCESS_CPUTIME_ID 2
#define CLOCK_THREAD_CPUTIME_ID  3
#define CLOCK_MONOTONIC_RAW      4
#define CLOCK_REALTIME_COARSE    5
#define CLOCK_MONOTONIC_COARSE   6
#define CLOCK_BOOTTIME           7
