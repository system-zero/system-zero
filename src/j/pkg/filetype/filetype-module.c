#define REQUIRE_STDIO
#define REQUIRE_STD_MODULE

#include <z/cenv.h>
#include <z/filetype.h>

MODULE(filetype)

static VALUE file_type (la_t *this, VALUE v_file) {
  ifnot (IS_STRING(v_file)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  char *file = AS_STRING_BYTES(v_file);

  char ft[MAXLEN_FILETYPE];
  int r = filetype (file, ft);
  if (r is FILETYPE_ERROR)
    return NULL_VALUE;

  string *s = String.new_with (ft);
  return STRING(s);
}

public int __init_filetype_module__ (la_t *this) {
  __INIT_MODULE__(this);

  LaDefCFun lafuns[] = {
    {"filetype", PTR(file_type), 1},
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    if is_defined ("File") {
      append filetype in File as "type";
    } else {
      public const File = {
        type : filetype
      }
    }
  );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;

  return LA_OK;
}

public void __deinit_filetype_module__ (la_t *this) {
  (void) this;
  return;
}
