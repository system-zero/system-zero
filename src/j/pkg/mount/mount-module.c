#define REQUIRE_MOUNT

#define REQUIRE_STDIO
#define REQUIRE_MODULE_COMPAT
#include <libc.h>

MODULE(mount);

#ifndef OPT_NO_FORCE
#define OPT_NO_FORCE       0
#endif
#define DEFAULT_FSTYPE "ext4"

#define GET_OPT_MOUNT_DATA() ({                                                  \
  char *_mount_data_ = NULL;                                               \
  VALUE _v_mount_data = La.get.qualifier (this, "mount_data", NULL_VALUE); \
  if (IS_STRING(_v_mount_data))                                            \
    _mount_data_ = AS_STRING_BYTES(_v_mount_data);                         \
  else if (IS_NULL(_v_mount_data) == 0)                                    \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string or null qualifier");    \
   _mount_data_;                                                           \
})

#define GET_OPT_MOUNT_FLAGS() ({                                           \
  VALUE _v_mount_flags = La.get.qualifier (this, "mount_flags", INT(0));   \
  ifnot (IS_INT(_v_mount_flags))                                           \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");          \
  AS_INT(_v_mount_flags);                                                  \
})

#define GET_OPT_NO_DEV() ({                                                \
  int _no_dev = La.qualifier_exists (this, "no_dev");                      \
  if (_no_dev) {                                                           \
    VALUE _v_no_dev = La.get.qualifier (this, "no_dev", INT(0));           \
    if (0 == IS_INT(_v_no_dev)) {                                          \
      if (IS_NULL(_v_no_dev))                                              \
        _no_dev = 1;                                                       \
      else                                                                 \
        THROW(LA_ERR_TYPE_MISMATCH, "no_dev, awaiting an integer qualifier"); \
     } else                                                                \
    _no_dev = AS_INT(_v_no_dev);                                           \
  }                                                                        \
  _no_dev;                                                                 \
})

#define GET_OPT_NO_SUID() ({                                               \
  int _no_suid = La.qualifier_exists (this, "no_suid");                    \
  if (_no_suid) {                                                          \
    VALUE _v_no_suid = La.get.qualifier (this, "no_suid", INT(0));         \
    if (0 == IS_INT(_v_no_suid)) {                                         \
      if (IS_NULL(_v_no_suid))                                             \
        _no_suid = 1;                                                      \
      else                                                                 \
        THROW(LA_ERR_TYPE_MISMATCH, "no_suid, awaiting an integer qualifier"); \
     } else                                                                \
    _no_suid = AS_INT(_v_no_suid);                                         \
  }                                                                        \
  _no_suid;                                                                \
})

#define GET_OPT_NO_EXEC() ({                                               \
  int _no_exec = La.qualifier_exists (this, "no_exec");                    \
  if (_no_exec) {                                                          \
    VALUE _v_no_exec = La.get.qualifier (this, "no_exec", INT(0));         \
    if (0 == IS_INT(_v_no_exec)) {                                         \
      if (IS_NULL(_v_no_exec))                                             \
        _no_exec = 1;                                                      \
      else                                                                 \
        THROW(LA_ERR_TYPE_MISMATCH, "no_exec, awaiting an integer qualifier"); \
     } else                                                                \
    _no_exec = AS_INT(_v_no_exec);                                         \
  }                                                                        \
  _no_exec;                                                                \
})

static VALUE system_bind (la_t *this, VALUE v_source, VALUE v_target) {
  ifnot (IS_STRING(v_source)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");         \
  ifnot (IS_STRING(v_target)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  const char *source = AS_STRING_BYTES(v_source);
  const char *target = AS_STRING_BYTES(v_target);

  La.set.Errno (this, 0);

  int r = sys_mount (source, target, NULL, MS_BIND, NULL);
  if (r is -1) La.set.Errno (this, sys_errno);

  return INT(r);
}

static VALUE system_umount (la_t *this, VALUE v_target) {
  (void) this;
  ifnot (IS_STRING(v_target)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  char *target = AS_STRING_BYTES(v_target);

  int force = GET_OPT_FORCE();

  La.set.Errno (this, 0);

  int r = sys_umount (target, force);
  if (-1 is r) La.set.Errno (this, sys_errno);

  return INT(r);
}

static VALUE system_mount (la_t *this, VALUE v_source, VALUE v_target, VALUE v_fstype) {
  (void) this;
  ifnot (IS_STRING(v_source)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");         \
  ifnot (IS_STRING(v_target)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_STRING(v_fstype))
    ifnot (IS_NULL(v_fstype)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string or null");

  char *source = AS_STRING_BYTES(v_source);
  char *target = AS_STRING_BYTES(v_target);
  const char *fstype = NULL;
  if (IS_STRING(v_fstype))
    fstype = AS_STRING_BYTES(v_fstype);
  if (fstype is NULL) fstype = DEFAULT_FSTYPE;

  void *data   = GET_OPT_MOUNT_DATA();
  ulong flags  = (ulong) GET_OPT_MOUNT_FLAGS();

  int read_only = GET_OPT_RDONLY();
  if (read_only) flags |= MS_RDONLY;
  int no_exec = GET_OPT_NO_EXEC();
  if (no_exec) flags |= MS_NOEXEC;
  int no_dev = GET_OPT_NO_DEV();
  if (no_dev) flags |= MS_NODEV;
  int no_suid = GET_OPT_NO_SUID();
  if (no_suid) flags |= MS_NOSUID;

  La.set.Errno (this, 0);

  int r = sys_mount (source, target, fstype, flags, data);
  if (-1 is r) La.set.Errno (this, sys_errno);

  return INT(r);
}

public int __init_mount_module__ (la_t *this) {
  __INIT_MODULE__(this);

  LaDefCFun lafuns[] = {
    { "system_mount",  PTR(system_mount), 3 },
    { "system_umount", PTR(system_umount), 1 },
    { "system_bind",   PTR(system_bind), 2 },
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    if is_defined ("System") {
      append system_mount in System as "mount";
      append system_bind in System as "bind";
      append system_umount in System as "umount";
    } else {
      public var System = {
        mount  : system_mount,
        bind   : system_bind,
        umount : system_umount
      };
    }
  );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_mount_module__ (la_t *this) {
  (void) this;
}
