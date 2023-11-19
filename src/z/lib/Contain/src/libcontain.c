/* contain:
 * https://github.com/arachsys/containers 
 * Copyright (C) 2013 Chris Webb <chris@arachsys.com>
 * see COPYING within this directory
 */

/* Adapted with adjustments specific for this environment: 
 * - get the terminal rows and columns and reset them when we switch to new root
 * - bind user's data directory to /home/user, and the sources directory from the
 *   distribution to /src
 * - librarize it and [re]move symbols with static state into a struct, that is
 *   passing around
 * - propagate runtime errors all the way back to the caller, as we do not want
 *   to exit back to the system
 *
 * Many, many thanks.
 */

#define LIBRARY "Contain"

#define _GNU_SOURCE

#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_STDARG
#define REQUIRE_POLL
#define REQUIRE_FCNTL
#define REQUIRE_SCHED
#define REQUIRE_SIGNAL
#define REQUIRE_LIMITS
#define REQUIRE_SYSCALL
#define REQUIRE_TERMIOS
#define REQUIRE_GRP
#define REQUIRE_SYS_IOCTL
#define REQUIRE_SYS_MOUNT
#define REQUIRE_SYS_MMAN
#define REQUIRE_SYS_WAIT
#define REQUIRE_SYS_STAT
#define REQUIRE_SYS_PRCTL
#define REQUIRE_SYS_TYPES
#define REQUIRE_SYS_SENDFILE
#define REQUIRE_SYS_SIGNALFD

#define REQUIRE_OS_TYPE       DECLARE
#define REQUIRE_DIR_TYPE      DECLARE
#define REQUIRE_FILE_TYPE     DECLARE
#define REQUIRE_PATH_TYPE     DECLARE
#define REQUIRE_ERROR_TYPE    DECLARE
#define REQUIRE_TERM_TYPE     DECLARE
#define REQUIRE_CSTRING_TYPE  DECLARE
#define REQUIRE_STRING_TYPE   DONOT_DECLARE
#define REQUIRE_VSTRING_TYPE  DONOT_DECLARE
#define REQUIRE_CONTAIN_TYPE  DONOT_DECLARE

#include <z/cenv.h>

#define MAXLEN_ERR 255
#define MAXLEN_LOGIN_NAME LOGIN_NAME_MAX
#define MAXLEN_GROUP_NAME MAXLEN_LOGIN_NAME

struct contain_t {
  char
    err_msg[MAXLEN_ERR],
    user[MAXLEN_LOGIN_NAME],
    group[MAXLEN_GROUP_NAME],
    **argv,
    *tmpDir,
    *gidmap,
    *uidmap,
    *rootDir;

  int
    err,
    rows,
    cols,
    masterFd;

  uid_t uid;
  gid_t gid;

  pid_t
    childPid,
    parentPid;

  term_t *term;
};

#define RETURN_NOTOK_IF(__s__, __v__, __e__, __fmt__, ...)   \
  do {                                                       \
    if (__v__) {                                             \
      contain_err (__s__, __e__, __fmt__, __VA_ARGS__);      \
      return NOTOK;                                          \
    }                                                        \
  } while (0)

#define RETURN_NULL_IF(__s__, __v__, __e__, __fmt__, ...)    \
  do {                                                       \
    if (__v__) {                                             \
      contain_err (__s__, __e__, __fmt__, __VA_ARGS__);      \
      return NULL;                                           \
    }                                                        \
  } while (0)

#define GID 0
#define UID 1
#define INVALID ((unsigned) -1)
#define SHELL "/bin/zs-static"

#define GETID(type) ((unsigned) ((type) == GID ? cnt->gid : cnt->uid))
#define IDFILE(type) ((type) == GID ? "gid_map" : "uid_map")
#define IDNAME(type) ((type) == GID ? "GID" : "UID")
#define SUBPATH(type) ((type) == GID ? "/etc/subgid" : "/etc/subuid")

static void contain_err (contain_t *cnt, int errnum, const char *format, ...) {
  va_list args;

  va_start(args, format);
  int n = vsnprintf (cnt->err_msg, MAXLEN_ERR + 1, format, args);
  va_end(args);

  if (errnum != 0) {
    char *err = Error.errno_string (errnum);
    Cstring.cp_fmt (cnt->err_msg + n, MAXLEN_ERR - n + 1, ": %s", err);
  }

  cnt->err = NOTOK;
}

static char *string_new (contain_t *cnt, const char *format, ...) {
  char *result;
  va_list args;
  va_start(args, format);
  RETURN_NULL_IF(cnt, vasprintf (&result, format, args) < 0, errno, "vasprintf", "");
  va_end(args);
  return result;
}

static char *string_append (contain_t *cnt, char **destination, const char *format, ...) {
  char *extra, *result;
  va_list args;
  va_start(args, format);
  RETURN_NULL_IF(cnt, vasprintf (&extra, format, args) < 0, errno, "vasprintf", "");
  va_end(args);

  if (*destination == NULL) {
    *destination = extra;
    return extra;
  }

  RETURN_NULL_IF(cnt, asprintf (&result, "%s%s", *destination, extra) < 0, errno, "asprintf", "");

  free (*destination);
  free (extra);
  *destination = result;
  return result;
}

static int denysetgroups (contain_t *cnt, pid_t pid) {
  const char *text = "deny";

  char *path = string_new (cnt, "/proc/%d/setgroups", pid);
  int fd = open (path, O_WRONLY);
  RETURN_NOTOK_IF(cnt, fd < 0, errno, "Failed to disable setgroups() in container", "");

  size_t len = bytelen (text);
  int retval = write (fd, text, len) == (ssize_t) len;
  RETURN_NOTOK_IF(cnt, retval == 0, errno, "Failed to disable setgroups() in container", "");

  close (fd);
  free (path);
  return OK;
}

static char *getmap (contain_t *cnt, pid_t pid, int type) {
  char *line = NULL, *result = NULL, *path;
  size_t size;
  unsigned count, first, lower;
  FILE *file;

  if (pid == -1)
    path = string_new (cnt, "/proc/self/%s", IDFILE(type));
  else
    path = string_new (cnt, "/proc/%d/%s", pid, IDFILE(type));

  if (NULL is path) return NULL;

  file = fopen (path, "r");
  RETURN_NULL_IF(cnt, file == NULL, errno, "Cannot read %s", path);

  while (getline (&line, &size, file) >= 0) {
    if (sscanf (line, " %u %u %u", &first, &lower, &count) != 3)
      RETURN_NULL_IF(cnt, 1, 0, "Invalid map data in %s", path);

    if (NULL == string_append (cnt, &result, "%s%u:%u:%u", result ? "," : "", first, lower, count)) return NULL;
  }

  RETURN_NULL_IF(cnt, result == NULL, 0, "Invalid map data in %s", path);

  fclose (file);
  free (line);
  free (path);
  return result;
}

static char *mapitem (contain_t *cnt, char *map, unsigned *first, unsigned *lower,
    unsigned *count) {
  ssize_t skip;

  while (map && *map && Cstring.byte.in_str (",;", *map))
    map++;

  if (map == NULL || *map == '\0')
    return NULL;

  RETURN_NULL_IF(cnt, sscanf (map, "%u:%u:%u%zn", first, lower, count, &skip) < 3, 0, "Invalid ID map '%s'", map);

  return map + skip;
}

static char *rangeitem (contain_t *cnt, char *range, unsigned *start, unsigned *length) {
  ssize_t skip;

  while (range && *range && Cstring.byte.in_str (",;", *range))
    range++;

  if (range == NULL || *range == '\0')
    return NULL;

  if (sscanf (range, "%u:%u%zn", start, length, &skip) < 2)
    RETURN_NULL_IF(cnt, 1, 0, "Invalid ID range '%s'", range);

  return range + skip;
}

static char *readranges (contain_t *cnt, int type) {
  char *line = NULL, *entry, *range, *user;
  size_t end, size;

  uid_t uid;
  unsigned int length, start;
  FILE *file;

  range = string_new (cnt, "%u:1", GETID(type));
  if (!(file = fopen (SUBPATH(type), "r")))
    return range;

  uid = cnt->uid;
  user = cnt->user;

  size_t len = bytelen (user);

  while (getline (&line, &size, file) >= 0) {
    if ((uid_t) strtol (line, &entry, 10) != uid || entry == line) {
      if (strncmp (line, user, len))
        continue;

      entry = line + len;
    }

    if (sscanf (entry, ":%u:%u%zn", &start, &length, &end) < 2)
      continue;

    if (Cstring.byte.in_str (":\n", entry[end + 1]))
      if (NULL == string_append (cnt, &range, ",%u:%u", start, length)) return NULL;
  }

  if (NULL isnot line)
    free (line);

  fclose (file);
  return range;
}

static char *rootdefault (contain_t *cnt, int type) {
  char *cursor, *map, *result;
  unsigned count, first, last = INVALID, lower;

  cursor = map = getmap (cnt, -1, type);
  if (NULL == map)
    if (cnt->err == NOTOK) return NULL;

  while (1) {
    cursor = mapitem (cnt, cursor, &first, &lower, &count);
    if (NULL == cursor) {
      if (cnt->err == NOTOK) return NULL;
      break;
    }

    if (last == INVALID || last < first + count - 1)
      last = first + count - 1;
  }


  result = string_new (cnt, "0:%u:1", last);
  if (NULL is result) return NULL;

  cursor = map;
  while (1) {
    cursor = mapitem (cnt, cursor, &first, &lower, &count);
    if (NULL == cursor) {
      if (cnt->err == NOTOK) return NULL;
      break;
    }

    if (first == 0) {
      RETURN_NULL_IF (cnt, count == 1 && first >= last, 0, "No unprivileged %s available\n", IDNAME(type));

      first++, lower++, count--;
    }

    if (last <= first + count - 1 && count > 0)
      count--;

    if (count > 0)
      if (NULL == string_append (cnt, &result, "%s%u:%u:%u", result ? "," : "", first, first, count)) return NULL;
  }

  free (map);
  return result;
}

static char *userdefault (contain_t *cnt, int type) {
  char *cursor, *map, *range, *result = NULL;
  unsigned count, first, index = 0, length, lower, start;

  if (cnt->uid != 0)
    return string_new (cnt, "%u:%u:1", GETID(type), GETID(type));

  map = getmap (cnt, -1, type);
  if (NULL == map) return NULL;

  range = readranges (cnt, type);
  if (NULL == range) return NULL;

  while (1) {
    range = rangeitem (cnt, range, &start, &length);
    if (NULL == range) {
      if (cnt->err == NOTOK) return NULL;
      break;
    }

    cursor = map;
    while (1) {
      cursor = mapitem (cnt, cursor, &first, &lower, &count);
      if (NULL is cursor) {
        if (cnt->err == NOTOK)  return NULL;
        break;
      }

      if (start + length <= first || first + count <= start)
        continue;

      if (first + count < start + length)
        length = start - first + count;

      if (start < first) {
        index += first - start;
        length -= first - start;
        start = first;
      }

      if (NULL == string_append (cnt, &result, "%s%u:%u:%u", result ? "," : "", index, start, length)) return NULL;
      index += length;
    }
  }

  free (map);
  free (range);
  return result;
}

static int validate (contain_t *cnt, char *range, unsigned first, unsigned count) {
  unsigned length, start;

  while (1) {
    range = rangeitem (cnt, range, &start, &length);
    if (NULL == range) {
      if (cnt->err == NOTOK) return NOTOK;
      break;
    }

    if (first < start + length && start < first + count) {
      if (first < start)
        if (NOTOK == validate (cnt, range, first, start - first)) return NOTOK;

      if (first + count > start + length)
        if (NOTOK == validate (cnt, range, start + length, first + count - start - length)) return NOTOK;

      return OK;
    }
  }

  Cstring.cp_fmt (cnt->err_msg, MAXLEN_ERR + 1, "Cannot map onto IDs that are not delegated to you");
  return NOTOK;
}

static int verifymap (contain_t *cnt, char *map, char *range) {
  unsigned count, first, lower;

  while (1) {
    map = mapitem (cnt, map, &first, &lower, &count);
    if (NULL == map) {
      if (cnt->err == NOTOK) return NOTOK;
      break;
    }

    if (NOTOK == validate (cnt, range, lower, count)) return NOTOK;
  }

  return OK;
}

static int writemap (contain_t *cnt, pid_t pid, int type, char *map) {
  char *path, *range, *text = NULL;
  int fd;
  char *map_allocated = NULL;
  unsigned count, first, lower;

  if (!map) {
    map = (cnt->uid == 0 ? rootdefault : userdefault) (cnt, type);
    if (map is NULL) return NOTOK;
    map_allocated = map;

  } else if (cnt->uid != 0) {
    range = readranges (cnt, type);
    if (range is NULL) return NOTOK;

    verifymap (cnt, map, range);
    free (range);
  }

  while (1) {
    map = mapitem (cnt, map, &first, &lower, &count);
    if (NULL == map) {
      if (cnt->err == NOTOK) return NOTOK;
      break;
    }

    if (NULL == string_append (cnt, &text, "%u %u %u\n", first, lower, count)) return NOTOK;
  }

  path = string_new (cnt, "/proc/%d/%s", pid, IDFILE(type));
  if (path == NULL) return NOTOK;

  fd = open (path, O_WRONLY);
  RETURN_NOTOK_IF(cnt, fd < 0, errno, "Failed to set container %s map", IDNAME(type));

  size_t len = bytelen (text);
  int retval = write (fd, text, len) == (ssize_t) len;
  RETURN_NOTOK_IF(cnt, retval == 0, errno, "Failed to set container %s map", IDNAME(type));

  close (fd);
  free (path);
  free (text);
  ifnot (NULL is map_allocated)
    free (map_allocated);

  return OK;
}

static char *tmpdir (contain_t *cnt) {
  char *dir = Cstring.dup ("/tmp/XXXXXX", 11);
  RETURN_NULL_IF(cnt, mkdtemp (dir) == NULL, errno, "Failed to create temporary directory", "");
  return dir;
}

static int waitforexit (contain_t *cnt, pid_t child) {
  int status;

  RETURN_NOTOK_IF(cnt, waitpid (child, &status, 0) < 0, errno, "waitpid()", "");

  if (WEXITSTATUS(status) != EXIT_SUCCESS)
    exit (WEXITSTATUS(status));

  return OK;
}

static int waitforstop (contain_t *cnt, pid_t child) {
  int status;

  RETURN_NOTOK_IF(cnt, waitpid(child, &status, WUNTRACED) < 0, errno, "waitpid()", "");

  if (!WIFSTOPPED(status))
    exit (WEXITSTATUS(status));

  return OK;
}

static int getconsole (contain_t *cnt) {
  int null = open("/dev/null", O_RDWR);
  RETURN_NOTOK_IF(cnt, null < 0, errno, "Failed to open /dev/null", "");

  if (fcntl (STDIN_FILENO, F_GETFD) < 0)
    dup2 (null, STDIN_FILENO);

  if (fcntl (STDOUT_FILENO, F_GETFD) < 0)
    dup2 (null, STDOUT_FILENO);

  if (fcntl (STDERR_FILENO, F_GETFD) < 0)
    dup2 (null, STDERR_FILENO);

  if (null != STDIN_FILENO)
    if (null != STDOUT_FILENO)
      if (null != STDERR_FILENO)
        close (null);

  int master = posix_openpt (O_RDWR | O_NOCTTY);
  RETURN_NOTOK_IF(cnt, null < 0, errno, "Failed to allocate a console pseudo-terminal", "");

  grantpt (master);
  unlockpt (master);
  return master;
}

static int rawmode (contain_t *cnt) {
  RETURN_NOTOK_IF(cnt, isatty (STDIN_FILENO) == 0, 0, "not a controlling terminal", "");
  Term.raw_mode (cnt->term);
  return OK;
}

static void restoremode (contain_t *cnt) {
  Term.orig_mode (cnt->term);
}

static int savemode (contain_t *cnt) {
  cnt->term = Term.new ();
  Term.raw_mode (cnt->term);
  Term.init_size (cnt->term, &cnt->rows, &cnt->cols);
  Term.orig_mode (cnt->term);
  return OK;
}

static int setconsole (contain_t *cnt, const char *name) {
  setsid ();

  int console = open(name, O_RDWR);
  RETURN_NOTOK_IF(cnt, console < 0, errno, "Failed to open  console in container", "");

  ioctl (console, TIOCSCTTY, NULL);

  struct termios termios;

  int retval =  tcgetattr (console, &termios);
  RETURN_NOTOK_IF(cnt, retval < 0, errno, "tcgetattr()", "");

  termios.c_iflag |= IGNBRK | IUTF8;
  tcsetattr (console, TCSANOW, &termios);

  RETURN_NOTOK_IF(cnt, -1 == dup2 (console, STDIN_FILENO), errno, "failed to dup2() stdin fileno", "");
  RETURN_NOTOK_IF(cnt, -1 == dup2 (console, STDOUT_FILENO), errno, "failed to dup2() stdout fileno", "");
  RETURN_NOTOK_IF(cnt, -1 == dup2 (console, STDERR_FILENO), errno, "failed to dup2() stderr fileno", "");

  if (console != STDIN_FILENO)
    if (console != STDOUT_FILENO)
      if (console != STDERR_FILENO)
        close (console);

  return OK;
}

static int supervise (contain_t *cnt) {
  char buffer[PIPE_BUF];
  int signals, slave, status;
  sigset_t mask;
  ssize_t count, length, offset;
  struct pollfd fds[3];

  if (cnt->masterFd < 0) {
    RETURN_NOTOK_IF(cnt, waitpid (cnt->childPid, &status, 0) < 0, errno, "waitpid", "");

    return WIFEXITED(status) ? WEXITSTATUS(status) : EXIT_FAILURE;
  }

  sigemptyset (&mask);
  sigaddset (&mask, SIGCHLD);
  sigprocmask (SIG_BLOCK, &mask, NULL);

  signals = signalfd (-1, &mask, 0);
  RETURN_NOTOK_IF(cnt, signals < 0, errno, "signalfd", "");

  if (waitpid (cnt->childPid, &status, WNOHANG) > 0)
    if (WIFEXITED(status) || WIFSIGNALED(status))
      raise (SIGCHLD);

  if (NOTOK is savemode (cnt)) return NOTOK;

  if (NOTOK is rawmode (cnt)) return NOTOK;

  char ptsnm[512];
  int retval = ptsname_r (cnt->masterFd, ptsnm, 512);
  RETURN_NOTOK_IF(cnt, retval != 0, errno, "ptsname_r()", "");

  slave = open (ptsnm, O_RDWR);
  RETURN_NOTOK_IF(cnt, slave < 0, errno, "couldn't open slave FD", "");

  struct winsize wsiz;
  wsiz.ws_row = cnt->rows;
  wsiz.ws_col = cnt->cols;
  wsiz.ws_xpixel = 0;
  wsiz.ws_ypixel = 0;
  retval = ioctl (slave, TIOCSWINSZ, &wsiz);
  RETURN_NOTOK_IF(cnt, retval == -1, errno, "ioctl()", "");

  fds[0].fd = cnt->masterFd;
  fds[0].events = POLLIN;
  fds[1].fd = STDIN_FILENO;
  fds[1].events = POLLIN;
  fds[2].fd = signals;
  fds[2].events = POLLIN;

  while (1) {
    if (poll (fds, 3, -1) < 0)
      RETURN_NOTOK_IF(cnt, errno != EAGAIN && errno != EINTR, errno, "poll()", "");

    if (fds[0].revents & POLLIN) {
      if ((length = read (cnt->masterFd, buffer, sizeof(buffer))) < 0)
        RETURN_NOTOK_IF(cnt, errno != EAGAIN && errno != EINTR, errno, "read()", "");

      for (offset = 0; length > 0; offset += count, length -= count)
        while ((count = write (STDOUT_FILENO, buffer + offset, length)) < 0)
          RETURN_NOTOK_IF(cnt, errno != EAGAIN && errno != EINTR, errno, "write()", "");
    }

    if (fds[1].revents & (POLLHUP | POLLIN)) {
      if ((length = read (STDIN_FILENO, buffer, sizeof(buffer))) == 0)
        fds[1].events = 0;
      else if (length < 0)
        RETURN_NOTOK_IF(cnt, errno != EAGAIN && errno != EINTR, errno, "read()", "");

      for (offset = 0; length > 0; offset += count, length -= count)
        while ((count = write (cnt->masterFd, buffer + offset, length)) < 0)
          RETURN_NOTOK_IF(cnt, errno != EAGAIN && errno != EINTR, errno, "write()", "");
    }

    if (fds[2].revents & POLLIN) {
      if (read (signals, buffer, sizeof(buffer)) < 0)
        RETURN_NOTOK_IF(cnt, errno != EAGAIN && errno != EINTR, errno, "read()", "");

      if (waitpid (cnt->childPid, &status, WNOHANG) > 0)
        if (WIFEXITED(status) || WIFSIGNALED(status))
          break;
    }
  }

  close (signals);
  close (slave);

  while ((length = read (cnt->masterFd, buffer, sizeof(buffer)))) {
    if (length < 0 && errno != EAGAIN && errno != EINTR)
      break;
    for (offset = 0; length > 0; offset += count, length -= count)
      while ((count = write (STDOUT_FILENO, buffer + offset, length)) < 0)
        RETURN_NOTOK_IF(cnt, errno != EAGAIN && errno != EINTR, errno, "write()", "");
  }

  restoremode (cnt);

  return WIFEXITED(status) ? WEXITSTATUS(status) : EXIT_FAILURE;
}

static int bindnode (contain_t *cnt, const char *src, const char *dst) {
  int fd = open (dst, O_WRONLY | O_CREAT, 0600);
  if (fd >= 0)  close (fd);

  int retval = mount (src, dst, NULL, MS_BIND, NULL);
  RETURN_NOTOK_IF(cnt, retval < 0, errno, "Failed to bind %s into new /dev filesystem", src);

  return OK;
}

static int createroot (contain_t *cnt) {
  mode_t mask;

  cnt->tmpDir = tmpdir (cnt);

  int retval = mount (cnt->rootDir, cnt->tmpDir, NULL, MS_BIND | MS_REC, NULL);

  RETURN_NOTOK_IF(cnt, retval < 0, errno, "Failed to bind new root filesystem", "");

  retval = chdir (cnt->tmpDir);
  RETURN_NOTOK_IF(cnt, retval isnot 0, errno, "Failed to enter new root filesystem", "");

  mask = umask (0);

  if (File.exists ("dev")) {
    RETURN_NOTOK_IF(cnt, Dir.is_directory ("dev") == 0, 0, "/dev isnot a directory", "");
  } else {
    retval = mkdir ("dev", 0755);
    RETURN_NOTOK_IF(cnt, retval isnot 0, errno, "Failed to create /dev directory", "");
  }

  retval = mount ("tmpfs", "dev", "tmpfs", 0, "mode=0755");
  RETURN_NOTOK_IF(cnt, retval isnot 0, errno, "Failed to mount /dev tmpfs in new root filesystem", "");

  retval = mkdir ("dev/pts", 0755);
  RETURN_NOTOK_IF(cnt, retval isnot 0, errno, "Failed to make /dev/pts directory", "");

  retval = mount ("devpts", "dev/pts", "devpts", 0, "newinstance,ptmxmode=666");
  RETURN_NOTOK_IF(cnt, retval isnot 0, errno, "Failed to mount /dev/pts in new root filesystem", "");

  retval = mkdir ("dev/shm", 0755);
  RETURN_NOTOK_IF(cnt, retval isnot 0, errno, "Failed to make /dev/shm directory", "");

  retval = mount ("tmpfs", "dev/shm", "tmpfs", MS_NOEXEC|MS_NOSUID|MS_RELATIME, NULL);
  RETURN_NOTOK_IF(cnt, retval isnot 0, errno, "Failed to mount /dev/shm in new root filesystem", "");

  retval = mkdir ("dev/tmp", 0755);
  RETURN_NOTOK_IF(cnt, retval isnot 0, errno, "Failed to make /dev/tmp directory", "");

  if (File.exists ("src")) {
    RETURN_NOTOK_IF(cnt, Dir.is_directory ("src") == 0, 0, "/src isnot a directory", "");
  } else {
    retval = mkdir ("src", 0755);
    RETURN_NOTOK_IF(cnt, retval isnot 0, errno, "Failed to create /src directory", "");
  }

  if (File.exists ("home")) {
    RETURN_NOTOK_IF(cnt, Dir.is_directory ("home") == 0, 0, "/home isnot a directory", "");
  } else {
    retval = mkdir ("home", 0755);
    RETURN_NOTOK_IF(cnt, retval isnot 0, errno, "Failed to create /home directory", "");
  }

  char home[MAXLEN_BUF];
  STRING_FMT(home, MAXLEN_BUF, "home/%s", cnt->user);

  if (File.exists (home)) {
    RETURN_NOTOK_IF(cnt, Dir.is_directory (home) == 0, 0, "%s isnot a directory", home);
  } else {
    retval = mkdir (home, 0755);
    RETURN_NOTOK_IF(cnt, retval isnot 0, errno, "Failed to create %s directory", home);
  }

  if (File.exists ("tmp")) {
    RETURN_NOTOK_IF(cnt, Dir.is_directory ("tmp") == 0, 0, "/tmp isnot a directory", cnt->user);
  } else {
    retval = mkdir ("tmp", 1777);
    RETURN_NOTOK_IF(cnt, retval isnot 0, errno, "Failed to create /tmp directory", cnt->user);
  }

  umask (mask);

  char ptsnm[512];
  retval = ptsname_r (cnt->masterFd, ptsnm, 512);
  RETURN_NOTOK_IF(cnt, retval != 0, errno, "ptsname_r()", "");

  if (NOTOK == bindnode (cnt, ptsnm, "dev/console")) return NOTOK;
  if (NOTOK == bindnode (cnt, "/dev/full", "dev/full")) return NOTOK;
  if (NOTOK == bindnode (cnt, "/dev/null", "dev/null")) return NOTOK;
  if (NOTOK == bindnode (cnt, "/dev/random", "dev/random")) return NOTOK;
  if (NOTOK == bindnode (cnt, "/dev/tty", "dev/tty")) return NOTOK;
  if (NOTOK == bindnode (cnt, "/dev/urandom", "dev/urandom")) return NOTOK;
  if (NOTOK == bindnode (cnt, "/dev/zero", "dev/zero")) return NOTOK;
  if (NOTOK == bindnode (cnt, SRCDIR, "src")) return NOTOK;
  if (NOTOK == bindnode (cnt, DATADIR, home)) return NOTOK;

  retval = symlink ("pts/ptmx", "dev/ptmx");
  RETURN_NOTOK_IF(cnt, retval != 0, errno, "Failed to creates symbolic link pts/ptmx -> dev/ptmx", "");

  return OK;
}

static int enterroot (contain_t *cnt) {
  int retval = syscall (__NR_pivot_root, ".", "dev/tmp");
  RETURN_NOTOK_IF(cnt, retval < 0, errno, "Failed to pivot into new root filesystem", "");

  if (chdir ("/dev/tmp") >= 0) {
    while (*cnt->tmpDir == '/')
      cnt->tmpDir++;

    rmdir (cnt->tmpDir);
  }

  cnt->tmpDir = NULL;

  retval = chdir ("/");
  RETURN_NOTOK_IF(cnt, retval < 0, errno, "Failed to detach old root filesystem", "");

  retval = umount2 ("/dev/tmp", MNT_DETACH);
  RETURN_NOTOK_IF(cnt, retval < 0, errno, "Failed to detach old root filesystem", "");

  rmdir ("/dev/tmp");
  return OK;
}

static int mountproc (contain_t *cnt) {
  int retval;

  mode_t mask = umask (0);

  if (File.exists ("proc")) {
    RETURN_NOTOK_IF(cnt, Dir.is_directory ("proc") == 0, 0, "/proc isnot a directory", "");
  } else {
    retval = mkdir ("proc", 0755);
    RETURN_NOTOK_IF(cnt, retval isnot 0, errno, "Failed to create /proc directory", "");
  }

  umask (mask);

  retval = mount ("proc", "proc", "proc", 0, NULL);
  RETURN_NOTOK_IF(cnt, retval isnot 0, errno, "Failed to mount /proc in new root filesystem", "");

  return OK;
}

static int mountsys (contain_t *cnt) {
  int retval;
  mode_t mask = umask (0);

  if (File.exists ("sys")) {
    RETURN_NOTOK_IF(cnt, Dir.is_directory ("sys") == 0, 0, "/sys isnot a directory", "");
  } else {
    retval = mkdir ("sys", 0755);
    RETURN_NOTOK_IF(cnt, retval isnot 0, errno, "Failed to create /sys directory", "");
  }

  umask (mask);

  retval = mount ("sysfs", "sys", "sysfs", 0, NULL);
  RETURN_NOTOK_IF(cnt, retval isnot 0, errno, "Failed to mount /sys in new root filesystem", "");

  retval = mount ("cgroup2", "sys/fs/cgroup", "cgroup2", 0, NULL);
  RETURN_NOTOK_IF(cnt, retval isnot 0, errno, "Failed to mount /sys/fs/cgroup in new root filesystem", "");

  return OK;
}

static void contain_release (contain_t **cnt_p) {
  if (NULL is *cnt_p) return;

  contain_t *cnt = *cnt_p;

  if (cnt->tmpDir) {
    umount2 (cnt->tmpDir, MNT_DETACH);
    rmdir (cnt->tmpDir);
    free (cnt->tmpDir);
  }

  ifnot (NULL is cnt->uidmap) free (cnt->uidmap);
  ifnot (NULL is cnt->gidmap) free (cnt->gidmap);
  ifnot (NULL is cnt->rootDir) free (cnt->rootDir);
  ifnot (NULL is cnt->term) Term.release (&cnt->term);

  free (cnt);

  *cnt_p = NULL;
}

static contain_t *contain_new (void) {
  contain_t *cnt = Alloc (sizeof (contain_t));
  cnt->rootDir = NULL;
  cnt->argv = NULL;
  cnt->tmpDir = NULL;
  cnt->uidmap = NULL;
  cnt->gidmap = NULL;
  cnt->term = NULL;
  cnt->err = OK;
  cnt->rows = 0;
  cnt->cols = 0;
  cnt->masterFd = -1;
  cnt->childPid = -1;
  cnt->parentPid = getpid ();
  cnt->uid = getuid ();
  cnt->gid = getgid ();

  char *user = getenv ("USER");
  user = user ? user : getenv ("LOGNAME");
  user = user ? user : getlogin ();

  if (user) {
    int u = OS.get.pwuid (user);
    if (-1 == u or cnt->uid != (uid_t) u) {
      contain_release (&cnt);
      return NULL;
    }

    Cstring.cp (cnt->user, MAXLEN_LOGIN_NAME, user, bytelen (user));

  } else {
    char *pwnm = OS.get.pwname (cnt->uid);
    if (NULL is pwnm) {
      contain_release (&cnt);
      return NULL;
    }

    Cstring.cp (cnt->user, MAXLEN_LOGIN_NAME, pwnm, bytelen (pwnm));
    free (pwnm);
  }

  char *group = getenv ("GROUP");

  if (group) {
    int g = OS.get.grgid (group);
    if (-1 == g or cnt->gid != (gid_t) g) {
      contain_release (&cnt);
      return NULL;
    }

    Cstring.cp (cnt->group, MAXLEN_GROUP_NAME, group, bytelen (group));

  } else {
    char *grnm = OS.get.grname (cnt->gid);
    if (NULL is grnm) {
      contain_release (&cnt);
      return NULL;
    }

    Cstring.cp (cnt->group, MAXLEN_GROUP_NAME, grnm, bytelen (grnm));
    free (grnm);
  }

  return cnt;
}

static char *contain_set_rootDir (contain_t *cnt, char *dir) {
  if (cnt is NULL) return NULL;
  if (dir is NULL) return NULL;

  char rpath[MAXLEN_PATH];

  char *p = Path.real (dir, rpath);
  if (p is NULL) {
    fprintf (stderr, "err %s\n", Error.errno_string (errno));
    return NULL;
  }

  size_t len = bytelen (p);
  ifnot (len) return NULL;

  if (cnt->rootDir isnot NULL)
    free (cnt->rootDir);

  cnt->rootDir = Alloc (len + 1);
  Cstring.cp (cnt->rootDir, len + 1, p, len);
  return cnt->rootDir;
}

static int contain_set_argv (contain_t *cnt, char **argv) {
  if (cnt is NULL) return NOTOK;
  if (argv is NULL) return NOTOK;
  cnt->argv = argv;
  return OK;
}

static int contain_set_uidmap (contain_t *cnt, char *uidmap) {
  if (cnt is NULL) return NOTOK;
  if (uidmap is NULL) return NOTOK;
  size_t len = bytelen (uidmap);
  ifnot (len) return NOTOK;
  if (cnt->uidmap isnot NULL)
    free (cnt->uidmap);

  cnt->uidmap = Alloc (len + 1);
  Cstring.cp (cnt->uidmap, len + 1, uidmap, len);
  return OK;
}

static int contain_set_gidmap (contain_t *cnt, char *gidmap) {
  if (cnt is NULL) return NOTOK;
  if (gidmap is NULL) return NOTOK;
  size_t len = bytelen (gidmap);
  ifnot (len) return NOTOK;
  if (cnt->gidmap isnot NULL)
    free (cnt->gidmap);

  cnt->gidmap = Alloc (len + 1);
  Cstring.cp (cnt->gidmap, len + 1, gidmap, len);
  return OK;
}

static char *contain_get_err_msg (contain_t *cnt) {
  return cnt->err_msg;
}

static int contain_run (contain_t *cnt) {
  int hostnet = 0;

  switch (cnt->childPid = fork ()) {
    case -1:
      RETURN_NOTOK_IF(cnt, 1, errno, "fork error", "");
      // fall through
    case 0:
      raise (SIGSTOP);
      if (cnt->uid != 0)
        if (NOTOK == denysetgroups (cnt, cnt->parentPid)) return NOTOK;

      if (NOTOK == writemap (cnt, cnt->parentPid, GID, cnt->gidmap)) return NOTOK;
      if (NOTOK == writemap (cnt, cnt->parentPid, UID, cnt->uidmap)) return NOTOK;

      exit (EXIT_SUCCESS);
  }

  if (setgid (cnt->gid) < 0 || setuid (cnt->uid) < 0)
    RETURN_NOTOK_IF(cnt, 1, errno, "Failed to drop privileges", "");

  prctl (PR_SET_DUMPABLE, 1);

  if (unshare (CLONE_NEWUSER) < 0)
    RETURN_NOTOK_IF(cnt, 1, 0, "Failed to unshare user namespace", "");

#ifdef CLONE_NEWCGROUP
  if (unshare (CLONE_NEWCGROUP) < 0)
    RETURN_NOTOK_IF(cnt, 1, 0, "Failed to unshare cgroup namespace", "");
#endif

  if (unshare (CLONE_NEWIPC) < 0)
    RETURN_NOTOK_IF(cnt, 1, 0, "Failed to unshare IPC namespace", "");

  if (!hostnet && unshare (CLONE_NEWNET) < 0)
    RETURN_NOTOK_IF(cnt, 1, 0, "Failed to unshare network namespace", "");

  if (unshare (CLONE_NEWNS) < 0)
    RETURN_NOTOK_IF(cnt, 1, 0, "Failed to unshare mount namespace", "");

#ifdef CLONE_NEWTIME
// this fails with kernel < 6 and recent glibc
  if (unshare (CLONE_NEWTIME) < 0)
    RETURN_NOTOK_IF(cnt, 1, 0, "Failed to unshare time namespace: %s", Error.errno_string (errno));
#endif

  if (unshare (CLONE_NEWUTS) < 0)
    RETURN_NOTOK_IF(cnt, 1, 0, "Failed to unshare UTS namespace", "");

  if (NOTOK == waitforstop (cnt, cnt->childPid)) return NOTOK;
  RETURN_NOTOK_IF(cnt, kill (cnt->childPid, SIGCONT) == -1, errno, "kill()", "");
  if (NOTOK == waitforexit (cnt, cnt->childPid)) return NOTOK;

  ifnot (cnt->uid) {
    RETURN_NOTOK_IF(cnt, setgid (0) == -1, errno, "setgid()", "");
    RETURN_NOTOK_IF(cnt, setgroups (0, NULL) == -1, errno, "setgroups()", "");
    RETURN_NOTOK_IF(cnt, setuid (0) == -1, errno, "setuid()", "");
  }

  cnt->masterFd = getconsole (cnt);
  if (cnt->masterFd == NOTOK) return NOTOK;

  if (createroot (cnt) == NOTOK) return NOTOK;

  unshare (CLONE_NEWPID);

  switch (cnt->childPid = fork ()) {
    case -1:
      RETURN_NOTOK_IF(cnt, 1, errno, "fork error", "");
      return NOTOK;

    case 0:
      if (NOTOK == mountproc (cnt)) return NOTOK;

      if (!hostnet)
        if (NOTOK == mountsys (cnt)) return NOTOK;

      if (NOTOK == enterroot (cnt)) return NOTOK;

      if (cnt->masterFd >= 0) {
        close (cnt->masterFd);
        if (NOTOK == setconsole (cnt, "/dev/console")) return NOTOK;
      }

      int retval = mount ("tmpfs", "tmp", "tmpfs", 0, "mode=1755");
      RETURN_NOTOK_IF(cnt, retval isnot 0, errno, "Failed to mount /tmp tmpfs in new root filesystem", "");

      clearenv ();

      putenv ((char *)"CONTAINER=1");
      putenv ((char *)"LD_LIBRARY_PATH=/lib:/lib/z");
      putenv ((char *)"TMPDIR=/tmp");
      putenv ((char *)"PATH=/bin");
      putenv ((char *)"SRCDIR=/src");
      putenv ((char *)"SYSDIR=/");

      char buf[MAXLEN_BUF];
      putenv (STRING_FMT(buf, MAXLEN_BUF, "HOME=/home/%s", cnt->user));
      putenv (STRING_FMT(buf, MAXLEN_BUF, "DATADIR=/home/%s", cnt->user));
      putenv (STRING_FMT(buf, MAXLEN_BUF, "USERNAME=%s", cnt->user));
      putenv (STRING_FMT(buf, MAXLEN_BUF, "GROUPNAME=%s", cnt->group));

      if (cnt->argv) {
        execv (cnt->argv[0], cnt->argv);
        fprintf (stderr, "execv(): %s %s\n", cnt->argv[0], Error.errno_string (errno));
      } else {
        execl (SHELL, SHELL, NULL);
        fprintf (stderr, "execl(): %s %s\n", SHELL, Error.errno_string (errno));
      }

      exit (1);
  }

  return supervise (cnt);
}

public contain_T __init_contain__ (void) {
  __INIT__(os);
  __INIT__(dir);
  __INIT__(path);
  __INIT__(file);
  __INIT__(error);
  __INIT__(term);
  __INIT__(cstring);

  return (contain_T) {
    .self = (contain_self) {
      .new = contain_new,
      .run = contain_run,
      .release = contain_release,
      .set = (contain_set_self) {
        .argv = contain_set_argv,
        .uidmap = contain_set_uidmap,
        .gidmap = contain_set_gidmap,
        .rootDir = contain_set_rootDir
      },
      .get = (contain_get_self) {
        .err_msg = contain_get_err_msg
      }
    }
  };
}

public void __deinit_contain__ (contain_T **thisp) {
  (void) thisp;
  return;
}
