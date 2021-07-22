#define LIBRARY "Sys"

#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_SYS_UNAME
#define REQUIRE_SYS_TYPES
#define REQUIRE_TIME
#define REQUIRE_PWD
#define REQUIRE_GRP

#define REQUIRE_STRING_TYPE   DECLARE
#define REQUIRE_CSTRING_TYPE  DECLARE
#define REQUIRE_VSTRING_TYPE  DECLARE
#define REQUIRE_FILE_TYPE     DECLARE
#define REQUIRE_DIR_TYPE      DECLARE
#define REQUIRE_PATH_TYPE     DECLARE
#define REQUIRE_SMAP_TYPE     DECLARE
#define REQUIRE_IO_TYPE       DECLARE
#define REQUIRE_ERROR_TYPE    DECLARE
#define REQUIRE_OS_TYPE       DECLARE
#define REQUIRE_SYS_TYPE      DONOT_DECLARE

#include <z/cenv.h>

typedef struct log_t {
  string_t *file;
  FILE *fp;
  Vstring_t *messages;
} log_t;

static log_t *__LOG__ = NULL;
static Smap_t *__ENV__ = NULL;

static sys_T __SYS__;
static int is_initialized = 0;

static string_t *sys_set_env_as (char *val, char *as, int replace) {
  if (NULL is __ENV__)
    __ENV__ = Smap.new (32);

  string_t *old = Smap.get (__ENV__, as);

  if (val is NULL)  val = "";

  if (old isnot NULL) {
    ifnot (replace)
      return old;

    String.replace_with_fmt (old, "%s=%s", as, val);
    return old;
  }

  string_t *new = String.new_with_fmt ("%s=%s", as, val);

  Smap.set (__ENV__, as, new);

  return new;
}

static string_t *sys_get_env (char *as) {
   return Smap.get (__ENV__, as);
}

static char *sys_get_env_value (char *as) {
  string_t *env = sys_get_env (as);
  if (NULL is env)
    return NULL;

  char *val = Cstring.byte.in_str (env->bytes, '=');
  if (NULL is val) return NULL;

  if (*(++val) is '\0') return NULL;

  return val;
}

static int sys_get_env_value_as_int (char *as) {
  char *val = sys_get_env_value (as);
  if (NULL is val) return 0;
  return atoi (val);
}

static long sys_get_clock_sec (clockid_t clock_id) {
  struct timespec cspec;
  clock_gettime (clock_id, &cspec);
  return cspec.tv_sec;
}

static string_t *sys_which (char *ex, char *path) {
  if (NULL is ex) return NULL;

  if (NULL is path)
    path = sys_get_env ("PATH")->bytes;

  size_t
    ex_len = bytelen (ex),
    p_len = bytelen (path);

  ifnot (ex_len and p_len) return NULL;
  char sep[2]; sep[0] = PATH_SEP; sep[1] = '\0';

  char *alpath = Cstring.dup (path, p_len);
  char *sp = strtok (alpath, sep);

  string_t *ex_path = NULL;

  while (sp) {
    size_t toklen = bytelen (sp) + 1;
    char tok[ex_len + toklen + 1];
    snprintf (tok, ex_len + toklen + 1, "%s/%s", sp, ex);
    if (File.is_executable (tok)) {
      ex_path = String.new_with_len (tok, toklen + ex_len);
      break;
    }

    sp = strtok (NULL, sep);
  }

  free (alpath);
  return ex_path;
}

static int sys_init_environment (sys_env_opts opts) {
 /* yet to be done:
  * return_a_proper error; for now return NOTOK
  * print the error? option
  * file_pointer option
  * error_string method
  * locked_after_first_set objects
  */

  pid_t pid = getpid ();
  sys_set_env_as (STR_FMT ("%d", pid), "PID", opts.overwrite);

  uid_t uid = getuid ();
  sys_set_env_as (STR_FMT ("%d", uid), "UID", opts.overwrite);

  gid_t gid = getgid ();
  sys_set_env_as (STR_FMT ("%d", gid), "GID", opts.overwrite);

  char *path = getenv ("PATH");
  if (NULL is path)
    sys_set_env_as (SYSTEM_PATH, "PATH", opts.overwrite);
  else
    sys_set_env_as (path, "PATH", opts.overwrite);

  errno = 0;

  char *pwname = OS.get.pwname (uid);
  ifnot (NULL is pwname) {
    sys_set_env_as (pwname, "USERNAME", opts.overwrite);
    free (pwname);
  } else {
    do {
      if (opts.username isnot NULL) {
        sys_set_env_as (opts.username, "USERNAME", opts.overwrite);
        break;
      }

      char *user = getenv ("USERNAME");
      ifnot  (NULL is user) {
        sys_set_env_as (user, "USERNAME", opts.overwrite);
        break;
      }

      #ifdef USERNAME
        sys_set_env_as (USERNAME, "USERNAME", opts.overwrite);
        break;
      #endif

      if (opts.return_on_error) {
        return NOTOK;
      }

      if (opts.exit_on_error) {
        Stderr.print_fmt ("Can not read passwd record|%s|%d\n",
            (0 is errno ? "" : Error.errno_string (errno)), errno);
        exit (1);
      }

    } while (0);
  }

  errno = 0;

  char *grname = OS.get.grname (gid);
  ifnot (NULL is grname) {
    sys_set_env_as (grname, "GROUPNAME", opts.overwrite);
    free (grname);
  } else {
    do {
      if (opts.groupname isnot NULL) {
        sys_set_env_as (opts.groupname, "GROUPNAME", opts.overwrite);
        break;
      }

      char *group = getenv ("GROUPNAME");
      ifnot  (NULL is group) {
        sys_set_env_as (group, "GROUPNAME", opts.overwrite);
        break;
      }

      #ifdef GROUPNAME
        sys_set_env_as (GROUPNAME, "GROUPNAME", opts.overwrite);
        break;
      #endif

      if (opts.return_on_error) {
        return NOTOK;
      }

      if (opts.exit_on_error) {
        Stderr.print_fmt ("Can not read group record%s\n",
            0 is errno ? "" : Error.errno_string (errno));
         exit (1);
      }
    } while (0);
  }

  char *hdir = getenv ("HOME");
  string_t *home = NULL;

  ifnot (NULL is hdir)
    home = sys_set_env_as (hdir, "HOME", opts.overwrite);
  else {
    do {
      if (opts.home isnot NULL) {
        home = sys_set_env_as (opts.home, "HOME", opts.overwrite);
        break;
      }

      char *hmname = OS.get.pwdir (uid);
      ifnot (NULL is hmname) {
        home = sys_set_env_as (hmname, "HOME", opts.overwrite);
        free (hmname);
        break;
      }

      #ifdef HOME
        home = sys_set_env_as (HOME, "HOME", opts.overwrite);
      #else
        home = sys_set_env_as (STR_FMT (DIR_SEP_STR "home" DIR_SEP_STR "%s", sys_get_env_value ("USERNAME")), "HOME", opts.overwrite);
      #endif
    } while (0);
  }

  String.trim_end (home, DIR_SEP);

  char *tdir = getenv ("TMPDIR");
  string_t *tmpdir = NULL;

  ifnot (NULL is tdir)
    tmpdir = sys_set_env_as (tdir, "TMPDIR", opts.overwrite);
  else {
    do {
      if (opts.tmpdir isnot NULL) {
        tmpdir = sys_set_env_as (opts.tmpdir, "TMPDIR", opts.overwrite);
        break;
      }

      #ifdef TMPDIR
        char tmp[PATH_MAX + 1];
        if (NULL is Path.real (TMPDIR, tmp))
          tmpdir = sys_set_env_as (TMPDIR, "TMPDIR", opts.overwrite);
        else
          tmpdir = sys_set_env_as (tmp, "TMPDIR", opts.overwrite);
      #else
        tmpdir = sys_set_env_as (DIR_SEP_STR "tmp", "TMPDIR", opts.overwrite);
     #endif
    } while (0);
  }

  String.trim_end (tmpdir, DIR_SEP);

  char *ddir = getenv ("DATADIR");
  string_t *datadir = NULL;

  ifnot (NULL is ddir)
    datadir = sys_set_env_as (ddir, "DATADIR", opts.overwrite);
  else {
    do {
      if (opts.datadir isnot NULL) {
        datadir = sys_set_env_as (opts.datadir, "DATADIR", opts.overwrite);
        break;
      }

      if (getenv ("CHROOT") isnot NULL) {
        datadir = sys_set_env_as (DIR_SEP_STR "data", "DATADIR", opts.overwrite);
        break;
      }

      #ifdef DATADIR
        char tmp[PATH_MAX + 1];
        if (NULL is Path.real (DATADIR, tmp))
          datadir = sys_set_env_as (DATADIR, "DATADIR", opts.overwrite);
        else
          datadir = sys_set_env_as (tmp, "DATADIR", opts.overwrite);
      #else
        datadir = sys_set_env_as (STR_FMT ("%s/.z",
          Cstring.byte.in_str (home->bytes, '=') + 1), "DATADIR", opts.overwrite);
      #endif
    } while (0);
  }

  String.trim_end (datadir, DIR_SEP);

  char *term_name = getenv ("TERM");
  ifnot (NULL is term_name)
    sys_set_env_as (term_name, "TERM", opts.overwrite);
  else {
    if (opts.termname isnot NULL)
      sys_set_env_as (opts.termname, "TERM", opts.overwrite);
    else
      sys_set_env_as ("unknown", "TERM", opts.overwrite);
  }

  struct utsname u;
  if (-1 is uname (&u))
    sys_set_env_as ("unknown", "PLATFORM", opts.overwrite);
  else
    sys_set_env_as (u.sysname, "PLATFORM", opts.overwrite);

  return OK;
}

static string_t *sys_battery_info (void) {
  string_t *info = NULL;
  char *platform = sys_get_env_value ("PLATFORM");

  if (NULL is platform) return info;

  ifnot (Cstring.eq_n ("Linux", platform, 5)) return info;

  char battery_dir[] = "/sys/class/power_supply";

  dirlist_t *dlist = Dir.list (battery_dir, 0);
  char *cap = NULL;
  char *status = NULL;

  if (NULL is dlist) return info;

  vstring_t *it = dlist->list->head;
  while (it) {
    ifnot (Cstring.cmp_n ("BAT", it->data->bytes, 3)) goto foundbat;
    it = it->next;
    }

  goto theend;

foundbat:;
  /* some maybe needless verbosity */
  char dir[64];
  snprintf (dir, 64, "%s/%s/", battery_dir, it->data->bytes);
  size_t len = bytelen (dir);
  Cstring.cp (dir + len, 64 - len, "capacity", 8);
  FILE *fp = fopen (dir, "r");
  if (NULL is fp) goto theend;

  size_t clen = 0;
  ssize_t nread = getline (&cap, &clen, fp);
  if (-1 is nread) goto theend;

  cap[nread - 1] = '\0';
  fclose (fp);

  dir[len] = '\0';
  Cstring.cp (dir + len, 64 - len, "status", 6);
  fp = fopen (dir, "r");
  if (NULL is fp) goto theend;

/* here clen it should be zero'ed because on the second call the application
 * segfaults (compiled with gcc and clang and at the first call with tcc);
 * this is when the code tries to free both char *variables arguments to getline();
 * this is as (clen) possibly interpeted as the length of the buffer
 */
  clen = 0;

  nread = getline (&status, &clen, fp);
  if (-1 is nread) goto theend;

  status[nread - 1] = '\0';
  fclose (fp);

  info = String.new_with_fmt ("Battery is %s. Remaining %s%%",
        status, cap);

theend:
  ifnot (NULL is cap) free (cap);
  ifnot (NULL is status) free (status);
  dlist->release (dlist);
  return info;
}

static void sys_release_log (void) {
  if (NULL is __LOG__) return;

  ifnot (NULL is __LOG__->file)
    String.release (__LOG__->file);

  ifnot (NULL is __LOG__->fp)
    fclose (__LOG__->fp);

  Vstring.release (__LOG__->messages);
  free (__LOG__);
  __LOG__ = NULL;
}

static void sys_log_append_message (char *msg) {
  if (NULL is __LOG__ or NULL is msg) return;

  size_t len = bytelen (msg);
  ifnot (len) return;

  ifnot (NULL is __LOG__->fp) {
    fprintf (__LOG__->fp, "%s\n", msg);
    return;
  }

  Vstring.append_with_len (__LOG__->messages, msg, len);
}

static string_t *sys_log_messages (void) {
  if (NULL is __LOG__) return NULL;

  ifnot (NULL is __LOG__->messages)
    return Vstring.join (__LOG__->messages, "\n");

  if (NULL is __LOG__->fp) return NULL;
  Vstring_t *lines = File.readlines_from_fp (__LOG__->fp, NULL, NULL, NULL);
  if (NULL is lines) return NULL;
  return Vstring.join (lines, "\n");
}

static int sys_log_init (char *file) {
  ifnot (NULL is __LOG__) {
    ifnot (NULL is file) {
      ifnot (NULL is __LOG__->file)
        String.replace_with (__LOG__->file, file);
      else
        __LOG__->file = String.new_with (file);

      ifnot (NULL is __LOG__->fp)
        fclose (__LOG__->fp);

      __LOG__->fp = fopen (__LOG__->file->bytes, "w+");
      if (NULL is __LOG__->fp) {
        sys_release_log ();
        return NOTOK;
      }
    }

    return OK;
  }

  __LOG__ = Alloc (sizeof (log_t));
  __LOG__ ->messages = NULL;
  __LOG__ ->file = NULL;
  __LOG__ ->fp = NULL;

  ifnot (NULL is file) {
    __LOG__->file = String.new_with (file);
    __LOG__->fp = fopen (__LOG__->file->bytes, "w+");
    if (NULL is __LOG__->fp) {
      sys_release_log ();
      return NOTOK;
    }
  }

  return OK;
}

public sys_T __init_sys__ (void) {
  if (is_initialized) return __SYS__;

  __INIT__ (io);
  __INIT__ (os);
  __INIT__ (dir);
  __INIT__ (path);
  __INIT__ (file);
  __INIT__ (smap);
  __INIT__ (error);
  __INIT__ (string);
  __INIT__ (cstring);
  __INIT__ (vstring);

  __SYS__ = (sys_T) {
    .self = (sys_self) {
      .which = sys_which,
      .log = (sys_log_self) {
        .init = sys_log_init,
        .messages = sys_log_messages,
        .append_message = sys_log_append_message
      },
      .init_environment = sys_init_environment,
      .get = (sys_get_self) {
        .env = sys_get_env,
        .env_value = sys_get_env_value,
        .clock_sec = sys_get_clock_sec,
        .battery_info = sys_battery_info,
        .env_value_as_int = sys_get_env_value_as_int
      },
      .set = (sys_set_self) {
        .env_as = sys_set_env_as
      }
    }
  };

  is_initialized++;
  return __SYS__;
}

public void __deinit_sys__ (void) {
  ifnot (NULL is __ENV__) {
    Smap.release (__ENV__);
    __ENV__ = NULL;
  }

  sys_release_log ();
}
