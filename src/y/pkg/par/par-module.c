
#define REQUIRE_STDIO

#define REQUIRE_STD_MODULE
#define REQUIRE_CSTRING_TYPE  DECLARE

#include <z/cenv.h>
#include <z/par.h>

MODULE(par);

typedef par_t Me;

#define IS_PAR(__v__)({ int _r_ = 0; \
  if (IS_OBJECT(__v__)) { object *_o_ = AS_OBJECT(__v__); _r_ = Cstring.eq (_o_->name, "ParType");}\
  _r_; \
})

#define GET_PAR(__v__)\
({ \
  ifnot (IS_PAR(__v__)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a par object");\
  object *_o_ = AS_OBJECT(__v__); \
   Me *_s_ = (Me *) AS_OBJECT (_o_->value); \
  _s_; \
})

#define GET_OPT_WIDTH() ({                                                      \
  VALUE _v_width = La.get.qualifier (this, "width", INT(DEFAULT_WIDTH));        \
  ifnot (IS_INT(_v_width))                                                      \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");               \
  AS_INT(_v_width);                                                             \
})

#define GET_OPT_TABWIDTH() ({                                                   \
  VALUE _v_tabwidth = La.get.qualifier (this, "tabwidth", INT(DEFAULT_TABWIDTH)); \
  ifnot (IS_INT(_v_tabwidth))                                                   \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");               \
  AS_INT(_v_tabwidth);                                                          \
})

#define GET_OPT_JUST() ({                                                       \
  int _just = La.qualifier_exists (this, "just");                               \
  if (_just) {                                                                  \
    VALUE _v_just = La.get.qualifier (this, "just", INT(DEFAULT_JUST));         \
    if (0 == IS_INT(_v_just)) {                                                 \
      if (IS_NULL(_v_just))                                                     \
        _just = 1;                                                              \
      else                                                                      \
        THROW(LA_ERR_TYPE_MISMATCH, "just, awaiting an integer qualifier");     \
     } else                                                                     \
      _just = AS_INT(_v_just);                                                  \
  }                                                                             \
  _just;                                                                        \
})

#define GET_OPT_TOSTDOUT() ({                                                   \
  int _tostdout = La.qualifier_exists (this, "tostdout");                       \
  if (_tostdout) {                                                              \
    VALUE _v_tostdout = La.get.qualifier (this, "tostdout", INT(0));            \
    if (0 == IS_INT(_v_tostdout)) {                                             \
      if (IS_NULL(_v_tostdout))                                                 \
        _tostdout = 1;                                                          \
      else                                                                      \
        THROW(LA_ERR_TYPE_MISMATCH, "tostdout, awaiting an integer qualifier"); \
     } else                                                                     \
      _tostdout = AS_INT(_v_tostdout);                                          \
  }                                                                             \
  _tostdout;                                                                    \
})

static int _par_output_byte (Me *My, int c) {
  string *s = My->userdata;
  String.append_byte (s, (unsigned char) c);
  return 0;
}

static int _par_output_line (Me *My, const char *line) {
  string *s = My->userdata;
  String.append_with_fmt (s, "%s\n", line);
  return 0;
}

static VALUE par_parse_argv (la_t *this, VALUE v_par, VALUE v_array) {
  Me *My = GET_PAR(v_par);

  ifnot (IS_ARRAY(v_array)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting an array");

  ArrayType *array = (ArrayType *) AS_ARRAY(v_array);
  if (array->type != STRING_TYPE) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string array");

  size_t len = array->len;

  string **s_ar = (string **) AS_ARRAY(array->value);
  for (size_t i = 0; i < len; i++)
    par_parsearg (My, s_ar[i]->bytes);

  return OK_VALUE;
}

static VALUE __par_release (la_t *this, VALUE v_par) {
  Me *My = GET_PAR(v_par);
  par_release (My);
  return OK_VALUE;
}

static VALUE __par_new (la_t *this) {
  Me *My = par_new ();
  if (NULL is My) return NULL_VALUE;

  int width    = GET_OPT_WIDTH();
  int tabwidth = GET_OPT_TABWIDTH();
  int just     = GET_OPT_JUST();

  My->width    = width;
  My->Tab      = tabwidth;
  My->just     = just;

  VALUE v = OBJECT(My);
  object *o = La.object.new (__par_release, NULL, "ParType", v);
  return OBJECT(o);
}

static VALUE __par_process (la_t *this, VALUE v_par, VALUE v_bytes) {
  ifnot (IS_STRING(v_bytes)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string as a key");

  Me *My = GET_PAR(v_par);
  char *bytes = AS_STRING_BYTES(v_bytes);

  string *s = NULL;

  int tostdout = GET_OPT_TOSTDOUT();

  ifnot (tostdout) {
    s = String.new (AS_STRING(v_bytes)->num_bytes);
    My->userdata = s;
    My->output_byte = _par_output_byte;
    My->output_line = _par_output_line;
  }

  int r = par_process (My, bytes);

  if (-1 is r)
    fprintf (stderr, "%s\n", My->errmsg);

  return (s is NULL ? INT(r) : STRING(s));
}

public int __init_par_module__ (la_t *this) {
  __INIT_MODULE__(this);
  __INIT__(cstring);

  LaDefCFun lafuns[] = {
    { "par_new",        PTR(__par_new), 0 },
    { "par_process",    PTR(__par_process), 2 },
    { "par_parse_argv", PTR(par_parse_argv), 2},
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Par = {
      new : par_new,
      process : par_process,
      parse_argv : par_parse_argv
     }
   );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_par_module__ (la_t *this) {
  (void) this;
}
