#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_SYS_TYPES

#define REQUIRE_VMAP_TYPE     DONOT_DECLARE
#define REQUIRE_STRING_TYPE   DECLARE
#define REQUIRE_SYS_TYPE      DECLARE
#define REQUIRE_LA_TYPE       DECLARE

#include <z/cenv.h>

static VALUE sys_which (la_t *this, VALUE v_prog) {
  (void) this;
  ifnot (IS_STRING(v_prog)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  char *prog = AS_STRING_BYTES(v_prog);
  string *ex = Sys.which (prog, NULL);
  if (NULL is ex)
    return NULL_VALUE;

  return STRING(ex);
}

#define EvalString(...) #__VA_ARGS__

public int __init_sys_module__ (la_t *this) {
  __INIT_MODULE__(this);
  __INIT__(sys);
  __INIT__(string);

  (void) vmapType;

  LaDefCFun lafuns[] = {
    { "which",          PTR(sys_which), 1 },
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Sys = {
      "which" : which,
     }
  );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;

  Sys.init_environment (SysEnvOpts());

  return LA_OK;
}

public void __deinit_sys_module__ (la_t *this) {
  (void) this;
  __deinit_sys__ ();
  return;
}
