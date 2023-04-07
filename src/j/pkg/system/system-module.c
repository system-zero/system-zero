#define REQUIRE_STD_MODULE
#define REQUIRE_SLEEP
#define REQUIRE_Z_ENV

#include <libc.h>
#include <z/system.h>

MODULE(system);

#ifndef POWER_STATE_FILE
#define POWER_STATE_FILE "/sys/power/state"
#endif

static VALUE system_sleep (la_t *this, VALUE v_sec) {
  ifnot (IS_INT(v_sec)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer");
  int sec = AS_INT(v_sec);

  if (sec < 0) return NOTOK_VALUE;
  ifnot (sec)  return OK_VALUE;

  int r = sys_sleep (sec);
  return INT(r);
}

static VALUE system_to_memory (la_t *this) {
  (void) this;
  system_t s;
  if (-1 == init_system (&s, SystemOpts(.power_state_file = POWER_STATE_FILE)))
    return NOTOK_VALUE;

  int r = sys_to_memory (&s);
  return (r == -1 ? NOTOK_VALUE : OK_VALUE);
}

public int __init_system_module__ (la_t *this) {
  __INIT_MODULE__(this);

  LaDefCFun lafuns[] = {
    { "system_to_memory", PTR(system_to_memory), 0 },
    { "system_sleep",     PTR(system_sleep), 1 },
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    if is_defined ("System") {
      append {memory : system_to_memory} in System as "to";
      append system_sleep in System as "sleep";
    } else {
      public var System = {
        to : {
          memory : system_to_memory
        },
        sleep : system_sleep
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
