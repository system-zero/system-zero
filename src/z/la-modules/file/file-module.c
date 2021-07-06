#define REQUIRE_STDIO
#define REQUIRE_STDARG
#define REQUIRE_SYS_TYPES
#define REQUIRE_SYS_STAT
#define REQUIRE_UNISTD

#define REQUIRE_VMAP_TYPE     DECLARE
#define REQUIRE_VSTRING_TYPE  DONOT_DECLARE
#define REQUIRE_STRING_TYPE   DONOT_DECLARE
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
    La.set.Errno (this, errno);
    r = NULL_VALUE;
  } else {
    r = STRING(s);
  }

  return r;
}

static VALUE file_hardlink (la_t *this, VALUE v_src_file, VALUE v_dest_file) {
  La.set.Errno (this, 0);
  VALUE r;
  char *src_file = AS_STRING_BYTES(v_src_file);
  char *dest_file = AS_STRING_BYTES(v_dest_file);
  int retval = link (src_file, dest_file);
  if (retval is -1) {
    La.set.Errno (this, errno);
    r = INT(LA_NOTOK);
  } else
    r = INT(LA_OK);

  return r;
}

static VALUE file_symlink (la_t *this, VALUE v_src_file, VALUE v_dest_file) {
  La.set.Errno (this, 0);
  VALUE r;
  char *src_file = AS_STRING_BYTES(v_src_file);
  char *dest_file = AS_STRING_BYTES(v_dest_file);
  int retval = symlink (src_file, dest_file);
  if (retval is -1) {
    La.set.Errno (this, errno);
    r = INT(LA_NOTOK);
  } else
    r = INT(LA_OK);

  return r;
}

static VALUE file_chown (la_t *this, VALUE v_file, VALUE v_uid, VALUE v_gid) {
  VALUE r;
  La.set.Errno (this, 0);

  char *file = AS_STRING_BYTES(v_file);
  uid_t uid = (uid_t) AS_INT(v_uid);
  gid_t gid = (gid_t) AS_INT(v_gid);

  int retval = chown (file, uid, gid);
  if (retval is -1) {
    La.set.Errno (this, errno);
    r = INT(LA_NOTOK);
  } else
    r = INT(LA_OK);

  return r;
}

static VALUE file_chmod (la_t *this, VALUE v_file, VALUE v_mode) {
  VALUE r;
  La.set.Errno (this, 0);

  char *file = AS_STRING_BYTES(v_file);
  mode_t mode = (mode_t) AS_INT(v_mode);

  int retval = chmod (file, mode);
  if (retval is -1) {
    La.set.Errno (this, errno);
    r = INT(LA_NOTOK);
  } else
    r = INT(LA_OK);

  return r;
}

static VALUE file_mkfifo (la_t *this, VALUE v_file, VALUE v_mode) {
  VALUE r;
  La.set.Errno (this, 0);

  char *file = AS_STRING_BYTES(v_file);
  int mode = AS_INT(v_mode);

  int retval = mkfifo (file, mode);
  if (retval is -1) {
    La.set.Errno (this, errno);
    r = INT(LA_NOTOK);
  } else
    r = INT(LA_OK);

  return r;
}

static VALUE file_remove (la_t *this, VALUE v_file) {
  VALUE r;
  char *file = AS_STRING_BYTES(v_file);
  La.set.Errno (this, 0);

  int retval = unlink (file);
  if (retval is -1) {
    La.set.Errno (this, errno);
    r = INT(LA_NOTOK);
  } else
    r = INT(LA_OK);

  return r;
}

static VALUE file_rename (la_t *this, VALUE v_src_file, VALUE v_dest_file) {
  La.set.Errno (this, 0);
  VALUE r;

  char *src_file = AS_STRING_BYTES(v_src_file);
  char *dest_file = AS_STRING_BYTES(v_dest_file);
  int retval = rename (src_file, dest_file);
  if (retval is -1) {
    La.set.Errno (this, errno);
    r = INT(LA_NOTOK);
  } else
    r = INT(LA_OK);

  return r;
}

static VALUE file_access (la_t *this, VALUE v_file, VALUE v_mode) {
  VALUE r;
  char *file = AS_STRING_BYTES(v_file);
  int mode = AS_INT(v_mode);
  La.set.Errno (this, 0);
  int retval = access (file, mode);
  if (retval is -1) {
    La.set.Errno (this, errno);
    r = INT(LA_NOTOK);
  } else
    r = INT(LA_OK);

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

static VALUE file_mode_to_string (la_t *this, VALUE v_mode) {
  (void) this;
  mode_t mode = AS_INT(v_mode);
  char mode_string[12];
  File.mode.stat_to_string (mode_string, mode);
  string *s = String.new_with (mode_string);
  return STRING(s);
}

static VALUE file_mode_to_octal_string (la_t *this, VALUE v_mode) {
  (void) this;
  mode_t mode = AS_INT(v_mode);
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

  (void) vstringType;

  LaDefCFun lafuns[] = {
    { "file_stat",       PTR(file_stat), 1 },
    { "file_size",       PTR(file_size), 1 },
    { "file_chown",      PTR(file_chown), 3 },
    { "file_chmod",      PTR(file_chmod), 2 },
    { "file_exists",     PTR(file_exists), 1 },
    { "file_access",     PTR(file_access), 2 },
    { "file_mkfifo",     PTR(file_mkfifo), 2 },
    { "file_remove",     PTR(file_remove), 1 },
    { "file_rename",     PTR(file_rename), 2 },
    { "file_symlink",    PTR(file_symlink), 2 },
    { "file_hardlink",   PTR(file_hardlink), 2 },
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

  if (La.def_std (this, "F_OK", INTEGER_TYPE, INT(F_OK), 1))
    return LA_NOTOK;
  if (La.def_std (this, "X_OK", INTEGER_TYPE, INT(X_OK), 1))
    return LA_NOTOK;
  if (La.def_std (this, "W_OK", INTEGER_TYPE, INT(W_OK), 1))
    return LA_NOTOK;
  if (La.def_std (this, "R_OK", INTEGER_TYPE, INT(R_OK), 1))
    return LA_NOTOK;
  if (La.def_std (this, "S_IRWXU", INTEGER_TYPE, INT(S_IRWXU), 1))
    return LA_NOTOK;
  if (La.def_std (this, "S_IRUSR", INTEGER_TYPE, INT(S_IRUSR), 1))
    return LA_NOTOK;
  if (La.def_std (this, "S_IWUSR", INTEGER_TYPE, INT(S_IWUSR), 1))
    return LA_NOTOK;
  if (La.def_std (this, "S_IXUSR", INTEGER_TYPE, INT(S_IXUSR), 1))
    return LA_NOTOK;
  if (La.def_std (this, "S_IRWXG", INTEGER_TYPE, INT(S_IRWXG), 1))
    return LA_NOTOK;
  if (La.def_std (this, "S_IRGRP", INTEGER_TYPE, INT(S_IRGRP), 1))
    return LA_NOTOK;
  if (La.def_std (this, "S_IWGRP", INTEGER_TYPE, INT(S_IWGRP), 1))
    return LA_NOTOK;
  if (La.def_std (this, "S_IXGRP", INTEGER_TYPE, INT(S_IXGRP), 1))
    return LA_NOTOK;
  if (La.def_std (this, "S_IRWXO", INTEGER_TYPE, INT(S_IRWXO), 1))
    return LA_NOTOK;
  if (La.def_std (this, "S_IROTH", INTEGER_TYPE, INT(S_IROTH), 1))
    return LA_NOTOK;
  if (La.def_std (this, "S_IWOTH", INTEGER_TYPE, INT(S_IWOTH), 1))
    return LA_NOTOK;
  if (La.def_std (this, "S_IXOTH", INTEGER_TYPE, INT(S_IXOTH), 1))
    return LA_NOTOK;


  const char evalString[] = EvalString (
    public var File = {
       "stat" : file_stat,
       "size" : file_size,
       "chown" : file_chown,
       "chmod" : file_chmod,
       "exists" : file_exists,
       "access" : file_access,
       "mkfifo" : file_mkfifo,
       "remove" : file_remove,
       "rename" : file_rename,
       "symlink" : file_symlink,
       "hardlink" : file_hardlink,
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
