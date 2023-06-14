#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_TERMIOS

#define REQUIRE_STD_MODULE
#define REQUIRE_TERM_TYPE    DONOT_DECLARE
#define REQUIRE_USTRING_TYPE DONOT_DECLARE
#define REQUIRE_CSTRING_TYPE DECLARE

#include <z/cenv.h>
#include <z/pager.h>

MODULE(pager);

#define IS_PAGER(__v__)({ int _r_ = 0; \
  if (IS_OBJECT(__v__)) { object *_o_ = AS_OBJECT(__v__); _r_ = Cstring.eq (_o_->name, "PagerType");}\
  _r_; \
})

#define GET_PAGER(__v__)\
({ \
  ifnot (IS_PAGER(__v__)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a pager object");\
  object *_o_ = AS_OBJECT(__v__); \
   Me *_s_ = (Me *) AS_OBJECT (_o_->value); \
  _s_; \
})

#define GET_OPT_FIRST_ROW() ({                                                \
  VALUE _v_frow = La.get.qualifier (this, "first_row", INT(1));             \
  ifnot (IS_INT(_v_frow))                                                   \
    THROW(LA_ERR_TYPE_MISMATCH, "first_row, awaiting an integer qualifier");\
  AS_INT(_v_frow);                                                          \
})

#define GET_OPT_FIRST_COL() ({                                                \
  VALUE _v_fcol = La.get.qualifier (this, "first_col", INT(1));             \
  ifnot (IS_INT(_v_fcol))                                                   \
    THROW(LA_ERR_TYPE_MISMATCH, "first_col, awaiting an integer qualifier");\
  AS_INT(_v_fcol);                                                          \
})

#define GET_OPT_LAST_ROW() ({                                               \
  VALUE _v_lrow = La.get.qualifier (this, "last_row", INT(-1));             \
  ifnot (IS_INT(_v_lrow))                                                   \
    THROW(LA_ERR_TYPE_MISMATCH, "last_row, awaiting an integer qualifier"); \
  AS_INT(_v_lrow);                                                          \
})

#define GET_OPT_LAST_COL() ({                                               \
  VALUE _v_lcol = La.get.qualifier (this, "last_col", INT(-1));             \
  ifnot (IS_INT(_v_lcol))                                                   \
    THROW(LA_ERR_TYPE_MISMATCH, "last_col, awaiting an integer qualifier"); \
  AS_INT(_v_lcol);                                                          \
})

#define GET_OPT_TABWIDTH() ({                                               \
  VALUE _v_tabwidth = La.get.qualifier (this, "tabwidth", INT(DEFAULT_TABWIDTH));\
  ifnot (IS_INT(_v_tabwidth))                                               \
    THROW(LA_ERR_TYPE_MISMATCH, "tabwidth, awaiting an integer qualifier"); \
  AS_INT(_v_tabwidth);                                                      \
})

#define GET_OPT_AS_MAN_PAGER() ({                                         \
  int _as_man_pager = La.qualifier_exists (this, "as_man_pager");         \
  if (_as_man_pager) {                                                    \
    VALUE _v_as_man_pager = La.get.qualifier (this, "as_man_pager", INT(0)); \
    if (0 == IS_INT(_v_as_man_pager)) {                                   \
      if (IS_NULL(_v_as_man_pager))                                       \
        _as_man_pager = 1;                                                \
      else                                                                \
        THROW(LA_ERR_TYPE_MISMATCH, "as_man_pager, awaiting an integer qualifier"); \
     } else                                                               \
    _as_man_pager = AS_INT(_v_as_man_pager);                              \
  }                                                                       \
  _as_man_pager;                                                          \
})

#define GET_OPT_HAS_STATUSLINE() ({                                       \
  int _has = La.qualifier_exists (this, "has_statusline");                \
  if (_has) {                                                             \
    VALUE _v_has = La.get.qualifier (this, "has_statusline", INT(0));     \
    if (0 == IS_INT(_v_has)) {                                            \
      if (IS_NULL(_v_has))                                                \
        _has = 1;                                                         \
      else                                                                \
        THROW(LA_ERR_TYPE_MISMATCH, "has_statusline, awaiting an integer qualifier"); \
     } else                                                               \
    _has = AS_INT(_v_has);                                                \
  }                                                                       \
  _has;                                                                   \
})

typedef struct pager_t Me;

static VALUE __pager_release (la_t *this, VALUE v_pager) {
  pager_release (GET_PAGER(v_pager));
  return OK_VALUE;
}

static VALUE __pager_main (la_t *this, VALUE v_pager) {
  return INT(pager_main (GET_PAGER(v_pager)));
}

static VALUE __pager_new (la_t *this, VALUE v_lines) {
  string **lines = NULL;
  size_t array_len = 0;
  int input_should_be_freed = 0;

  ifnot (IS_ARRAY(v_lines)) {
    if (IS_STRING(v_lines)) {
      input_should_be_freed = 1;
      lines = Alloc (array_len * sizeof (string));
      char *sp = AS_STRING_BYTES(v_lines);

      char *beg = sp;
      string *s = NULL;
      int end = 0;

      tokenize:
        ifnot (*sp) {
          end = 1;
          goto add_element;
        }

        if (*sp is '\n')
          goto add_element;

        sp++;
        goto tokenize;

      add_element:  // donot add a trailing new empty line at the end of string
        if ((sp - beg) + *beg) {
          array_len++;
          s = String.new_with_len (beg, sp - beg);
          lines = Realloc (lines, array_len * sizeof (string));
          lines[array_len - 1] = s;
        }

        ifnot (end) {
          sp++;
          beg = sp;
          goto tokenize;
        }

    } else
      THROW(LA_ERR_TYPE_MISMATCH, "awaiting an array");

  } else {
    ArrayType *array = (ArrayType *) AS_ARRAY(v_lines);
    ifnot (array->type is STRING_TYPE)
      THROW(LA_ERR_TYPE_MISMATCH, "awaiting a StringType array");

    array_len = array->len;
    ifnot (array_len) return OK_VALUE;
    lines = (string **) AS_ARRAY(array->value);
  }

  int first_row = GET_OPT_FIRST_ROW();
  int first_col = GET_OPT_FIRST_COL();
  int last_row  = GET_OPT_LAST_ROW();
  int last_col  = GET_OPT_LAST_COL();
  int tabwidth  = GET_OPT_TABWIDTH();
  int has_statusline = GET_OPT_HAS_STATUSLINE();
  int as_man_pager= GET_OPT_AS_MAN_PAGER();
  term_t *term  = GET_OPT_TERM();

  Me *My = pager_new (lines, array_len, PagerOpts (
    .first_row  = first_row,
    .first_col  = first_col,
    .last_row   = last_row,
    .last_col   = last_col,
    .tabwidth   = tabwidth,
    .buf_has_statusline = has_statusline,
    .as_man_pager = as_man_pager,
    .term       = term,
    .input_should_be_freed = input_should_be_freed,
    .term_should_be_freed  = term == NULL
   ));


  VALUE v = OBJECT(My);
  object *o = La.object.new (__pager_release, NULL, "PagerType", v);
  return OBJECT(o);
}

static VALUE pager_new_from_stdin (la_t *this) {
  VALUE r = NULL_VALUE;

  if (isatty (STDIN_FILENO)) return r;

  int ifd = dup (STDIN_FILENO);

  FILE *fpa = freopen ("/dev/tty", "r", stdin);
  if (NULL is fpa) return r;

  FILE *fp = fdopen (ifd, "r");

  int alen = 0;
  ArrayType *array = ARRAY_INIT_WITH_LEN(STRING_TYPE, alen);

  char *buf = NULL;
  ssize_t nread;
  size_t len = 0;

  while (-1 isnot (nread = getline (&buf, &len, fp))) {
    if (buf[nread - 1] == '\n')
      nread--;
    buf[nread] = '\0';

    VALUE sv = STRING(String.new_with_len (buf, nread));
    array = ARRAY_APPEND(array, sv);
  }

  fclose (fp);

  ifnot (NULL is buf)
    free (buf);

  string **ary = (string **) AS_ARRAY(array->value);

  ifnot (array->len) goto theend;

  r = __pager_new (this, ARRAY(array));

theend:
  for (size_t i = 0; i < array->len; i++) String.release (ary[i]);
  free (ary);
  free (array);

  return r;
}

public int __init_pager_module__ (la_t *this) {
  __INIT_MODULE__(this);
  __INIT__(cstring);

  LaDefCFun lafuns[] = {
    { "pager_new",  PTR(__pager_new), 1 },
    { "pager_main", PTR(__pager_main), 1 },
    { "pager_new_from_stdin",  PTR(pager_new_from_stdin), 0 },
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Pager = {
      new  : pager_new,
      main : pager_main,
      new_from_stdin : pager_new_from_stdin
     }
   );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_pager_module__ (la_t *this) {
  (void) this;
}
