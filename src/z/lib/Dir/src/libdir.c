/* The idea is to let the main unit to take care for the details
 * and create the environment for us.
 * So if something change, it will have to change in one place */

#define LIBRARY "Dir"

#define REQUIRE_STD_DEFAULT_SOURCE  /* for DT_* */

#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_SYS_STAT
#define REQUIRE_SYS_TYPES
#define REQUIRE_DIRENT

#define REQUIRE_LIST_MACROS
#define REQUIRE_STRING_TYPE  DECLARE
#define REQUIRE_CSTRING_TYPE DECLARE
#define REQUIRE_VSTRING_TYPE DECLARE
#define REQUIRE_PATH_TYPE    DECLARE
#define REQUIRE_ERROR_TYPE   DECLARE
#define REQUIRE_OS_TYPE      DECLARE
#define REQUIRE_DIR_TYPE     DONOT_DECLARE

#include <z/cenv.h>
#include "__dir.h"

/* TODO:
   - add and expose error
 */

static int dir_is_directory (const char *dname) { // -> const
  struct stat st;
  if (NOTOK is lstat (dname, &st)) return 0;
  return S_ISDIR (st.st_mode);
}

static int dir_lnk_is_directory (const char *dname) {
  struct stat st;
  if (NOTOK is stat (dname, &st)) return 0;
  return S_ISDIR (st.st_mode);
}

static char *dir_current (void) {
  size_t size = 64;
  char *buf = Alloc (size);
  char *dir = NULL;

  while ((dir = getcwd (buf, size)) is NULL) {
    if (errno isnot ERANGE) break;
    size += (size / 2);
    buf = Realloc (buf, size);
  }

  return dir;
}

static void dir_list_release (dirlist_t *dlist) {
  Vstring.release (dlist->list);
  free (dlist);
}

/* this needs a simplification and enhancement */
static dirlist_t *dir_list (const char *dir, int flags) {
  if (NULL is dir) return NULL;
  ifnot (flags & DIRLIST_DONOT_CHECK_DIRECTORY) {
    int isdir = 0;
    if (flags & DIRLIST_LNK_IS_DIRECTORY)
      isdir = dir_lnk_is_directory (dir);
    else
      isdir = dir_is_directory (dir);

    ifnot (isdir) return NULL;
  }

  int is_long = flags & DIRLIST_LONG_FORMAT;

  DIR *dh = NULL;
  if (NULL is (dh = opendir (dir))) return NULL;
  struct dirent *dp;

  size_t len;
  dirlist_t *dlist = Alloc (sizeof (dirlist_t));
  dlist->release = dir_list_release;
  dlist->list = Vstring.new ();
  Cstring.cp (dlist->dir, PATH_MAX, dir, PATH_MAX - 1);
  size_t dirlen = bytelen (dlist->dir);

  while (1) {
    errno = 0;

    if (NULL is (dp = readdir (dh)))
      break;

    len = bytelen (dp->d_name);

    if (len < 3 and dp->d_name[0] is '.')
      if (len is 1 or dp->d_name[1] is '.')
        continue;

    vstring_t *vstr = Vstring.add.sort_and_uniq (dlist->list, dp->d_name);

    /* continue logic (though not sure where to store) */
    switch (dp->d_type) {
      case DT_DIR:
        String.append_byte (vstr->data, '/');
    }

    if (is_long) {
      struct stat st;
      char f[dirlen + len + 2];
      Cstring.cp_fmt (f, dirlen + len + 2, "%s/%s", dlist->dir, dp->d_name);
      int retval = stat (f, &st);
      ifnot (retval) {
        char *suid = OS.get.pwname (st.st_uid);
        char *sgid = OS.get.grname (st.st_gid);
        char buf[16];
        OS.mode.stat_to_string (buf, st.st_mode);
        String.prepend_with_fmt (vstr->data, "%s %4d %s %s %d ",
            buf, st.st_nlink, suid, sgid, st.st_size);
        if (NULL isnot suid) free (suid);
        if (NULL isnot sgid) free (sgid);
      }
    }
  }

  closedir (dh);
  return dlist;
}

static void dir_walk_release (dirwalk_t **thisp) {
  if (NULL is thisp) return;
  dirwalk_t *this = *thisp;
  Vstring.release (this->files);
  String.release (this->dir);
  free (this);
  *thisp = NULL;
}

static int dir_walk_process_dir_def (dirwalk_t *this, const char *dir, struct stat *st) {
  (void) st;
  Vstring.add.sort_and_uniq (this->files, dir);
  return 1;
}

static int dir_walk_process_file_def (dirwalk_t *this, const char *file, struct stat *st) {
  (void) st;
  Vstring.add.sort_and_uniq (this->files, file);
  return 1;
}

static dirwalk_t *dir_walk_new (DirProcessDir_cb process_dir, DirProcessFile_cb process_file) {
  dirwalk_t *this = Alloc (sizeof (dirwalk_t));
  this->orig_depth = this->depth = 0;
  this->dir = String.new (PATH_MAX);
  this->files = NULL;
  this->process_dir = (NULL is process_dir ? dir_walk_process_dir_def : process_dir);
  this->process_file = (NULL is process_file ? dir_walk_process_file_def : process_file);
  this->stat_file = stat;
  this->user_data = NULL;

  return this;
}

static int __dir_walk_run__ (dirwalk_t *this, const char *dir) {
  if (NOTOK is this->status) return this->status;

  int depth = 0;
  char *sp = (char *) dir;
  while (*sp) {
    if (*sp is DIR_SEP) depth++;
    sp++;
  }

  depth -= this->orig_depth;

  struct stat st;
  ifnot (OK is (this->status = this->stat_file (dir, &st)))
    return this->status;

  ifnot (S_ISDIR (st.st_mode)) {
    this->status = this->process_file (this, dir, &st);
    return NOTOK;
  }

  if (depth >= this->depth) {
    this->status = this->process_dir (this, dir, &st);
    return OK;
  }

  DIR *dh = NULL;
  if (NULL is (dh = opendir (dir))) return OK;
  struct dirent *dp;

  string_t *new = String.new (PATH_MAX);

  while (1) {
    errno = 0;
    if (NULL is (dp = readdir (dh)))
      break;

    size_t len = bytelen (dp->d_name);

    if (len < 3 and dp->d_name[0] is '.')
      if (len is 1 or dp->d_name[1] is '.')
        continue;

    String.replace_with_fmt (new, "%s/%s", dir, dp->d_name);

    switch (dp->d_type) {
      case DT_DIR:
      case DT_UNKNOWN:
        this->status = this->process_dir (this, new->bytes, &st);
        if (1 is this->status) {
          __dir_walk_run__ (this, new->bytes);
        } else if (NOTOK is this->status)
          goto theend;
        break;

      default:
        this->status = this->process_file (this, new->bytes, &st);
        if (NOTOK is this->status)
          goto theend;
    }
  }

theend:
  closedir (dh);
  String.release (new);
  return this->status;
}

static int dir_walk_run (dirwalk_t *this, const char *dir) {
  if (NULL is this->files)
    this->files = Vstring.new ();

  String.replace_with (this->dir, dir);
  char *sp = (char *) dir;
  size_t len = 0;
  while (*sp) {
    len++;
    if (*sp is DIR_SEP) this->orig_depth++;
    sp++;
  }

  if (dir[len-1] is DIR_SEP)
    //dir[len-1] = '\0';
    String.trim_end (this->dir, DIR_SEP);
  else
    this->orig_depth++;

  __dir_walk_run__ (this, this->dir->bytes);
  return OK;
}

static int dir_make (const char *dir, mode_t mode, dir_opts opts) {
  if (NULL is dir) return NOTOK;

  if (*dir is '-' or *dir is ' ') {
    DIR_ERROR ("|%c| (%d) character is not allowed in front of a directory name\n",
        *dir, *dir);
    return NOTOK;
  }

  char *sp = (char *) dir;
  while (*sp) {
    if (' ' > *sp or *sp > 'z' or
        Cstring.byte.in_str (NOT_ALLOWED_IN_A_DIRECTORY_NAME, *sp)) {
      DIR_ERROR ("|%c| |%d| character is not allowed in a directory name\n", *sp, *sp);
      return NOTOK;
    }

    sp++;
  }

  if (*(sp - 1) is ' ') {
    DIR_ERROR ("trailing spaces are not allowed in a directory name\n");
    return NOTOK;
  }

  if (mkdir (dir, mode) isnot 0) {
    if (errno is EEXIST)
      return (dir_is_directory (dir) ? OK : NOTOK);

    DIR_ERROR ("mkdir: %s, %s\n", dir, Error.errno_string (errno));

    return NOTOK;
  }

  DIR_MSG ("created directory: %s\n", dir);

  return OK;
}

static int dir_make_parents (const char *, mode_t, dir_opts opts);
static int dir_make_parents (const char *dir, mode_t mode, dir_opts opts) {
  if (Cstring.eq (dir, "."))
    return OK;

  mode *= 2;

  char *dname = Path.dirname (dir);

  int retval = 0;

  if (Cstring.eq (dname, "/"))
    goto theend;

  if ((retval = dir_make_parents (dname, mode, opts)) isnot 0)
    goto theend;

  mode /= 2;

  mode_t m;
  if (mode > 0777) {
    mode_t mask = umask (0);
    m = 0777 & ~mask;
    umask (mask);
  } else
    m = mode;

  retval = dir_make (dir, m, opts);

theend:
  free (dname);
  return retval;
}

static int dir_rm  (const char *dir, dir_opts opts) {
  if (dir is NULL) return NOTOK;
  int retval = rmdir (dir);

  if (retval isnot OK)
    DIR_ERROR ("rmdir: %s %s\n", dir, Error.errno_string (errno));
  else
    DIR_MSG ("removed directory: %s\n", dir);

  return retval;
}

/* Here we use common techniques found in higher level languages.
 * Such style allows consentration to the logic, rather to optimize
 * the specific code. We can do, but we loose a couple of advantages
 * (like when you rereading the code after time). Also, since tiny
 * (some silly) mistakes are quite normal to happen, it is always
 * wise to reuse algorithms. This is the most precious jewel in C.
 * Since C will never change, an algorithm that does the right thing
 * will always do it forever.
 * Small price for an abstraction, and a generous gift. */

static int dir_rm_parents (const char *dir, dir_opts opts) {
  int retval = 0;

  Vstring_t *path = Path.split (dir);

  string_t *p = String.new (bytelen (dir));

  int num_items = path->num_items;

  for (idx_t i = 0; i < num_items; i++) {
    Vstring.join_allocated (path, DIR_SEP_STR, p);

    retval = dir_rm (p->bytes, opts);

    ifnot (OK is retval) break;

    Vstring.remove_at (path, -1);
  }

  String.release (p);
  Vstring.release (path);

  return retval;
}

public dir_T __init_dir__ (void) {
  __INIT__(path);
  __INIT__(os);
  __INIT__(error);
  __INIT__ (string);
  __INIT__ (cstring);
  __INIT__ (vstring);

  return (dir_T) {
    .self = (dir_self) {
      .rm = dir_rm,
      .list = dir_list,
      .make = dir_make,
      .current = dir_current,
      .rm_parents = dir_rm_parents,
      .make_parents = dir_make_parents,
      .is_directory = dir_is_directory,
      .lnk_is_directory = dir_lnk_is_directory,
      .walk = (dir_walk_self) {
        .release = dir_walk_release,
        .new = dir_walk_new,
        .run = dir_walk_run
      }
    }
  };
}
