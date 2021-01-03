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

#include <z/cenv.h>

#define ROOT_UID  0
#define ROOT_GID  0
#define ROOT_NAME "root"

#define TIMEOUT        1200
#define MAXLEN_DIR     4096
#define MAXLEN_COMMAND 8192

int main (int argc, char **argv) {
  if (geteuid()) {
    fprintf (stderr, "%s: is not installed setuid\n", argv[0]);
    return 1;
  }

  argc--; argv++;

  __INIT__ (cstring);
  __INIT__ (string);
  __INIT__ (proc);
  __INIT__ (auth);

  char dir[MAXLEN_DIR];
  dir[0] = '\0';

  char command[MAXLEN_COMMAND];
  command[0] = '\0';

  int timeout = TIMEOUT;

  int nargc = 0;
  for (int i = 0; i < argc; i++) {
    if (Cstring.eq_n (argv[i], "--chdir=", 8)) {
      Cstring.cp (dir, MAXLEN_DIR, argv[i] + 8, MAXLEN_DIR);
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
    fprintf (stderr, "not enough arguments: a program name is missing\n");
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
    fprintf (stderr, "probably a race condition\n");
    goto theend;
  }

  struct stat st;
  if (-1 isnot stat (VSU_DIR, &st)) {
    ifnot (S_ISDIR (st.st_mode)) {
      fprintf (stderr, "%s: is not a directory\n", VSU_DIR);
      goto theend;
    }

    if (st.st_uid isnot ROOT_UID or st.st_gid isnot ROOT_GID) {
      fprintf (stderr, "%s: is not owned by root\n", VSU_DIR);
      goto theend;
    }

  } else {
    if (-1 is mkdir (VSU_DIR, S_IRWXU)) {
      fprintf (stderr, "%s: failed to create, %s\n", VSU_DIR, strerror (errno));
      goto theend;
    }

    if (-1 is chown (VSU_DIR, ROOT_UID, ROOT_GID)) {
      fprintf (stderr, "%s: failed to change ownership to root/root, %s\n", VSU_DIR,
          strerror (errno));
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
        fprintf (stderr, "%s: is not a regular file\n", vsu_timestamp_file->bytes);
        goto theend;
      }

      if (sta.st_uid isnot ROOT_UID) {
        fprintf (stderr, "%s: is not owned by root\n", vsu_timestamp_file->bytes);
        goto theend;
      }

      gid_t gid = Auth.get.gid (auth);
      if (gid isnot sta.st_gid) {
        fprintf (stderr, "%s: group id does not match\n", vsu_timestamp_file->bytes);
        goto theend;
      }

      struct timespec ts;
      if (-1 is clock_gettime (CLOCK_BOOTTIME, &ts)) {
        fprintf (stderr, "clock_gettime() failed: %s\n",
            strerror (errno));
        goto theend;
      }

      time_t cur = time (NULL);

      if (sta.st_mtime > ts.tv_sec and sta.st_mtime < cur)
        Auth.set.timeout (auth, sta.st_mtime);
    }
  }

  if (NOTOK is Auth.check (auth)) {
    fprintf (stdout, "authorization failure\n");
    goto theend;
  }

  if (-1 is setresgid (ROOT_GID, ROOT_GID, ROOT_GID)) {
    fprintf (stderr, "setresgid() failed: %s\n",
        strerror (errno));
    goto theend;
  }

  if (-1 is initgroups (ROOT_NAME, ROOT_GID)) {
    fprintf (stderr, "initgroups() failed: %s\n",
        strerror (errno));
    goto theend;
  }

  if (-1 is setresuid (ROOT_UID, ROOT_UID, ROOT_UID)) {
    fprintf (stderr, "setresuid() failed: %s\n",
        strerror (errno));
    goto theend;
  }

  ifnot (NULL is vsu_timestamp_file) {
    if (-1 is access (vsu_timestamp_file->bytes, F_OK)) {
      int fd = open (vsu_timestamp_file->bytes, O_CREAT, S_IRUSR|S_IWUSR);
      if (-1 is fd) {
        fprintf (stderr, "open() failed to open %s: %s\n",
            vsu_timestamp_file->bytes, strerror (errno));
        goto theend;
      }

      if (-1 is chown (vsu_timestamp_file->bytes, ROOT_UID, Auth.get.uid (auth))) {
        fprintf (stderr, "%s: failed to change ownership to root, %s\n",
            vsu_timestamp_file->bytes, strerror (errno));
        goto theend;
      }

    } else {
      int fd = open (vsu_timestamp_file->bytes, O_RDONLY|O_NOFOLLOW);
      if (-1 is fd) {
        fprintf (stderr, "open() failed to open %s: %s\n",
            vsu_timestamp_file->bytes, strerror (errno));
        goto theend;
      }

      struct timespec ts[2];
      ts[0].tv_nsec = UTIME_NOW;
      ts[1].tv_nsec = UTIME_NOW;

      if (-1 is futimens (fd, ts)) {
        fprintf (stderr, "futimens() failed to change timestamps: %s\n", strerror (errno));
        goto theend;
      }
    }
  }

  if (dir[0]) chdir (dir);

    #ifdef OPEN_MAX
  int maxfd = OPEN_MAX;
    #else
  int maxfd = sysconf (_SC_OPEN_MAX);
    #endif

  for (int fd = STDERR_FILENO + 1; fd < maxfd; fd++) {
    errno = 0;
    if (close (fd) is -1 and errno is EBADF)
      break;
  }

  proc_t *this = Proc.new ();
  retval = Proc.exec (this, command);
  Proc.release (this);

theend:
  String.release (vsu_timestamp_file);
  Auth.release (auth);
  return retval;
}
