#define REQUIRE_STDIO
#define REQUIRE_STDARG
#define REQUIRE_SYS_TYPES
#define REQUIRE_SYS_STAT
#define REQUIRE_UNISTD
#define REQUIRE_FCNTL

#define REQUIRE_IO_TYPE       DECLARE
#define REQUIRE_VMAP_TYPE     DECLARE
#define REQUIRE_VSTRING_TYPE  DECLARE
#define REQUIRE_STRING_TYPE   DECLARE
#define REQUIRE_FILE_TYPE     DECLARE
#define REQUIRE_LA_TYPE       DECLARE

#include <z/cenv.h>

static VALUE file_exists (la_t *this, VALUE v_file) {
  (void) this;
  ifnot (IS_STRING(v_file)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");

  char *file = AS_STRING_BYTES(v_file);
  return INT(File.exists (file));
}

static VALUE file_size (la_t *this, VALUE v_file) {
  (void) this;
  ifnot (IS_STRING(v_file)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");

  char *file = AS_STRING_BYTES(v_file);
  return INT(File.size (file));
}

static VALUE file_readlink (la_t *this, VALUE v_file) {
  ifnot (IS_STRING(v_file)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");

  char *file = AS_STRING_BYTES(v_file);

  La.set.Errno (this, 0);

  string *s = File.readlink (file);
  if (NULL is s) {
    La.set.Errno (this, errno);
    return NULL_VALUE;
  } else {
    return STRING(s);
  }

  return NULL_VALUE;
}

static VALUE file_hardlink (la_t *this, VALUE v_src_file, VALUE v_dest_file) {
  ifnot (IS_STRING(v_src_file)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_STRING(v_dest_file)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");

  char *src_file = AS_STRING_BYTES(v_src_file);
  char *dest_file = AS_STRING_BYTES(v_dest_file);

  La.set.Errno (this, 0);

  int retval = link (src_file, dest_file);
  if (retval is -1) {
    La.set.Errno (this, errno);
    return NOTOK_VALUE;
  } else
    return OK_VALUE;

  return OK_VALUE;
}

static VALUE file_symlink (la_t *this, VALUE v_src_file, VALUE v_dest_file) {
  ifnot (IS_STRING(v_src_file)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_STRING(v_dest_file)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");

  char *src_file = AS_STRING_BYTES(v_src_file);
  char *dest_file = AS_STRING_BYTES(v_dest_file);

  La.set.Errno (this, 0);

  int retval = symlink (src_file, dest_file);
  if (retval is -1) {
    La.set.Errno (this, errno);
    return NOTOK_VALUE;
  } else
    return OK_VALUE;

  return OK_VALUE;
}

static VALUE file_chown (la_t *this, VALUE v_file, VALUE v_uid, VALUE v_gid) {
  ifnot (IS_STRING(v_file)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_INT(v_uid)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer");
  ifnot (IS_INT(v_gid)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer");

  char *file = AS_STRING_BYTES(v_file);
  uid_t uid = (uid_t) AS_INT(v_uid);
  gid_t gid = (gid_t) AS_INT(v_gid);

  La.set.Errno (this, 0);

  int retval = chown (file, uid, gid);
  if (retval is -1) {
    La.set.Errno (this, errno);
    return NOTOK_VALUE;
  } else
    return OK_VALUE;

  return OK_VALUE;
}

static VALUE file_chmod (la_t *this, VALUE v_file, VALUE v_mode) {
  ifnot (IS_STRING(v_file)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_INT(v_mode)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer");

  char *file = AS_STRING_BYTES(v_file);
  mode_t mode = (mode_t) AS_INT(v_mode);

  La.set.Errno (this, 0);

  int retval = chmod (file, mode);
  if (retval is -1) {
    La.set.Errno (this, errno);
    return NOTOK_VALUE;
  } else
    return OK_VALUE;

  return OK_VALUE;
}

static VALUE file_mkfifo (la_t *this, VALUE v_file, VALUE v_mode) {
  ifnot (IS_STRING(v_file)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_INT(v_mode)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer");

  char *file = AS_STRING_BYTES(v_file);
  int mode = AS_INT(v_mode);

  La.set.Errno (this, 0);

  int retval = mkfifo (file, mode);
  if (retval is -1) {
    La.set.Errno (this, errno);
    return NOTOK_VALUE;
  } else
    return OK_VALUE;

  return OK_VALUE;
}

static VALUE file_remove (la_t *this, VALUE v_file) {
  ifnot (IS_STRING(v_file)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");

  char *file = AS_STRING_BYTES(v_file);

  La.set.Errno (this, 0);

  int retval = unlink (file);
  if (retval is -1) {
    La.set.Errno (this, errno);
    return NOTOK_VALUE;
  } else
    return OK_VALUE;

  return OK_VALUE;
}

static VALUE file_rename (la_t *this, VALUE v_src_file, VALUE v_dest_file) {
  ifnot (IS_STRING(v_src_file)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_STRING(v_dest_file)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");

  char *src_file = AS_STRING_BYTES(v_src_file);
  char *dest_file = AS_STRING_BYTES(v_dest_file);

  La.set.Errno (this, 0);

  int retval = rename (src_file, dest_file);
  if (retval is -1) {
    La.set.Errno (this, errno);
    return NOTOK_VALUE;
  } else
    return OK_VALUE;

  return OK_VALUE;
}

static VALUE file_access (la_t *this, VALUE v_file, VALUE v_mode) {
  ifnot (IS_STRING(v_file)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_INT(v_mode)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer");

  char *file = AS_STRING_BYTES(v_file);
  int mode = AS_INT(v_mode);
  La.set.Errno (this, 0);

  int retval = access (file, mode);
  if (retval is -1) {
    La.set.Errno (this, errno);
    return NOTOK_VALUE;
  } else
    return OK_VALUE;

  return OK_VALUE;
}

static VALUE do_stat (la_t *this, char *file, int (*fun) (const char *, struct stat *)) {
  struct stat st;
  int retval = fun (file, &st);
  if (-1 is retval) {
    La.set.Errno (this, errno);
    return NULL_VALUE;
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

  return MAP(m);
}

static VALUE file_stat (la_t *this, VALUE v_file) {
  ifnot (IS_STRING(v_file)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");

  La.set.Errno (this, 0);

  char *file = AS_STRING_BYTES(v_file);
  ifnot (File.exists (file)) {
    La.set.Errno (this, ENOENT);
    return NULL_VALUE;
  }

  return do_stat (this, file, stat);
}

static VALUE file_lstat (la_t *this, VALUE v_file) {
  ifnot (IS_STRING(v_file)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");

  La.set.Errno (this, 0);

  char *file = AS_STRING_BYTES(v_file);
  ifnot (File.exists (file)) {
    La.set.Errno (this, ENOENT);
    return NULL_VALUE;
  }

  return do_stat (this, file, lstat);
}

static VALUE file_mode_to_string (la_t *this, VALUE v_mode) {
  (void) this;
  ifnot (IS_INT(v_mode)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer");

  mode_t mode = AS_INT(v_mode);
  char mode_string[12];
  File.mode.stat_to_string (mode_string, mode);
  string *s = String.new_with (mode_string);
  return STRING(s);
}

static VALUE file_mode_to_octal_string (la_t *this, VALUE v_mode) {
  (void) this;
  ifnot (IS_INT(v_mode)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer");

  mode_t mode = AS_INT(v_mode);
  char oct_str[16];
  snprintf (oct_str, 16, "%o", mode);
  string *s = String.new_with (oct_str+2);
  if ('0' isnot s->bytes[0])  String.prepend_byte (s, '0');
  return STRING(s);
}

static VALUE file_is_rwx (la_t *this, VALUE v_file) {
  (void) this;
  ifnot (IS_STRING(v_file)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  char *file = AS_STRING_BYTES(v_file);
  return INT(File.is_rwx (file));
}

static VALUE file_is_reg (la_t *this, VALUE v_file) {
  (void) this;
  ifnot (IS_STRING(v_file)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  char *file = AS_STRING_BYTES(v_file);
  return INT(File.is_reg (file));
}

static VALUE file_is_lnk (la_t *this, VALUE v_file) {
  (void) this;
  ifnot (IS_STRING(v_file)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  char *file = AS_STRING_BYTES(v_file);
  return INT(File.is_lnk (file));
}

static VALUE file_is_fifo (la_t *this, VALUE v_file) {
  (void) this;
  ifnot (IS_STRING(v_file)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  char *file = AS_STRING_BYTES(v_file);
  return INT(File.is_fifo (file));
}

static VALUE file_is_sock (la_t *this, VALUE v_file) {
  (void) this;
  ifnot (IS_STRING(v_file)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  char *file = AS_STRING_BYTES(v_file);
  return INT(File.is_sock (file));
}

static VALUE file_is_readable (la_t *this, VALUE v_file) {
  (void) this;
  ifnot (IS_STRING(v_file)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  char *file = AS_STRING_BYTES(v_file);
  return INT(File.is_readable (file));
}

static VALUE file_is_writable (la_t *this, VALUE v_file) {
  (void) this;
  ifnot (IS_STRING(v_file)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  char *file = AS_STRING_BYTES(v_file);
  return INT(File.is_writable (file));
}

static VALUE file_is_executable (la_t *this, VALUE v_file) {
  (void) this;
  ifnot (IS_STRING(v_file)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  char *file = AS_STRING_BYTES(v_file);
  return INT(File.is_executable (file));
}

static VALUE file_readlines (la_t *this, VALUE v_file) {
  (void) this;
  ifnot (IS_STRING(v_file)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");

  char *file = AS_STRING_BYTES(v_file);

  Vstring_t *vs = File.readlines (file, NULL, NULL, NULL);
  if (NULL is vs)
    return NULL_VALUE;

  ArrayType *array = ARRAY_NEW(STRING_TYPE, vs->num_items);
  string **ar = (string **) AS_ARRAY(array->value);

  vstring_t *it = vs->head;

  int idx = 0;
  while (it) {
    String.replace_with_len (ar[idx++], it->data->bytes, it->data->num_bytes);
    it = it->next;
  }

  Vstring.release (vs);

  return ARRAY(array);
}

static VALUE file_writelines (la_t *this, VALUE v_file, VALUE v_ar) {
  (void) this;
  ifnot (IS_STRING(v_file)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_ARRAY(v_ar)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting an array");

  char *file = AS_STRING_BYTES(v_file);
  ArrayType *array = (ArrayType *) AS_ARRAY(v_ar);
  if (array->type isnot STRING_TYPE)
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string type array");

  FILE *fp = fopen (file, "w");
  if (NULL is fp)
    return NOTOK_VALUE;

  string **ar = (string **) AS_ARRAY(array->value);

  for (size_t i = 0; i < array->len; i++) {
    if (fprintf (fp, "%s\n", ar[i]->bytes) isnot (int) ar[i]->num_bytes + 1) {
      fclose (fp);
      return NOTOK_VALUE;
    }
  }

  fclose (fp);
  return OK_VALUE;
}

static VALUE file_copy (la_t *this, VALUE v_src, VALUE v_dest) {
  ifnot (IS_STRING(v_src)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_STRING(v_dest)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");

  char *src = AS_STRING_BYTES(v_src);
  char *dest = AS_STRING_BYTES(v_dest);

  VALUE v_verbose = La.get.qualifier (this, "verbose", INT(0));
  ifnot (IS_INT(v_verbose)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");

  VALUE v_force = La.get.qualifier (this, "force", INT(0));
  ifnot (IS_INT(v_force)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");

  VALUE v_follow_lnk = La.get.qualifier (this, "follow_lnk", INT(0));
  ifnot (IS_INT(v_follow_lnk)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");

  VALUE v_preserve = La.get.qualifier (this, "preserve", INT(0));
  ifnot (IS_INT(v_preserve)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");

  VALUE v_backup = La.get.qualifier (this, "backup", INT(0));
  ifnot (IS_INT(v_backup)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");

  VALUE v_recursive = La.get.qualifier (this, "recursive", INT(0));
  ifnot (IS_INT(v_recursive)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");

  VALUE v_all = La.get.qualifier (this, "all", INT(0));
  ifnot (IS_INT(v_all)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");

  VALUE v_update = La.get.qualifier (this, "update", INT(0));
  ifnot (IS_INT(v_update)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");

  int verbose = AS_INT(v_verbose);
  int force = AS_INT(v_force);
  int follow_lnk = AS_INT(v_follow_lnk);
  int preserve = AS_INT(v_preserve);
  int backup = AS_INT(v_backup);
  int recursive = AS_INT(v_recursive);
  int update = AS_INT(v_update);
  int all = AS_INT(v_all);

  FILE *fp = NULL;
  VALUE v_stream = La.get.qualifier (this, "out_stream", NULL_VALUE);
  if (IS_NULL(v_stream)) {
    fp = stdout;
  } else {
    ifnot (IS_FILEPTR(v_stream)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a file pointer");
    object *o = AS_OBJECT(v_stream);
    fp = (FILE *) AS_PTR(o->value);
  }

  La.set.Errno (this, errno);

  int retval = File.copy (src, dest, FileCopyOpts (
      .verbose = verbose, .force = force,
      .backup = backup, .follow_lnk = follow_lnk,
      .preserve = preserve, .recursive = recursive,
      .update = update, .all = all,
      .out_stream = fp));

  if (retval is NOTOK)
    La.set.Errno (this, errno);

  return INT(retval);
}

static VALUE file_type_to_string (la_t *this, VALUE v_mode) {
  ifnot (IS_INT(v_mode)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer");
  int mode = AS_INT(v_mode);
  string *s = NULL;

  if (S_ISSOCK (mode))
    s = String.new_with_len ("socket", 6);
  else if (S_ISFIFO (mode))
    s = String.new_with_len ("fifo", 4);
  else if (S_ISBLK (mode))
    s = String.new_with_len ("block special file", 18);
  else if (S_ISDIR (mode))
    s = String.new_with_len ("directory", 9);
  else if (S_ISREG (mode))
    s = String.new_with_len ("regular file", 12);
  else if (S_ISLNK (mode))
    s = String.new_with_len ("symbolic link", 13);
  else if (S_ISCHR (mode))
    s = String.new_with_len ("character special", 17);
  else
    s = String.new_with_len ("unknown type", 12);

  return STRING(s);
}

#define EvalString(...) #__VA_ARGS__

public int __init_file_module__ (la_t *this) {
  __INIT_MODULE__(this);
  __INIT__(io);
  __INIT__(file);
  __INIT__(vmap);
  __INIT__(string);
  __INIT__(vstring);

  (void) vstringType;

  LaDefCFun lafuns[] = {
    { "file_copy",       PTR(file_copy), 2 },
    { "file_stat",       PTR(file_stat), 1 },
    { "file_lstat",      PTR(file_lstat), 1 },
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
    { "file_is_rwx",     PTR(file_is_rwx), 1 },
    { "file_is_reg",     PTR(file_is_reg), 1 },
    { "file_is_lnk",     PTR(file_is_lnk), 1 },
    { "file_is_fifo",    PTR(file_is_fifo), 1 },
    { "file_is_sock",    PTR(file_is_sock), 1 },
    { "file_is_readable",PTR(file_is_readable), 1 },
    { "file_is_writable",PTR(file_is_writable), 1 },
    { "file_is_executable", PTR(file_is_executable), 1 },
    { "file_readlines",  PTR(file_readlines), 1 },
    { "file_writelines", PTR(file_writelines), 2 },
    { "file_type_to_string", PTR(file_type_to_string), 1 },
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
      "copy" : file_copy,
      "stat" : file_stat,
      "size" : file_size,
      "lstat" : file_lstat,
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
      "is_rwx" : file_is_rwx,
      "is_reg" : file_is_reg,
      "is_lnk" : file_is_lnk,
      "is_fifo" : file_is_fifo,
      "is_sock" : file_is_sock,
      "is_readable" : file_is_readable,
      "is_writable" : file_is_writable,
      "is_executable" : file_is_executable,
      "readlines"     : file_readlines,
      "writelines"    : file_writelines,
      "type_to_string" : file_type_to_string,
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
