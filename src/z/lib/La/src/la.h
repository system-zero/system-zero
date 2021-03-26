#ifndef I_HDR
#define I_HDR

typedef struct i_T i_T;
typedef struct i_t i_t;
typedef struct i_prop i_prop;
typedef struct funType funT;
typedef struct ValueType ValueType;

typedef double      number;
typedef ptrdiff_t   integer;
typedef char *      cstring;
typedef integer     memsize;
typedef integer     pointer;

#define NUMBER_TYPE    (1 << 0)
#define INTEGER_TYPE   (1 << 1)
#define CSTRING_TYPE   (1 << 2)
#define POINTER_TYPE   INTEGER_TYPE
#define MEMSIZE_TYPE   INTEGER_TYPE

struct ValueType {
  int type;

  union {
    number   asNumber;
    integer  asInteger;
    cstring  asCString;
  };
};

typedef ValueType VALUE;

#define AS_NUMBER(__v__) __v__.asNumber
#define    NUMBER(__d__) (VALUE) {.type = NUMBER_TYPE, .asNumber = __d__}

#define AS_INT(__v__) __v__.asInteger
#define    INT(__i__) (VALUE) {.type = INTEGER_TYPE, .asInteger = __i__}

#define AS_CSTRING(__v__) __v__.asCString
#define    CSTRING(__s__) (VALUE) {.type = CSTRING_TYPE, .asCString = __s__}

#define AS_PTR AS_INT
#define    PTR    INT

#define AS_MEMSIZE AS_INT
#define    MEMSIZE    INT

typedef VALUE (*Cfunc) (i_t *, VALUE, VALUE, VALUE, VALUE, VALUE, VALUE, VALUE, VALUE, VALUE);
typedef VALUE (*Opfunc) (VALUE, VALUE);

enum {
  I_ERR_OUTOFBOUNDS = -7,
  I_ERR_TOOMANYARGS = -6,
  I_ERR_BADARGS = -5,
  I_ERR_UNKNOWN_SYM = -4,
  I_ERR_SYNTAX = -3,
  I_ERR_NOMEM = -2,
  I_NOTOK = -1,
  I_OK = 0,
  I_ERR_OK_ELSE = 1,
  I_ERR_BREAK = 2,
  I_ERR_CONTINUE = 3,
  I_ERR_EXIT = 4
};

/* Interface */
typedef int (*IPrintByte_cb) (FILE *, int);
typedef int (*IPrintBytes_cb) (FILE *, const char *);
typedef int (*IPrintFmtBytes_cb) (FILE *, const char *, ...);
typedef int (*ISyntaxError_cb) (i_t *, const char *);
typedef int (*IDefineFuns_cb) (i_t *);

typedef struct i_opts {
  char  *name;
  char  *idir;
  int    name_gen;
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
  .user_data = NULL,          \
  __VA_ARGS__}

typedef struct i_get_self {
  i_t *(*current) (i_T *);
  void *(*user_data) (i_t *);
  char
    *(*eval_str) (i_t *),
    *(*message) (i_t *);
  int   (*current_idx) (i_T *);
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
    (*def) (i_t *, const char *, int, VALUE),
    (*init) (i_T *, i_t *, i_opts),
    (*eval_file) (i_t *, const char *),
    (*load_file) (i_T *, i_t *, char *),
    (*eval_string) (i_t *, const char *);

  VALUE
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
