// as: file_remove
// provides: int file_remove (const char *file, file_remove_opts)
// requires: string/bytelen.c
// requires: dir/dirwalk.c
// requires: dir/rmdir.c
// requires: std/unlink.c
// requires: term/term.c
// requires: term/term_getkey.c
// requires: convert/format.c
// requires: file/remove.h

#define QUIT DIRWALK_STOP

#define toErrFd(fmt, ...)  format_to_fd (opts.err_fd, fmt, ##__VA_ARGS__)
#define toOutFd(fmt, ...)  format_to_fd (opts.out_fd, fmt, ##__VA_ARGS__)
#define toFd(fd, fmt, ...) format_to_fd (fd, fmt, ##__VA_ARGS__)

typedef struct {
  file_remove_opts *opts;
  bool outToOutFd;
  bool outToErrFd;
  uchar decision;
} file_remove_private;

static int file_remove_on_interactive (const char *file, const char *msg, void *user_data) {
  file_remove_private *self = user_data;

  if (self->decision == 'a')
    return 1;

  term_t *term = term_new ();
  term_raw_mode (term);

  tostdout ("%s `%s'? y[es]/n[o]/q[uit]/a[ll]", msg, file);

  int retval = 0;

  while (1) {
    int c = term_getkey (0);
    if (c == 'y') { retval =  1;    self->decision = 'y'; break; }
    if (c == 'a') { retval =  1;    self->decision = 'a'; break; }
    if (c == 'n') { retval =  0;    self->decision = 'n'; break; }
    if (c == 'q') { retval =  QUIT; self->decision = 'q'; break; }
  }

  term_orig_mode (term);
  term_release (&term);
  tostdout ("\n");
  return retval;
}

static int file_remove_interactive (const char *obj, int type, file_remove_private *self) {
  char msg[32];
  switch (type) {
    case DT_DIR:  str_copy (msg, 32, "remove directory", 16); break;
    case DT_REG:  str_copy (msg, 32, "remove regular file", 19); break;
    case DT_LNK:  str_copy (msg, 32, "remove link", 16); break;
    case DT_CHR:  str_copy (msg, 32, "remove character device", 23); break;
    case DT_BLK:  str_copy (msg, 32, "remove block", 13); break;
    case DT_SOCK: str_copy (msg, 32, "remove socket", 13); break;
    case DT_FIFO: str_copy (msg, 32, "remove fifo", 12); break;
  }

  return self->opts->on_interactive ((char *) obj, msg, self);
}

static int file_remove_cb (dirwalk_t *this, const char *file, size_t len, int type) {
  (void) len; (void) type;
  file_remove_private *self = this->user_data;
  file_remove_opts opts = *self->opts;
  int what = 1;

  if (0 == opts.force)
    if (opts.interactive)
      what = file_remove_interactive (file, type, self);

  if (what ==DIRWALK_STOP || what == 0)
    return what;

  int retval = sys_unlink (file);

  if (-1 == retval) {
    if (self->outToErrFd)
      toErrFd ("failed to remove '%s': %s\n", file, errno_string (sys_errno));
    return -1;
  }

  if (self->outToOutFd)
    toOutFd ("removed '%s'\n", file);

  return DIRWALK_PROCESS;
}

int file_remove (const char *file, file_remove_opts opts) {
  if (opts.interactive) opts.force = 0;

  bool outToErrFd = (opts.verbose_on_error == VERBOSE_ON && -1 != opts.err_fd);
  bool outToOutFd = (opts.verbose == VERBOSE_ON && -1 != opts.out_fd);

  if (NULL == file) {
    if (outToErrFd)
      toErrFd ("failed to remove: file argument is a NULL pointer\n");
    return -1;
  }

  size_t file_len = bytelen (file);

  if (0 == file_len) {
    if (outToErrFd)
      toErrFd ("failed to remove: file argument is an empty string\n");
    return -1;
  }

  dirwalk_t *dw = NULL;

  opts.on_interactive = file_remove_on_interactive;

  file_remove_private self = (file_remove_private) {
    .opts = &opts,
    .decision = 0,
    .outToOutFd = outToOutFd,
    .outToErrFd = outToErrFd
  };

  int retval = -1;

  struct stat file_st;

  if ((retval = sys_lstat (file, &file_st)) == -1) {
    if (outToErrFd)
      toErrFd ("%s: %s\n", file, errno_string (sys_errno));

    goto theend;
  }

  int is_dir = S_ISDIR(file_st.st_mode);

  retval = -1;

  if (is_dir) {
    if (0 == opts.recursive) {
      if (outToErrFd)
        toErrFd ("%s: is a directory\n", file);

      goto theend;
    }

    if (opts.curdepth == opts.maxdepth) {
      if (outToErrFd)
        toErrFd ("'%d' depth exceeded '%d' maxdepth\n", opts.curdepth, opts.maxdepth);

      goto theend;
    }

    dw = dirwalk_new (NULL, file_remove_cb, NULL);

    dw->user_data = &self;

    dirwalk_add_entry (dw, file, file_len, DT_DIR);

    retval = dirwalk_run (dw, file);

    if (DIRWALK_ERROR == retval)
      goto theend;

    if (DIRWALK_STOP == retval) {
      retval = 0;
      goto theend;
    }

    char *dir = NULL;

    for (int i = (int) dw->num_entries - 1; i >= 0; i--) {
      dir = dw->entries[i]->bytes;

      if (0 == opts.force) {
        if (opts.interactive) {
          int what = file_remove_interactive (dir, DT_DIR, &self);

          switch (what) {
            case DIRWALK_STOP:
              retval = 0;
              goto theend;

            case 0:
              continue;
          }
        }
      }

      if ((retval = sys_rmdir (dir)) == -1) {
        if (outToErrFd)
          toErrFd ("failed to remove '%s' directory: %s\n",
              dir, errno_string (sys_errno));

        goto theend;
      }

      if (outToOutFd)
        toOutFd("removed '%s'\n", dir);
    }

    goto theend;
  }

  retval = 0;

  int what = 1;

  if (0 == opts.force) {
    if (opts.interactive) {
      int type = DT_REG;

      if (S_ISLNK(file_st.st_mode))
        type = DT_LNK;
      else if (S_ISSOCK(file_st.st_mode))
        type = DT_SOCK;
      else if (S_ISCHR(file_st.st_mode))
        type = DT_CHR;
      else if (S_ISBLK(file_st.st_mode))
        type = DT_BLK;
      else if (S_ISFIFO(file_st.st_mode))
        type = DT_FIFO;

      what = file_remove_interactive (file, type, &self);
    }
  }

  if (what == 0 || what == QUIT)
    goto theend;

  retval = sys_unlink (file);

  if (-1 == retval) {
    if (outToErrFd)
      toErrFd ("failed to remove '%s': %s\n", file, errno_string (sys_errno));

    goto theend;
  }

  if (outToOutFd)
    toOutFd("removed '%s'\n", file);

theend:
  if (NULL != dw)
    dirwalk_release (&dw);

  return retval;
}

/* test {
// num-tests: 1

#define REQUIRE_MAKE_DIR
#define REQUIRE_OPEN
#define REQUIRE_RMDIR
#define REQUIRE_FILE_REMOVE
#define REQUIRE_TERM_GETKEY
#define REQUIRE_FORMAT
#define REQUIRE_DIRWALK
#define REQUIRE_PATH_DIRNAME
#define REQUIRE_ATOI
#define REQUIRE_IS_DIRECTORY

#include <libc.h>

#define MYTESTDIR "/tmp/file_remove_test"

static int test_init (void) {
  if (-1 == make_dir_parents (MYTESTDIR "/a/b/c", 0700)) return -1;
  if (-1 == sys_open3 (MYTESTDIR "/a/b/c/1", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)) return -1;
  if (-1 == sys_open3 (MYTESTDIR "/a/b/c/2", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)) return -1;
  if (-1 == sys_open3 (MYTESTDIR "/a/b/1", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)) return -1;
  if (-1 == sys_open3 (MYTESTDIR "/a/b/2", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)) return -1;
  if (-1 == sys_open3 (MYTESTDIR "/a/1", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)) return -1;
  if (-1 == sys_open3 (MYTESTDIR "/a/2", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)) return -1;

  if (-1 == make_dir_parents (MYTESTDIR "/d/e/f", 0700)) return -1;
  if (-1 == sys_open3 (MYTESTDIR "/d/e/f/1", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)) return -1;
  if (-1 == sys_open3 (MYTESTDIR "/d/e/f/2", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)) return -1;
  if (-1 == sys_open3 (MYTESTDIR "/d/e/1", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)) return -1;
  if (-1 == sys_open3 (MYTESTDIR "/d/e/2", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)) return -1;
  if (-1 == sys_open3 (MYTESTDIR "/d/1", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)) return -1;
  if (-1 == sys_open3 (MYTESTDIR "/d/2", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)) return -1;

  if (-1 == make_dir_parents (MYTESTDIR "/d/g/h", 0700)) return -1;
  if (-1 == make_dir_parents (MYTESTDIR "/d/j/k", 0700)) return -1;
  if (-1 == make_dir_parents (MYTESTDIR "/d/f/h", 0700)) return -1;
  return 0;
}

static int first_test (int total) {
  int retval = -1;
  tostdout ("[%d] testing file_remove() %s - ", total, __func__);

  if (-1 == test_init ()) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  retval = file_remove (MYTESTDIR, FileRemoveOpts(.verbose_on_error = VERBOSE_OFF, .recursive = 0));

  if (retval != -1) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  retval = file_remove (MYTESTDIR, FileRemoveOpts(.recursive = 1));

  if (is_directory (MYTESTDIR)) {
    retval = -1;
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  tostdout ("\e[32m[OK]\e[m\n");
  retval = 0;

theend:
  return retval;
}

int main (int argc, char **argv) {
  if (1 == argc) return 1;

  int total = str_to_int (argv[1]);
  int failed = 0;

  total++;
  if (first_test (total) == -1) failed++;

  return failed;
}

} */
