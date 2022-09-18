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

#define GET_OPT_SET() ({                                                  \
  int _set = La.qualifier_exists (this, "set");                           \
  if (_set) {                                                             \
    VALUE _v_set = La.get.qualifier (this, "set", INT(0));                \
    if (0 == IS_INT(_v_set)) {                                            \
      if (IS_NULL(_v_set))                                                \
        _set = 1;                                                         \
      else                                                                \
        THROW(LA_ERR_TYPE_MISMATCH, "set, awaiting an integer qualifier");\
     } else                                                               \
    _set = AS_INT(_v_set);                                                \
  }                                                                       \
  _set;                                                                   \
})

#define GET_OPT_SAVE_SCREEN() ({                                          \
  int _save_screen = La.qualifier_exists (this, "save_screen");           \
  if (_save_screen) {                                                     \
    VALUE _v_save_screen = La.get.qualifier (this, "save_screen", INT(0));\
    if (0 == IS_INT(_v_save_screen)) {                                    \
      if (IS_NULL(_v_save_screen))                                        \
        _save_screen = 1;                                                 \
      else                                                                \
        THROW(LA_ERR_TYPE_MISMATCH, "save_screen, awaiting an integer qualifier");\
     } else                                                               \
    _save_screen = AS_INT(_v_save_screen);                                \
  }                                                                       \
  _save_screen;                                                           \
})

#define GET_OPT_CLEAR_SCREEN() ({                                         \
  int _clear_screen = La.qualifier_exists (this, "clear_screen");         \
  if (_clear_screen) {                                                    \
    VALUE _v_clear_screen = La.get.qualifier (this, "clear_screen", INT(0));\
    if (0 == IS_INT(_v_clear_screen)) {                                   \
      if (IS_NULL(_v_clear_screen))                                       \
        _clear_screen = 1;                                                \
      else                                                                \
        THROW(LA_ERR_TYPE_MISMATCH, "clear_screen, awaiting an integer qualifier");\
     } else                                                               \
    _clear_screen = AS_INT(_v_clear_screen);                              \
  }                                                                       \
  _clear_screen;                                                          \
})

static VALUE term_getkey (la_t *this, VALUE v_fd) {
  ifnot (IS_FILEDES(v_fd)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a file descriptor");
  utf8 k = IO.input.getkey (AS_FILEDES(v_fd));
  return INT(k);
}

static VALUE term_release (la_t *this, VALUE v_term) {
  term_t *term = GET_TERM(v_term);
  Term.release (&term);
  return OK_VALUE;
}

static VALUE term_new (la_t *this) {
  (void) this;
  term_t *term = Term.new ();
  int set = GET_OPT_SET();
  if (set) {
    int save_screen = GET_OPT_SAVE_SCREEN();
    int clear_screen = GET_OPT_CLEAR_SCREEN();
    ifnot (save_screen) Term.set_state_bit (term, TERM_DONOT_SAVE_SCREEN);
    ifnot (clear_screen) Term.set_state_bit (term, TERM_DONOT_CLEAR_SCREEN);
    Term.set (term);
  }

  VALUE v = OBJECT(term);
  object *o = La.object.new (term_release, NULL, "TermType", v);
  return OBJECT(o);
}

static VALUE term_raw_mode (la_t *this, VALUE v_term) {
  term_t *term = GET_TERM(v_term);
  int retval = Term.raw_mode (term);
  return INT(retval);
}

static VALUE term_sane_mode (la_t *this, VALUE v_term) {
  term_t *term = GET_TERM(v_term);
  int retval = Term.sane_mode (term);
  return INT(retval);
}

static VALUE term_orig_mode (la_t *this, VALUE v_term) {
  term_t *term = GET_TERM(v_term);
  int retval = Term.orig_mode (term);
  return INT(retval);
}

static VALUE term_init_size (la_t *this, VALUE v_term) {
  term_t *term = GET_TERM(v_term);
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

static VALUE term_set_pos (la_t *this, VALUE v_term, VALUE v_row, VALUE v_col) {
  ifnot (IS_INT(v_row))   THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer");
  ifnot (IS_INT(v_col))   THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer");
  term_t *term = GET_TERM(v_term);
  Term.cursor.set_pos (term, AS_INT(v_row), AS_INT(v_col));
  return OK_VALUE;
}

static VALUE term_get_pos (la_t *this, VALUE v_term) {
  term_t *term = GET_TERM(v_term);
  int row, col;
  int r = Term.cursor.get_pos (term, &row, &col);
  if (r is NOTOK) return NULL_VALUE;
  Vmap_t *m = Vmap.new (2);
  La.map.set_value (this, m, "row", INT(row), 1);
  La.map.set_value (this, m, "col", INT(col), 1);
  return MAP(m);
}

static VALUE term_get_rows (la_t *this, VALUE v_term) {
  term_t *term = GET_TERM(v_term);
  return INT(term->num_rows);
}

static VALUE term_get_cols (la_t *this, VALUE v_term) {
  term_t *term = GET_TERM(v_term);
  return INT(term->num_cols);
}

static VALUE term_screen_save (la_t *this, VALUE v_term) {
  term_t *term = GET_TERM(v_term);
  Term.screen.save (term);
  return OK_VALUE;
}

static VALUE term_screen_clear (la_t *this, VALUE v_term) {
  term_t *term = GET_TERM(v_term);
  Term.screen.clear (term);
  return OK_VALUE;
}

static VALUE term_screen_restore (la_t *this, VALUE v_term) {
  term_t *term = GET_TERM(v_term);
  Term.screen.restore (term);
  return OK_VALUE;
}

public int __init_term_module__ (la_t *this) {
  __INIT_MODULE__(this);
  __INIT__(io);
  __INIT__(term);
  __INIT__(cstring);

  LaDefCFun lafuns[] = {
    { "term_new",         PTR(term_new), 0 },
    { "term_getkey",      PTR(term_getkey), 1 },
    { "term_set_pos",     PTR(term_set_pos), 3 },
    { "term_get_pos",     PTR(term_get_pos), 1 },
    { "term_get_rows",    PTR(term_get_rows), 1 },
    { "term_get_cols",    PTR(term_get_cols), 1 },
    { "term_raw_mode",    PTR(term_raw_mode), 1 },
    { "term_sane_mode",   PTR(term_sane_mode), 1 },
    { "term_orig_mode",   PTR(term_orig_mode), 1 },
    { "term_init_size",   PTR(term_init_size), 1 },
    { "term_screen_save", PTR(term_screen_save), 1},
    { "term_screen_clear",PTR(term_screen_clear), 1},
    { "term_screen_restore", PTR(term_screen_restore), 1},
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Term = {
      new : term_new,
      getkey : term_getkey,
      raw_mode : term_raw_mode,
      sane_mode : term_sane_mode,
      orig_mode : term_orig_mode,
      init_size : term_init_size,
      set : {
        pos : term_set_pos,
      },
      screen : {
        save : term_screen_save,
        clear : term_screen_clear,
        restore : term_screen_restore,
      },
      get : {
        pos : term_get_pos,
        rows : term_get_rows,
        cols : term_get_cols,
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
