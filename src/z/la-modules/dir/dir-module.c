#define REQUIRE_STDIO
#define REQUIRE_STDARG
#define REQUIRE_UNISTD

#define REQUIRE_VMAP_TYPE    DONOT_DECLARE
#define REQUIRE_VSTRING_TYPE DONOT_DECLARE
#define REQUIRE_STRING_TYPE  DECLARE
#define REQUIRE_DIR_TYPE     DECLARE
#define REQUIRE_LA_TYPE      DECLARE

#include <z/cenv.h>

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

#define EvalString(...) #__VA_ARGS__

public int __init_dir_module__ (la_t *this) {
  __INIT_MODULE__(this);
  __INIT__(dir);
  __INIT__(string);

  (void) vstringType;
  (void) vmapType;

  LaDefCFun lafuns[] = {
    { "dir_make",           PTR(dir_make), 2 },
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
