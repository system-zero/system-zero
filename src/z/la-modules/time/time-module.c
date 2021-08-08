#define REQUIRE_STDIO
#define REQUIRE_TIME

#define REQUIRE_VMAP_TYPE     DONOT_DECLARE
#define REQUIRE_STRING_TYPE   DECLARE
#define REQUIRE_LA_TYPE       DECLARE

#include <z/cenv.h>

static VALUE time_now (la_t *this) {
  (void) this;
  return INT(time (NULL));
}

static VALUE time_to_string (la_t *this, VALUE v_tim) {
  (void) this;
  ifnot (IS_INT(v_tim))
    ifnot (IS_NULL(v_tim))
      THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer or null");

  time_t tim = 0;
  if (IS_INT(v_tim))
    tim = (time_t) AS_INT(v_tim);
  else
    tim = time (NULL);

  string *s = String.new_with (ctime (&tim));
  String.clear_at (s, -1);
  return STRING(s);
}

static VALUE time_to_tm (la_t *this, time_t t, int local_or_gmt) {
  struct tm *tmp;
  struct tm *tim;

  La.set.Errno (this, 0);
  ifnot (local_or_gmt)
    tmp = localtime (&t);
  else
    tmp = gmtime (&t);

  if (tmp is NULL) {
    tim = tmp;
    La.set.Errno (this, errno);
    return NULL_VALUE;
  }

  tim = tmp;

  Vmap_t *m = Vmap.new (16);
  La.map.set_value (this, m, "tm_sec",   INT(tim->tm_sec), 1);
  La.map.set_value (this, m, "tm_min",   INT(tim->tm_min), 1);
  La.map.set_value (this, m, "tm_hour",  INT(tim->tm_hour), 1);
  La.map.set_value (this, m, "tm_mday",  INT(tim->tm_mday), 1);
  La.map.set_value (this, m, "tm_mon",   INT(tim->tm_mon), 1);
  La.map.set_value (this, m, "tm_year",  INT(tim->tm_year), 1);
  La.map.set_value (this, m, "tm_wday",  INT(tim->tm_wday), 1);
  La.map.set_value (this, m, "tm_yday",  INT(tim->tm_yday), 1);
  La.map.set_value (this, m, "tm_isdst", INT(tim->tm_isdst), 1);
  return MAP(m);
}

static VALUE time_local (la_t *this, VALUE v_tim) {
  ifnot (IS_INT(v_tim))
    ifnot (IS_NULL(v_tim))
      THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer or null");

  time_t tim = 0;
  if (IS_INT(v_tim))
    tim = (time_t) AS_INT(v_tim);
  else
    tim = time (NULL);

  return time_to_tm (this, tim, 0);
}

static VALUE time_gmt (la_t *this, VALUE v_tim) {
  ifnot (IS_INT(v_tim))
    ifnot (IS_NULL(v_tim))
      THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer or null");

  time_t tim = 0;
  if (IS_INT(v_tim))
    tim = (time_t) AS_INT(v_tim);
  else
    tim = time (NULL);

  return time_to_tm (this, tim, 1);
}

static VALUE time_to_seconds (la_t *this, VALUE v_tm) {
  ifnot (IS_MAP(v_tm))
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting a tm map");

  struct tm tm_p;
  Vmap_t *_tm = AS_MAP (v_tm);

  VALUE *v;
  tm_p.tm_sec   = AS_INT((*(v = (VALUE *) Vmap.get (_tm, "tm_sec"))));
  tm_p.tm_min   = AS_INT((*(v = (VALUE *) Vmap.get (_tm, "tm_min"))));
  tm_p.tm_hour  = AS_INT((*(v = (VALUE *) Vmap.get (_tm, "tm_hour"))));
  tm_p.tm_mday  = AS_INT((*(v = (VALUE *) Vmap.get (_tm, "tm_mday"))));
  tm_p.tm_mon   = AS_INT((*(v = (VALUE *) Vmap.get (_tm, "tm_mon"))));
  tm_p.tm_year  = AS_INT((*(v = (VALUE *) Vmap.get (_tm, "tm_year"))));
  tm_p.tm_wday  = AS_INT((*(v = (VALUE *) Vmap.get (_tm, "tm_wday"))));
  tm_p.tm_yday  = AS_INT((*(v = (VALUE *) Vmap.get (_tm, "tm_yday"))));
  tm_p.tm_isdst = AS_INT((*(v = (VALUE *) Vmap.get (_tm, "tm_isdst"))));

  La.set.Errno (this, 0);

  time_t t = mktime (&tm_p);
  if (t is -1) {
    La.set.Errno (this, errno);
    return NOTOK_VALUE;
  }

  return INT(t);
}

#define EvalString(...) #__VA_ARGS__

public int __init_time_module__ (la_t *this) {
  __INIT_MODULE__(this);
  __INIT__(string);

  LaDefCFun lafuns[] = {
    { "time_now",            PTR(time_now), 0 },
    { "time_gmt",            PTR(time_gmt), 1 },
    { "time_local",          PTR(time_local), 1 },
    { "time_to_string",      PTR(time_to_string), 1 },
    { "time_to_seconds",     PTR(time_to_seconds), 1 },
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Time = {
      "now" : time_now,
      "gmt" : time_gmt,
      "local" : time_local,
      "to_string" : time_to_string,
      "to_seconds" : time_to_seconds
    }
 );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_time_module__ (la_t *this) {
  (void) this;
  return;
}
