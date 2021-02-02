#define LIBRARY "Sys"

#define REQUIRE_STDIO
#define REQUIRE_TIME
#define REQUIRE_SYS_UNAME

#define REQUIRE_STRING_TYPE   DECLARE
#define REQUIRE_CSTRING_TYPE  DECLARE
#define REQUIRE_VSTRING_TYPE  DONOT_DECLARE
#define REQUIRE_FILE_TYPE     DECLARE
#define REQUIRE_DIR_TYPE      DECLARE
#define REQUIRE_SMAP_TYPE     DECLARE
#define REQUIRE_SYS_TYPE      DONOT_DECLARE

#include <z/cenv.h>

static Smap_t *__ENV__ = NULL;

static string_t *sys_set_env_as (char *val, char *as, int replace) {
  if (NULL is __ENV__)
    __ENV__ = Smap.new (2);

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
  return ++val;
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

static void sys_set_env (void) {
  char *env = getenv ("PATH");
  sys_set_env_as (env, "PATH", 1);

  env = getenv ("HOME");
  sys_set_env_as (env, "HOME", 1);

  struct utsname u;
  if (-1 is uname (&u))
    sys_set_env_as ("unknown", "PLATFORM", 1);
  else
    sys_set_env_as (u.sysname, "PLATFORM", 1);
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

public sys_T __init_sys__ (void) {
  __INIT__ (string);
  __INIT__ (cstring);
  __INIT__ (file);
  __INIT__ (smap);
  __INIT__ (dir);

  sys_set_env ();

  return (sys_T) {
    .self = (sys_self) {
      .which = sys_which,
      .get = (sys_get_self) {
        .env = sys_get_env,
        .env_value = sys_get_env_value,
        .clock_sec = sys_get_clock_sec,
        .battery_info = sys_battery_info
      },
      .set = (sys_set_self) {
        .env_as = sys_set_env_as
      }
    }
  };
}

public void __deinit_sys__ (void) {
  Smap.release (__ENV__);
}
