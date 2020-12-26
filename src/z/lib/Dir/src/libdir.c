#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE

#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

#include <zc.h>
#include <dlist.h>
#include <libstring.h>
#include <libvstring.h>
#include <libcstring.h>
#include <libdir.h>

static  string_T StringT;
#define String   StringT.self

static  vstring_T VstringT;
#define Vstring   VstringT.self

static  cstring_T CstringT;
#define Cstring   CstringT.self

#ifndef DIR_SEP
#define DIR_SEP         '/'
#endif

static int is_directory (char *dname) {
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

static void dir_list_free (dirlist_t *dlist) {
  Vstring.release (dlist->list);
  free (dlist);
}

/* this needs a simplification and enhancement */
static dirlist_t *dir_list (char *dir, int flags) {
  if (NULL is dir) return NULL;
  ifnot (flags & DIRLIST_DONOT_CHECK_DIRECTORY)
    ifnot (is_directory (dir)) return NULL;

  DIR *dh = NULL;
  if (NULL is (dh = opendir (dir))) return NULL;
  struct dirent *dp;

  size_t len;

  dirlist_t *dlist = Alloc (sizeof (dirlist_t));
  dlist->free = dir_list_free;
  dlist->list = Vstring.new ();
  Cstring.cp (dlist->dir, PATH_MAX, dir, PATH_MAX - 1);

  while (1) {
    errno = 0;

    if (NULL is (dp = readdir (dh)))
      break;

    len = bytelen (dp->d_name);

    if (len < 3 and dp->d_name[0] is '.')
      if (len is 1 or dp->d_name[1] is '.')
        continue;

    vstring_t *vstr = Vstring.new_item ();
    vstr->data = String.new_with (dp->d_name);
/* continue logic (though not sure where to store) */
    switch (dp->d_type) {
      case DT_DIR:
        String.append_byte (vstr->data, DIR_SEP);
    }

    DListAppendCurrent (dlist->list, vstr);
  }

  closedir (dh);
  return dlist;
}

static void dir_walk_free (dirwalk_t **thisp) {
  if (NULL is thisp) return;
  dirwalk_t *this = *thisp;
  Vstring.release (this->files);
  String.release (this->dir);
  free (this);
  *thisp = NULL;
}

static int dir_walk_process_dir_def (dirwalk_t *this, char *dir, struct stat *st) {
  (void) st;
  Vstring.add.sort_and_uniq (this->files, dir);
  return 1;
}

static int dir_walk_process_file_def (dirwalk_t *this, char *file, struct stat *st) {
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
  this->object = NULL;

  return this;
}

static int __dir_walk_run__ (dirwalk_t *this, char *dir) {
  if (NOTOK is this->status) return this->status;

  int depth = 0;
  char *sp = dir;
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

static int dir_walk_run (dirwalk_t *this, char *dir) {
  if (NULL is this->files)
    this->files = Vstring.new ();

  String.replace_with (this->dir, dir);
  char *sp = dir;
  size_t len = 0;
  while (*sp) {
    len++;
    if (*sp is DIR_SEP) this->orig_depth++;
    sp++;
  }

  if (dir[len-1] is DIR_SEP)
    dir[len-1] = '\0';
  else
    this->orig_depth++;

  __dir_walk_run__ (this, dir);
  return OK;
}

public dir_T __init_dir__ (void) {
  StringT = __init_string__ ();
  VstringT = __init_vstring__ ();
  CstringT = __init_cstring__ ();

  return (dir_T) {
    .self = (dir_self) {
      .list = dir_list,
      .current = dir_current,
      .is_directory = is_directory,
      .walk = (dir_walk_self) {
        .free = dir_walk_free,
        .new = dir_walk_new,
        .run = dir_walk_run
      }
    }
  };
}
