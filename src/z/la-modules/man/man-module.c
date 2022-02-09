#define REQUIRE_STDIO

#define REQUIRE_STD_MODULE

#include <z/cenv.h>
#include <z/man.h>

MODULE(man)

#define MAN_DEFAULT_SECTION "2"

#include <stdarg.h>

static int man_output_string (man_t *man, const char *fmt, ...) {
  string *s = (string *) man->userData;

  char buf[256];
  va_list ap; va_start(ap, fmt);
  int len = vsnprintf (buf, 256, fmt, ap);
  va_end(ap);
  String.append_with_len (s, buf, len);
  return 0;
}

static VALUE man_page (la_t *this, VALUE v_page, VALUE v_section) {
  ifnot (IS_STRING(v_page)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  char *page = AS_STRING_BYTES(v_page);

  man_t new;
  man_t *man = manInit (&new);
  man->output_fp = stdout;

  string *s = NULL;

  int verbose = GET_OPT_VERBOSE_WITH(0);

  ifnot (verbose) {
    s = String.new (32);
    man->output_string = man_output_string;
    man->userData = s;
  }

  if (IS_NULL(v_section))
    manPage (man, page, MAN_DEFAULT_SECTION);
  else {
    ifnot (IS_STRING(v_section)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
    manPage (man, page, AS_STRING_BYTES(v_section));
  }

  ifnot (verbose)
    return STRING(s);

  return NULL_VALUE;
}

#define EvalString(...) #__VA_ARGS__

public int __init_man_module__ (la_t *this) {
  __INIT_MODULE__(this);

  LaDefCFun lafuns[] = {
    {"man_page", PTR(man_page), 2},
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Man = {
      page : man_page
    }
  );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_man_module__ (la_t *this) {
  (void) this;
  return;
}
