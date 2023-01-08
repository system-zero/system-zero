
#define REQUIRE_STDIO

#define REQUIRE_STD_MODULE

#include <z/cenv.h>
#include <z/system.h>

MODULE(system);

static VALUE system_to_memory (la_t *this) {
  (void) this;
  system_t s;
  const char state[] = "/sys/power/state";
  if (-1 == init_system (&s, state)) return NOTOK_VALUE;
  int r = sys_to_memory_linux (&s);
  free (s.power_state_file);
  return (r == -1 ? NOTOK_VALUE : OK_VALUE);
}

public int __init_system_module__ (la_t *this) {
  __INIT_MODULE__(this);

  LaDefCFun lafuns[] = {
    { "system_to_memory", PTR(system_to_memory), 0 },
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var System = {
      to : {
        memory : system_to_memory
      }
    }
  );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_system_module__ (la_t *this) {
  (void) this;
}
