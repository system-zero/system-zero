#ifndef I_HDR
#define I_HDR

typedef struct i_t i_t;
typedef struct i_prop i_prop;
typedef struct i_T i_T;

#define I_CFUNC(x) (((x) << 8) + 'B')

enum {
  I_ERR_TOOMANYARGS = -6,
  I_ERR_BADARGS = -5,
  I_ERR_UNKNOWN_SYM = -4,
  I_ERR_SYNTAX = -3,
  I_ERR_NOMEM = -2,
  I_NOTOK = -1,
  I_OK = 0,
  I_ERR_OK_ELSE = 1
};
typedef int (*IPrintByte_cb) (FILE *, int);
typedef int (*IPrintBytes_cb) (FILE *, const char *);
typedef int (*IPrintFmtBytes_cb) (FILE *, const char *, ...);
typedef int  (*ISyntaxError_cb) (i_t *, const char *);
typedef int  (*IDefineFuns_cb) (i_t *);

typedef intptr_t ival_t;
typedef ival_t (*Cfunc) (i_t *, ival_t, ival_t, ival_t, ival_t, ival_t, ival_t, ival_t, ival_t, ival_t);
typedef ival_t (*Opfunc) (ival_t, ival_t);

typedef struct i_opts {
  char  *name;
  char  *idir;
  int    name_gen;
  size_t mem_size;
  size_t max_script_size;
  FILE  *err_fp;
  FILE  *out_fp;

  IPrintByte_cb print_byte;
  IPrintBytes_cb print_bytes;
  IPrintFmtBytes_cb print_fmt_bytes;
  ISyntaxError_cb syntax_error;
  IDefineFuns_cb define_funs_cb;

  void *user_data;
} i_opts;

#define IOpts(...) (i_opts) { \
  .mem_size = 0,              \
  .print_byte = NULL,         \
  .print_bytes = NULL,        \
  .print_fmt_bytes = NULL,    \
  .syntax_error = NULL,       \
  .define_funs_cb = NULL,     \
  .err_fp = stderr,           \
  .out_fp = stdout,           \
  .name = NULL,               \
  .idir = NULL,               \
  .name_gen = 97,             \
  .max_script_size = 1 << 16, \
  .user_data = NULL,          \
  __VA_ARGS__}

typedef struct i_get_self {
  i_t *(*current) (i_T *);
  int (*current_idx) (i_T *);
  void *(*user_data) (i_t *);
  char *(*message) (i_t *);
} i_get_self;

typedef struct i_set_self {
  i_t *(*current) (i_T *, int);

  void
    (*idir) (i_t *, char *),
    (*user_data) (i_t *, void *),
    (*define_funs_cb) (i_t *, IDefineFuns_cb);

} i_set_self;

typedef struct i_self {
  i_get_self get;
  i_set_self set;

  void
    (*release) (i_t **),
    (*remove_instance) (i_T *, i_t *);

  i_t
    *(*new) (i_T *),
    *(*init_instance) (i_T *, i_opts),
    *(*append_instance) (i_T *, i_t *);

  int
    (*def) (i_t *, const char *, int, ival_t),
    (*init) (i_T *, i_t *, i_opts),
    (*eval_file) (i_t *, const char *),
    (*load_file) (i_T *, i_t *, char *),
    (*eval_string) (i_t *, const char *, int, int);

  ival_t
    (*print_byte) (i_t *, char),
    (*print_bytes) (i_t *, char *);

} i_self;

struct i_T {
  i_self self;
  i_prop *prop;
};

public i_T *__init_i__ (void);
public void __deinit_i__ (i_T **);
#endif /* I_HDR */
