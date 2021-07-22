#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_SYS_TYPES

#define REQUIRE_VMAP_TYPE     DONOT_DECLARE
#define REQUIRE_STRING_TYPE   DECLARE
#define REQUIRE_OS_TYPE       DECLARE
#define REQUIRE_LA_TYPE       DECLARE

#include <z/cenv.h>

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

#define EvalString(...) #__VA_ARGS__

public int __init_os_module__ (la_t *this) {
  __INIT_MODULE__(this);
  __INIT__(os);
  __INIT__(string);

  (void) vmapType;

  LaDefCFun lafuns[] = {
    { "getpid",         PTR(os_getpid), 0 },
    { "getuid",         PTR(os_getuid), 0 },
    { "getgid",         PTR(os_getgid), 0 },
    { "getpwdir",       PTR(os_getpwdir), 1 },
    { "getgrname",      PTR(os_getgrname), 1 },
    { "getpwname",      PTR(os_getpwname), 1 },
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Os = {
       "getpid" : getpid,
       "getuid" : getuid,
       "getgid" : getgid,
       "getpwdir" : getpwdir,
       "getgrname" : getgrname,
       "getpwname" : getpwname
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
