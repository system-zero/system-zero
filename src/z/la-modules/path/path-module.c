#define REQUIRE_STDIO
#define REQUIRE_STDARG

#define REQUIRE_VMAP_TYPE    DONOT_DECLARE
#define REQUIRE_VSTRING_TYPE DECLARE
#define REQUIRE_STRING_TYPE  DECLARE
#define REQUIRE_CSTRING_TYPE DECLARE
#define REQUIRE_PATH_TYPE    DECLARE
#define REQUIRE_LA_TYPE      DECLARE

#include <z/cenv.h>

static VALUE path_basename (la_t *this, VALUE v) {
  (void) this;
  string *p = AS_STRING(v);
  char *tmp = Cstring.dup (p->bytes, p->num_bytes);
  char *bn = Path.basename (tmp);
  string *s = String.new_with (bn);
  free (tmp);
  VALUE r = STRING(s);
  return r;
}

static VALUE path_basename_sans_extname (la_t *this, VALUE v) {
  (void) this;
  string *p = AS_STRING(v);
  char *tmp = Cstring.dup (p->bytes, p->num_bytes);
  char *bn = Path.basename_sans_extname (tmp);
  string *s = String.new_with (bn);
  free (tmp);
  free (bn);
  VALUE r = STRING(s);
  return r;
}

static VALUE path_extname (la_t *this, VALUE v) {
  (void) this;
  string *p = AS_STRING(v);
  char *n = Path.extname (p->bytes);
  string *s = String.new_with (n);
  VALUE r = STRING(s);
  return r;
}

static VALUE path_dirname (la_t *this, VALUE v) {
  (void) this;
  string *p = AS_STRING(v);
  char *n = Path.dirname (p->bytes);
  string *s = String.new_with (n);
  free (n);
  VALUE r = STRING(s);
  return r;
}

static VALUE path_is_absolute (la_t *this, VALUE v) {
  (void) this;
  string *p = AS_STRING(v);
  VALUE r = INT(Path.is_absolute (p->bytes));
  return r;
}

static VALUE path_real (la_t *this, VALUE v) {
  La.set.Errno (this, 0);
  string *p = AS_STRING(v);
  char buf[PATH_MAX];
  char *n = Path.real (p->bytes, buf);
  VALUE r;
  if (n is NULL) {
    La.set.Errno (this, errno);
    r = NULL_VALUE;
  } else {
    string *s = String.new_with (n);
    r = STRING(s);
  }
  return r;
}

static VALUE path_split (la_t *this, VALUE v) {
  (void) this;
  string *p = AS_STRING(v);
  Vstring_t *a = Path.split (p->bytes);
  ArrayType *array = ARRAY_NEW(STRING_TYPE, a->num_items);
  string **ar = (string **) AS_ARRAY(array->value);
  vstring_t *it = a->head;
  int idx = 0;
  while (it) {
    String.replace_with (ar[idx++], it->data->bytes);
    it = it->next;
  }
  Vstring.release (a);
  return ARRAY(array);
}

#define EvalString(...) #__VA_ARGS__

public int __init_path_module__ (la_t *this) {
  __INIT_MODULE__(this);
  __INIT__(path);
  __INIT__(string);
  __INIT__(cstring);
  __INIT__(vstring);

  LaDefCFun lafuns[] = {
    { "path_real",        PTR(path_real), 1 },
    { "path_split",       PTR(path_split), 1 },
    { "path_extname",     PTR(path_extname), 1 },
    { "path_dirname",     PTR(path_dirname), 1 },
    { "path_basename",    PTR(path_basename), 1 },
    { "path_is_absolute", PTR(path_is_absolute), 1 },
    { "path_basename_sans_extname", PTR(path_basename_sans_extname), 1 },
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Path = {
       "real" : path_real,
       "split" : path_split,
       "dirname"  : path_dirname,
       "extname"  : path_extname,
       "basename" : path_basename,
       "is_absolute" : path_is_absolute,
       "basename_sans_extname" : path_basename_sans_extname
     }
   );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_path_module__ (la_t *this) {
  (void) this;
  return;
}
