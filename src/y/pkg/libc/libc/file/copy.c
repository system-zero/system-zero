// as: filecopy
// provides: int filecopy (const char *, const char *, int, int)
// provides: int file_copy (const char *, const char *, file_copy_opts)
// requires: string/string.c
// requires: string/str_eq_n.c
// requires: string/bytelen.c
// requires: string/str_copy_fmt.c
// requires: std/unlink.c
// requires: std/symlink.c
// requires: std/mknod.c
// requires: std/fchownat.c
// requires: std/fchmodat.c
// requires: std/utimensat.c
// requires: dir/make_dir.c
// requires: dir/dirlist.c
// requires: sys/lstat.c
// requires: sys/stat.h
// requires: path/basename.c
// requires: file/exists.c
// requires: file/is_readable.c
// requires: convert/format.c
// requires: convert/decimal_to_string.c
// requires: unistd/fd.c
// requires: unistd/fcntl.h
// requires: term/term.c
// requires: term/term_getkey.c
// requires: file/copy.h

#define toErrFd(fmt, ...)  format_to_fd (opts.err_fd, fmt, ##__VA_ARGS__)
#define toOutFd(fmt, ...)  format_to_fd (opts.out_fd, fmt, ##__VA_ARGS__)
#define toFd(fd, fmt, ...) format_to_fd (fd, fmt, ##__VA_ARGS__)

int filecopy (const char *src, const char *dest, int override, int outfd) {
  if (file_exists (dest)) {
    if (0 == override) {
      sys_errno = EEXIST;
      return -1;
    }
  }

  int destflags = O_CREAT|O_WRONLY|O_TRUNC;
  int perm = S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP;

  int destfd = sys_open3 (dest, destflags, perm);
  if (-1 == destfd)
    return -1;

  int srcfd = sys_open (src, O_RDONLY);
  if (-1 == srcfd)
    return -1;

  int bufsize = 4096;
  char buf[bufsize];
  int tbytes = 0;
  int numread = 0;
  int numwritten = 0;

  if (outfd == -1) {
    while (1) {
      numread = fd_read (srcfd, buf, bufsize);
      if (numread <= 0)
        goto theend;

      numwritten = sys_write (destfd, buf, numread);
      if (numwritten != numread)
        return -1;

      tbytes += numwritten;
    }
  }

  int n = 0;
  char back[] = "\b\b\b\b";

  while (1) {
    numread = fd_read (srcfd, buf, bufsize);
    if (numread <= 0)
      goto theend;

    numwritten = sys_write (destfd, buf, numread);
    if (numwritten != numread)
      return -1;

    tbytes += numwritten;

    int p = (((200 * numwritten) / tbytes) % 2) + ((100 * numwritten) / tbytes);
    // or int p = ((100 * numwritten) / tbytes) + (((1000 * numwritten) / tbytes) % 10 >= 5 ? 1 : 0);
    // https://stackoverflow.com/questions/24284460/calculating-rounded-percentage-in-shell-script-without-using-bc

    n = toFd (outfd, "%.*s(%d%%)", n, back, p);
  }

theend:
  sys_close (srcfd);
  sys_close (destfd);
  return tbytes;
}

static int file_on_interactive (const char *file, const char *msg, file_copy_opts opts) {
  term_t *term = term_new ();
  term_raw_mode (term);

  toOutFd ("%s `%s'? y[es]/n[o]/q[uit]", msg, file);

  int retval = 0;

  while (1) {
    int c = term_getkey (0);
    if (c == 'y') { retval =  1; break; }
    if (c == 'n') { retval =  0; break; }
    if (c == 'q') { retval = -1; break; }
  }

  term_orig_mode (term);
  term_release (&term);
  toOutFd ("\n");
  return retval;
}

static string *file_backup (const char *file, size_t file_len, struct stat file_st, file_copy_opts opts) {
  int outToErrFd = (opts.verbose > FILECOPY_NO_VERBOSE && -1 != opts.err_fd);

  if (opts.backup_suffix == NULL) {
    if (outToErrFd)
      toErrFd ("failed to create backup: backup suffix is a NULL pointer\n");

    return NULL;
  }

  if (0 == S_ISREG(file_st.st_mode)) {
    if (outToErrFd)
      toErrFd ("%s: can make backups only in regular files\n", file);

    return NULL;
  }

  size_t blen = bytelen (opts.backup_suffix);
  if (0 == blen) {
    if (outToErrFd)
      toErrFd ("failed to backup %s: empty backup suffix\n", file);

    return NULL;
  }

  blen += file_len;
  char dbuf[blen + 1];
  str_copy_fmt (dbuf, blen + 1, "%s%s", file, opts.backup_suffix);

  if (-1 == file_copy (file, dbuf, FileCopyOpts
      (.force = true, .verbose = opts.verbose)))
    return NULL;

  return string_new_with (path_basename (dbuf));
}

int file_copy (const char *src, const char *o_dest, file_copy_opts opts) {
  int retval = -1;

  char *dest = (char *) o_dest;

  if (opts.all == true) {
    opts.recursive = true;
    opts.preserve = true;
    opts.dereference = true;
  }

  if (opts.interactive) opts.force = true;

  int outToErrFd = (opts.verbose > FILECOPY_NO_VERBOSE && -1 != opts.err_fd);
  int outToOutFd = (opts.verbose > FILECOPY_NO_VERBOSE && -1 != opts.out_fd);

  if (NULL == src || NULL == dest) {
    if (outToErrFd)
      toErrFd ("failed to copy: either src or dest are NULL pointers\n");

    return -1;
  }

  string *backup_file = NULL;
  string *dests = string_new_with (dest);
  dest = dests->bytes;

  size_t dest_len = dests->num_bytes;
  size_t src_len = bytelen (src);

  if (0 == src_len || 0 == dest_len) {
    if (outToErrFd)
      toErrFd ("failed to copy: either src or dest are zero length\n");

    goto theerror;
  }

  struct stat src_st;
  if (-1 == sys_lstat (src, &src_st)) {
    if (outToErrFd)
      toErrFd ("'%s': %s\n", src, errno_string (sys_errno));

    goto theerror;
  }

  int src_exists = file_exists (src);

  if (0 == src_exists) {
    sys_errno = ENOENT;
    if (outToErrFd)
      toErrFd ("'%s': %s\n", src, errno_string (sys_errno));

    goto theerror;

  } else {
    if (0 == file_is_readable (src)) {
      if (S_ISLNK(src_st.st_mode)) {
        sys_errno = EPERM;

        if (outToErrFd)
          toErrFd ("'%s': %s\n", src, errno_string (sys_errno));

        goto theerror;
      }
    }
  }

  int dest_exists = file_exists (dest);
  struct stat dest_st;

  if (dest_exists) {
    if (-1 == sys_lstat (dest, &dest_st)) {
      if (outToErrFd)
        toErrFd ("'%s': %s\n", dest, errno_string (sys_errno));
      goto theerror;
    }
  }

  if (S_ISDIR(src_st.st_mode)) {
    if (0 == opts.recursive) {
      if (outToErrFd)
        toErrFd ("%s: is a directory\n", src);

      goto theerror;
    }

    if (dest_exists) {
      if (0 == S_ISDIR(dest_st.st_mode)) {
        if (outToErrFd)
          toErrFd ("cannot overwrite non directory `%s' with directory `%s'",
              dest, src);

        goto theerror;
      }
    }

    char *src_basename = path_basename ((char *) src);
    char *dest_basename = path_basename ((char *) dest);
    size_t sblen = bytelen (src_basename);
    int idx = sblen;

    while (idx > 1 && src_basename[--idx] == DIR_SEP)
      sblen--;

    if (0 == str_eq_n (src_basename, dest_basename, sblen) && dest_exists) {
      if (dests->bytes[dests->num_bytes - 1] == DIR_SEP)
        string_append_with_len (dests, src_basename, sblen);
      else
        string_append_with_fmt (dests, "/%s", src_basename);

      dest = dests->bytes;
      dest_len = dests->num_bytes;
      dest_exists = file_exists (dest);
    } else if (dest_exists) {
      if (0 == S_ISDIR(dest_st.st_mode)) {
        if (outToErrFd)
          toErrFd ("%s: is not a directory\n", dest);

        goto theerror;
      }
    }

    if (0 == dest_exists) {
      if (-1 == make_dir (dest, src_st.st_mode)) {
        if (outToErrFd)
          toErrFd ("cannot make directory %s, %s\n",
              dest, errno_string (sys_errno));

        goto theerror;
      }
    }

    if (opts.curdepth == opts.maxdepth) {
      toErrFd ("'%d' depth exceeded '%d' maxdepth\n",
          opts.curdepth, opts.maxdepth);

      goto theerror;
    }

    opts.curdepth++;

    dirlist_t *dlist = dirlist ((char *) src);

    if (0 == dlist->num_entries)
      retval = 0;

    for (uint i = 0; i < dlist->num_entries; i++) {
      string *s = dlist->entries[i];
      string_trim_end (s, DIR_SEP);
      char *entry = path_basename (s->bytes);
      size_t entrylen = bytelen (entry);
      size_t newlen = dest_len + 1 + entrylen;
      char newdest[newlen + 1];
      str_copy_fmt (newdest, newlen + 1, "%s/%s", dest, entrylen);
      size_t srclen = src_len + 1 + entrylen;
      char newsrc[srclen + 1];
      str_copy_fmt (newsrc, srclen + 1, "%s/%s", src, entrylen);

      retval = file_copy (newsrc, newdest, opts);

      if (retval != 0)
        break;
    }

    dirlist_release (&dlist);
    goto theerror;  // unconditionally, as it might not be an error

  } else {
    if (dest_exists) {
      if (-1 == sys_lstat (dest, &dest_st)) {
        if (outToErrFd)
          toErrFd ("'%s': %s\n", dest, errno_string (sys_errno));

        goto theerror;
      }

      if (S_ISDIR(dest_st.st_mode)) {
        string_trim_end (dests, DIR_SEP);
        string_append_with_fmt (dests, "/%s", path_basename ((char *)src));
        dest = dests->bytes;
        dest_len = dests->num_bytes;
        dest_exists = file_exists (dest);
      }
    }
  }

  if (opts.update && dest_exists) {
    if (src_st.st_size == dest_st.st_size &&
        src_st.st_mtime <= dest_st.st_mtime) {
      retval = 0;   // its a duck

      goto theerror; // just release sources
    }
  }

  char src_orig[PATH_MAX];
  if (S_ISLNK(src_st.st_mode)) {
    int r;
    if (-1 == (r = sys_readlink (src, src_orig, PATH_MAX - 1))) {
      if (outToErrFd)
        toErrFd ("'%s': %s\n", src, errno_string (sys_errno));

      goto theerror;
    }

    src_orig[r] = '\0';

    if (opts.dereference == true) {
      retval = file_copy (src_orig, dest, opts);
      goto theerror;
    }
  }

  dest_exists = file_exists (dest);
  if (dest_exists) {
    if (false == opts.force) {
      if (opts.interactive) {
        int what = 0;
        if (NULL == opts.on_interactive)
          what = file_on_interactive (dest, "copy: overwrite", opts);
        else
          what = opts.on_interactive (dest, "copy: overwrite", opts);

        switch (what) {
          case  1: goto are_same;
          case  0: retval = 0; goto theerror;
          case -1: goto theerror;
        }
      }

      sys_errno = EEXIST;
      if (outToErrFd)
        toErrFd ("failed to copy '%s' to '%s': %s and `force` is not set\n",
            src, dest, errno_string (sys_errno));

      goto theerror;
    }

    are_same:
    if (src_st.st_dev == dest_st.st_dev && src_st.st_ino == dest_st.st_ino) {
      if (outToErrFd)
        toErrFd ("'%s' and '%s' are the same file\n", src, dest);

      goto theerror;
    }
  }

  if (opts.backup == true && dest_exists) {
    backup_file = file_backup (dest, dest_len, dest_st, opts);
    if (NULL == backup_file)
      goto theerror;
  }

  if (S_ISSOCK(src_st.st_mode) || S_ISCHR(src_st.st_mode) ||
      S_ISBLK(src_st.st_mode)) {
    if (dest_exists) {
      if (-1 == sys_unlink (dest)) {
        if (outToErrFd)
          toErrFd ("failed to remove '%s': %s\n", dest, errno_string (sys_errno));

        goto theerror;
      }
    }

    if (-1 == sys_mknod (dest, src_st.st_mode, src_st.st_rdev)) {
      if (outToErrFd)
        toErrFd ("mknod failed '%s': %s\n", dest, errno_string (sys_errno));

      goto theerror;
    }

    if (outToOutFd) {
      toOutFd ("'%s' -> '%s'", src, dest);
      if (NULL != backup_file)
        toOutFd (" (backup: %s)", backup_file->bytes);

      toOutFd ("\n");
    }

    retval = 0;
    goto theend;

  }

  if (S_ISLNK(src_st.st_mode)) {
    string_release (backup_file);
    if (dest_exists) {
      if (-1 == sys_unlink (dest)) {
        if (outToErrFd)
          toErrFd ("failed to remove '%s': %s\n", dest, errno_string (sys_errno));

        goto theerror;
      }
    }

    if (-1 == sys_symlink (src_orig, dest)) {
      if (outToErrFd)
        toErrFd ("failed to create symlink '%s' to '%s': %s\n",
            src_orig, dest, errno_string (sys_errno));

      goto theerror;
    }

    if (outToOutFd) {
      toOutFd ("'%s' -> '%s'", src, dest);
      if (NULL != backup_file)
        toOutFd (" (backup: %s)", backup_file->bytes);

      toOutFd ("\n");
    }

    retval = 0;
    goto theend;
  }

  int verbose = opts.verbose <= FILECOPY_VERBOSE && outToOutFd;
  int outfd = (opts.verbose == FILECOPY_VERBOSE_EXTRA && outToOutFd)
      ? opts.out_fd : -1;

  if (verbose)
    toOutFd ("'%s' -> '%s'", src, dest);

  int copy = filecopy (src, dest, opts.force, outfd);
  if (-1 == copy) {
    if (outToErrFd)
      toErrFd ("failed to copy '%s' to '%s': %s\n", src, dest, errno_string (sys_errno));

    goto theerror;
  }

  if (verbose) {
    if (NULL != backup_file)
      toOutFd (" (backup: %s)\n", backup_file->bytes);
    else
      toOutFd ("\n");
   }

   retval = 0;

theend:
  if (retval == 0 && opts.preserve >= FILECOPY_PRESERVE) {
    struct timespec times[2];
    times[0].tv_sec = src_st.st_atime;
    times[0].tv_nsec =src_st.st_atime_nsec;
    times[1].tv_sec = src_st.st_mtime;
    times[1].tv_nsec =src_st.st_mtime_nsec;

    if (-1 == sys_utimensat (AT_FDCWD, dest, times, AT_SYMLINK_NOFOLLOW)) {
      if (outToErrFd)
        toErrFd ("failed to change utime and atime to '%s': %s\n", dest, errno_string (sys_errno));

      src_st.st_mode &= ~(S_ISUID | S_ISGID);
      retval = -1;
    }

    if (retval == 0) {
      int retv_chown = 0;
      int retv_chmod = 0;

      if (S_ISLNK(src_st.st_mode)) {
        if (opts.preserve == FILECOPY_PRESERVE_OWNER)
          retv_chown = sys_fchownat (AT_FDCWD, dest, src_st.st_uid, src_st.st_gid, AT_SYMLINK_NOFOLLOW);
      } else {
        if (opts.preserve == FILECOPY_PRESERVE_OWNER)
          retv_chown = sys_fchownat (AT_FDCWD, dest, src_st.st_uid, src_st.st_gid, 0);

        retv_chmod = sys_fchmodat (AT_FDCWD, dest, src_st.st_mode, 0);
      }

      retval = (retv_chown == -1 || retv_chmod == -1 ? -1 : 0);

      if (retv_chown == -1)
        if (outToErrFd)
          toErrFd ("failed to change ownership to '%s': %s\n", dest, errno_string (sys_errno));

      if (retv_chmod == -1)
        if (outToErrFd)
          toErrFd ("failed to change mode to '%s': %s\n", dest, errno_string (sys_errno));
    }
  }

theerror:
  string_release (dests);
  string_release (backup_file);

  return retval;
}

/* test {
// num-tests: 1
#define REQUIRE_FILECOPY
#define REQUIRE_MAKE_DIR
#define REQUIRE_RMDIR
#define REQUIRE_READFILE
#define REQUIRE_UNLINK
#define REQUIRE_FD
#define REQUIRE_ATOI
#define REQUIRE_FORMAT

#include <libc.h>

#define FILECOPY_DIR "/tmp/filecopy_test"

static char t[] = "asfg\n1234\nαβψδ\n";
static const char *bfile = FILECOPY_DIR "/b";

static int test_init (void) {
  if (-1 == make_dir (FILECOPY_DIR, 0700)) return -1;
  int fd = sys_open3 ( FILECOPY_DIR "/a", O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
  if (-1 == fd) return -1;
  if (sys_write (fd, t, sizeof (t)) == -1) return -1;
  sys_close (fd);
  fd = sys_open3 ( FILECOPY_DIR "/b", O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
  if (-1 == fd) return -1;
  sys_close (fd);
  return 0;
}

static int first_test (int total) {
  int retval = -1;
  tostdout ("[%d] testing file_copy() %s - ", total, __func__);

  if (-1 == test_init ()) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    return -1;
  }

  retval = file_copy (FILECOPY_DIR "/a", FILECOPY_DIR "/b",
     FileCopyOpts(.verbose = 0));

  if (-1 != retval) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    return -1;
  }

  retval = file_copy (FILECOPY_DIR "/a", FILECOPY_DIR "/b",
     FileCopyOpts(.verbose = 0, .force = true));

  if (-1 == retval) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    return -1;
  }

  readfile_t rf;
  rf.bytes = Alloc (199);
  rf.num_bytes = 0;
  rf.file = bfile;
  readfile_u (&rf);

  if (0 == str_eq (rf.bytes, t)) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  retval = file_copy (FILECOPY_DIR "/a", FILECOPY_DIR "/b",
     FileCopyOpts(.verbose = 0, .force = true, .all = true));

  if (-1 == retval) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  tostdout ("\e[32m[OK]\e[m\n");
  retval = 0;

theend:
  sys_unlink (FILECOPY_DIR "/b");
  sys_unlink (FILECOPY_DIR "/a");
  sys_rmdir  (FILECOPY_DIR);

  Release (rf.bytes);
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
