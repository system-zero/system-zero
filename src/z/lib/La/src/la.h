#ifndef LA_HDR
#define LA_HDR

typedef struct la_T la_T;
typedef struct la_t la_t;
typedef struct la_prop la_prop;
typedef struct funType funT;
typedef struct ValueType ValueType;

typedef double      number;
typedef ptrdiff_t   integer;
typedef char *      cstring;
typedef integer     memsize;
typedef integer     pointer;

#define NONE_TYPE      0
#define NUMBER_TYPE    (1 << 0)
#define INTEGER_TYPE   (1 << 1)
#define CSTRING_TYPE   (1 << 2)
#define ARRAY_TYPE     (1 << 3)
#define FUNPTR_TYPE    (1 << 4)
#define POINTER_TYPE   INTEGER_TYPE
#define MEMSIZE_TYPE   INTEGER_TYPE

struct ValueType {
  int type;

  union {
    number   asNumber;
    integer  asInteger;
    cstring  asCString;
    void *   asNone;
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

#define AS_NONE(__v__) __v__.asNone
#define    NONE(__s__) (VALUE) {.type = NONE_TYPE, .asNone = (void *) 0}

typedef VALUE (*Cfunc) (la_t *, VALUE, VALUE, VALUE, VALUE, VALUE, VALUE, VALUE, VALUE, VALUE);
typedef VALUE (*Opfunc) (VALUE, VALUE);

enum {
  LA_ERR_OUTOFBOUNDS = -7,
  LA_ERR_TOOMANYARGS = -6,
  LA_ERR_BADARGS = -5,
  LA_ERR_UNKNOWN_SYM = -4,
  LA_ERR_SYNTAX = -3,
  LA_ERR_NOMEM = -2,
  LA_NOTOK = -1,
  LA_OK = 0,
  LA_ERR_OK_ELSE = 1,
  LA_ERR_BREAK = 2,
  LA_ERR_CONTINUE = 3,
  LA_ERR_EXIT = 4
};

/* Interface */
typedef int (*LaPrintByte_cb) (FILE *, int);
typedef int (*LaPrintBytes_cb) (FILE *, const char *);
typedef int (*LaPrintFmtBytes_cb) (FILE *, const char *, ...);
typedef int (*LaSyntaxError_cb) (la_t *, const char *);
typedef int (*LaDefineFuns_cb) (la_t *);

typedef struct la_opts {
  char  *name;
  char  *la_dir;
  int    name_gen;
  FILE  *err_fp;
  FILE  *out_fp;

  LaPrintByte_cb print_byte;
  LaPrintBytes_cb print_bytes;
  LaPrintFmtBytes_cb print_fmt_bytes;
  LaSyntaxError_cb syntax_error;
  LaDefineFuns_cb define_funs_cb;

  void *user_data;
} la_opts;

#define LaOpts(...) (la_opts) { \
  .print_byte = NULL,         \
  .print_bytes = NULL,        \
  .print_fmt_bytes = NULL,    \
  .syntax_error = NULL,       \
  .define_funs_cb = NULL,     \
  .err_fp = stderr,           \
  .out_fp = stdout,           \
  .name = NULL,               \
  .la_dir = NULL,             \
  .name_gen = 97,             \
  .user_data = NULL,          \
  __VA_ARGS__}

typedef struct la_get_self {
  la_t *(*current) (la_T *);
  void *(*user_data) (la_t *);
  char
    *(*eval_str) (la_t *),
    *(*message) (la_t *);
  int (*current_idx) (la_T *);
} la_get_self;

typedef struct la_set_self {
  la_t *(*current) (la_T *, int);

  void
    (*la_dir) (la_t *, char *),
    (*user_data) (la_t *, void *),
    (*define_funs_cb) (la_t *, LaDefineFuns_cb);

} la_set_self;

typedef struct la_self {
  la_get_self get;
  la_set_self set;

  void
    (*release) (la_t **),
    (*remove_instance) (la_T *, la_t *);

  la_t
    *(*new) (la_T *),
    *(*init_instance) (la_T *, la_opts),
    *(*append_instance) (la_T *, la_t *);

  int
    (*def) (la_t *, const char *, int, VALUE),
    (*init) (la_T *, la_t *, la_opts),
    (*eval_file) (la_t *, const char *),
    (*load_file) (la_T *, la_t *, char *),
    (*eval_string) (la_t *, const char *);

  VALUE
    (*print_byte) (la_t *, char),
    (*print_bytes) (la_t *, char *);

} la_self;

struct la_T {
  la_self self;
  la_prop *prop;
};

public la_T *__init_la__ (void);
public void __deinit_la__ (la_T **);

#endif /* LA_HDR */
