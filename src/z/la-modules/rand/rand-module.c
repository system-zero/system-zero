#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_FCNTL

#define REQUIRE_STD_MODULE
#define REQUIRE_RANDOM_TYPE   DECLARE

#include <z/cenv.h>

MODULE(rand)

static VALUE rand_new (la_t *this) {
  (void) this;
  uint32_t r = Random.new ();
  return INT(r);
}

static VALUE rand_get_entropy_bytes (la_t *this, VALUE v_size) {
  (void) this;
  ifnot (IS_INT(v_size)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer");
  size_t size = AS_INT(v_size);
  char buf[size + 1];
  if (NOTOK is Random.get.entropy_bytes (buf, size))
    return NULL_VALUE;

  buf[size] = '\0';

  string *s = String.new_with_len (buf, size);
  return STRING(s);
}

public int __init_rand_module__ (la_t *this) {
  __INIT_MODULE__(this);
  __INIT__ (random);

  LaDefCFun lafuns[] = {
    { "rand_new",  PTR(rand_new), 0 },
    { "rand_get_entropy_bytes", PTR(rand_get_entropy_bytes), 1},
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Rand = {
      "new" : rand_new,
      "get_entropy_bytes" : rand_get_entropy_bytes
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
