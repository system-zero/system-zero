#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_SYS_TYPES
#define REQUIRE_SYS_SELECT
#define REQUIRE_SYS_UNAME

#define REQUIRE_VMAP_TYPE     DONOT_DECLARE
#define REQUIRE_STRING_TYPE   DECLARE
#define REQUIRE_CSTRING_TYPE  DECLARE
#define REQUIRE_OS_TYPE       DECLARE
#define REQUIRE_LA_TYPE       DECLARE

#include <z/cenv.h>

extern char **environ;

static VALUE os_environ (la_t *this) {
  (void) this;
  char **env = environ;
  int num = 0;
  while (*env isnot NULL) {
    num++;
    env++;
  }

  ArrayType *array = ARRAY_NEW(STRING_TYPE, num);
  string **ar = (string **) AS_ARRAY(array->value);

  env = environ;
  for (int i = 0; i < num; i++)
    String.replace_with (ar[i], env[i]);

  return ARRAY(array);
}

static VALUE os_getenv (la_t *this, VALUE v_e) {
  (void) this;
  ifnot (IS_STRING(v_e)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");

  string *s = AS_STRING(v_e);
  char *e = s->bytes;
  size_t len = s->num_bytes;

  char **env = environ;
  while (*env isnot NULL) {
    ifnot (Cstring.cmp_n (*env, e, len))
      if (*((*env) + len) is '=')
        goto found;
    env++;
  }

  return NULL_VALUE;

found: {}
  char *val = (*env) + len + 1;
  s = String.new_with (val);
  return STRING(s);
}

static VALUE os_setenv (la_t *this, VALUE v_as, VALUE v_val, VALUE v_overwrite) {
  (void) this;
  ifnot (IS_STRING(v_as)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_STRING(v_val)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_INT(v_overwrite)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer");

  char *as = AS_STRING_BYTES(v_as);
  char *val = AS_STRING_BYTES(v_val);
  int overwrite = AS_INT(v_overwrite);
  return INT(setenv (as, val, overwrite));
}

static VALUE os_getpid (la_t *this) {
  (void) this;
  return INT(getpid ());
}

static VALUE os_getuid (la_t *this) {
  (void) this;
  return INT(getuid ());
}

static VALUE os_getgid (la_t *this) {
  (void) this;
  return INT(getgid ());
}

static VALUE os_getgrname (la_t *this, VALUE v_gid) {
  ifnot (IS_INT(v_gid)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer");
  gid_t gid = AS_INT(v_gid);
  char *grname = OS.get.grname (gid);
  if (NULL is grname)
    return NULL_VALUE;

  string *s = String.new_with (grname);
  free (grname);
  return STRING(s);
 /* valgring reports probably wrong that the string wasn't released,
  * though it looks that it is, so pay an overhead to silence valgrind
  size_t len = bytelen (grname);
  string *v = String.new (len + 1);
  v->num_bytes = len;
  v->bytes = grname;
  return STRING(v);
  */
}

static VALUE os_getpwname (la_t *this, VALUE v_uid) {
  ifnot (IS_INT(v_uid)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer");
  uid_t uid = AS_INT(v_uid);
  char *pwname = OS.get.pwname (uid);
  if (NULL is pwname)
    return NULL_VALUE;

  /* likewise with the above */
  string *s = String.new_with (pwname);
  free (pwname);
  return STRING(s);
}

static VALUE os_getpwdir (la_t *this, VALUE v_uid) {
  ifnot (IS_INT(v_uid)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer");
  uid_t uid = AS_INT(v_uid);
  char *hmdir = OS.get.pwdir (uid);
  if (NULL is hmdir)
    return NULL_VALUE;

  /* likewise with the above */
  string *s = String.new_with (hmdir);
  free (hmdir);
  return STRING(s);
}

static VALUE os_sleep (la_t *this, VALUE v_nsecs) {
  ifnot (IS_NUMBER(v_nsecs)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a double");
  number nsecs = AS_NUMBER(v_nsecs);

  /* with a little bit of help of sltime.c at S-Lang distribution */
  unsigned long usecs;
  unsigned int secs;

  if (nsecs < 0.0)
    nsecs = 0.0;
  secs = (unsigned int) nsecs;
  sleep (secs);
  nsecs -= (double) secs;
  usecs = (unsigned long) (1e6 * nsecs);
  struct timeval tv;
  tv.tv_sec = usecs / 1000000;
  tv.tv_usec = usecs % 1000000;
  return INT(select (0, NULL, NULL, NULL, &tv));
}

static VALUE os_platform (la_t *this) {
  (void) this;
  struct utsname u;
  string *s = String.new_with ("unknown");

  if (-1 is uname (&u))
    return STRING(s);

  String.replace_with (s, u.sysname);
  return STRING(s);
}

static VALUE os_arch (la_t *this) {
  (void) this;
  struct utsname u;
  string *s = String.new_with ("unknown");

  if (-1 is uname (&u))
    return STRING(s);

  String.replace_with (s, u.machine);
  return STRING(s);
}

#define EvalString(...) #__VA_ARGS__

public int __init_os_module__ (la_t *this) {
  __INIT_MODULE__(this);
  __INIT__(os);
  __INIT__(string);
  __INIT__(cstring);

  (void) vmapType;

  LaDefCFun lafuns[] = {
    { "sleep",          PTR(os_sleep), 1 },
    { "getpid",         PTR(os_getpid), 0 },
    { "getuid",         PTR(os_getuid), 0 },
    { "getgid",         PTR(os_getgid), 0 },
    { "getenv",         PTR(os_getenv), 1 },
    { "setenv",         PTR(os_setenv), 3 },
    { "environ",        PTR(os_environ), 0 },
    { "getpwdir",       PTR(os_getpwdir), 1 },
    { "getgrname",      PTR(os_getgrname), 1 },
    { "getpwname",      PTR(os_getpwname), 1 },
    { "os_arch",        PTR(os_arch), 0 },
    { "os_platform",    PTR(os_platform), 0 },
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Os = {
      "sleep" : sleep,
      "getpid" : getpid,
      "getuid" : getuid,
      "getgid" : getgid,
      "getenv" : getenv,
      "setenv" : setenv,
      "environ": environ,
      "getpwdir" : getpwdir,
      "getgrname" : getgrname,
      "getpwname" : getpwname,
      "arch"      : os_arch,
      "platform"  : os_platform
     }
  );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_os_module__ (la_t *this) {
  (void) this;
  return;
}
