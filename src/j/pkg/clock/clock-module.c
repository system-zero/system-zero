#define REQUIRE_CLOCK_SETTIME
#define REQUIRE_OPEN
#define REQUIRE_CLOSE
#define REQUIRE_IOCTL
#define REQUIRE_RTC_H

#define REQUIRE_STDIO
#define REQUIRE_VMAP_TYPE_COMPAT
#define REQUIRE_MODULE_COMPAT
#include <libc.h>

MODULE(clock);

static VALUE clock_readhw (la_t *this) {
  int fd = sys_open (RTCDEVICE, O_RDONLY);
  if (fd == -1) {
    La.set.Errno (this, sys_errno);
    return NULL_VALUE;
  }

  int r;
  struct rtc_time rtc;
  if (-1 == (r = sys_ioctl (fd, RTC_RD_TIME, &rtc))) {
    La.set.Errno (this, sys_errno);
    return NULL_VALUE;
  }

  sys_close (fd);

  Vmap_t *m = Vmap.new (16);
  La.map.set_value (this, m, "tm_sec",   INT(rtc.tm_sec),   1);
  La.map.set_value (this, m, "tm_min",   INT(rtc.tm_min),   1);
  La.map.set_value (this, m, "tm_hour",  INT(rtc.tm_hour),  1);
  La.map.set_value (this, m, "tm_mday",  INT(rtc.tm_mday),  1);
  La.map.set_value (this, m, "tm_mon",   INT(rtc.tm_mon),   1);
  La.map.set_value (this, m, "tm_year",  INT(rtc.tm_year),  1);
  La.map.set_value (this, m, "tm_wday",  INT(rtc.tm_wday),  1);
  La.map.set_value (this, m, "tm_yday",  INT(rtc.tm_yday),  1);
  La.map.set_value (this, m, "tm_isdst", INT(rtc.tm_isdst), 1);
  return MAP(m);
}

static struct rtc_time map_to_rtc (Vmap_t *map, struct rtc_time rtc) {
  VALUE *v;
  rtc.tm_sec   = AS_INT((*(v = (VALUE *) Vmap.get (map, "tm_sec"))));
  rtc.tm_min   = AS_INT((*(v = (VALUE *) Vmap.get (map, "tm_min"))));
  rtc.tm_hour  = AS_INT((*(v = (VALUE *) Vmap.get (map, "tm_hour"))));
  rtc.tm_mday  = AS_INT((*(v = (VALUE *) Vmap.get (map, "tm_mday"))));
  rtc.tm_mon   = AS_INT((*(v = (VALUE *) Vmap.get (map, "tm_mon"))));
  rtc.tm_year  = AS_INT((*(v = (VALUE *) Vmap.get (map, "tm_year"))));
  rtc.tm_wday  = AS_INT((*(v = (VALUE *) Vmap.get (map, "tm_wday"))));
  rtc.tm_yday  = AS_INT((*(v = (VALUE *) Vmap.get (map, "tm_yday"))));
  rtc.tm_isdst = AS_INT((*(v = (VALUE *) Vmap.get (map, "tm_isdst"))));
  return rtc;
}

static VALUE clock_sethw (la_t *this, VALUE v_rtc) {
  ifnot (IS_MAP(v_rtc)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a rtc map");

  Vmap_t *tm = AS_MAP (v_rtc);

  struct rtc_time rtc = {0};
  rtc = map_to_rtc (tm, rtc);
  rtc.tm_year -= 1900;
  rtc.tm_mon--;

  int fd = sys_open (RTCDEVICE, O_WRONLY);
  if (fd == -1) {
    La.set.Errno (this, sys_errno);
    return NOTOK_VALUE;
  }

  int r;
  if (-1 == (r = sys_ioctl (fd, RTC_SET_TIME, &rtc)))
    La.set.Errno (this, sys_errno);

  sys_close (fd);
  return INT(r);
}

static VALUE clock_settime_ (la_t *this, VALUE v_sec) {
  ifnot (IS_INT(v_sec)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer");
  struct timespec ts;
  ts.tv_sec = AS_INT(v_sec);
  ts.tv_nsec = 0;

  int r = sys_clock_settime (CLOCK_REALTIME, &ts);
  if (r != 0)
    La.set.Errno (this, sys_errno);

  return INT(r);
}

public int __init_clock_module__ (la_t *this) {
  __INIT__(vmap);
  __INIT_MODULE__(this);

  LaDefCFun lafuns[] = {
    { "clock_sethw",   PTR(clock_sethw), 1 },
    { "clock_readhw",  PTR(clock_readhw), 0 },
    { "clock_settime", PTR(clock_settime_), 1 },
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK != (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Clock = {
      sethw   : clock_sethw,
      readhw  : clock_readhw,
      settime : clock_settime
     }
   );

  err = La.eval_string (this, evalString);
  if (err != LA_OK) return err;
  return LA_OK;
}

public void __deinit_clock_module__ (la_t *this) {
  (void) this;
}
