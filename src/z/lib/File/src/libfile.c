#define LIBRARY "File"

#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_SYS_STAT
#define REQUIRE_SYS_TYPES
#define REQUIRE_FCNTL
#define REQUIRE_TIME

#define REQUIRE_STRING_TYPE  DECLARE
#define REQUIRE_CSTRING_TYPE DECLARE
#define REQUIRE_VSTRING_TYPE DECLARE
#define REQUIRE_DIR_TYPE     DECLARE
#define REQUIRE_FILE_TYPE    DONOT_DECLARE

#include <z/cenv.h>

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

private char *file_mode_stat_to_string (char *mode_string, mode_t mode) {
  /* assumed at least 11 bytes */

  if (S_ISREG(mode)) mode_string[0] = '-';
  else if (S_ISDIR(mode)) mode_string[0] = 'd';
  else if (S_ISLNK(mode)) mode_string[0] = 'l';
  else if (S_ISCHR(mode)) mode_string[0] = 'c';
  else if (S_ISFIFO(mode)) mode_string[0] = 'p';
  else if (S_ISSOCK(mode)) mode_string[0] = 's';
  else if (S_ISBLK(mode)) mode_string[0] = 'b';

  if (mode & S_IRUSR) mode_string[1] = 'r'; else mode_string[1] = '-';
  if (mode & S_IWUSR) mode_string[2] = 'w'; else mode_string[2] = '-';
  if (mode & S_IXUSR) mode_string[3] = 'x'; else mode_string[3] = '-';
  if (mode & S_ISUID) mode_string[3] = 's';

  if (mode & S_IRGRP) mode_string[4] = 'r'; else mode_string[4] = '-';
  if (mode & S_IWGRP) mode_string[5] = 'w'; else mode_string[5] = '-';
  if (mode & S_IXGRP) mode_string[6] = 'x'; else mode_string[6] = '-';
  if (mode & S_ISGID) mode_string[6] = 'g';

  if (mode & S_IROTH) mode_string[7] = 'r'; else mode_string[7] = '-';
  if (mode & S_IWOTH) mode_string[8] = 'w'; else mode_string[8] = '-';
  if (mode & S_IXOTH) mode_string[9] = 'x'; else mode_string[9] = '-';
  if (mode & S_ISVTX) mode_string[9] = 't';

  mode_string[10] = '\0';
  return mode_string;
}

static Vstring_t *file_readlines (char *file, Vstring_t *lines,
                                 FileReadLines_cb cb, void *obj) {
  Vstring_t *llines = lines;
  if (NULL is llines) llines = Vstring.new ();
  if (-1 is access (file, F_OK|R_OK)) goto theend;
  FILE *fp = fopen (file, "r");
  char *buf = NULL;
  size_t len;
  ssize_t nread;

  if (cb isnot NULL) {
    int num = 0;
    while (-1 isnot (nread = getline (&buf, &len, fp))) {
      cb (llines, buf, nread, ++num, obj);
    }
  } else {  /* by default an array of lines */
    while (-1 isnot (nread = getline (&buf, &len, fp))) {
      buf[nread - 1] = '\0';
      Vstring.current.append_with (llines, buf);
    }
  }

  fclose (fp);
  ifnot (buf is NULL) free (buf);

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

static void file_tmpfname_release (tmpfname_t *this) {
  ifnot (this) return;
  ifnot (NULL is this->fname) {
    unlink (this->fname->bytes);
    String.release (this->fname);
    this->fname = NULL;
  }

  if (-1 isnot this->fd)
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
    file_tmpfname_release (this);
    this = NULL;
  }

theend:
  dlist->free (dlist);
  return this;
}

public file_T __init_file__ (void) {
  __INIT__ (dir);
  __INIT__ (string);
  __INIT__ (vstring);
  __INIT__ (cstring);

  return (file_T) {
    .self = (file_self) {
      .size = file_size,
      .write = file_write,
      .append = file_append,
      .exists = file_exists,
      .readlines = file_readlines,
      .is_rwx = file_is_rwx,
      .is_elf = file_is_elf,
      .is_reg = file_is_reg,
      .is_sock = file_is_sock,
      .is_readable = file_is_readable,
      .is_writable = file_is_writable,
      .is_executable = file_is_executable,
      .tmpfname = (file_tmpfname_self) {
        .new = file_tmpfname_new,
        .release = file_tmpfname_release
      },
      .mode = (file_mode_self) {
        .stat_to_string = file_mode_stat_to_string,
        .from_octal_string = file_mode_from_octal_string

      }
    }
  };
}
