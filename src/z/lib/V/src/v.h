#ifndef V_HDR
#define V_HDR

enum {
  MSG_PUSH    = 0,
  MSG_ATTACH  = 1,
  MSG_DETACH  = 2,
  MSG_WINCH   = 3,
  MSG_REDRAW  = 4,
};

typedef struct v_t v_t;
typedef struct v_prop v_prop;

typedef int (*PtyOnExecChild_cb) (v_t *, int, char **);
typedef PtyOnExecChild_cb PtyMain_cb;
typedef void (*PtyAtExit_cb) (v_t *);

typedef struct v_init_opts {
  char
    *as,
    *data,
    *loadfile,
    *sockname,
    **argv;

  int
    argc,
    exit,
    force,
    attach,
    send_data,
    parse_argv,
    remove_socket,
    exit_on_no_command;

  PtyOnExecChild_cb at_exec_child;
  PtyMain_cb        at_pty_main;
} v_opts;

#define VOpts(...)         \
  (v_opts) {               \
  .as = NULL,              \
  .data = NULL,            \
  .loadfile = NULL,        \
  .sockname = NULL,        \
  .argv = NULL,            \
  .argc = 0,               \
  .exit = 0,               \
  .force = 0,              \
  .attach = 0,             \
  .send_data = 0,          \
  .parse_argv = 1,         \
  .remove_socket = 0,      \
  .at_pty_main = NULL,     \
  .at_exec_child = NULL,   \
  .exit_on_no_command = 1, \
  __VA_ARGS__              \
}

typedef struct v_init_self {
  int
    (*pty) (v_t *, char *);

  term_t
    *(*term) (v_t *, int *, int *);

  string_t *(*sockname) (v_t *, char *, char *);
} v_init_self;


typedef struct v_sock_self {
  int
    (*create) (v_t *, char *),
    (*connect) (v_t *, char *),
    (*send_data) (v_t *, int, char *, size_t, int);
} v_sock_self;

typedef struct v_pty_self {
  int (*main) (v_t *this, int, char **);
} v_pty_self;

typedef struct v_tty_self {
  int (*main) (v_t *this);
} v_tty_self;

typedef struct v_get_self {
  char *(*sockname) (v_t *);
  void *(*user_data_at) (v_t *, int);
  size_t (*sock_max_data_size) (v_t *);
  term_t *(*term) (v_t *);
} v_get_self;

typedef struct v_set_self {
  void
    (*object) (v_t *, void *, int),
    (*save_image) (v_t *, int),
    (*image_file) (v_t *, char *),
    (*image_name) (v_t *, char *),
    (*at_exit_cb) (v_t *, PtyAtExit_cb),
    (*pty_main_cb) (v_t *, PtyMain_cb),
    (*exec_child_cb) (v_t *, PtyOnExecChild_cb);

  int
    (*la_dir) (v_t *, char *),
    (*tmp_dir) (v_t *, char *),
    (*data_dir) (v_t *, char *),
    (*current_dir) (v_t *, char *, int);
} v_set_self;

typedef struct v_unset_self {
  void
    (*tmp_dir) (v_t *),
    (*data_dir) (v_t *);
} v_unset_self;

typedef struct v_self {
  v_get_self get;
  v_set_self set;
  v_unset_self unset;
  v_pty_self  pty;
  v_tty_self  tty;
  v_sock_self sock;
  v_init_self init;

  int
    (*main) (v_t *),
    (*send) (v_t *, char *, char *),
    (*save_image) (v_t *, char *);

} v_self;

struct v_t {
  v_self self;
  v_prop *prop;
};

public v_t *__init_v__ (v_opts *);
public void __deinit_v__ (v_t **);

#endif /* V_HDR */
