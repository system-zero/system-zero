#define REQUIRE_STDIO

#define REQUIRE_STD_MODULE
#define REQUIRE_CLOCK_SETTIME
#define REQUIRE_OPEN
#define REQUIRE_CLOSE
#define REQUIRE_IOCTL

#include <z/cenv.h>
#include "../../../zlibc/zlibc.c"

MODULE(clock);

// from <linux/rtc.h>
struct rtc_time {
  int tm_sec;
  int tm_min;
  int tm_hour;
  int tm_mday;
  int tm_mon;
  int tm_year;
  int tm_wday;
  int tm_yday;
  int tm_isdst;
};

#ifndef _IOC_WRITE
#define _IOC_WRITE  1U
#endif

#ifndef _IOC_READ
#define _IOC_READ  2U
#endif

#ifndef _IOC_SIZEBITS
#define _IOC_SIZEBITS  14
#endif

#define _IOC_NRBITS    8
#define _IOC_TYPEBITS  8
#define _IOC_NRSHIFT   0

#define _IOC_TYPESHIFT  (_IOC_NRSHIFT   + _IOC_NRBITS)
#define _IOC_SIZESHIFT  (_IOC_TYPESHIFT + _IOC_TYPEBITS)
#define _IOC_DIRSHIFT   (_IOC_SIZESHIFT + _IOC_SIZEBITS)

#define _IOC(dir,type,nr,size)  \
  (((dir)  << _IOC_DIRSHIFT)  | \
   ((type) << _IOC_TYPESHIFT) | \
   ((nr)   << _IOC_NRSHIFT)   | \
   ((size) << _IOC_SIZESHIFT))

#define _IOC_TYPECHECK(t) (sizeof(t))

#define _IOR(type,nr,size)  _IOC(_IOC_READ,(type),(nr),(_IOC_TYPECHECK(size)))
#define _IOW(type,nr,size)  _IOC(_IOC_WRITE,(type),(nr),(_IOC_TYPECHECK(size)))

#define RTC_RD_TIME   _IOR('p', 0x09, struct rtc_time) /* Read RTC time   */
#define RTC_SET_TIME  _IOW('p', 0x0a, struct rtc_time) /* Set RTC time    */

#define RTCDEVICE "/dev/rtc"

static VALUE clock_readhw (la_t *this) {
  int fd = open (RTCDEVICE, O_RDONLY);
  if (fd is -1) {
    La.set.Errno (this, errno);
    return NULL_VALUE;
  }

  int r;
  struct rtc_time rtc;
  if (-1 is (r = sys_ioctl (fd, RTC_RD_TIME, &rtc))) {
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

  int fd = open (RTCDEVICE, O_WRONLY);
  if (fd is -1) {
    La.set.Errno (this, sys_errno);
    return NOTOK_VALUE;
  }

  int r;
  if (-1 is (r = sys_ioctl (fd, RTC_SET_TIME, &rtc)))
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
  if (r isnot OK)
    La.set.Errno (this, sys_errno);

  return INT(r);
}

public int __init_clock_module__ (la_t *this) {
  __INIT_MODULE__(this);

  LaDefCFun lafuns[] = {
    { "clock_sethw",   PTR(clock_sethw), 1 },
    { "clock_readhw",  PTR(clock_readhw), 0 },
    { "clock_settime", PTR(clock_settime_), 1 },
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
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
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_clock_module__ (la_t *this) {
  (void) this;
}
