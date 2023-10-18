#define REQUIRE_STRING_TO_HEXSTRING
#define REQUIRE_HEXSTRING_TO_STRING

#define REQUIRE_STDIO
#define REQUIRE_MODULE_COMPAT
#define REQUIRE_STRING_TYPE_COMPAT
#include <libc.h>

MODULE(convert);

static VALUE convert_string_to_hexstring (la_t *this, VALUE v_str) {
  ifnot (IS_STRING(v_str)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  string *s = AS_STRING (v_str);
  char *str = s->bytes;
  size_t len = (s->num_bytes * 2);
  string *sn = String.new (len + 1);

  int n = string_to_hexstring (sn->bytes, len + 1, str, s->num_bytes);
  if (n is NOTOK) {
    La.set.Errno (this, sys_errno);
    return NULL_VALUE;
  }

  sn->num_bytes = n;
  return STRING(sn);
}

static VALUE convert_hexstring_to_string (la_t *this, VALUE v_str) {
  ifnot (IS_STRING(v_str)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  string *s = AS_STRING (v_str);
  char *str = s->bytes;
  size_t len = (s->num_bytes / 2);
  string *sn = String.new (len + 1);

  int n = hexstring_to_string ((unsigned char *) sn->bytes, len + 1, str, s->num_bytes);
  if (n is NOTOK) {
    La.set.Errno (this, sys_errno);
    return NULL_VALUE;
  }

  sn->num_bytes = n;
  return STRING(sn);
}

public int __init_convert_module__ (la_t *this) {
  __INIT__(string);
  __INIT_MODULE__(this);

  LaDefCFun lafuns[] = {
    { "convert_string_to_hexstring", PTR(convert_string_to_hexstring), 1 },
    { "convert_hexstring_to_string", PTR(convert_hexstring_to_string), 1 },
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Convert = {
      string_to_hexstring : convert_string_to_hexstring,
      hexstring_to_string : convert_hexstring_to_string
     }
   );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_convert_module__ (la_t *this) {
  (void) this;
}
