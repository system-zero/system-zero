#ifndef LA_HDR
#define LA_HDR

#define MAX_BUILTIN_PARAMS 9
#define MAXLEN_SYMBOL      63
#define MAXLEN_TYPE_NAME   32

typedef struct la_T la_T;
typedef struct la_t la_t;
typedef struct la_prop la_prop;
typedef struct funType funT;
typedef struct ValueType ValueType;
typedef struct sym_t sym_t;

typedef double      number;
typedef ptrdiff_t   integer;
typedef integer     memsize;
typedef integer     pointer;
typedef string_t    string;

#define NONE_TYPE      0
#define NUMBER_TYPE    (1 << 0)
#define INTEGER_TYPE   (1 << 1)
#define FUNCPTR_TYPE   (1 << 2)
#define STRING_TYPE    (1 << 3)
#define ARRAY_TYPE     (1 << 4)
#define OBJECT_TYPE    (1 << 5)
#define POINTER_TYPE   INTEGER_TYPE

struct ValueType {
  union {
    number   asNumber;
    integer  asInteger;
    string*  asString;
     void *  asNone;
  };

  int type;
  int refcount;
  sym_t *sym;
};

typedef ValueType VALUE;

#define AS_NUMBER(__v__) __v__.asNumber
#define    NUMBER(__d__) (VALUE) {.type = NUMBER_TYPE, .asNumber = __d__, .refcount = 0, .sym = NULL}

#define AS_INT(__v__) __v__.asInteger
#define    INT(__i__) (VALUE) {.type = INTEGER_TYPE, .refcount = 0, .asInteger = __i__, .sym = NULL}

#define  AS_STRING_BYTES(__v__) AS_STRING(__v__)->bytes
#define  AS_STRING(__v__)  __v__.asString
#define     STRING(__s__) (VALUE) {.type = STRING_TYPE, .refcount = 0, .asString = __s__, .sym = NULL}
#define STRING_NEW(__s__) STRING(String.new_with (__s__))
#define STRING_NEW_WITH_LEN(__s__, __l__) STRING(String.new_with_len (__s__, __l__))

#define AS_ARRAY AS_PTR
#define    ARRAY(__a__) (VALUE) {.type = ARRAY_TYPE, .asInteger = (pointer) __a__, .refcount = 0, .sym = NULL}
#define    ARRAY_NEW(__type__, __len__) ({              \
  ArrayType *array_ = Alloc (sizeof (ArrayType));       \
  VALUE ary_;                                           \
  array_->type = __type__;                              \
  array_->len  = __len__;                               \
  if (array_->type is INTEGER_TYPE) {                   \
    integer *i_ar = Alloc (__len__ * sizeof (integer)); \
    for (integer i = 0; i < __len__; i++)               \
      i_ar[i] = 0;                                      \
    ary_ = ARRAY(i_ar);                                 \
                                                        \
  }  else if (array_->type is NUMBER_TYPE) {            \
    number *n_ar = Alloc (__len__ * sizeof (number));   \
    for (integer i = 0; i < __len__; i++)               \
      n_ar[i] = 0.0;                                    \
    ary_ = ARRAY(n_ar);                                 \
                                                        \
  } else if (array_->type is STRING_TYPE) {             \
    string **s_ar = Alloc (__len__ * sizeof (string));  \
    for (integer i = 0; i < __len__; i++) {             \
      s_ar[i] = String.new_with ("");                   \
    }                                                   \
    ary_ = ARRAY(s_ar);                                 \
  }                                                     \
                                                        \
  array_->value = ary_;                                 \
  array_;                                               \
})

#define AS_PTR AS_INT
#define    PTR(__p__) INT((pointer) __p__)

#define  AS_MEMSIZE(__v__) (size_t) AS_INT(__v__)
#define AS_VOID_PTR(__v__) (void *) AS_PTR(__v__)
#define AS_FUNC_PTR(__v__) (funT *) AS_PTR(__v__)

#define AS_NONE(__v__) __v__.asNone
#define    NONE (VALUE) {.type = NONE_TYPE, .refcount = 0, .asNone = (void *) 0, .sym = NULL}

typedef VALUE (*ObjectRelease) (la_t *, VALUE);
typedef VALUE (*ObjectToString) (VALUE);

typedef struct ObjectType {
  ObjectRelease  release;
  ObjectToString toString;
  VALUE          value;
} ObjectType;

typedef ObjectType object;

#define AS_OBJECT(__o__) (object *) AS_PTR(__o__)
#define    OBJECT(__o__) (VALUE) {.type = OBJECT_TYPE, .asInteger = (pointer) __o__, .refcount = 0, .sym = NULL}

#define AS_FILEPTR(__o__) ({                \
  object *_o_ = AS_OBJECT(__o__);           \
  FILE *_fp_ = (FILE *) AS_PTR(_o_->value); \
  _fp_;                                     \
})

typedef VALUE (*CFunc) (la_t *, VALUE, VALUE, VALUE, VALUE, VALUE, VALUE, VALUE, VALUE, VALUE);
typedef VALUE (*OpFunc) (la_t *, VALUE, VALUE);

enum {
  LA_ERR_OUTOFBOUNDS  = -8,
  LA_ERR_TOOMANYARGS  = -7,
  LA_ERR_BADARGS      = -6,
  LA_ERR_UNKNOWN_SYM  = -5,
  LA_ERR_UNKNOWN_TYPE = -4,
  LA_ERR_SYNTAX       = -3,
  LA_ERR_NOMEM        = -2,
  LA_NOTOK            = -1,
  LA_OK               = 0,
  LA_ERR_OK_ELSE      = 1,
  LA_ERR_BREAK        = 2,
  LA_ERR_CONTINUE     = 3,
  LA_ERR_EXIT         = 4
};

/* Interface */
typedef int (*LaPrintByte_cb) (FILE *, int);
typedef int (*LaPrintBytes_cb) (FILE *, const char *);
typedef int (*LaPrintFmtBytes_cb) (FILE *, const char *, ...);
typedef int (*LaSyntaxError_cb) (la_t *, const char *);
typedef int (*LaDefineFuns_cb) (la_t *);

#define LA_CFUNC(x) (((x) << 8) + 'B')

typedef struct la_opts {
  char  *name;
  char  *la_dir;
  int    name_gen;
  int    argc;
  char **argv;
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
  .name = NULL,                 \
  .la_dir = NULL,               \
  .name_gen = 97,               \
  .argc = 0,                    \
  .argv = NULL,                 \
  .err_fp = stderr,             \
  .out_fp = stdout,             \
  .print_byte = NULL,           \
  .print_bytes = NULL,          \
  .print_fmt_bytes = NULL,      \
  .syntax_error = NULL,         \
  .define_funs_cb = NULL,       \
  .user_data = NULL,            \
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
    (*eval_expr) (la_t *, const char *, VALUE *),
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
