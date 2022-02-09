#define REQUIRE_STDIO

#define REQUIRE_STD_MODULE

#include <z/cenv.h>

#include <z/sun.h>
MODULE(sun)

static VALUE sun_compute (la_t *this) {
  int verbose = GET_OPT_VERBOSE_WITH(0);
  int year = GET_OPT_YEAR();
  int month = GET_OPT_MONTH();
  int day = GET_OPT_DAY();

  VALUE v_utc = La.get.qualifier (this, "utc", INT(0));
  ifnot (IS_INT(v_utc))
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");
  int utc = AS_INT(v_utc);

  VALUE v_lon = La.get.qualifier (this, "lon", NUMBER(0.0));
  ifnot (IS_NUMBER(v_lon))
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting a number qualifier");
  double lon = AS_NUMBER(v_lon);

  VALUE v_lat = La.get.qualifier (this, "lat", NUMBER(0.0));
  ifnot (IS_NUMBER(v_lat))
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting a number qualifier");
  double lat = AS_NUMBER(v_lat);

  FILE *out_fp = GET_OPT_OUT_STREAM();

  sun_t new;
  new.year = year;
  new.month = month;
  new.day = day;
  new.utc = utc;
  new.lat = lat;
  new.lon = lon;
  new.tm = NULL;
  sun_t *sun = SunInit (&new);

  SunCompute (sun);
  SunToString (sun);

  if (verbose and NULL isnot out_fp) {
    fprintf (out_fp, "%s", sun->buf);
    return NULL_VALUE;
  }

  string *s = String.new_with (sun->buf);
  return STRING(s);
}

#define EvalString(...) #__VA_ARGS__

public int __init_sun_module__ (la_t *this) {
  __INIT_MODULE__(this);

  LaDefCFun lafuns[] = {
    {"sun_compute", PTR(sun_compute), 0},
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Sun = {
      compute : sun_compute
    }
  );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_sun_module__ (la_t *this) {
  (void) this;
  return;
}

