#define REQUIRE_STDIO

#define REQUIRE_STD_MODULE
#define REQUIRE_CSTRING_TYPE DECLARE
#define REQUIRE_SH_TYPE      DECLARE

#include <z/cenv.h>

MODULE(sh)

#define IS_SH(__v__)({ int _r_ = 0; \
  if (IS_OBJECT(__v__)) { object *_o_ = AS_OBJECT(__v__); _r_ = Cstring.eq (_o_->name, "ShType");}\
  _r_; })

#define AS_SH(__v__)\
({object *_o_ = AS_OBJECT(__v__); sh_t *_s_ = (sh_t *) AS_OBJECT (_o_->value); _s_;})

static VALUE sh_exec (la_t *this, VALUE v_sh, VALUE v_command) {
  (void) this;
  ifnot (IS_SH(v_sh)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a sh object");
  ifnot (IS_STRING(v_command)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");

  sh_t *sh = AS_SH(v_sh);
  char *command = AS_STRING_BYTES(v_command);
  Sh.release_list (sh);
  return INT(Sh.exec (sh, command));
}

static VALUE sh_release (la_t *this, VALUE v_sh) {
  (void) this;
  ifnot (IS_SH(v_sh)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a sh object");
  sh_t *sh = AS_SH(v_sh);
  Sh.release (sh);
  return OK_VALUE;
}

static VALUE sh_new (la_t *this) {
  (void) this;

  sh_t *sh = Sh.new ();
  VALUE v = OBJECT(sh);
  object *o = La.object.new (sh_release, NULL, "ShType", v);
  return OBJECT(o);
}

#define EvalString(...) #__VA_ARGS__

public int __init_sh_module__ (la_t *this) {
  __INIT_MODULE__(this);
  __INIT__(sh);
  __INIT__(cstring);

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
    public var Sh = {};

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
  sh_T *shp = &shType;
  __deinit_sh__ (&shp);
  return;
}
