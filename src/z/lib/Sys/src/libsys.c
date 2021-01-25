#define LIBRARY "Sys"

#define REQUIRE_STDIO
#define REQUIRE_TIME

#define REQUIRE_STRING_TYPE   DECLARE
#define REQUIRE_CSTRING_TYPE  DECLARE
#define REQUIRE_VSTRING_TYPE  DONOT_DECLARE
#define REQUIRE_FILE_TYPE     DECLARE
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
}

public sys_T __init_sys__ (void) {
  __INIT__ (string);
  __INIT__ (cstring);
  __INIT__ (file);
  __INIT__ (smap);

  sys_set_env ();

  return (sys_T) {
    .self = (sys_self) {
      .which = sys_which,
      .get = (sys_get_self) {
        .env = sys_get_env,
        .clock_sec = sys_get_clock_sec
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
