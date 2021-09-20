#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_FCNTL

#define REQUIRE_VMAP_TYPE     DONOT_DECLARE
#define REQUIRE_STRING_TYPE   DONOT_DECLARE
#define REQUIRE_RANDOM_TYPE   DECLARE
#define REQUIRE_LA_TYPE       DECLARE

#include <z/cenv.h>

static VALUE rand_new (la_t *this) {
  (void) this;
  uint32_t r = Random.new ();
  return INT(r);
}

#define EvalString(...) #__VA_ARGS__

public int __init_rand_module__ (la_t *this) {
  __INIT_MODULE__(this);
  __INIT__ (random);

  (void) vmapType;
  (void) stringType;

  LaDefCFun lafuns[] = {
    { "rand_new",  PTR(rand_new), 0 },
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Rand = {
      "new" : rand_new
     }
   );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_rand_module__ (la_t *this) {
  (void) this;
  return;
}
