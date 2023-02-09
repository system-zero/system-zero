// APPLICATION "zsu"
#define REQUIRE_STD_DEFAULT_SOURCE
#define REQUIRE_STD_GNU_SOURCE

#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_STDARG
#define REQUIRE_TIME
#define REQUIRE_SYS_TYPES
#define REQUIRE_SYS_STAT
#define REQUIRE_FCNTL
#define REQUIRE_GRP

#define REQUIRE_CSTRING_TYPE DECLARE
#define REQUIRE_STRING_TYPE  DECLARE
#define REQUIRE_PROC_TYPE    DECLARE
#define REQUIRE_AUTH_TYPE    DECLARE
#define REQUIRE_IO_TYPE      DECLARE
#define REQUIRE_ERROR_TYPE   DECLARE

#include <z/cenv.h>

#define ROOT_UID  0
#define ROOT_GID  0
#define ROOT_NAME "root"

#define TIMEOUT        1200
#define MAXLEN_DIR     4096
#define MAXLEN_COMMAND 8192

int main (int argc, char **argv) {
  if (geteuid()) {
    Stderr.print_fmt ("%s: is not installed setuid\n", argv[0]);
    return 1;
  }

  argc--; argv++;

  __INIT__ (io);
  __INIT__ (proc);
  __INIT__ (auth);
  __INIT__ (error);
  __INIT__ (string);
  __INIT__ (cstring);

  char dir[MAXLEN_DIR];
  dir[0] = '\0';

  char command[MAXLEN_COMMAND];
  command[0] = '\0';

  int timeout = TIMEOUT;

  int nargc = 0;
  for (int i = 0; i < argc; i++) {
    if (Cstring.eq_n (argv[i], "--chdir=", 8)) {
      size_t len = bytelen (argv[i]) - 8;
      if (len >= MAXLEN_DIR) {
        Stderr.print_fmt ("%s length (%d): exceeded maximum length (%d)\n",
           argv[i] + 8, len, MAXLEN_DIR - 1);
        return 1;
      }

      Cstring.cp (dir, MAXLEN_DIR, argv[i] + 8, len);
      continue;
    }

    if (Cstring.eq_n (argv[i], "--timeout=", 10)) {
      int tmp = atoi (argv[i] + 10);
      if (tmp >= 0)
        timeout = tmp;
      continue;
    }

    nargc++;

    if (1 isnot nargc)
      Cstring.cat (command, MAXLEN_COMMAND, " ");

    Cstring.cat (command, MAXLEN_COMMAND, argv[i]);
  }

  ifnot (nargc) {
    Stderr.print ("not enough arguments: a program name is missing\n");
    return 1;
  }

  auth_t *auth = Auth.new (NULL, NULL, TIMEOUT);
  if (NULL is auth)
    return 1;

  Auth.set.cached_time (auth, timeout);

  int retval = 1;

  string_t *vsu_timestamp_file = NULL;

#ifdef VSU_DIR
  int num_iter = 0;
stat_again:
  if (++num_iter is 3) {
    Stderr.print ("probably a race condition\n");
    goto theend;
  }

  struct stat st;
  if (-1 isnot stat (VSU_DIR, &st)) {
    ifnot (S_ISDIR (st.st_mode)) {
      Stderr.print_fmt ("%s: is not a directory\n", VSU_DIR);
      goto theend;
    }

    if (st.st_uid isnot ROOT_UID or st.st_gid isnot ROOT_GID) {
      Stderr.print_fmt ("%s: is not owned by root\n", VSU_DIR);
      goto theend;
    }

  } else {
    if (-1 is mkdir (VSU_DIR, S_IRWXU)) {
      Stderr.print_fmt ("%s: failed to create, %s\n", VSU_DIR,
          Error.errno_string (errno));
      goto theend;
    }

    if (-1 is chown (VSU_DIR, ROOT_UID, ROOT_GID)) {
      Stderr.print_fmt ("%s: failed to change ownership to root/root, %s\n", VSU_DIR,
          Error.errno_string (errno));
      goto theend;
    }

    goto stat_again;
  }

  char *name = Auth.get.user (auth);
  if (NULL is name)
    goto theend;

  vsu_timestamp_file = String.new_with_fmt (
      VSU_DIR "/timestamp-%s", name);

#endif /* VSU_DIR */

  ifnot (NULL is vsu_timestamp_file) {
    struct stat sta;
    if (-1 isnot stat (vsu_timestamp_file->bytes, &sta)) {
      ifnot (S_ISREG (sta.st_mode)) {
        Stderr.print_fmt ("%s: is not a regular file\n", vsu_timestamp_file->bytes);
        goto theend;
      }

      if (sta.st_uid isnot ROOT_UID) {
        Stderr.print_fmt ("%s: is not owned by root\n", vsu_timestamp_file->bytes);
        goto theend;
      }

      gid_t gid = Auth.get.gid (auth);
      if (gid isnot sta.st_gid) {
        Stderr.print_fmt ("%s: group id does not match\n", vsu_timestamp_file->bytes);
        goto theend;
      }

      struct timespec ts;
      if (-1 is clock_gettime (CLOCK_BOOTTIME, &ts)) {
        Stderr.print_fmt ("clock_gettime() failed: %s\n",
            Error.errno_string (errno));
        goto theend;
      }

      time_t cur = time (NULL);

      if (sta.st_mtime > ts.tv_sec and sta.st_mtime < cur)
        Auth.set.timeout (auth, sta.st_mtime);
    }
  }

  if (NOTOK is Auth.check (auth)) {
    Stdout.print ("authorization failure\n");
    goto theend;
  }

  if (-1 is setresgid (ROOT_GID, ROOT_GID, ROOT_GID)) {
    Stderr.print_fmt ("setresgid() failed: %s\n",
        Error.errno_string (errno));
    goto theend;
  }

  if (-1 is initgroups (ROOT_NAME, ROOT_GID)) {
    Stderr.print_fmt ("initgroups() failed: %s\n",
        Error.errno_string (errno));
    goto theend;
  }

  if (-1 is setresuid (ROOT_UID, ROOT_UID, ROOT_UID)) {
    Stderr.print_fmt ("setresuid() failed: %s\n",
        Error.errno_string (errno));
    goto theend;
  }

  ifnot (NULL is vsu_timestamp_file) {
    if (-1 is access (vsu_timestamp_file->bytes, F_OK)) {
      int fd = open (vsu_timestamp_file->bytes, O_CREAT, S_IRUSR|S_IWUSR);
      if (-1 is fd) {
        Stderr.print_fmt ("open() failed to open %s: %s\n",
            vsu_timestamp_file->bytes, Error.errno_string (errno));
        goto theend;
      }

      if (-1 is chown (vsu_timestamp_file->bytes, ROOT_UID, Auth.get.uid (auth))) {
        Stderr.print_fmt ("%s: failed to change ownership to root, %s\n",
            vsu_timestamp_file->bytes, Error.errno_string (errno));
        goto theend;
      }

    } else {
      int fd = open (vsu_timestamp_file->bytes, O_RDONLY|O_NOFOLLOW);
      if (-1 is fd) {
        Stderr.print_fmt ("open() failed to open %s: %s\n",
            vsu_timestamp_file->bytes, Error.errno_string (errno));
        goto theend;
      }

      struct timespec ts[2];
      ts[0].tv_nsec = UTIME_NOW;
      ts[1].tv_nsec = UTIME_NOW;

      if (-1 is futimens (fd, ts)) {
        Stderr.print_fmt ("futimens() failed to change timestamps: %s\n",
            Error.errno_string (errno));
        goto theend;
      }
    }
  }

  if (dir[0]) {
    if (-1 is chdir (dir)) {
      Stderr.print_fmt ("%s: failed to change directory, %s\n",
          dir, Error.errno_string (errno));
      goto theend;
    }
  }

#ifdef OPEN_MAX
  int maxfd = OPEN_MAX;
#else
  int maxfd = sysconf (_SC_OPEN_MAX);
#endif

  for (int fd = STDERR_FILENO + 1; fd < maxfd; fd++) {
    errno = 0;
    if (-1 is close (fd)) {
      if (errno is EBADF)
        break;

      Stderr.print_fmt ("%d: failed to close file descriptor, %s\n",
          fd, Error.errno_string (errno));
      goto theend;
    }
  }

  proc_t *this = Proc.new ();
  retval = Proc.exec (this, command);
  Proc.release (this);

theend:
  String.release (vsu_timestamp_file);
  Auth.release (auth);
  return retval;
}
