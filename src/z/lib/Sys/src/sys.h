#ifndef SYS_HDR
#define SYS_HDR

typedef struct sys_set_self {
  string_t *(*env_as) (char *, char *, int);
} sys_set_self;

typedef struct sys_get_self {
  char *(*env_value) (char *);

  string_t
    *(*env) (char *),
    *(*error_string) (int),
    *(*battery_info) (void);

  long (*clock_sec) (clockid_t);
} sys_get_self;

typedef struct sys_env_opts {
  int exit_on_error;
  int return_on_error;
  int overwrite;
  char *username;
  char *groupname;
  char *termname;
  char *home;
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
  .termname = NULL,     \
  .home = NULL,         \
  .uid = -1,            \
  .gid = -1,            \
  __VA_ARGS__ }

typedef struct sys_self {
  sys_get_self get;
  sys_set_self set;

  int (*init_environment) (sys_env_opts);
  string_t *(*which) (char *, char *);

} sys_self;

typedef struct sys_T {
  sys_self self;
} sys_T;

public sys_T __init_sys__ (void);

public void __deinit_sys__ (void);

#endif /* SYS_HDR */
