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
  return OK_VALUE;
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

static term_t *__get_term_arg (VALUE v_term) {
  ifnot (IS_OBJECT(v_term))
    return NULL;

  object *o = AS_OBJECT(v_term);
  term_t *term = (term_t *) AS_PTR(o->value);
  return term;
}

static VALUE term_init_size (la_t *this, VALUE v_term) {
  (void) this;
  term_t *term = __get_term_arg (v_term);
  if (NULL is term)
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting a term object");

  char mode = term->mode;

  if (mode isnot 'r') {
    if (NOTOK is Term.raw_mode (term))
      return NOTOK_VALUE;
  }

  int rows = 0;
  int cols = 0;
  Term.init_size (term, &rows, &cols);

  switch (mode) {
    case 'o':
      if (NOTOK is Term.orig_mode (term))
        return NOTOK_VALUE;
      break;

    case 's':
      if (NOTOK is Term.sane_mode (term))
        return NOTOK_VALUE;

  }

  return OK_VALUE;
}

static VALUE term_get_rows (la_t *this, VALUE v_term) {
  (void) this;
  term_t *term = __get_term_arg (v_term);
  if (NULL is term)
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting a term object");
  return INT(term->num_rows);
}

static VALUE term_get_cols (la_t *this, VALUE v_term) {
  (void) this;
  term_t *term = __get_term_arg (v_term);
  if (NULL is term)
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting a term object");
  return INT(term->num_cols);
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
    { "term_getkey",      PTR(term_getkey), 1 },
    { "term_get_rows",    PTR(term_get_rows), 1 },
    { "term_get_cols",    PTR(term_get_cols), 1 },
    { "term_raw_mode",    PTR(term_raw_mode), 1 },
    { "term_sane_mode",   PTR(term_sane_mode), 1 },
    { "term_init_size",   PTR(term_init_size), 1 },
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
       "sane_mode" : term_sane_mode,
       "init_size" : term_init_size,
       "get" : {
         "rows" : term_get_rows,
         "cols" : term_get_cols,
       }
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
