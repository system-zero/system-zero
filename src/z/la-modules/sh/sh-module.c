#define REQUIRE_STDIO

#define REQUIRE_VMAP_TYPE    DONOT_DECLARE
#define REQUIRE_VSTRING_TYPE DONOT_DECLARE
#define REQUIRE_STRING_TYPE  DONOT_DECLARE
#define REQUIRE_SH_TYPE      DECLARE
#define REQUIRE_LA_TYPE      DECLARE

#include <z/cenv.h>

static VALUE sh_exec (la_t *this, VALUE v_sh, VALUE v_command) {
  (void) this;
  ifnot (IS_OBJECT(v_sh)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a sh object");
  ifnot (IS_STRING(v_command)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");

  object *o = AS_OBJECT(v_sh);
  sh_t *sh = (sh_t *) AS_PTR(o->value);
  char *command = AS_STRING_BYTES(v_command);
  Sh.release_list (sh);
  return INT(Sh.exec (sh, command));
}

static VALUE sh_release (la_t *this, VALUE v_sh) {
  (void) this;
  ifnot (IS_OBJECT(v_sh)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a sh object");

  object *o = AS_OBJECT(v_sh);
  sh_t *sh = (sh_t *) AS_PTR(o->value);
  Sh.release (sh);
  return OK_VALUE;
}

static VALUE sh_new (la_t *this) {
  (void) this;

  sh_t *sh = Sh.new ();
  VALUE v = OBJECT(sh);
  object *o = La.object.new (sh_release, NULL, v);
  return OBJECT(o);
}

#define EvalString(...) #__VA_ARGS__

public int __init_sh_module__ (la_t *this) {
  __INIT_MODULE__(this);
  __INIT__(sh);

  (void) vstringType;
  (void) vmapType;
  (void) stringType;

  LaDefCFun lafuns[] = {
    { "sh_new",             PTR(sh_new), 0 },
    { "sh_exec",            PTR(sh_exec), 2 },
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Sh =  {};

    Type Shell {
    private
      "instance",

      "init" : func {
        this.instance = sh_new ()
      },

    public
      "exec" : func (command) {
        return sh_exec (this.instance, command)
      }
    }
  );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_sh_module__ (la_t *this) {
  (void) this;
  return;
}
