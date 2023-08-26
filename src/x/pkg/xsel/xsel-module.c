#define REQUIRE_STDIO

#define REQUIRE_STD_MODULE

#include <z/cenv.h>
#include <z/xsel.h>

MODULE(xsel);

#define GET_OPT_RMLASTNL() ({                                             \
  int _rmlastnl = La.qualifier_exists (this, "rm_last_nl");               \
  if (_rmlastnl) {                                                        \
    VALUE _v_rmlastnl = La.get.qualifier (this, "rm_last_nl", INT(0));    \
    if (0 == IS_INT(_v_rmlastnl)) {                                       \
      if (IS_NULL(_v_rmlastnl))                                           \
        _rmlastnl = 1;                                                    \
      else                                                                \
        THROW(LA_ERR_TYPE_MISMATCH, "rm_last_nl, awaiting an integer qualifier"); \
     } else                                                               \
    _rmlastnl = AS_INT(_v_rmlastnl);                                      \
  }                                                                       \
  _rmlastnl;                                                              \
})

#define GET_OPT_XSELECTION() ({                                           \
  VALUE _v_selection = La.get.qualifier (this, "selection", INT(0));      \
  ifnot (IS_INT(_v_selection))                                            \
    THROW(LA_ERR_TYPE_MISMATCH, "selection: awaiting an integer qualifier");         \
  AS_INT(_v_selection);                                                   \
})

static VALUE _xsel_put (la_t *this, VALUE v_sel) {
  (void) this;
  ifnot (IS_STRING(v_sel)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");

  char *sel = AS_STRING_BYTES(v_sel);
  int rmlastnl = GET_OPT_RMLASTNL();
  int selection = GET_OPT_XSELECTION();

  int r = xsel_put (sel, XselOpts(.rm_last_nl = rmlastnl, .selection = selection));
  return INT(r);
}

static VALUE _xsel_get (la_t *this) {
  (void) this;
  int rmlastnl = GET_OPT_RMLASTNL();
  int selection = GET_OPT_XSELECTION();

  char *out = xsel_get (XselOpts(.rm_last_nl = rmlastnl, .selection = selection));
  if (NULL == out)
    return NULL_VALUE;

  string *s = String.new_with (out);
  return STRING(s);
}

public int __init_xsel_module__ (la_t *this) {
  __INIT_MODULE__(this);

  LaDefCFun lafuns[] = {
    { "xsel_put" , PTR(_xsel_put), 1 },
    { "xsel_get" , PTR(_xsel_get), 0 },
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  if (La.def_std (this, "X_PRIMARY", INTEGER_TYPE, INT(XSEL_PRIMARY), -1)) return LA_NOTOK;
  if (La.def_std (this, "X_CLIPBOARD", INTEGER_TYPE, INT(XSEL_CLIPBOARD), -1)) return LA_NOTOK;

  const char evalString[] = EvalString (
    public const Xsel =  {
      put : xsel_put,
      get : xsel_get
     }
   );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_xsel_module__ (la_t *this) {
  (void) this;
}
