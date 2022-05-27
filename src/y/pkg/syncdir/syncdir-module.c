#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_SYS_STAT

#define REQUIRE_STD_MODULE
#define REQUIRE_CSTRING_TYPE  DECLARE
#define REQUIRE_STRING_TYPE   DECLARE
#define REQUIRE_PATH_TYPE     DECLARE
#define REQUIRE_FILE_TYPE     DECLARE
#define REQUIRE_DIR_TYPE      DECLARE
#define REQUIRE_ERROR_TYPE    DECLARE

#include <z/cenv.h>

MODULE(syncdir)

struct syncdir_t {
  const char *src;
  const char *dest;
  string **exclude_dirs;
  int exclude_dirs_len;
  size_t src_len;
  size_t dest_len;
  int src_has_dir_sep;
  int dest_has_dir_sep;
  int err;
  int verbose;
};

static int process_dir (dirwalk_t *dw, const char *dir, struct stat *st) {
  struct syncdir_t *syncdir = (struct syncdir_t *) dw->user_data;
  const char *bn = dir + syncdir->src_len + (syncdir->src_has_dir_sep is 0);

  if (syncdir->exclude_dirs isnot NULL) {
    for (int i = 0; i < syncdir->exclude_dirs_len; i++) {
      if (Cstring.eq (bn, syncdir->exclude_dirs[i]->bytes)) return 0;
    }
  }

  size_t len = syncdir->dest_len + (syncdir->dest_has_dir_sep is 0) + bytelen (bn);

  char dest[len + 1];
  Cstring.cp_fmt (dest, len + 1, "%s%s%s", syncdir->dest,
    (syncdir->src_has_dir_sep ? "" : DIR_SEP_STR), bn);

  struct stat dest_st;
  if (access (dest, F_OK) and -1 is lstat (dest, &dest_st)) {
    if (NOTOK is Dir.make (dest, st->st_mode, DirOpts(.msg = syncdir->verbose))) {
      syncdir->err = errno;
      return -1;
    }
  }

  return 1;
}

static int process_file (dirwalk_t *dw, const char *file, struct stat *st) {
  (void) st;
  struct syncdir_t *syncdir = (struct syncdir_t *) dw->user_data;
  const char *bn = file + syncdir->src_len + (syncdir->src_has_dir_sep is 0);
  size_t len = syncdir->dest_len + (syncdir->dest_has_dir_sep is 0) + bytelen (bn);
  char dest[len + 1];
  Cstring.cp_fmt (dest, len + 1, "%s%s%s", syncdir->dest,
    (syncdir->dest_has_dir_sep ? "" : DIR_SEP_STR), bn);

  if (NOTOK is File.copy (file, dest, FileCopyOpts (
      .update = 1,
      .verbose = syncdir->verbose + 1,
      .preserve = 1,
      .force = 1))) {
    syncdir->err = errno;
    return -1;
  }

  return 1;
}

static int process_dir_remove (dirwalk_t *dw, const char *dir, struct stat *st) {
  (void) st;
  struct syncdir_t *syncdir = (struct syncdir_t *) dw->user_data;
  const char *bn = dir + syncdir->dest_len + (syncdir->dest_has_dir_sep is 0);

  if (syncdir->exclude_dirs isnot NULL)
    for (int i = 0; i < syncdir->exclude_dirs_len; i++)
      if (Cstring.eq (bn, syncdir->exclude_dirs[i]->bytes)) return 0;

  size_t len = syncdir->src_len + (syncdir->src_has_dir_sep is 0) + bytelen (bn);
  char src[len + 1];

  Cstring.cp_fmt (src, len + 1, "%s%s%s", syncdir->src,
    (syncdir->src_has_dir_sep ? "" : DIR_SEP_STR), bn);

  struct stat src_st;
  if (access (src, F_OK) and -1 is lstat (src, &src_st)) {
    if (NOTOK is File.remove (dir, FileRemoveOpts(
      .verbose = syncdir->verbose + 1,
      .force = 1,
      .recursive = 1))) {
      syncdir->err = errno;
      return -1;
    }

    return 0;
  }

  return 1;
}

static int process_file_remove (dirwalk_t *dw, const char *file, struct stat *st) {
  (void) st;
  struct syncdir_t *syncdir = (struct syncdir_t *) dw->user_data;
  const char *bn = file + syncdir->dest_len + (syncdir->dest_has_dir_sep is 0);
  size_t len = syncdir->src_len + (syncdir->src_has_dir_sep is 0) + bytelen (bn);
  char src[len + 1];
  Cstring.cp_fmt (src, len + 1, "%s%s%s", syncdir->src,
    (syncdir->src_has_dir_sep ? "" : DIR_SEP_STR), bn);

  struct stat src_st;
  if (access (src, F_OK) and -1 is lstat (src, &src_st)) {
    if (NOTOK is File.remove (file, FileRemoveOpts (
        .force = 1,
        .verbose = syncdir->verbose + 1,
        .force = 1))) {
      syncdir->err = errno;
      return -1;
    }
    return 0;
  }

  return 1;
}

static VALUE sync_dir (la_t *this, VALUE v_src_dir, VALUE v_dest_dir) {
  ifnot (IS_STRING(v_src_dir)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_STRING(v_dest_dir)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");

  char src[PATH_MAX];
  char dest[PATH_MAX];

  char *src_dir = Path.real (AS_STRING_BYTES(v_src_dir), src);
  char *dest_dir =Path.real (AS_STRING_BYTES(v_dest_dir), dest);

  int verbose = GET_OPT_VERBOSE();
  int len = 0;
  string **exclude_dirs = GET_OPT_EXCLUDE_DIRS(&len);

  ifnot (Dir.is_directory (src_dir)) {
    fprintf (stderr, "%s, source is not a directory\n", src_dir);
    La.set.Errno (this, ENOTDIR);
    return NOTOK_VALUE;
  }

  struct stat src_st;
  struct stat dest_st;

  if (access (src_dir, R_OK)) {
    fprintf (stderr, "%s, source directory is not readable\n", src_dir);
    La.set.Errno (this, errno);
    return NOTOK_VALUE;
  }

  if (NOTOK is stat (src_dir, &src_st)) {
    fprintf (stderr, "%s, stat failed for source directory\n", src_dir); // redundant
    La.set.Errno (this, errno);
    return NOTOK_VALUE;
  }

  if (access (dest, F_OK) and -1 is lstat (dest, &dest_st)) {
    int retval = File.copy (src_dir, dest_dir, FileCopyOpts (
      .verbose = verbose + 1,
      .preserve = 1,
      .recursive = 1));

    if (retval is NOTOK)
      La.set.Errno (this, errno);

    return INT(retval);
  }

  if (access (dest_dir, R_OK|W_OK)) {
    fprintf (stderr, "%s, destination directory is not readable or writable\n", dest_dir);
    La.set.Errno (this, errno);
    return NOTOK_VALUE;
  }

  ifnot (Dir.is_directory (dest_dir)) {
    fprintf (stderr, "%s, destination is not a directory\n", dest_dir);
    La.set.Errno (this, ENOTDIR);
    return NOTOK_VALUE;
  }

  if (NOTOK is stat (dest_dir, &dest_st)) {
    fprintf (stderr, "%s, stat failed for destination directory\n", dest_dir);
    La.set.Errno (this, errno);
    return NOTOK_VALUE;
  }

  if (src_st.st_ino is dest_st.st_ino and
      src_st.st_dev is dest_st.st_dev) {
    fprintf (stderr, "%s and %s are same\n", src_dir, dest_dir);
    La.set.Errno (this, EUNKNOWN);
    return NOTOK_VALUE;
  }

  dirwalk_t *dw = NULL;

  size_t src_len = bytelen (src_dir);
  size_t dest_len = bytelen (dest_dir);

  struct syncdir_t data = {
    .src = src_dir,
    .dest = dest_dir,
    .exclude_dirs = exclude_dirs,
    .exclude_dirs_len = len,
    .src_len = src_len,
    .dest_len = dest_len,
    .src_has_dir_sep = src_dir[src_len - 1] is DIR_SEP,
    .dest_has_dir_sep = dest_dir[dest_len - 1] is DIR_SEP,
    .err = 0,
    .verbose = verbose
   };

  dw = Dir.walk.new (process_dir, process_file);
  dw->depth = DIRWALK_MAX_DEPTH;
  dw->user_data = &data;
  int retval = Dir.walk.run (dw, src_dir);
  Dir.walk.release (&dw);

  if (data.err isnot 0) {
    La.set.Errno (this, data.err);
    return NOTOK_VALUE;
  }

  dw = Dir.walk.new (process_dir_remove, process_file_remove);
  dw->user_data = &data;
  dw->depth = DIRWALK_MAX_DEPTH;
  retval = Dir.walk.run (dw, dest_dir);
  Dir.walk.release (&dw);

  return INT(retval);
}

#define EvalString(...) #__VA_ARGS__

public int __init_syncdir_module__ (la_t *this) {
  __INIT_MODULE__(this);
  __INIT__(dir);
  __INIT__(file);
  __INIT__(path);
  __INIT__(error);
  __INIT__(string);
  __INIT__(cstring);

  LaDefCFun lafuns[] = {
    { "syncdir", PTR(sync_dir), 2 },
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Sync = {
       "dir"    : syncdir,
     }
   );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_syncdir_module__ (la_t *this) {
  (void) this;
}
