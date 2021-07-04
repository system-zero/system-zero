#define REQUIRE_STDIO

#define REQUIRE_VMAP_TYPE     DONOT_DECLARE
#define REQUIRE_STRING_TYPE   DECLARE
#define REQUIRE_CSTRING_TYPE  DECLARE
#define REQUIRE_USTRING_TYPE  DECLARE
#define REQUIRE_LA_TYPE       DECLARE

#include <z/cenv.h>

static VALUE string_cmp_n (la_t *this, VALUE v_sa, VALUE v_sb, VALUE v_n) {
  (void) this;
  char *sa = AS_STRING_BYTES(v_sa);
  char *sb = AS_STRING_BYTES(v_sb);
  int n = AS_INT(v_n);

  int retval = Cstring.cmp_n (sa, sb, n);

  ifnot (retval) return INT(0);
  if (retval < 0) return INT(-1);
  return INT(1);
}

static VALUE string_eq_n (la_t *this, VALUE v_sa, VALUE v_sb, VALUE v_n) {
  (void) this;
  char *sa = AS_STRING_BYTES(v_sa);
  char *sb = AS_STRING_BYTES(v_sb);
  int n = AS_INT(v_n);

  return INT(Cstring.eq_n (sa, sb, n));
}

static VALUE string_eq (la_t *this, VALUE v_sa, VALUE v_sb) {
  (void) this;
  char *sa = AS_STRING_BYTES(v_sa);
  char *sb = AS_STRING_BYTES(v_sb);
  return INT(Cstring.eq (sa, sb));
}

static VALUE string_tokenize (la_t *this, VALUE v_str, VALUE v_tok) {
  char *str = AS_STRING_BYTES(v_str);
  char *tok = AS_STRING_BYTES(v_tok);
  cstring_tok *ctok = Cstring.tokenize (NULL, str, tok, NULL, NULL);
  if (NULL is ctok)
    return NULL_VALUE;

  ArrayType *array = ARRAY_NEW(STRING_TYPE, ctok->num_tokens);
  string **ar = (string **) AS_ARRAY(array->value);
  for (int i = 0; i < ctok->num_tokens; i++)
    String.replace_with_len (ar[i], ctok->tokens[i], ctok->length[i]);

  for (int i = 0; i < ctok->num_tokens; i++)
    free (ctok->tokens[i]);
  free (ctok->tokens); free (ctok->length); free (ctok);

  return ARRAY(array);
}

static VALUE string_character (la_t *this, VALUE v_c) {
  utf8 c = AS_INT(v_c);
  char buf[8];
  int len = 0;
  Ustring.character (c, buf, &len);
  string *s = String.new_with (buf);
  return STRING(s);
}

#define EvalString(...) #__VA_ARGS__

public int __init_string_module__ (la_t *this) {
  __INIT_MODULE__(this);
  __INIT__(string);
  __INIT__(cstring);
  __INIT__(ustring);

  LaDefCFun lafuns[] = {
    { "string_eq",       PTR(string_eq), 2 },
    { "string_eq_n",     PTR(string_eq_n), 3 },
    { "string_cmp_n",    PTR(string_cmp_n), 3 },
    { "string_tokenize", PTR(string_tokenize), 2 },
    { "string_character",PTR(string_character), 1 },
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var String = {
       "eq" : string_eq,
       "eq_n" : string_eq_n,
       "cmp_n" : string_cmp_n,
       "tokenize" : string_tokenize,
       "character" : string_character
     }
   );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_string_module__ (la_t *this) {
  (void) this;
  return;
}
