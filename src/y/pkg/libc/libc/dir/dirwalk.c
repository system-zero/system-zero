// provides: void dirwalk_release (dirwalk_t **)
// provides: dirwalk_t *dirwalk_new (DirProcessDir_cb, DirProcessFile_cb, DirStatFile_cb)
// provides: int dirwalk_run (dirwalk_t *, const char *)
// requires: convert/format.c
// requires: sys/stat.c
// requires: stdlib/alloc.c
// requires: string/str_new.c
// requires: string/string.c
// requires: string/bytelen.c
// requires: path/realpath.c
// requires: path/dirname.c
// requires: dir/is_directory.c
// requires: dir/opendir.c
// requires: dir/dirent.h
// requires: dir/dirwalk.h

/* test {
// num-tests: 1
} */

void dirwalk_release (dirwalk_t **thisp) {
  if (NULL == thisp) return;
  dirwalk_t *this = *thisp;

  if (NULL != this->entries) {
    for (uint i = 0; i < this->num_entries; i++)
      string_release (this->entries[i]);

    Release (this->entries);

    Release (this->types);
  }

  string_release (this->dir);

  Release (this);
  *thisp = NULL;
}

static int __dirwalk_add_entry (dirwalk_t *this, const char *o, size_t len, int type) {
  if (this->num_entries == this->mem_entries) {
      this->mem_entries *= 2;
      this->entries = Realloc (this->entries, sizeof (string) * this->mem_entries);
      this->types = Realloc (this->types, sizeof (int) * this->mem_entries);
  }

  this->entries[this->num_entries] = string_new_with_len (o, len);
  this->types  [this->num_entries++] = type;

  return 1;
}

static int dirwalk_process_dir_def (dirwalk_t *this, const char *dir, size_t len, struct stat *st, int type) {
  (void) st;
  return __dirwalk_add_entry (this, dir, len, type);
}

static int dirwalk_process_file_def (dirwalk_t *this, const char *file, size_t len, struct stat *st, int type) {
  (void) st;
  return __dirwalk_add_entry (this, file, len, type);
}

static int dirwalk_on_error_def (dirwalk_t *this, const char *msg, const char *obj, int err) {
  (void) this;
  format_to_string (this->error_msg, DIRWALK_MAXLEN_ERROR_MSG, "dirwalk error: %s: %s %s\n", obj, msg, errno_string (err));
  return -1;
}

dirwalk_t *dirwalk_new (DirProcessDir_cb process_dir, DirProcessFile_cb process_file, DirStatFile_cb stat_file) {
  dirwalk_t *this = Alloc (sizeof (dirwalk_t));
  this->status = 0;
  this->orig_depth = 0;
  this->depth = DIRWALK_MAX_DEPTH;
  this->dir = string_new (MAXLEN_PATH);
  this->entries = NULL;
  this->num_entries = this->mem_entries = 0;
  this->types = NULL;

  this->process_dir = (NULL == process_dir) ? dirwalk_process_dir_def : process_dir;
  this->process_file = (NULL == process_file) ? dirwalk_process_file_def : process_file;
  this->stat_file = (NULL == stat_file) ? sys_stat : stat_file;
  this->on_error = dirwalk_on_error_def;
  this->user_data = NULL;
  return this;
}

static int __dirwalk_run__ (dirwalk_t *this, const char *dir, size_t dirlen) {
  if (-1 == this->status)
    return this->status;

  struct stat st;
  if (0 != (this->status = this->stat_file (dir, &st))) {
    if (-1 == this->on_error (this, "stat()", dir, sys_errno))
      return this->status;
  }

  if (0 == S_ISDIR (st.st_mode)) {
    this->status = -1;
    sys_errno = EINVAL;
    if (-1 == this->on_error (this, "not a directory", dir, sys_errno))
      return this->status;
 //   this->status = this->process_file (this, dir, dirlen, &st, ?type);
    return this->status;
  }

  int depth = 0;
  char *sp = (char *) dir;
  uchar prev = 0;

  while (*sp) {
    if (*sp == DIR_SEP && prev != DIR_SEP)
      depth++;
    prev = *sp;
    sp++;
  }

  depth -= (this->orig_depth - 1);

  if (depth >= this->depth)
    return 0;

  DIR *dh = NULL;
  if (NULL == (dh = sys_opendir (dir))) {
    if (-1 == this->on_error (this, "opendir()", dir, sys_errno))
      return -1;
  }

  struct dirent *dp;

  string *new = string_new (MAXLEN_PATH);

  while (1) {
    sys_errno = 0;
    if (NULL == (dp = sys_readdir (dh))) {
      if (sys_errno) {
        if (-1 == this->on_error (this, "readdir()", dir, sys_errno))
          this->status = -1;
      }

      goto theend;
    }

    size_t namelen = bytelen (dp->d_name);

    if (namelen <= 2 &&
        dp->d_name[0] == '.' && (dp->d_name[1] == '\0' || dp->d_name[1] == '.'))
      continue;

    if (dirlen > 1 || *dir != DIR_SEP) {
      string_replace_with_fmt (new, "%s%c%s", dir, DIR_SEP, dp->d_name);
    } else {
      this->orig_depth--;
      string_replace_with_fmt (new, "%c%s", DIR_SEP, dp->d_name);
    }

    if (-1 == (this->status = this->stat_file (new->bytes, &st))) {
      if (-1 == this->on_error (this, "stat()", new->bytes, sys_errno))
        goto theend;
      continue;
    }

    switch (dp->d_type)  {
      case DT_DIR:
      case DT_UNKNOWN:
        this->status = this->process_dir (this, new->bytes, new->num_bytes, &st, dp->d_type);
        if (1 == this->status) {
          if (-1 == __dirwalk_run__ (this, new->bytes, new->num_bytes))
            goto theend;
        } else if (-1 == this->status)
          goto theend;
        break;

      default:
        this->status = this->process_file (this, new->bytes, new->num_bytes, &st, dp->d_type);
        if (-1 == this->status)
          goto theend;
    }
  }

theend:
  sys_closedir (dh);
  string_release (new);
  return this->status;
}

/* testing multiply blocks in a unit, as dirwalk was the first unit
   that 've tried the functionality
 */

/* test {
#define REQUIRE_MAKE_DIR
#define REQUIRE_OPEN
#define REQUIRE_RMDIR
#define REQUIRE_UNLINK
#define REQUIRE_FORMAT
#define REQUIRE_DIRWALK
#define REQUIRE_PATH_DIRNAME
#define REQUIRE_ATOI

#include <libc.h>
} */

int dirwalk_run (dirwalk_t *this, const char *dirp) {
  if (NULL == dirp) {
    this->on_error (this, "directory is a NULL pointer", "", EINVAL);
    return -1;
  }

  char *dir = NULL;

  char p[MAXLEN_PATH + 1];
  dir = path_real (dirp, p);
  if (NULL == dir) {
    this->on_error (this, "path_real()", dirp, sys_errno);
    return -1;
  }

  if (0 == is_directory (dir)) {
    this->on_error (this, dir, ": not a directory", EINVAL);
    return -1;
  }

  size_t len = bytelen (dir);

  if (0 == len) return 0;

  string_replace_with_len (this->dir, dir, len);

  if (NULL == this->entries) {
    this->mem_entries = 8;
    this->entries = Alloc (sizeof (string) * this->mem_entries);
    this->types = Alloc (sizeof (int) * this->mem_entries);
  }

  char *sp = dir;

  while (*sp) {
    if (*sp == DIR_SEP) this->orig_depth++;
    sp++;
  }

  if (len > 1 && dir[len-1] == DIR_SEP)

    string_trim_end (this->dir, DIR_SEP);
  else
    this->orig_depth++;

  __dirwalk_run__ (this, this->dir->bytes, this->dir->num_bytes);
  return 0;
}

/* test {

static int test_init (void) {
  if (-1 == make_dir_parents ("/tmp/dirwalk_test/a/b/c", 0700)) return -1;
  if (-1 == sys_open3 ("/tmp/dirwalk_test/a/b/c/1", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)) return -1;
  if (-1 == sys_open3 ("/tmp/dirwalk_test/a/b/c/2", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)) return -1;
  if (-1 == sys_open3 ("/tmp/dirwalk_test/a/b/1", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)) return -1;
  if (-1 == sys_open3 ("/tmp/dirwalk_test/a/b/2", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)) return -1;
  if (-1 == sys_open3 ("/tmp/dirwalk_test/a/1", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)) return -1;
  if (-1 == sys_open3 ("/tmp/dirwalk_test/a/2", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)) return -1;

  if (-1 == make_dir_parents ("/tmp/dirwalk_test/d/e/f", 0700)) return -1;
  if (-1 == sys_open3 ("/tmp/dirwalk_test/d/e/f/1", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)) return -1;
  if (-1 == sys_open3 ("/tmp/dirwalk_test/d/e/f/2", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)) return -1;
  if (-1 == sys_open3 ("/tmp/dirwalk_test/d/e/1", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)) return -1;
  if (-1 == sys_open3 ("/tmp/dirwalk_test/d/e/2", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)) return -1;
  if (-1 == sys_open3 ("/tmp/dirwalk_test/d/1", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)) return -1;
  if (-1 == sys_open3 ("/tmp/dirwalk_test/d/2", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)) return -1;

  if (-1 == make_dir_parents ("/tmp/dirwalk_test/d/g/h", 0700)) return -1;
  if (-1 == make_dir_parents ("/tmp/dirwalk_test/d/j/k", 0700)) return -1;
  if (-1 == make_dir_parents ("/tmp/dirwalk_test/d/f/h", 0700)) return -1;
  return 0;
}

static int file_cb (dirwalk_t *this, const char *file, size_t len, struct stat *st, int type) {
  (void) this; (void) len; (void) st; (void) type;
  return sys_unlink (file);
}

static int first_test (int total) {
  int retval = -1;
  tostdout ("[%d] testing dirwalk() %s - ", total, __func__);

  dirwalk_t *dw = dirwalk_new (NULL, file_cb, NULL);

  if (-1 == test_init ()) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  dirwalk_run (dw, "/tmp/dirwalk_test");

  for (int i = dw->num_entries - 1; i >= 0; i--)
    sys_rmdir (dw->entries[i]->bytes);

  tostdout ("\e[32m[OK]\e[m\n");
  retval = 0;

theend:
  dirwalk_release (&dw);
  return retval;
}

int main (int argc, char **argv) {
  if (1 == argc) return 1;

  int total = str_to_int (argv[1]);
  int num_tests = 0;
  int failed = 0;

  num_tests++;  total++;
  if (first_test (total) == -1) failed++;

  return failed;
}
} */
