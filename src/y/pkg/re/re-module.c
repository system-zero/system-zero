#define REQUIRE_STDIO

#define REQUIRE_STD_MODULE
#define REQUIRE_RE_TYPE      DECLARE
#define REQUIRE_STRING_TYPE  DECLARE

#include <z/cenv.h>

MODULE(re);

static VALUE re_match (la_t *this, VALUE v_str, VALUE v_pat) {
  ifnot (IS_STRING(v_str)) THROW(LA_ERR_TYPE_MISMATCH, "re_match(): awaiting a string");
  ifnot (IS_STRING(v_pat)) THROW(LA_ERR_TYPE_MISMATCH, "re_match(): awaiting a string");

  string *str = AS_STRING(v_str);
  char *pat = AS_STRING_BYTES(v_pat);

  re_t *re = Re.new (pat, 0, RE_MAX_NUM_CAPTURES, Re.compile);
  int r = Re.exec (re, str->bytes, str->num_bytes);
  Re.release (re);
  return INT(r >= 0);
}

public int __init_re_module__ (la_t *this) {
  __INIT_MODULE__(this);
  __INIT__(re);
  __INIT__(string);

  LaDefCFun lafuns[] = {
    { "re_match", PTR(re_match), 2 },
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Re = {
      match : re_match
    }
  );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_re_module__ (la_t *this) {
  (void) this;
}
