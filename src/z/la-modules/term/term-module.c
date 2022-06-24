#define REQUIRE_STDIO
#define REQUIRE_TERMIOS

#define REQUIRE_STD_MODULE
#define REQUIRE_CSTRING_TYPE  DECLARE
#define REQUIRE_IO_TYPE       DECLARE
#define REQUIRE_TERM_TYPE     DECLARE
#define REQUIRE_TERM_MACROS
#define REQUIRE_KEYS_MACROS

#include <z/cenv.h>

MODULE(term)

#define IS_TERM(__v__)({ int _r_ = 0; \
  if (IS_OBJECT(__v__)) { object *_o_ = AS_OBJECT(__v__); _r_ = Cstring.eq (_o_->name, "TermType");}\
  _r_; \
})

#define AS_TERM(__v__)\
({object *_o_ = AS_OBJECT(__v__); term_t *_s_ = (term_t *) AS_OBJECT (_o_->value); _s_;})

static VALUE term_getkey (la_t *this, VALUE v_fd) {
  (void) this;
  ifnot (IS_FILEDES(v_fd)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a file descriptor");
  utf8 k = IO.input.getkey (AS_FILEDES(v_fd));
  return INT(k);
}

static VALUE term_release (la_t *this, VALUE v_term) {
  (void) this;
  ifnot (IS_TERM(v_term)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a term object");
  term_t *term = AS_TERM(v_term);
  Term.release (&term);
  return OK_VALUE;
}

static VALUE term_new (la_t *this) {
  (void) this;
  term_t *term = Term.new ();
  VALUE v = OBJECT(term);
  object *o = La.object.new (term_release, NULL, "TermType", v);
  return OBJECT(o);
}

static VALUE term_raw_mode (la_t *this, VALUE v_term) {
  (void) this;
  ifnot (IS_TERM(v_term)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a term object");
  term_t *term = AS_TERM(v_term);
  int retval = Term.raw_mode (term);
  return INT(retval);
}

static VALUE term_sane_mode (la_t *this, VALUE v_term) {
  (void) this;
  ifnot (IS_TERM(v_term)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a term object");
  term_t *term = AS_TERM(v_term);
  int retval = Term.sane_mode (term);
  return INT(retval);
}

static VALUE term_orig_mode (la_t *this, VALUE v_term) {
  (void) this;
  ifnot (IS_TERM(v_term)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a term object");
  term_t *term = AS_TERM(v_term);
  int retval = Term.orig_mode (term);
  return INT(retval);
}

static VALUE term_init_size (la_t *this, VALUE v_term) {
  (void) this;
  ifnot (IS_TERM(v_term)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a term object");
  term_t *term = AS_TERM(v_term);
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
  ifnot (IS_TERM(v_term)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a term object");
  term_t *term = AS_TERM(v_term);
  return INT(term->num_rows);
}

static VALUE term_get_cols (la_t *this, VALUE v_term) {
  (void) this;
  ifnot (IS_TERM(v_term)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a term object");
  term_t *term = AS_TERM(v_term);
  return INT(term->num_cols);
}

public int __init_term_module__ (la_t *this) {
  __INIT_MODULE__(this);
  __INIT__(io);
  __INIT__(term);
  __INIT__(cstring);

  LaDefCFun lafuns[] = {
    { "term_new",         PTR(term_new), 0 },
    { "term_getkey",      PTR(term_getkey), 1 },
    { "term_get_rows",    PTR(term_get_rows), 1 },
    { "term_get_cols",    PTR(term_get_cols), 1 },
    { "term_raw_mode",    PTR(term_raw_mode), 1 },
    { "term_sane_mode",   PTR(term_sane_mode), 1 },
    { "term_orig_mode",   PTR(term_orig_mode), 1 },
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
      "orig_mode" : term_orig_mode,
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
