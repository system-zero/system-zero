#define REQUIRE_TERM
#define REQUIRE_TERM_GETKEY
#define REQUIRE_MAP

#define REQUIRE_STD_MODULE
#define REQUIRE_CSTRING_TYPE  DECLARE
#define REQUIRE_IO_TYPE       DECLARE
#define REQUIRE_TERM_TYPE     DECLARE
#define REQUIRE_TERM_MACROS
#define REQUIRE_KEYS_MACROS

#include <libc.h>

MODULE(term);

#define GET_OPT_SET() ({                                                  \
  int _set = L.qualifier_exists (this, "set");                           \
  if (_set) {                                                             \
    VALUE _v_set = L.get.qualifier (this, "set", INT(0));                \
    if (0 == IS_INT(_v_set)) {                                            \
      if (IS_NULL(_v_set))                                                \
        _set = 1;                                                         \
      else                                                                \
        THROW(L_ERR_TYPE_MISMATCH, "set, awaiting an integer qualifier");\
     } else                                                               \
    _set = AS_INT(_v_set);                                                \
  }                                                                       \
  _set;                                                                   \
})

#define GET_OPT_SAVE_SCREEN() ({                                          \
  int _save_screen = L.qualifier_exists (this, "save_screen");           \
  if (_save_screen) {                                                     \
    VALUE _v_save_screen = L.get.qualifier (this, "save_screen", INT(0));\
    if (0 == IS_INT(_v_save_screen)) {                                    \
      if (IS_NULL(_v_save_screen))                                        \
        _save_screen = 1;                                                 \
      else                                                                \
        THROW(L_ERR_TYPE_MISMATCH, "save_screen, awaiting an integer qualifier");\
     } else                                                               \
    _save_screen = AS_INT(_v_save_screen);                                \
  }                                                                       \
  _save_screen;                                                           \
})

#define GET_OPT_CLEAR_SCREEN() ({                                         \
  int _clear_screen = L.qualifier_exists (this, "clear_screen");         \
  if (_clear_screen) {                                                    \
    VALUE _v_clear_screen = L.get.qualifier (this, "clear_screen", INT(0));\
    if (0 == IS_INT(_v_clear_screen)) {                                   \
      if (IS_NULL(_v_clear_screen))                                       \
        _clear_screen = 1;                                                \
      else                                                                \
        THROW(L_ERR_TYPE_MISMATCH, "clear_screen, awaiting an integer qualifier");\
     } else                                                               \
    _clear_screen = AS_INT(_v_clear_screen);                              \
  }                                                                       \
  _clear_screen;                                                          \
})

static VALUE _term_getkey (l_t *this, VALUE v_fd) {
  ifnot (IS_FILEDES(v_fd)) THROW(L_ERR_TYPE_MISMATCH, "awaiting a file descriptor");
  utf8 k = term_getkey (AS_FILEDES(v_fd));
  return INT(k);
}

static VALUE _term_release (l_t *this, VALUE v_term) {
  term_t *term = GET_TERM(v_term);
  term_release (&term);
  return OK_VALUE;
}

static VALUE _term_new (l_t *this) {
  (void) this;
  term_t *term = term_new ();
  int set = GET_OPT_SET();
  if (set) {
    int save_screen = GET_OPT_SAVE_SCREEN();
    int clear_screen = GET_OPT_CLEAR_SCREEN();
    ifnot (save_screen) term_set_state_bit (term, TERM_DONOT_SAVE_SCREEN);
    ifnot (clear_screen) term_set_state_bit (term, TERM_DONOT_CLEAR_SCREEN);
    term_set (term);
  }

  VALUE v = OBJECT(term);
  object *o = L.object.new (_term_release, NULL, "TermType", v);
  return OBJECT(o);
}

static VALUE _term_raw_mode (l_t *this, VALUE v_term) {
  term_t *term = GET_TERM(v_term);
  int retval = term_raw_mode (term);
  return INT(retval);
}

static VALUE _term_sane_mode (l_t *this, VALUE v_term) {
  term_t *term = GET_TERM(v_term);
  int retval = term_sane_mode (term);
  return INT(retval);
}

static VALUE _term_orig_mode (l_t *this, VALUE v_term) {
  term_t *term = GET_TERM(v_term);
  int retval = term_orig_mode (term);
  return INT(retval);
}

static VALUE _term_init_size (l_t *this, VALUE v_term) {
  term_t *term = GET_TERM(v_term);
  char mode = term->mode;

  if (mode != 'r') {
    if (-1 == term_raw_mode (term))
      return NOTOK_VALUE;
  }

  int rows = 0;
  int cols = 0;
  term_init_size (term, &rows, &cols);

  switch (mode) {
    case 'o':
      if (-1 == term_orig_mode (term))
        return NOTOK_VALUE;
      break;

    case 's':
      if (-1 == term_sane_mode (term))
        return NOTOK_VALUE;

  }

  return OK_VALUE;
}

static VALUE _term_set_pos (l_t *this, VALUE v_term, VALUE v_row, VALUE v_col) {
  ifnot (IS_INT(v_row))   THROW(L_ERR_TYPE_MISMATCH, "awaiting an integer");
  ifnot (IS_INT(v_col))   THROW(L_ERR_TYPE_MISMATCH, "awaiting an integer");
  term_t *term = GET_TERM(v_term);
  term_cursor_set_pos (term, AS_INT(v_row), AS_INT(v_col));
  return OK_VALUE;
}

static VALUE _term_get_pos (l_t *this, VALUE v_term) {
  term_t *term = GET_TERM(v_term);
  int row, col;
  int r = term_cursor_get_pos (term, &row, &col);
  if (r == -1) return NULL_VALUE;
  Map_Type *m = map_new (2);
  L.map.set_value (this, m, "row", INT(row), 1);
  L.map.set_value (this, m, "col", INT(col), 1);
  return MAP(m);
}

static VALUE _term_get_rows (l_t *this, VALUE v_term) {
  term_t *term = GET_TERM(v_term);
  return INT(term->num_rows);
}

static VALUE _term_get_cols (l_t *this, VALUE v_term) {
  term_t *term = GET_TERM(v_term);
  return INT(term->num_cols);
}

static VALUE _term_screen_save (l_t *this, VALUE v_term) {
  term_t *term = GET_TERM(v_term);
  term_screen_save (term);
  return OK_VALUE;
}

static VALUE _term_screen_clear (l_t *this, VALUE v_term) {
  term_t *term = GET_TERM(v_term);
  term_screen_clear (term);
  return OK_VALUE;
}

static VALUE _term_screen_restore (l_t *this, VALUE v_term) {
  term_t *term = GET_TERM(v_term);
  term_screen_restore (term);
  return OK_VALUE;
}

public int __init_term_module__ (l_t *this) {
  __INIT_MODULE__(this);

  LDefCFun lafuns[] = {
    { "term_new",         PTR(_term_new), 0 },
    { "term_getkey",      PTR(_term_getkey), 1 },
    { "term_set_pos",     PTR(_term_set_pos), 3 },
    { "term_get_pos",     PTR(_term_get_pos), 1 },
    { "term_get_rows",    PTR(_term_get_rows), 1 },
    { "term_get_cols",    PTR(_term_get_cols), 1 },
    { "term_raw_mode",    PTR(_term_raw_mode), 1 },
    { "term_sane_mode",   PTR(_term_sane_mode), 1 },
    { "term_orig_mode",   PTR(_term_orig_mode), 1 },
    { "term_init_size",   PTR(_term_init_size), 1 },
    { "term_screen_save", PTR(_term_screen_save), 1},
    { "term_screen_clear",PTR(_term_screen_clear), 1},
    { "term_screen_restore", PTR(_term_screen_restore), 1},
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (L_OK != (err = L.def (this, lafuns[i].name, L_CFUNC (lafuns[i].nargs), lafuns[i].val)))
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

  err = L.eval_string (this, evalString);
  if (err != L_OK) return err;
  return L_OK;
}

public void __deinit_term_module__ (l_t *this) {
  (void) this;
  return;
}
