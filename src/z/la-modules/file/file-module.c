#define REQUIRE_STDIO
#define REQUIRE_STDARG
#define REQUIRE_SYS_TYPES
#define REQUIRE_SYS_STAT
#define REQUIRE_UNISTD

#define REQUIRE_VMAP_TYPE     DECLARE
#define REQUIRE_VSTRING_TYPE  DONOT_DECLARE
#define REQUIRE_STRING_TYPE   DECLARE
#define REQUIRE_FILE_TYPE     DECLARE
#define REQUIRE_LA_TYPE       DECLARE

#include <z/cenv.h>

static VALUE file_exists (la_t *this, VALUE v_file) {
  (void) this;
  char *file = AS_STRING_BYTES(v_file);
  VALUE r = INT(File.exists (file));
  return r;
}

static VALUE file_size (la_t *this, VALUE v_file) {
  (void) this;
  char *file = AS_STRING_BYTES(v_file);
  VALUE r = INT(File.size (file));
  return r;
}

static VALUE file_readlink (la_t *this, VALUE v_file) {
  La.set.Errno (this, 0);
  VALUE r;
  char *file = AS_STRING_BYTES(v_file);
  string *s = File.readlink (file);
  if (NULL is s) {
    La.set.Errno (this, 0);
    r = NULL_VALUE;
  } else {
    r = STRING(s);
  }

  return r;
}

static VALUE file_stat (la_t *this, VALUE v_file) {
  La.set.Errno (this, 0);

  VALUE r;
  char *file = AS_STRING_BYTES(v_file);
  ifnot (File.exists (file)) {
    La.set.Errno (this, ENOENT);
    r = NULL_VALUE;
    return r;
  }

  struct stat st;
  int retval = stat (file, &st);
  if (-1 is retval) {
    La.set.Errno (this, errno);
    r = NULL_VALUE;
    return r;
  }

  Vmap_t *m = Vmap.new (16);
  La.map.set_value (this, m, "st_dev",   INT(st.st_dev), 1);
  La.map.set_value (this, m, "st_ino",   INT(st.st_ino), 1);
  La.map.set_value (this, m, "st_mode",  INT(st.st_mode), 1);
  La.map.set_value (this, m, "st_nlink", INT(st.st_nlink), 1);
  La.map.set_value (this, m, "st_uid",   INT(st.st_uid), 1);
  La.map.set_value (this, m, "st_gid",   INT(st.st_gid), 1);
  La.map.set_value (this, m, "st_rdev",  INT(st.st_rdev), 1);
  La.map.set_value (this, m, "st_size",  INT(st.st_size), 1);
  La.map.set_value (this, m, "st_atime", INT(st.st_atime), 1);
  La.map.set_value (this, m, "st_mtime", INT(st.st_mtime), 1);
  La.map.set_value (this, m, "st_ctime", INT(st.st_ctime), 1);
  La.map.set_value (this, m, "st_blocks", INT(st.st_blocks), 1);
  La.map.set_value (this, m, "st_blksize", INT(st.st_blksize), 1);

  r = MAP(m);
  return r;
}

static VALUE file_mode_to_string (la_t *this, mode_t mode) {
  char mode_string[12];
  File.mode.stat_to_string (mode_string, mode);
  string *s = String.new_with (mode_string);
  return STRING(s);
}

static VALUE file_mode_to_octal_string (la_t *this, mode_t mode) {
  char oct_str[16];
  snprintf (oct_str, 16, "%o", mode);
  string *s = String.new_with (oct_str+2);
  return STRING(s);
}

#define EvalString(...) #__VA_ARGS__

public int __init_file_module__ (la_t *this) {
  __INIT_MODULE__(this);
  __INIT__(file);
  __INIT__(vmap);
  __INIT__(string);

  LaDefCFun lafuns[] = {
    { "file_stat",       PTR(file_stat), 1 },
    { "file_size",       PTR(file_size), 1 },
    { "file_exists",     PTR(file_exists), 1 },
    { "file_readlink",   PTR(file_readlink), 1 },
    { "file_mode_to_string", PTR(file_mode_to_string), 1 },
    { "file_mode_to_octal_string", PTR(file_mode_to_octal_string), 1 },
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var File = {
       "stat" : file_stat,
       "size" : file_size,
       "exists" : file_exists,
       "readlink" : file_readlink,
       "mode_to_string" : file_mode_to_string,
       "mode_to_octal_string" : file_mode_to_octal_string
     }
   );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_file_module__ (la_t *this) {
  (void) this;
  return;
}
