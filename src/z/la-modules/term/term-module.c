#define REQUIRE_STDIO
#define REQUIRE_TERMIOS

#define REQUIRE_VMAP_TYPE     DONOT_DECLARE
#define REQUIRE_STRING_TYPE   DONOT_DECLARE
#define REQUIRE_IO_TYPE       DECLARE
#define REQUIRE_TERM_TYPE     DECLARE
#define REQUIRE_TERM_MACROS
#define REQUIRE_KEYS_MACROS
#define REQUIRE_LA_TYPE       DECLARE

#include <z/cenv.h>

static VALUE term_getkey (la_t *this, VALUE v_fd) {
  (void) this;
  ifnot (IS_INT(v_fd))
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer as fd descriptor");

  utf8 k = IO.input.getkey (AS_INT(v_fd));
  return INT(k);
}

static VALUE term_release (la_t *this, VALUE v_term) {
  (void) this;
  ifnot (IS_OBJECT(v_term))
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting a term object");

  object *o = AS_OBJECT(v_term);
  term_t *term = (term_t *) AS_PTR(o->value);
  Term.release (&term);
  return INT(LA_OK);
}

static VALUE term_new (la_t *this) {
  (void) this;
  term_t *term = Term.new ();
  VALUE v = OBJECT(term);
  object *o = La.object.new (term_release, NULL, v);
  return OBJECT(o);
}

static VALUE term_raw_mode (la_t *this, VALUE v_term) {
  (void) this;
  ifnot (IS_OBJECT(v_term))
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting a term object");

  object *o = AS_OBJECT(v_term);
  term_t *term = (term_t *) AS_PTR(o->value);
  int retval = Term.raw_mode (term);
  return INT(retval);
}

static VALUE term_sane_mode (la_t *this, VALUE v_term) {
  (void) this;
  ifnot (IS_OBJECT(v_term))
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting a term object");

  object *o = AS_OBJECT(v_term);
  term_t *term = (term_t *) AS_PTR(o->value);
  int retval = Term.sane_mode (term);
  return INT(retval);
}

#define EvalString(...) #__VA_ARGS__

public int __init_term_module__ (la_t *this) {
  __INIT_MODULE__(this);
  __INIT__(io);
  __INIT__(term);

  (void) vmapType;
  (void) stringType;

  LaDefCFun lafuns[] = {
    { "term_new",         PTR(term_new), 0 },
    { "term_raw_mode",    PTR(term_raw_mode), 1 },
    { "term_sane_mode",   PTR(term_sane_mode), 1 },
    { "term_getkey",      PTR(term_getkey), 1 },
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Term = {
       "new" : term_new,
       "getkey" : term_getkey,
       "raw_mode" : term_raw_mode,
       "sane_mode" : term_sane_mode
     }
 );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_term_module__ (la_t *this) {
  (void) this;
  return;
}
