#ifndef PROC_H
#define PROC_H

typedef struct proc_t proc_t;
typedef struct Proc_t Proc_t;

typedef int (*ProcRead_cb) (proc_t *, FILE *, FILE *);
typedef int (*ProcAtFork_cb) (proc_t *);
typedef int (*ProcPreFork_cb) (proc_t *);

#define PROC_READ_STDOUT (1 << 1)
#define PROC_READ_STDERR (1 << 2)

typedef struct proc_get_self {
  pid_t (*pid) (proc_t *);

  proc_t
    *(*next) (proc_t *);

  void *(*user_data) (proc_t *);
} proc_get_self;

typedef struct proc_unset_self {
  void
    (*stdin) (proc_t *);
} proc_unset_self;

typedef struct proc_set_self {
  void
    (*prev) (proc_t *, proc_t *),
    (*next) (proc_t *, proc_t *),
    (*stdin) (proc_t *, char *, size_t),
    (*dup_stdin) (proc_t *, int),
    (*at_fork_cb) (proc_t *, ProcAtFork_cb),
    (*pre_fork_cb) (proc_t *, ProcPreFork_cb),
    (*read_stream_cb) (proc_t *, int, ProcRead_cb),
    (*user_data) (proc_t *, void *);
} proc_set_self;

typedef struct proc_self {
  proc_get_self get;
  proc_set_self set;
  proc_unset_self unset;

  proc_t *(*new) (void);

  void
    (*release) (proc_t *),
    (*release_argv) (proc_t *);

  char **(*parse) (proc_t *, char *);

  int
    (*open) (proc_t *),
    (*exec) (proc_t *, char *),
    (*read) (proc_t *);

  pid_t (*wait) (proc_t *);
} proc_self;

typedef struct proc_T {
  proc_self self;
} proc_T;

public proc_T  __init_proc__ (void);

#endif /* PROC_H */
