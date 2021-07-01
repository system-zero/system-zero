#ifndef LA_HDR
#define LA_HDR

#define MAX_BUILTIN_PARAMS 9
#define MAXLEN_SYMBOL      63
#define MAXLEN_MSG         255

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

#define NULL_TYPE      0
#define NUMBER_TYPE    (1 << 0)
#define INTEGER_TYPE   (1 << 1)
#define FUNCPTR_TYPE   (1 << 2)
#define STRING_TYPE    (1 << 3)
#define ARRAY_TYPE     (1 << 4)
#define OBJECT_TYPE    (1 << 5)
#define MAP_TYPE       (1 << 6)
#define CFUNCTION_TYPE (1 << 7)
#define POINTER_TYPE   INTEGER_TYPE

struct ValueType {
  union {
    number   asNumber;
    integer  asInteger;
    string*  asString;
     void *  asNull;
  };

  int type;
  int refcount;
  sym_t *sym;
};

typedef ValueType VALUE;

#define AS_NUMBER(__v__) __v__.asNumber
#define    NUMBER(__n__) (VALUE) {.type = NUMBER_TYPE, .asNumber = __n__, .refcount = 0, .sym = NULL}

#define AS_INT(__v__) __v__.asInteger
#define    INT(__i__) (VALUE) {.type = INTEGER_TYPE, .asInteger = __i__, .refcount = 0, .sym = NULL}

#define  AS_STRING_BYTES(__v__) AS_STRING(__v__)->bytes
#define  AS_STRING(__v__)  __v__.asString
#define     STRING(__s__) (VALUE) {.type = STRING_TYPE, .asString = __s__, .refcount = 0, .sym = NULL}
#define STRING_NEW(__s__) STRING(String.new_with (__s__))
#define STRING_NEW_WITH_LEN(__s__, __l__) STRING(String.new_with_len (__s__, __l__))

typedef struct ArrayType {
  VALUE value;
  int type;
  size_t len;
} ArrayType;

#define AS_ARRAY AS_PTR
#define    ARRAY(__a__) (VALUE) {.type = ARRAY_TYPE, .asInteger = (pointer) __a__, .refcount = 0, .sym = NULL}
#define    ARRAY_NEW(__type__, __len__) ({               \
  ArrayType *array_ = Alloc (sizeof (ArrayType));        \
  VALUE ary_;                                            \
  array_->type = __type__;                               \
  array_->len  = __len__;                                \
  if (array_->type is INTEGER_TYPE) {                    \
    integer *i_ar = Alloc (__len__ * sizeof (integer));  \
    for (integer i = 0; i < __len__; i++)                \
      i_ar[i] = 0;                                       \
    ary_ = ARRAY(i_ar);                                  \
                                                         \
  }  else if (array_->type is NUMBER_TYPE) {             \
    number *n_ar = Alloc (__len__ * sizeof (number));    \
    for (integer i = 0; i < __len__; i++)                \
      n_ar[i] = 0.0;                                     \
    ary_ = ARRAY(n_ar);                                  \
                                                         \
  } else if (array_->type is STRING_TYPE) {              \
    string **s_ar = Alloc (__len__ * sizeof (string));   \
    for (integer i = 0; i < __len__; i++) {              \
      s_ar[i] = String.new_with ("");                    \
    }                                                    \
    ary_ = ARRAY(s_ar);                                  \
                                                         \
  } else if (array_->type is MAP_TYPE) {                 \
    Vmap_t **m_ar = Alloc (__len__ * Vmap.size_of ());   \
    for (integer i = 0; i < __len__; i++) {              \
      m_ar[i] = NULL;                                    \
    }                                                    \
    ary_ = ARRAY(m_ar);                                  \
                                                         \
  } else if (array_->type is ARRAY_TYPE) {               \
    ArrayType **a_ar = Alloc (sizeof (ArrayType) * __len__);\
    for (integer i = 0; i < __len__; i++) {              \
      a_ar[i] = NULL;                                    \
    }                                                    \
    ary_ = ARRAY(a_ar);                                  \
                                                         \
  }                                                      \
                                                         \
  array_->value = ary_;                                  \
  array_;                                                \
})

#define AS_PTR AS_INT
#define    PTR(__p__) INT((pointer) __p__)

#define  AS_MEMSIZE(__v__) (size_t) AS_INT(__v__)
#define AS_VOID_PTR(__v__) (void *) AS_PTR(__v__)
#define AS_FUNC_PTR(__v__) (funT *) AS_PTR(__v__)

#define AS_NULL(__v__) __v__.asNull
#define    NULL_VALUE (VALUE) {.type = NULL_TYPE, .asNull = (void *) 0, .refcount = 0, .sym = NULL}

#define  TRUE_VALUE INT(1)
#define FALSE_VALUE INT(0)

#define    OK_VALUE INT(0)
#define NOTOK_VALUE INT(-1)

#define AS_MAP(__v__) (Vmap_t *) AS_PTR(__v__)
#define    MAP(__v__) (VALUE) {.type = MAP_TYPE, .asInteger = (pointer) __v__, .refcount = 0, .sym = NULL}

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

typedef int  (*ModuleInit) (la_t *);
typedef void (*ModuleDeinit) (la_t *);

enum {
  LA_ERR_DYNLINK       = -12,
  LA_ERR_IMPORT        = -11,
  LA_ERR_LOAD          = -10,
  LA_ERR_TYPE_MISMATCH = -9,
  LA_ERR_OUTOFBOUNDS   = -8,
  LA_ERR_TOOMANYARGS   = -7,
  LA_ERR_BADARGS       = -6,
  LA_ERR_UNKNOWN_TYPE  = -5,
  LA_ERR_UNKNOWN_SYM   = -4,
  LA_ERR_SYNTAX        = -3,
  LA_ERR_NOMEM         = -2,
  LA_NOTOK             = -1,
  LA_OK                = 0,
  LA_ERR_OK_ELSE       = 1,
  LA_ERR_BREAK         = 2,
  LA_ERR_CONTINUE      = 3,
  LA_ERR_EXIT          = 4
};

typedef struct LaDefCFun {
  const char *name;
  VALUE val;
  int nargs;
} LaDefCFun;

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

   la_T *(*root) (la_t *);
} la_get_self;

typedef struct la_set_self {
  la_t *(*current) (la_T *, int);

  void
    (*Errno) (la_t *, int),
    (*la_dir) (la_t *, char *),
    (*user_data) (la_t *, void *),
    (*CFuncError) (la_t *, int),
    (*curMsg) (la_t *, char *),
    (*define_funs_cb) (la_t *, LaDefineFuns_cb);

} la_set_self;

typedef struct la_object_self {
  object *(*new) (ObjectRelease, ObjectToString, VALUE);
} la_object_self;

typedef struct la_map_self {
  void
    (*release_value) (la_t *, VALUE *);

  int
    (*set_value) (la_t *, Vmap_t *, char *, VALUE, int),
    (*reset_value) (la_t *, Vmap_t *, char *, VALUE);
} la_map_self;

typedef struct la_self {
  la_get_self get;
  la_set_self set;
  la_map_self map;
  la_object_self object;

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
    (*def_std) (la_t *, char *, int, VALUE, int),
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
public la_T *la_get_root (la_t *);

#endif /* LA_HDR */
