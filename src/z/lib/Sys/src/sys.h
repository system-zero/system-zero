#ifndef SYS_HDR
#define SYS_HDR

typedef struct sys_set_self {
  string_t *(*env_as) (char *, char *, int);
} sys_set_self;

typedef struct sys_get_self {
  string_t *(*env) (char *);
  long (*clock_sec) (clockid_t);
} sys_get_self;

typedef struct sys_self {
  sys_get_self get;
  sys_set_self set;

  string_t *(*which) (char *, char *);
} sys_self;

typedef struct sys_T {
  sys_self self;
} sys_T;

public sys_T __init_sys__ (void);

public void __deinit_sys__ (void);

#endif /* SYS_HDR */
