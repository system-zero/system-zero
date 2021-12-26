#ifndef PROC_HDR
#define PROC_HDR

typedef struct proc_t proc_t;
typedef struct Proc_t Proc_t;

typedef int (*ProcRead_cb) (proc_t *, FILE *, FILE *);
typedef int (*ProcAtFork_cb) (proc_t *);
typedef int (*ProcPreFork_cb) (proc_t *);
typedef int (*ProcPipe_cb) (proc_t *, char *, string **);

#define PROC_READ_STDOUT (1 << 1)
#define PROC_READ_STDERR (1 << 2)

typedef struct proc_get_self {
  pid_t (*pid) (proc_t *);

  proc_t
    *(*next) (proc_t *);

  char **(*argv) (proc_t *);

  void *(*user_data) (proc_t *);
  ProcPipe_cb (*pipe_cb) (proc_t *);
} proc_get_self;

typedef struct proc_unset_self {
  void
    (*stdin) (proc_t *);
} proc_unset_self;

typedef struct proc_set_self {
  void
    (*argv) (proc_t *, int, const char **),
    (*prev) (proc_t *, proc_t *),
    (*next) (proc_t *, proc_t *),
    (*stdin) (proc_t *, const char *, size_t),
    (*dup_stdin) (proc_t *, int),
    (*at_fork_cb) (proc_t *, ProcAtFork_cb),
    (*pre_fork_cb) (proc_t *, ProcPreFork_cb),
    (*read_stream_cb) (proc_t *, int, ProcRead_cb),
    (*user_data) (proc_t *, void *),
    (*pipe_cb) (proc_t *, ProcPipe_cb);
} proc_set_self;

typedef struct proc_self {
  proc_get_self get;
  proc_set_self set;
  proc_unset_self unset;

  proc_t *(*new) (void);

  void
    (*add_env) (proc_t *, const char *, size_t),
    (*release) (proc_t *),
    (*release_argv) (proc_t *);

  char **(*parse) (proc_t *, const char *);

  int
    (*open) (proc_t *),
    (*exec) (proc_t *, const char *),
    (*read) (proc_t *);

  pid_t (*wait) (proc_t *);
} proc_self;

typedef struct proc_T {
  proc_self self;
} proc_T;

public proc_T  __init_proc__ (void);

#endif /* PROC_HDR */
