#define REQUIRE_STDIO
#define REQUIRE_STDARG

#define REQUIRE_STD_MODULE
#define REQUIRE_CSTRING_TYPE DECLARE
#define REQUIRE_PATH_TYPE    DECLARE

#include <z/cenv.h>

MODULE(path)

static VALUE path_basename (la_t *this, VALUE v_path) {
  (void) this;
  ifnot (IS_STRING(v_path)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  string *p = AS_STRING(v_path);
  char *tmp = Cstring.dup (p->bytes, p->num_bytes);
  char *bn = Path.basename (tmp);
  string *s = String.new_with (bn);
  free (tmp);
  return STRING(s);
}

static VALUE path_basename_sans_extname (la_t *this, VALUE v_path) {
  (void) this;
  ifnot (IS_STRING(v_path)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  string *p = AS_STRING(v_path);
  char *tmp = Cstring.dup (p->bytes, p->num_bytes);
  char *bn = Path.basename_sans_extname (tmp);
  string *s = String.new_with (bn);
  free (tmp);
  free (bn);
  return STRING(s);
}

static VALUE path_extname (la_t *this, VALUE v_path) {
  (void) this;
  ifnot (IS_STRING(v_path)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  string *p = AS_STRING(v_path);
  char *n = Path.extname (p->bytes);
  string *s = String.new_with (n);
  return STRING(s);
}

static VALUE path_dirname (la_t *this, VALUE v_path) {
  (void) this;
  ifnot (IS_STRING(v_path)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  string *p = AS_STRING(v_path);
  char *n = Path.dirname (p->bytes);
  string *s = String.new_with (n);
  free (n);
  return STRING(s);
}

static VALUE path_is_absolute (la_t *this, VALUE v_path) {
  (void) this;
  ifnot (IS_STRING(v_path)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  string *p = AS_STRING(v_path);
  return INT(Path.is_absolute (p->bytes));
}

static VALUE path_real (la_t *this, VALUE v_path) {
  ifnot (IS_STRING(v_path)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");

  La.set.Errno (this, 0);

  string *p = AS_STRING(v_path);
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

static VALUE path_split (la_t *this, VALUE v_path) {
  (void) this;
  ifnot (IS_STRING(v_path)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  string *p = AS_STRING(v_path);
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

static VALUE path_concat (la_t *this, VALUE v_apath, VALUE v_bpath) {
  (void) this;
  ifnot (IS_STRING(v_apath)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_STRING(v_bpath)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  string *pa = AS_STRING(v_apath);
  string *pb = AS_STRING(v_bpath);

  string *new = String.dup (pa);
  String.trim_end (new, DIR_SEP);

  int idx = 0;
  while (pb->bytes[idx] is DIR_SEP) idx++;
  String.append_byte (new, DIR_SEP);
  String.append_with (new, pb->bytes + idx);

  return STRING(new);
}

#define EvalString(...) #__VA_ARGS__

public int __init_path_module__ (la_t *this) {
  __INIT_MODULE__(this);
  __INIT__(path);
  __INIT__(cstring);

  LaDefCFun lafuns[] = {
    { "path_real",        PTR(path_real), 1 },
    { "path_split",       PTR(path_split), 1 },
    { "path_concat",      PTR(path_concat), 2 },
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
       "concat" : path_concat,
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
