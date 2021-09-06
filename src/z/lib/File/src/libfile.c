#define LIBRARY "File"

#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_SYS_STAT
#define REQUIRE_SYS_TYPES
#define REQUIRE_TERMIOS
#define REQUIRE_FCNTL
#define REQUIRE_TIME

#define REQUIRE_STRING_TYPE  DECLARE
#define REQUIRE_CSTRING_TYPE DECLARE
#define REQUIRE_VSTRING_TYPE DECLARE
#define REQUIRE_PATH_TYPE    DECLARE
#define REQUIRE_DIR_TYPE     DECLARE
#define REQUIRE_OS_TYPE      DECLARE
#define REQUIRE_IO_TYPE      DECLARE
#define REQUIRE_TERM_TYPE    DECLARE
#define REQUIRE_ERROR_TYPE   DECLARE
#define REQUIRE_FILE_TYPE    DONOT_DECLARE

#include <z/cenv.h>

/* TODO:
  - FileState file_touch (char *file, FileState ());
    FileState:
      (.retval, .state, .error, .perm, .mode, .struct timespec times[2],
       .self)
  - FileState file_chmod (char *file, FileState());
 */

/* Public TODO:
   StateRetval = fun (args, SomeState()).retval
 */

static int file_is_lnk (const char *fname) {
  struct stat st;
  if (NOTOK is lstat (fname, &st)) return 0;
  return S_ISLNK(st.st_mode);
}

static int file_is_reg (const char *fname) {
  struct stat st;
  if (NOTOK is stat (fname, &st)) return 0;
  return S_ISREG (st.st_mode);
}

static int file_is_sock (const char *fname) {
  struct stat st;
  if (NOTOK is stat (fname, &st)) return 0;
  return S_ISSOCK (st.st_mode);
}

static int file_is_fifo (const char *fname) {
  struct stat st;
  if (NOTOK is stat (fname, &st)) return 0;

  return S_ISFIFO (st.st_mode);
}

static int file_is_executable (const char *fname) {
  return (0 is access (fname, F_OK|X_OK));
}

static int file_is_readable (const char *fname) {
  return (0 is access (fname, R_OK));
}

static int file_is_writable (const char *fname) {
  return (0 is access (fname, R_OK));
}

static int file_is_rwx (const char *fname) {
  return (0 is access (fname, R_OK|W_OK|X_OK));
}

static int file_is_elf (const char *file) {
  int fd = open (file, O_RDONLY);
  if (-1 is fd) return 0;
  int retval = 0;
  char buf[8];
  ssize_t bts = read (fd, buf, 7);
  buf[bts] = '\0';
  retval = buf[0] is 0x7f and Cstring.eq_n (buf + 1, "ELF", 3);
  ifnot (retval) // static .a files magic number !<arch> (for archive)
    retval = Cstring.eq_n (buf, "!<arch>", 7);

  close (fd);
  return retval;
}

static int file_exists (const char *fname) {
  return (0 is access (fname, F_OK));
}

static size_t file_size (const char *fname) {
  struct stat st;
  if (NOTOK is stat (fname, &st)) return 0;
  return st.st_size;
}

static mode_t file_mode_from_octal_string (char *oct_str) {
  if (bytelen (oct_str) isnot 4) return 0;

  mode_t mode = 0;
  char c;
  while ((c = *oct_str++)) {
    if ('0' > c or c > '7')
      return 0;

    mode = mode * 8 + c - '0';
  }

  return mode;
}

/* from slang sources slsh/slsh.c
 * Copyright (C) 2005-2017,2018 John E. Davis
 * 
 * This file is part of the S-Lang Library.
 * 
 * The S-Lang Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 */

#if 0
//moved to libos
static char *file_mode_stat_to_string (char *mode_string, mode_t mode) {
  /* assumed at least 11 bytes */

  if      (S_ISREG(mode))  mode_string[0] = REG_CHAR;
  else if (S_ISDIR(mode))  mode_string[0] = DIR_CHAR;
  else if (S_ISLNK(mode))  mode_string[0] = LNK_CHAR;
  else if (S_ISCHR(mode))  mode_string[0] = CHR_CHAR;
  else if (S_ISBLK(mode))  mode_string[0] = BLK_CHAR;
  else if (S_ISFIFO(mode)) mode_string[0] = FIFO_CHAR;
  else if (S_ISSOCK(mode)) mode_string[0] = SOCK_CHAR;

  if (mode & S_IRUSR) mode_string[1] = 'r'; else mode_string[1] = '-';
  if (mode & S_IWUSR) mode_string[2] = 'w'; else mode_string[2] = '-';
  if (mode & S_IXUSR) mode_string[3] = 'x'; else mode_string[3] = '-';
  if (mode & S_ISUID) mode_string[3] = SUID_CHAR;

  if (mode & S_IRGRP) mode_string[4] = 'r'; else mode_string[4] = '-';
  if (mode & S_IWGRP) mode_string[5] = 'w'; else mode_string[5] = '-';
  if (mode & S_IXGRP) mode_string[6] = 'x'; else mode_string[6] = '-';
  if (mode & S_ISGID) mode_string[6] = SGID_CHAR;

  if (mode & S_IROTH) mode_string[7] = 'r'; else mode_string[7] = '-';
  if (mode & S_IWOTH) mode_string[8] = 'w'; else mode_string[8] = '-';
  if (mode & S_IXOTH) mode_string[9] = 'x'; else mode_string[9] = '-';
  if (mode & S_ISVTX) mode_string[9] = SVTX_CHAR;

  mode_string[10] = '\0';
  return mode_string;
}
#endif

static string_t *file_readlink (const char *file) {
  size_t size = MAXLEN_PATH;

  again: {}
  char buf[size];
  ssize_t retval = readlink (file, buf, size);
  if (-1 is retval)
    return NULL;

  if ((size_t) retval is size) {
    size *=  2;
    goto again;
  }

  string_t *lnk = String.new_with_len (buf, (size_t) retval);
  return lnk;
}

static Vstring_t *file_readlines_from_fp (FILE *fp, Vstring_t *lines,
                                FileReadLines_cb cb, void *user_data) {
  char *buf = NULL;
  size_t len;
  ssize_t nread;

  if (cb isnot NULL) {
    int num = 0;
    while (-1 isnot (nread = getline (&buf, &len, fp))) {
      cb (lines, buf, nread, ++num, user_data);
    }
  } else {  /* by default an array of lines */
    while (-1 isnot (nread = getline (&buf, &len, fp))) {
      buf[nread - 1] = '\0';
      Vstring.current.append_with (lines, buf);
    }
  }

  ifnot (buf is NULL) free (buf);

  return lines;
}

static Vstring_t *file_readlines (char *file, Vstring_t *lines,
                          FileReadLines_cb cb, void *user_data) {
  Vstring_t *llines = lines;
  if (NULL is llines) llines = Vstring.new ();
  if (-1 is access (file, F_OK|R_OK)) goto theend;

  FILE *fp = fopen (file, "r");
  if (fp is NULL)
    return NULL;

  llines = file_readlines_from_fp (fp, llines, cb, user_data);

  fclose (fp);
theend:
  return llines;
}

static ssize_t __file_write__ (char *fname, char *bytes, ssize_t size, char *mode) {
  if (size < 0) size = bytelen (bytes);
  if (size <= 0) return NOTOK;

  FILE *fp = fopen (fname, mode);
  if (NULL is fp) return NOTOK;
  size = fwrite (bytes, 1, size, fp);
  fclose (fp);
  return size;
}

static ssize_t file_write (char *fname, char *bytes, ssize_t size) {
  return __file_write__ (fname, bytes, size, "w");
}

static ssize_t file_append (char *fname, char *bytes, ssize_t size) {
  return __file_write__ (fname, bytes, size, "a+");
}

static void file_tmpfname_release (tmpfname_t *this, int flags) {
  ifnot (this) return;
  ifnot (NULL is this->fname) {
    if (flags & FILE_TMPFNAME_UNLINK_FILE)
      unlink (this->fname->bytes);
    String.release (this->fname);
    this->fname = NULL;
  }

  if (-1 isnot this->fd)
    if (flags & FILE_TMPFNAME_CLOSE_FD)
      close (this->fd);

  free (this);
}

static tmpfname_t *file_tmpfname_new (char *dname, char *prefix) {
  static unsigned int seed = 12252;
  if (NULL is dname) return NULL;
  ifnot (Dir.is_directory (dname)) return NULL;

  tmpfname_t *this = NULL;

  char bpid[6];
  pid_t pid = getpid ();
  Cstring.itoa ((int) pid, bpid, 10);

  int len = bytelen (dname) + bytelen (bpid) + bytelen (prefix) + 10;

  char name[len];
  snprintf (name, len, "%s/%s-%s.xxxxxx", dname, prefix, bpid);

  srand ((unsigned int) time (NULL) + (unsigned int) pid + seed++);

  dirlist_t *dlist = Dir.list (dname, 0);
  if (NULL is dlist) return NULL;

  int
    found = 0,
    loops = 0,
    max_loops = 1024,
    inner_loops = 0,
    max_inner_loops = 1024;
  char c;

  while (1) {
again:
    found = 0;
    if (++loops is max_loops) goto theend;

    for (int i = 0; i < 6; i++) {
      inner_loops = 0;
      while (1) {
        if (++inner_loops is max_inner_loops) goto theend;

        c = (char) (rand () % 123);
        if ((c <= 'z' and c >= 'a') or (c >= '0' and c <= '9') or
            (c >= 'A' and c <= 'Z') or c is '_') {
          name[len - i - 2] = c;
          break;
        }
      }
    }

    vstring_t *it = dlist->list->head;
    while (it) {
      if (Cstring.eq (name, it->data->bytes)) goto again;
      it = it->next;
    }

    found = 1;
    break;
  }

  ifnot (found) goto theend;

  this = Alloc (sizeof (tmpfname_t));
  this->fd = open (name, O_RDWR|O_CREAT|O_EXCL, S_IRUSR|S_IWUSR);

  if (-1 is this->fd) goto theerror;
  if (-1 is fchmod (this->fd, 0600)) {
    close (this->fd);
    goto theerror;
  }

  this->fname = String.new_with (name);
  goto theend;

theerror:
  ifnot (NULL is this) {
    file_tmpfname_release (this, FILE_TMPFNAME_UNLINK_FILE| FILE_TMPFNAME_CLOSE_FD);
    this = NULL;
  }

theend:
  dlist->release (dlist);
  return this;
}

static int file_copy_on_interactive (char *file) {
  term_t *term = Term.new ();
  Term.raw_mode (term);

  fprintf (stdout, "copy: overwrite `%s'? y[es]/n[o]/q[uit]", file);
  fflush (stdout);

  int retval = 0;

  while (1) {
    int c = IO.input.getkey (STDIN_FILENO);
    if (c is 'y') { retval =  1; break; }
    if (c is 'n') { retval =  0; break; }
    if (c is 'q') { retval = -1; break; }
  }

  Term.orig_mode (term);
  Term.release (&term);
  fprintf (stdout, "\n");
  fflush (stdout);
  return retval;
}

static int file_copy (const char *, const char *, file_copy_opts);
static int file_copy (const char *src, const char *o_dest, file_copy_opts opts) {
  int retval = NOTOK;

  char *dest = (char *) o_dest;

  if (opts.all) {
    opts.recursive = 1;
    opts.preserve = 1;
    opts.dereference = 0;
  }

  if (opts.interactive) opts.force = 0;

  int outToErrStream = (opts.verbose > OPT_NO_VERBOSE and NULL isnot opts.err_stream);

  if (NULL is src or NULL is dest) {
    if (outToErrStream)
      fprintf (opts.err_stream, "failed to copy: either src or dest are NULL pointers\n");
    return NOTOK;
  }

  idx_t len = 4096;
  char buf[len];

  string *backup_file = NULL;
  string *dests = String.new_with (dest);
  dest = dests->bytes;

  size_t dest_len = dests->num_bytes;
  size_t src_len = bytelen (src);

  if (0 is src_len or 0 is dest_len) {
    if (outToErrStream)
      fprintf (opts.err_stream, "failed to copy: either src or dest are zero length\n");
    goto theerror;
  }

  int src_exists = file_exists (src);
  ifnot (src_exists) {
    errno = ENOENT;
    if (outToErrStream)
      fprintf (opts.err_stream, "`%s': %s\n", src, Error.errno_string (errno));
    goto theerror;

  } else {
    ifnot (file_is_readable (src)) {
      errno = EPERM;
      if (outToErrStream)
        fprintf (opts.err_stream, "'%s': %s\n", src, Error.errno_string (errno));
      goto theerror;
    }
  }

  struct stat src_st;
  if (-1 is lstat (src, &src_st)) {
    if (outToErrStream)
      fprintf (opts.err_stream, "%s: %s\n", src, Error.errno_string (errno));
    goto theerror;
  }

  int dest_exists = file_exists (dest);
  struct stat dest_st;

  if (dest_exists) {
    if (-1 is lstat (dest, &dest_st)) {
      if (outToErrStream)
        fprintf (opts.err_stream, "%s: %s\n", dest, Error.errno_string (errno));
      goto theerror;
    }
  }

  if (S_ISDIR(src_st.st_mode)) {
    ifnot (opts.recursive) {
      if (outToErrStream)
        fprintf (opts.err_stream, "%s: is a directory\n", src);
      goto theerror;
    }

    if (dest_exists) {
      ifnot (S_ISDIR(dest_st.st_mode)) {
        if (outToErrStream)
          fprintf (opts.err_stream, "cannot overwrite non directory `%s' with directory `%s'",
              dest, src);
        goto theerror;
      }
    }

    char *src_basename = Path.basename ((char *) src);
    char *dest_basename = Path.basename ((char *) dest);
    size_t sblen = bytelen (src_basename);
    int idx = sblen;
    while (idx > 1 and src_basename[--idx] is DIR_SEP) sblen--;

    if (0 is Cstring.eq_n (src_basename, dest_basename, sblen) and
        dest_exists) {
      if (dests->bytes[dests->num_bytes - 1] is DIR_SEP)
        String.append_with_len (dests, src_basename, sblen);
      else
        String.append_with_fmt (dests, "/%s", src_basename);

      dest = dests->bytes;
      dest_len = dests->num_bytes;
      dest_exists = file_exists (dest);
    } else if (dest_exists) {
      ifnot (S_ISDIR(dest_st.st_mode)) {
        if (outToErrStream)
          fprintf (opts.err_stream, "%s: is not a directory\n", dest);
        goto theerror;
      }
    }

    ifnot (dest_exists) {
      if (-1 is mkdir (dest, src_st.st_mode)) {
        if (outToErrStream)
          fprintf (opts.err_stream, "cannot make directory %s, %s\n",
              dest, Error.errno_string (errno));
        goto theerror;
      }
    }

    if (opts.curdepth is opts.maxdepth) {
      fprintf (opts.err_stream, "'%d' depth exceeded '%d' maxdepth\n",
          opts.curdepth, opts.maxdepth);
      goto theerror;
    }

    opts.curdepth++;

    dirlist_t *dlist = Dir.list ((char *) src, 0);
    ifnot (dlist->list->num_items)
      retval = OK;

    vstring_t *it = dlist->list->head;

    while (it) {
      String.trim_end (it->data, DIR_SEP);
      size_t newlen = dest_len + 1 + it->data->num_bytes;
      char newdest[newlen + 1];
      Cstring.cp_fmt (newdest, newlen + 1, "%s/%s", dest, it->data->bytes);
      size_t srclen = src_len + 1 + it->data->num_bytes;
      char newsrc[srclen + 1];
      Cstring.cp_fmt (newsrc, srclen + 1, "%s/%s", src, it->data->bytes);

      retval = file_copy (newsrc, newdest, opts);
      if (retval isnot OK)
        break;

      it = it->next;
    }

    dlist->release (dlist);
    goto theerror;  // unconditionally, as it might be not an error

  } else {
    if (dest_exists) {
      if (-1 is lstat (dest, &dest_st)) {
        if (outToErrStream)
          fprintf (opts.err_stream, "%s: %s\n", dest, Error.errno_string (errno));
        goto theerror;
      }

      if (S_ISDIR(dest_st.st_mode)) {
        String.trim_end (dests, DIR_SEP);
        String.append_with_fmt (dests, "/%s", Path.basename ((char *)src));
        dest = dests->bytes;
        dest_len = dests->num_bytes;
        dest_exists = file_exists (dest);
      }
    }
  }

  if (opts.update and dest_exists) {
    if (src_st.st_size is dest_st.st_size and
        src_st.st_mtime <= dest_st.st_mtime) {
      retval = OK;   // its a duck
      goto theerror; // just release sources
    }
  }

  char src_orig[PATH_MAX];
  if (S_ISLNK(src_st.st_mode)) {
    int r;
    if (-1 is (r = readlink (src, src_orig, PATH_MAX - 1))) {
      if (outToErrStream)
        fprintf (opts.err_stream, "%s: %s\n", src, Error.errno_string (errno));
      goto theerror;
    }

    src_orig[r] = '\0';

    if (opts.dereference) {
      retval = file_copy (src_orig, dest, opts);
      goto theerror;
    }
  }

  dest_exists = file_exists (dest);
  if (dest_exists) {
    ifnot (opts.force) {
      if (opts.interactive) {
        int what = 0;
        if (NULL is opts.on_interactive)
          what = file_copy_on_interactive (dest);
        else
          what = opts.on_interactive (dest);

        switch (what) {
          case  1: goto are_same;
          case  0: retval = OK; goto theerror;
          case -1: goto theerror;
        }
      }

      errno = EEXIST;
      if (outToErrStream)
        fprintf (opts.err_stream, "failed to copy '%s' to '%s': %s and `force` is not set\n",
            src, dest, Error.errno_string (errno));
      goto theerror;
    }

    are_same:
    if (src_st.st_dev is dest_st.st_dev and src_st.st_ino is dest_st.st_ino) {
      if (opts.verbose > OPT_NO_VERBOSE and NULL isnot opts.err_stream)
        fprintf (opts.err_stream, "'%s' and '%s' are the same file\n", src, dest);
      goto theerror;
    }
  }

  if (opts.backup is OPT_BACKUP and dest_exists) {
    if (opts.backup_suffix is NULL) {
      if (outToErrStream)
        fprintf (opts.err_stream, "failed to create backup: backup suffix is a NULL pointer\n");
      goto theerror;
    }

    ifnot (S_ISREG(dest_st.st_mode)) {
      if (outToErrStream)
        fprintf (opts.err_stream, "%s: can make backups only in regular files\n", dest);
      goto theerror;
    }

    size_t blen = bytelen (opts.backup_suffix);
    ifnot (blen) {
      if (outToErrStream)
        fprintf (opts.err_stream, "failed to backup %s: empty backup suffix\n", dest);
    }

    blen += dest_len;
    char dbuf[blen + 1];
    Cstring.cp_fmt (dbuf, blen + 1, "%s%s", dest, opts.backup_suffix);
    if (NOTOK is file_copy (dest, dbuf, FileCopyOpts
        (.force = OPT_FORCE, .verbose = opts.verbose)))
      goto theerror;

    backup_file = String.new_with (Path.basename (dbuf));
  }

  if (S_ISSOCK(src_st.st_mode) or
      S_ISCHR(src_st.st_mode) or
      S_ISBLK(src_st.st_mode)) {
    if (dest_exists) {
      if (-1 is unlink (dest)) {
        if (outToErrStream)
          fprintf (opts.err_stream, "failed to remove '%s': %s\n",
              dest, Error.errno_string (errno));
        goto theerror;
      }
    }

    if (-1 is mknod (dest, src_st.st_mode, src_st.st_rdev)) {
      if (outToErrStream)
        fprintf (opts.err_stream, "mknod failed '%s': %s\n",
            dest, Error.errno_string (errno));
      goto theerror;
    }

    if (opts.verbose >= OPT_VERBOSE and opts.out_stream isnot NULL) {
      fprintf (opts.out_stream, "'%s' -> '%s'", src, dest);
      if (NULL isnot backup_file)
        fprintf (opts.out_stream, " (backup: %s)", backup_file->bytes);
      fprintf (opts.out_stream, "\n");
    }

    retval = OK;
    goto theend;

  } else if (S_ISLNK(src_st.st_mode)) {
    String.release (backup_file);
    if (dest_exists) {
      if (-1 is unlink (dest)) {
        if (outToErrStream)
          fprintf (opts.err_stream, "failed to remove '%s': %s\n",
              dest, Error.errno_string (errno));
        goto theerror;
      }
    }

    if (-1 is symlink (src_orig, dest)) {
      if (outToErrStream)
        fprintf (opts.err_stream, "failed to create symlink '%s' to '%s': %s\n",
            src_orig, dest, Error.errno_string (errno));
      goto theerror;
    }

    if (opts.verbose >= OPT_VERBOSE and opts.out_stream isnot NULL) {
      fprintf (opts.out_stream, "'%s' -> '%s'", src, dest);
      if (NULL isnot backup_file)
        fprintf (opts.out_stream, " (backup: %s)", backup_file->bytes);
      fprintf (opts.out_stream, "\n");
    }

    retval = OK;
    goto theend;
  }

  if (opts.verbose is OPT_VERBOSE_EXTRA and opts.out_stream isnot NULL) {
    FILE *dfp = NULL;
    FILE *sfp = fopen (src, "r");
    if (NULL is sfp) {
      if (outToErrStream)
        fprintf (opts.err_stream, "%s, failed to open source FP: %s\n",
            src, Error.errno_string (errno));
      goto exit_verbose;
    }

    int num = 1;
    do {
      dfp = fopen (dest, "w");
      if (NULL is dfp) {
        if (opts.force is OPT_FORCE) {
          if (errno is EACCES)
            ifnot (unlink (dest))
              continue;
        }

        if (outToErrStream)
          fprintf (opts.err_stream, "%s, failed to open destination FP: %s\n",
              dest, Error.errno_string (errno));
        goto exit_verbose;
      }

      break;
    } while (num--);

    if (-1 is fseek (sfp, 0, SEEK_END)) {
      if (outToErrStream)
        fprintf (opts.err_stream, "%s, fseek error to source FP: %s\n",
            src, Error.errno_string (errno));
      goto exit_verbose;
    }

    idx_t total = ftell (sfp);
    if (-1 is total) {
      if (outToErrStream)
        fprintf (opts.err_stream, "%s, ftell error to source FP: %s\n",
            src, Error.errno_string (errno));
      goto exit_verbose;
    }

    if (-1 is fseek (sfp, 0, SEEK_SET)) {
      if (outToErrStream)
        fprintf (opts.err_stream, "%s, fseek error to source FP: %s\n",
            src, Error.errno_string (errno));
      goto exit_verbose;
    }

    idx_t written = 0;
    idx_t n;

    errno = 0;

    idx_t msglen = 0;
    char msg[128];

    fprintf (opts.out_stream, "'%s' -> '%s' ", src, dest);
    while ((n = fread (&buf, 1, len, sfp)) > 0) {
      written += n;

      if ((idx_t) fwrite (buf, 1, n, dfp) isnot n)
        goto exit_verbose;

      if (feof (sfp))
        break;

      for (idx_t i = 0; i < msglen; i++) { msg[i] = '\b'; } msg[msglen] = '\0';
      fprintf (opts.out_stream, "%s", msg);
      msglen = fprintf (opts.out_stream, "(%.0f%%)", (double) written / total * 100);
      fflush (opts.out_stream);
    }

    for (idx_t i = 0; i < msglen + 1; i++) { msg[i] = '\b'; } msg[msglen + 1] = '\0';
    fprintf (opts.out_stream, "%s", msg);

    ifnot (ferror (sfp)) {
      if (written isnot total) {
        ifnot (NULL is opts.err_stream)
          fprintf (opts.err_stream, " copied %ld bytes instead of %ld", written, total);
        else
          fprintf (opts.out_stream, " copied %ld bytes instead of %ld", written, total);

      } else {
        fprintf (opts.out_stream, " (100%%)");
        retval = OK;
      }

      if (NULL isnot backup_file)
        fprintf (opts.out_stream, " (backup: %s)", backup_file->bytes);
      fprintf (opts.out_stream, "\n");

    } else {
      if (NULL isnot opts.err_stream)
        fprintf (opts.err_stream, "ferrailed to copy '%s' to '%s': %s\n",
            src, dest, Error.errno_string (errno));
    }

    exit_verbose:
    if (NULL isnot sfp) fclose (sfp);
    if (NULL isnot dfp) fclose (dfp);
    goto theend;
  }

  int sfd = -1;
  int dfd = -1;

  sfd = open (src, O_RDONLY);
  if (sfd is -1) {
    if (outToErrStream)
      fprintf (opts.err_stream, "%s: failed to open source FD : %s\n",
          src, Error.errno_string (errno));
    goto theend_no_verbose;
  }

  int num = 1;
  do {
    dfd = creat (dest, src_st.st_mode);

    if (-1 is dfd) {
      if (opts.force is OPT_FORCE) {
        if (errno is EACCES)
          ifnot (unlink (dest))
            continue;
      }

      if (outToErrStream)
        fprintf (opts.err_stream, "%s: failed to open destination FD : %s\n",
            dest, Error.errno_string (errno));
      goto theend_no_verbose;
    }

    break;
  } while (num--);

  idx_t n;

  while ((n = IO.fd.read (sfd, buf, len + 1)) > 0)
    if (IO.fd.write (dfd, buf, n) < 0) {
      if (outToErrStream)
        fprintf (opts.err_stream, "failed to write '%s' to '%s': %s\n",
            src, dest, Error.errno_string (errno));
      goto theend_no_verbose;
    }

  if (n is -1) {
    if (outToErrStream)
      fprintf (opts.err_stream, "fsailed to copy '%s' to '%s': %s\n",
          src, dest, Error.errno_string (errno));
    goto theend_no_verbose;
  }

  if (opts.verbose >= OPT_VERBOSE and NULL isnot opts.out_stream) {
    fprintf (opts.out_stream, "'%s' -> '%s'", src, dest);
    if (NULL isnot backup_file)
      fprintf (opts.out_stream, " (backup: %s)", backup_file->bytes);
    fprintf (opts.out_stream, "\n");
  }

  retval = OK;

theend_no_verbose:
  if (-1 isnot sfd) close (sfd);
  if (-1 isnot dfd) close (dfd);

theend:
  if (retval is OK and opts.preserve >= OPT_PRESERVE) {
    struct timespec times[2];
    times[0] = src_st.st_atim;
    times[1] = src_st.st_mtim;

    if (-1 is utimensat (AT_FDCWD, dest, times, AT_SYMLINK_NOFOLLOW)) {
      if (outToErrStream)
        fprintf (opts.err_stream, "failed to change utime and atime to '%s': %s\n",
            dest, Error.errno_string (errno));

      src_st.st_mode &= ~(S_ISUID | S_ISGID);
      retval = NOTOK;
    }

    if (retval is OK) {
      int retv_chown = 0;
      int retv_chmod = 0;

      if (S_ISLNK(src_st.st_mode)) {
        if (opts.preserve is OPT_PRESERVE_OWNER)
          retv_chown = lchown (dest, src_st.st_uid, src_st.st_gid);
      } else {
        if (opts.preserve is OPT_PRESERVE_OWNER)
          retv_chown = chown (dest, src_st.st_uid, src_st.st_gid);
        retv_chmod = chmod (dest, src_st.st_mode);
      }

      retval = (retv_chown is -1 or retv_chmod is -1 ? NOTOK : OK);

      if (retv_chown is -1)
        if (outToErrStream)
          fprintf (opts.err_stream, "failed to change ownership to '%s': %s\n",
              dest, Error.errno_string (errno));

      if (retv_chmod is -1)
        if (outToErrStream)
          fprintf (opts.err_stream, "failed to change mode to '%s': %s\n",
              dest, Error.errno_string (errno));
    }
  }

theerror:
  String.release (dests);
  String.release (backup_file);
  return retval;
}

public file_T __init_file__ (void) {
  __INIT__ (io);
  __INIT__ (os);
  __INIT__ (dir);
  __INIT__ (path);
  __INIT__ (error);
  __INIT__ (term);
  __INIT__ (string);
  __INIT__ (vstring);
  __INIT__ (cstring);

  return (file_T) {
    .self = (file_self) {
      .size = file_size,
      .copy = file_copy,
      .write = file_write,
      .append = file_append,
      .exists = file_exists,
      .is_rwx = file_is_rwx,
      .is_elf = file_is_elf,
      .is_reg = file_is_reg,
      .is_lnk = file_is_lnk,
      .is_sock = file_is_sock,
      .is_fifo = file_is_fifo,
      .is_readable = file_is_readable,
      .is_writable = file_is_writable,
      .is_executable = file_is_executable,
      .readlink = file_readlink,
      .readlines = file_readlines,
      .readlines_from_fp = file_readlines_from_fp,
      .tmpfname = (file_tmpfname_self) {
        .new = file_tmpfname_new,
        .release = file_tmpfname_release
      },
      .mode = (file_mode_self) {
        .stat_to_string = OS.mode.stat_to_string,
        .from_octal_string = file_mode_from_octal_string

      }
    }
  };
}
