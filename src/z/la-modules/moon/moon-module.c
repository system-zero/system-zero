#define REQUIRE_STDIO
#define REQUIRE_TIME
#define REQUIRE_STD_MODULE
#define REQUIRE_MOON_TYPE

#include <z/cenv.h>

MODULE(moon)

static VALUE moon_phase (la_t *this) {
  int verbose = GET_OPT_VERBOSE_WITH(0);
  int year = GET_OPT_YEAR();
  int month = GET_OPT_MONTH();
  int day = GET_OPT_DAY();

  time_t now = time (NULL);
  struct tm *tm = localtime (&now);

  if (year < 0)
    tm->tm_year += 1900;
  else
    tm->tm_year = year;

  if (month >= 0)
    tm->tm_mon = month;

  if (day > 0)
    tm->tm_mday = day;

  tm->tm_hour = 12;

  FILE *out_fp = GET_OPT_OUT_STREAM();

  char phase[MOON_MAXLEN_REPORT];
  MoonPhase (tm, phase);

  VALUE val;

  if (verbose and NULL isnot out_fp) {
    fprintf (out_fp, "%s", phase);
    val = NULL_VALUE;
  } else {
    string *s = String.new_with (phase);
    val = STRING(s);
  }

  return val;
}

public int __init_moon_module__ (la_t *this) {
  __INIT_MODULE__(this);

  LaDefCFun lafuns[] = {
    {"moon_phase", PTR(moon_phase), 0},
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Moon = {
      phase : moon_phase
    }
  );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_moon_module__ (la_t *this) {
  (void) this;
  return;
}

