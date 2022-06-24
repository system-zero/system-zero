#define REQUIRE_STDIO
#define REQUIRE_STDARG
#define REQUIRE_UNISTD

#define REQUIRE_STD_MODULE
#define REQUIRE_DIR_TYPE     DECLARE

#include <z/cenv.h>

MODULE(dir)

static VALUE dir_remove (la_t *this, VALUE v_dir) {
  ifnot (IS_STRING(v_dir))
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");

  return INT(Dir.rm (AS_STRING_BYTES(v_dir), DirOpts(.err = 0)));
}

static VALUE dir_remove_parents (la_t *this, VALUE v_dir) {
  ifnot (IS_STRING(v_dir))
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");

  return INT(Dir.rm_parents (AS_STRING_BYTES(v_dir), DirOpts(.err = 0)));
}

static VALUE dir_make (la_t *this, VALUE v_dir, VALUE v_mode) {
  ifnot (IS_STRING(v_dir))
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");

  ifnot (IS_INT(v_mode))
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer");

  return INT(Dir.make (AS_STRING_BYTES(v_dir), AS_INT(v_mode), DirOpts(.err = 0)));
}

static VALUE dir_make_parents (la_t *this, VALUE v_dir, VALUE v_mode) {
  ifnot (IS_STRING(v_dir))
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");

  ifnot (IS_INT(v_mode))
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer");

  return INT(Dir.make_parents (AS_STRING_BYTES(v_dir), AS_INT(v_mode), DirOpts(.err = 0)));
}

static VALUE dir_is_directory (la_t *this, VALUE v_dir) {
  ifnot (IS_STRING(v_dir))
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");

  return INT(Dir.is_directory (AS_STRING_BYTES(v_dir)));
}

static VALUE dir_current (la_t *this) {
  (void) this;
  char *dir = Dir.current ();
  if (NULL is dir)
    return NULL_VALUE;

  string *cwd = String.new_with (dir);
  free (dir);
  return (STRING(cwd));
}

static VALUE dir_change (la_t *this, VALUE v_dir) {
  ifnot (IS_STRING(v_dir))
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");

  La.set.Errno (this, 0);
  int retval = chdir (AS_STRING_BYTES(v_dir));
  if (retval)
    La.set.Errno (this, errno);
  return INT(retval);
}

static VALUE dir_list_recursive (la_t *this, VALUE v_dir, VALUE v_depth) {
  ifnot (IS_STRING(v_dir))
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");

  ifnot (IS_INT(v_depth))
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer");

  char *dir = AS_STRING_BYTES (v_dir);
  int depth = AS_INT(v_depth);

  if (depth < 0) depth = DIRWALK_MAX_DEPTH;

  dirwalk_t *dw = NULL;

  dw = Dir.walk.new (NULL, NULL);
  dw->depth = depth;
  Dir.walk.run (dw, dir);

  ArrayType *array = ARRAY_NEW(STRING_TYPE, dw->files->num_items);
  string **ar = (string **) AS_ARRAY(array->value);

  vstring_t *it = dw->files->head;

  int idx = 0;
  while (it) {
    String.replace_with_len (ar[idx++], it->data->bytes, it->data->num_bytes);
    it = it->next;
  }

  Dir.walk.release (&dw);
  return ARRAY(array);
}

/* temporarly method, as it needs a serious revision, so ignore */
static VALUE dir_list (la_t *this, VALUE v_dir) {
  ifnot (IS_STRING(v_dir))
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");

  int recursive = GET_OPT_RECURSIVE();
  if (recursive) {
    int depth = GET_OPT_DEPTH();
    ifnot (depth) depth = -1;
    return dir_list_recursive (this, v_dir, INT(depth));
  }

  char *dir = AS_STRING_BYTES (v_dir);

  ifnot (Dir.is_directory (dir)) {
    return NULL_VALUE;
    // ArrayType *array = ARRAY_NEW(STRING_TYPE, 1);
    // string **ar = (string **) AS_ARRAY(array->value);
    // String.replace_with (ar[0], dir);
    // return ARRAY(array);
  }

  dirlist_t *dlist = Dir.list (dir, 0);
  if (NULL is dlist)
    return NULL_VALUE;

  Vstring_t *vs = Vstring.new ();
  vstring_t *it = dlist->list->head;

  while (it) {
    Vstring.add.sort_and_uniq (vs, it->data->bytes);
    it = it->next;
  }

  dlist->release (dlist);

  ArrayType *array = ARRAY_NEW(STRING_TYPE, vs->num_items);
  string **ar = (string **) AS_ARRAY(array->value);

  it = vs->head;
  int idx = 0;
  while (it) {
    String.replace_with_len (ar[idx++], it->data->bytes, it->data->num_bytes);
    it = it->next;
  }

  Vstring.release (vs);

  return ARRAY(array);
}

public int __init_dir_module__ (la_t *this) {
  __INIT_MODULE__(this);
  __INIT__(dir);

  LaDefCFun lafuns[] = {
    { "dir_make",           PTR(dir_make), 2 },
    { "dir_list",           PTR(dir_list), 1 },
    { "dir_remove",         PTR(dir_remove), 1 },
    { "dir_change",         PTR(dir_change), 1 },
    { "dir_current",        PTR(dir_current), 0 },
    { "dir_is_directory",   PTR(dir_is_directory), 1 },
    { "dir_make_parents",   PTR(dir_make_parents), 2 },
    { "dir_remove_parents", PTR(dir_remove_parents), 1 },
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Dir = {
       "make"   : dir_make,
       "list"   : dir_list,
       "remove" : dir_remove,
       "change" : dir_change,
       "current" : dir_current,
       "is_directory" : dir_is_directory,
       "make_parents" : dir_make_parents,
       "remove_parents" : dir_remove_parents
     }
   );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_dir_module__ (la_t *this) {
  (void) this;
  return;
}
