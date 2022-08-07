#define REQUIRE_STDIO
#define REQUIRE_TIME

#define REQUIRE_ERROR_TYPE  DECLARE
#define REQUIRE_VMAP_TYPE   DECLARE
#define REQUIRE_STD_MODULE

#include <z/cenv.h>

MODULE(time);

typedef struct retval_t {
  VALUE value;
  const char *msg;
  int print_error;
  void *userdata;
} retval_t;

#define Notok(...) (retval_t) {              \
  .value = NOTOK_VALUE,                      \
  .msg = NULL,                               \
  .print_error = 0,                          \
  .userdata = NULL,                          \
  __VA_ARGS__}

#if 0
#define Ok(...) (retval_t) {                 \
  .value = OK_VALUE,                         \
  .msg = NULL,                               \
  .print_error = 0,                          \
  .userdata = NULL,                          \
  __VA_ARGS__}
#endif

#define True(...) (retval_t) {               \
  .value = TRUE_VALUE,                       \
  .msg = NULL,                               \
  .print_error = 0,                          \
  .userdata = NULL,                          \
  __VA_ARGS__}

#define False(...) (retval_t) {              \
  .value = FALSE_VALUE,                      \
  .msg = NULL,                               \
  .print_error = 0,                          \
  .userdata = NULL,                          \
  __VA_ARGS__}

#if 0
#define Null(...) (retval_t) {               \
  .value = NULL_VALUE),                      \
  .msg = NULL,                               \
  .print_error = 0,                          \
  .userdata = NULL,                          \
  __VA_ARGS__}
#endif

// #define RETURN_OK(...) return value(this, Ok(__VA_ARGS__))
// #define RETURN_NOTOK(...) return error(this, Notok(__VA_ARGS__))
// #define PRINT_AND_RETURN_NOTOK(...) RETURN_NOTOK (.print_error = 1, __VA_ARGS__)
#define PRINT_AND_RETURN_FALSE(...) RETURN_FALSE (.print_error = 1, __VA_ARGS__)
#define RETURN_FALSE(...) return error (this, False(__VA_ARGS__))
#define RETURN_TRUE(...)  return value (this, True(__VA_ARGS__))

typedef struct tm_value {
  VALUE
    *v_sec,
    *v_min,
    *v_hour,
    *v_mday,
    *v_mon,
    *v_year;
} tm_value;

#define TIME_TM_ERROR(...) error (this, Notok(.print_error = 1, __VA_ARGS__))

static VALUE value (la_t *this, retval_t v) {
  Vmap_t *m = Vmap.new (4);
  La.map.set_value (this, m, "value", v.value, 1);
  La.map.set_value (this, m, "msg",
    (NULL is v.msg ? NULL_VALUE : STRING_NEW(v.msg)), 1);
  return MAP(m);
}

static VALUE error (la_t *this, retval_t v) {
  if (v.print_error) {
    ifnot (NULL is v.msg) fprintf (stderr, "%s\n", v.msg);
    if (errno and v.print_error) fprintf (stderr, "%s\n", Error.errno_string (errno));
  }

  return value (this, v);
}

static VALUE time_year_isleap (la_t *this, VALUE v_year) {
  ifnot (IS_INT(v_year)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer");
  int year = AS_INT(v_year);
  if ((0 is year % 4 and 0 isnot (year % 100)) or
      (0 is (year % 400)))
    return INT(1);
  return INT(0);
}

static int validate_tm_value (la_t *this, Vmap_t *tm, retval_t *vp) {
  tm_value *v = vp->userdata;

  v->v_sec = Vmap.get (tm, "tm_sec");
  if (v->v_sec is NULL) {
    vp->value = TIME_TM_ERROR(.msg = "tm map has no member tm_sec");
    return NOTOK;
  }

  v->v_min = Vmap.get (tm, "tm_min");
  if (v->v_min is NULL) {
    vp->value = TIME_TM_ERROR(.msg = "tm map has no member tm_v_min");
    return NOTOK;
  }

  v->v_hour = Vmap.get (tm, "tm_hour");
  if (v->v_hour is NULL) {
    vp->value = TIME_TM_ERROR(.msg = "tm map has no member tm_hour");
    return NOTOK;
  }

  v->v_mday = Vmap.get (tm, "tm_mday");
  if (v->v_mday is NULL) {
    vp->value = TIME_TM_ERROR(.msg = "tm map has no member tm_mday");
    return NOTOK;
  }

  v->v_mon = Vmap.get (tm, "tm_mon");
  if (v->v_mon is NULL) {
    vp->value = TIME_TM_ERROR(.msg = "tm map has no member tm_mon");
    return NOTOK;
  }

  v->v_year = Vmap.get (tm, "tm_year");
  if (v->v_year is NULL) {
    vp->value = TIME_TM_ERROR(.msg = "tm map has no member tm_year");
    return NOTOK;
  }

  return OK;
}

static VALUE time_validate_tm (la_t *this, VALUE v_tm) {
  ifnot (IS_MAP(v_tm))
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting a tm map");

  Vmap_t *tm = AS_MAP (v_tm);
  tm_value tm_val;
  retval_t retval;
  retval.userdata = &tm_val;
  int r = validate_tm_value (this, tm, &retval);
  if (r is NOTOK) return retval.value;

  int
    sec  = AS_INT((*tm_val.v_sec)),
    min  = AS_INT((*tm_val.v_min)),
    hour = AS_INT((*tm_val.v_hour)),
    mday = AS_INT((*tm_val.v_mday)),
    mon  = AS_INT((*tm_val.v_mon)),
    year = AS_INT((*tm_val.v_year));

  ifnot (year) PRINT_AND_RETURN_FALSE(.msg = "0: is not a valid year");

  if (0 > mon or mon > 11) PRINT_AND_RETURN_FALSE(.msg = STR_FMT("%d is not a valid month", mon));

  int m_ar[] = {31, 28 + AS_INT(time_year_isleap (this, INT(year))),
    31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  if (1 > mday or m_ar[mon] < mday)
    PRINT_AND_RETURN_FALSE(.msg = STR_FMT("%d: mday is not a valid day", mday));

  if ((hour > 23 || min > 59 || sec > 59) ||
      (hour < 0  || min < 0  || sec < 0 ))
    PRINT_AND_RETURN_FALSE(.msg = "not a valid hour/minutes/second format");

   RETURN_TRUE();
}

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

static struct tm map_to_tm (Vmap_t *map, struct tm stm) {
  VALUE *v;
  stm.tm_sec   = AS_INT((*(v = (VALUE *) Vmap.get (map, "tm_sec"))));
  stm.tm_min   = AS_INT((*(v = (VALUE *) Vmap.get (map, "tm_min"))));
  stm.tm_hour  = AS_INT((*(v = (VALUE *) Vmap.get (map, "tm_hour"))));
  stm.tm_mday  = AS_INT((*(v = (VALUE *) Vmap.get (map, "tm_mday"))));
  stm.tm_mon   = AS_INT((*(v = (VALUE *) Vmap.get (map, "tm_mon"))));
  stm.tm_year  = AS_INT((*(v = (VALUE *) Vmap.get (map, "tm_year"))));
  stm.tm_wday  = AS_INT((*(v = (VALUE *) Vmap.get (map, "tm_wday"))));
  stm.tm_yday  = AS_INT((*(v = (VALUE *) Vmap.get (map, "tm_yday"))));
  stm.tm_isdst = AS_INT((*(v = (VALUE *) Vmap.get (map, "tm_isdst"))));
  return stm;
}

static VALUE time_to_seconds (la_t *this, VALUE v_tm) {
  ifnot (IS_MAP(v_tm))
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting a tm map");

  struct tm tm_p = {0};
  Vmap_t *_tm = AS_MAP (v_tm);

  tm_p = map_to_tm (_tm, tm_p);

  La.set.Errno (this, 0);

  time_t t = mktime (&tm_p);

  if (t is -1) {
    La.set.Errno (this, errno);
    return NOTOK_VALUE;
  }

  return INT(t);
}

/* some code is from a personal patch to Dictu */
static VALUE time_format (la_t *this, VALUE v_fmt, VALUE v_tm) {
  ifnot (IS_STRING(v_fmt)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");

  struct tm stm = {0};

  ifnot (IS_MAP(v_tm)) {
    ifnot (IS_NULL(v_tm))
      THROW(LA_ERR_TYPE_MISMATCH, "awaiting a tm map");
    time_t t = time (NULL);
    struct tm *tmp;

    La.set.Errno (this, 0);
    tmp = localtime (&t);

    if (tmp is NULL) {
      La.set.Errno (this, errno);
      return NULL_VALUE;
    }
    stm = *tmp;

  } else {
    Vmap_t *map = AS_MAP (v_tm);
    stm = map_to_tm (map, stm);
  }

  string *s_fmt = AS_STRING(v_fmt);
  ifnot (s_fmt->num_bytes) {
    string *res = String.new_with ("");
    return STRING(res);
  }

  size_t len = (s_fmt->num_bytes > 128 ? s_fmt->num_bytes * 4 : 128);
  char buffer[len], *ptr = buffer;

  /*
   * strtime returns 0 when it fails to write - this would be due to the buffer
   * not being large enough. In that instance we double the buffer length until
   * there is a big enough buffer.
   */

  /* however is not guaranteed that 0 indicates a failure (`man strftime' says so).
   * So we might want to catch up the eternal loop, by using a maximum iterator.
   */

  int max_iterations = 8;  // maximum 65536 bytes with the default 128 len,
                           // more if the given string is > 128
  int iterator = 0;
  string *res = NULL;

  while (strftime (ptr, sizeof(char) * len, s_fmt->bytes, &stm) == 0) {
    if (++iterator > max_iterations) {
      res = String.new_with ("");
      return STRING(res);
    }

    len *= 2;

    if (buffer is ptr)
       ptr = Alloc (len);
     else
       ptr = Realloc (ptr, len);
  }

  res = String.new_with (ptr);
  return STRING(res);
}

public int __init_time_module__ (la_t *this) {
  __INIT_MODULE__(this);
  __INIT__ (vmap);
  __INIT__ (error);

  LaDefCFun lafuns[] = {
    { "time_now",            PTR(time_now), 0 },
    { "time_gmt",            PTR(time_gmt), 1 },
    { "time_local",          PTR(time_local), 1 },
    { "time_format",         PTR(time_format), 2 },
    { "time_to_string",      PTR(time_to_string), 1 },
    { "time_to_seconds",     PTR(time_to_seconds), 1 },
    { "time_validate_tm",    PTR(time_validate_tm), 1 },
    { "time_year_isleap",    PTR(time_year_isleap), 1 },
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Time = {
      now : time_now,
      gmt : time_gmt,
      local : time_local,
      format : time_format,
      to_string : time_to_string,
      to_seconds : time_to_seconds,
      validate_tm : time_validate_tm,
      year : {
        isleap : time_year_isleap
      }
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
