#ifndef SYS_HDR
#define SYS_HDR

#define SYSENV_LEN  64

typedef struct sys_set_self {
  string_t *(*env_as) (const char *, const char *, int);
} sys_set_self;

typedef struct sys_get_self {
  char *(*env_value) (const char *);

  int (*env_value_as_int) (const char *);

  string_t
    *(*env) (const char *),
    *(*error_string) (int),
    *(*battery_info) (void);

  long (*clock_sec) (clockid_t);
} sys_get_self;

typedef struct sys_env_opts {
  int exit_on_error;
  int return_on_error;
  int overwrite;
  const char *username;
  const char *groupname;
  const char *termname;
  const char *home;
  const char *tmpdir;
  const char *datadir;
  const char *sysdir;
  const char *srcdir;
  int   uid;
  int   gid;
} sys_env_opts;

#define SysEnvOpts(...) \
(sys_env_opts) {        \
  .exit_on_error = 1,   \
  .return_on_error = 1, \
  .overwrite = 1,       \
  .username = NULL,     \
  .groupname = NULL,    \
  .termname = "vt100",  \
  .home = NULL,         \
  .tmpdir = NULL,       \
  .datadir = NULL,      \
  .sysdir = NULL,       \
  .srcdir = NULL,       \
  .uid = -1,            \
  .gid = -1,            \
  __VA_ARGS__ }

typedef struct sys_log_self {
  int (*init) (char *);
  void (*append_message) (const char *);
  string_t *(*messages) (void);
} sys_log_self;

typedef struct sys_self {
  sys_get_self get;
  sys_set_self set;
  sys_log_self log;

  int (*init_environment) (sys_env_opts);

  string_t *(*which) (const char *, const char *);

} sys_self;

typedef struct sys_T {
  sys_self self;
} sys_T;

public sys_T __init_sys__ (void);

public void __deinit_sys__ (void);

#endif /* SYS_HDR */
