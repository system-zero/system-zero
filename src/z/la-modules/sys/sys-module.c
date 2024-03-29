#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_SYS_TYPES

#define REQUIRE_STD_MODULE
#define REQUIRE_SYS_TYPE      DECLARE

#include <z/cenv.h>

MODULE(sys)

static VALUE sys_which (la_t *this, VALUE v_prog) {
  (void) this;
  ifnot (IS_STRING(v_prog)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  char *prog = AS_STRING_BYTES(v_prog);
  string *ex = Sys.which (prog, NULL);
  if (NULL is ex)
    return NULL_VALUE;

  return STRING(ex);
}

static VALUE sys_get (la_t *this, VALUE v_val) {
  (void) this;
  ifnot (IS_STRING(v_val)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  char *setting = AS_STRING_BYTES(v_val);
  char *v = Sys.get.env_value (setting);
  if (NULL is v)
    return NULL_VALUE;
  string *s = String.new_with (v);
  return STRING(s);
}

static VALUE sys_set (la_t *this, VALUE v_key, VALUE v_val, VALUE v_replace) {
  ifnot (IS_STRING(v_key)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_STRING(v_val)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_INT(v_replace)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer");

  char *key = AS_STRING_BYTES(v_key);
  char *val = AS_STRING_BYTES(v_val);
  int replace = AS_INT(v_replace);
  Sys.set.env_as (val, key, replace);
  return OK_VALUE;
}

static VALUE sys_battery_info (la_t *this) {
  (void) this;
  string *info = Sys.get.battery_info ();
  if (NULL is info) return NULL_VALUE;
  return STRING(info);
}

public int __init_sys_module__ (la_t *this) {
  __INIT_MODULE__(this);
  __INIT__(sys);

  LaDefCFun lafuns[] = {
    { "sys_set",         PTR(sys_set), 3 },
    { "sys_get",         PTR(sys_get), 1 },
    { "sys_which",       PTR(sys_which), 1 },
    { "sys_battery_info", PTR(sys_battery_info), 0 },
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Sys = {
      "set"   : sys_set,
      "get"   : sys_get,
      "which" : sys_which,
      "battery_info" : sys_battery_info
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
