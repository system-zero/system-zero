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

#define NULL_TYPE      0
#define BOOLEAN_TYPE   (1 << 0)
#define NUMBER_TYPE    (1 << 1)
#define INTEGER_TYPE   (1 << 2)
#define CFUNCTION_TYPE (1 << 3)
#define FUNCPTR_TYPE   (1 << 4)
#define STRING_TYPE    (1 << 5)
#define ARRAY_TYPE     (1 << 6)
#define MAP_TYPE       (1 << 7)
#define OBJECT_TYPE    (1 << 8)
#define LIST_TYPE      (1 << 9)
#define FILEPTR_TYPE   (1 << 10)
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

typedef struct ArrayType {
  VALUE value;
  int type;
  size_t len;
} ArrayType;

typedef VALUE (*ObjectRelease) (la_t *, VALUE);
typedef VALUE (*ObjectToString) (VALUE);

typedef struct ObjectType {
  ObjectRelease  release;
  ObjectToString toString;
  VALUE          value;
} ObjectType;

typedef ObjectType object;

typedef struct listType listType;

#define    INT(__i__) (VALUE) {.type = INTEGER_TYPE, .asInteger = __i__, .refcount = 0, .sym = NULL}
#define AS_INT(__v__) __v__.asInteger

#define    NUMBER(__n__) (VALUE) {.type = NUMBER_TYPE, .asNumber = __n__, .refcount = 0, .sym = NULL}
#define AS_NUMBER(__v__) __v__.asNumber

#define     STRING(__s__) (VALUE) {.type = STRING_TYPE, .asString = __s__, .refcount = 0, .sym = NULL}
#define  AS_STRING(__v__)  __v__.asString
#define  AS_STRING_BYTES(__v__) AS_STRING(__v__)->bytes
#define STRING_NEW(__s__) STRING(String.new_with (__s__))
#define STRING_NEW_WITH_LEN(__s__, __l__) STRING(String.new_with_len (__s__, __l__))

#define    ARRAY(__a__) (VALUE) {.type = ARRAY_TYPE, .asInteger = (pointer) __a__, .refcount = 0, .sym = NULL}
#define AS_ARRAY AS_PTR
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

#define    MAP(__v__) (VALUE) {.type = MAP_TYPE, .asInteger = (pointer) __v__, .refcount = 0, .sym = NULL}
#define AS_MAP(__v__) (Vmap_t *) AS_PTR(__v__)

#define    OBJECT(__o__) (VALUE) {.type = OBJECT_TYPE, .asInteger = (pointer) __o__, .refcount = 0, .sym = NULL}
#define AS_OBJECT(__o__) (object *) AS_PTR(__o__)

#define    LIST(__l__) (VALUE) {.type = LIST_TYPE, .asInteger = (pointer) __l__, .refcount = 0, .sym = NULL}
#define AS_LIST(__v__) ({                          \
  object *_o_ = AS_OBJECT(__v__);                  \
  listType *_l_ = (listType *) AS_PTR(_o_->value); \
  _l_;                                             \
})

#define FILEPTR(__fp__) (VALUE) {.type = FILEPTR_TYPE, .asInteger = (pointer) __fp__, .refcount = 0, .sym = NULL}
#define AS_FILEPTR(__v__) ({                       \
  object *_o_ = AS_OBJECT(__v__);                  \
  FILE *_fp_ = (FILE *) AS_PTR(_o_->value);        \
  _fp_;                                            \
})

#define    PTR(__p__) INT((pointer) __p__)
#define AS_PTR AS_INT

#define  AS_MEMSIZE(__v__) (size_t) AS_INT(__v__)
#define AS_VOID_PTR(__v__) (void *) AS_PTR(__v__)
#define AS_FUNC_PTR(__v__) (funT *) AS_PTR(__v__)
#define AS_NULL(__v__) __v__.asNull

#define  TRUE_VALUE INT(1)
#define FALSE_VALUE INT(0)
#define    OK_VALUE INT(0)
#define NOTOK_VALUE INT(-1)
#define  NULL_VALUE (VALUE) {.type = NULL_TYPE, .asNull = (void *) 0, .refcount = 0, .sym = NULL}

#define IS_INT(__v__)    (__v__.type == INTEGER_TYPE)
#define IS_MAP(__v__)    (__v__.type == MAP_TYPE)
#define IS_NULL(__v__)   (__v__.type == NULL_TYPE)
#define IS_ARRAY(__v__)  (__v__.type == ARRAY_TYPE)
#define IS_STRING(__v__) (__v__.type == STRING_TYPE)
#define IS_NUMBER(__v__) (__v__.type == NUMBER_TYPE)
#define IS_OBJECT(__v__) (__v__.type == OBJECT_TYPE)
#define IS_LIST(__v__)   (__v__.type == LIST_TYPE)
#define IS_FILEPTR(__v__)(__v__.type == FILEPTR_TYPE)
#define IS_PTR IS_INT

#define THROW(__e__, __m__) do {    \
  La.set.CFuncError (this,  __e__); \
  La.set.curMsg (this, __m__);      \
  return NULL_VALUE;                \
} while (0)

typedef VALUE (*CFunc) (la_t *, VALUE, VALUE, VALUE, VALUE, VALUE, VALUE, VALUE, VALUE, VALUE);
typedef VALUE (*OpFunc) (la_t *, VALUE, VALUE);

typedef int  (*ModuleInit) (la_t *);
typedef void (*ModuleDeinit) (la_t *);

#define GET_OPT_ERR_STREAM() ({                                           \
  FILE *_err_fp = NULL;                                                   \
  VALUE _v_err_stream = La.get.qualifier (this, "err_stream", NULL_VALUE);\
  if (IS_NULL(_v_err_stream)) {                                           \
    _err_fp = stderr;                                                     \
  } else {                                                                \
    ifnot (IS_OBJECT(_v_err_stream))                                      \
      THROW(LA_ERR_TYPE_MISMATCH, "awaiting a file pointer");             \
    object *o = AS_OBJECT(_v_err_stream);                                 \
    _err_fp = (FILE *) AS_PTR(o->value);                                  \
  }                                                                       \
  _err_fp;                                                                \
})

#define GET_OPT_OUT_STREAM() ({                                           \
  FILE *_out_fp = NULL;                                                   \
  VALUE _v_out_stream = La.get.qualifier (this, "out_stream", NULL_VALUE);\
  if (IS_NULL(_v_out_stream)) {                                           \
    _out_fp = stdout;                                                     \
  } else {                                                                \
    ifnot (IS_OBJECT(_v_out_stream))                                      \
      THROW(LA_ERR_TYPE_MISMATCH, "awaiting a file pointer");             \
    object *o = AS_OBJECT(_v_out_stream);                                 \
    _out_fp = (FILE *) AS_PTR(o->value);                                  \
  }                                                                       \
  _out_fp;                                                                \
})

#define GET_OPT_VERBOSE() ({                                              \
  VALUE _v_verbose = La.get.qualifier (this, "verbose", INT(OPT_VERBOSE_ON_ERROR)); \
  ifnot (IS_INT(_v_verbose))                                              \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");         \
  AS_INT(_v_verbose);                                                     \
})

#define GET_OPT_FORCE() ({                                                \
  VALUE _v_force = La.get.qualifier (this, "force", INT(OPT_NO_FORCE));   \
  ifnot (IS_INT(_v_force))                                                \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");         \
  AS_INT(_v_force);                                                       \
})

#define GET_OPT_RECURSIVE() ({                                            \
  VALUE _v_recursive = La.get.qualifier (this, "recursive", INT(OPT_NO_RECURSIVE));  \
  ifnot (IS_INT(_v_recursive))                                            \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");         \
  AS_INT(_v_recursive);                                                   \
})

#define GET_OPT_INTERACTIVE() ({                                          \
  VALUE _v_interactive = La.get.qualifier (this, "interactive", INT(OPT_NO_INTERACTIVE));  \
  ifnot (IS_INT(_v_interactive))                                          \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");         \
  AS_INT(_v_interactive);                                                 \
})

#define GET_OPT_BACKUP() ({                                               \
  VALUE _v_backup = La.get.qualifier (this, "backup", INT(OPT_NO_BACKUP));\
  ifnot (IS_INT(_v_backup))                                               \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");         \
  AS_INT(_v_backup);                                                      \
})

#define GET_OPT_DEREFERENCE() ({                                          \
  VALUE _v_dereference = La.get.qualifier (this, "dereference", INT(OPT_NO_DEREFERENCE));\
  ifnot (IS_INT(_v_dereference))                                          \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");         \
  AS_INT(_v_dereference);                                                 \
})

#define GET_OPT_PRESERVE() ({                                             \
  VALUE _v_preserve = La.get.qualifier (this, "preserve", INT(OPT_NO_PRESERVE));\
  ifnot (IS_INT(_v_preserve))                                             \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");         \
  AS_INT(_v_preserve);                                                    \
})

#define GET_OPT_UPDATE() ({                                               \
  VALUE _v_update = La.get.qualifier (this, "update", INT(OPT_NO_UPDATE));\
  ifnot (IS_INT(_v_update))                                               \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");         \
  AS_INT(_v_update);                                                      \
})

#define GET_OPT_ALL() ({                                                  \
  VALUE _v_all = La.get.qualifier (this, "all", INT(OPT_NO_ALL));         \
  ifnot (IS_INT(_v_all))                                                  \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");         \
  AS_INT(_v_all);                                                         \
})

#define GET_OPT_TARGET_DIRECTORY() ({                                     \
  char * _t_ = NULL;                                                      \
  VALUE _v_targetDir = La.get.qualifier (this, "targetDirectory", NULL_VALUE);  \
  if (IS_STRING(_v_targetDir))                                            \
    _t_ = AS_STRING_BYTES(_v_targetDir);                                  \
  else if (IS_NULL(_v_targetDir) == 0)                                    \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string or null qualifier");   \
  _t_;                                                                    \
})

enum {
  LA_ERR_DYNLINK = -13,
  LA_ERR_IMPORT,
  LA_ERR_LOAD,
  LA_ERR_INV_ARRAY_TYPE,
  LA_ERR_TYPE_MISMATCH,
  LA_ERR_OUTOFBOUNDS,
  LA_ERR_NUM_FUNCTION_ARGS_MISMATCH,
  LA_ERR_TOOMANY_FUNCTION_ARGS,
  LA_ERR_UNKNOWN_TYPE,
  LA_ERR_UNKNOWN_SYM,
  LA_ERR_SYNTAX,
  LA_ERR_NOMEM,
  LA_NOTOK = -1,
  LA_OK = 0,
  LA_ERR_OK_ELSE,
  LA_ERR_BREAK,
  LA_ERR_CONTINUE,
  LA_ERR_EXIT = 4,
  LA_MMT_REASSIGN = 1000
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

   int
     (*didExit) (la_t *),
     (*current_idx) (la_T *);

   VALUE (*qualifier) (la_t *, char *, VALUE);

   Vmap_t *(*qualifiers) (la_t *);

   la_T *(*root) (la_t *);
} la_get_self;

typedef struct la_set_self {
  la_t *(*current) (la_T *, int);

  void
    (*Errno) (la_t *, int),
    (*la_dir) (la_t *, char *),
    (*curMsg) (la_t *, char *),
    (*user_data) (la_t *, void *),
    (*CFuncError) (la_t *, int),
    (*define_funs_cb) (la_t *, LaDefineFuns_cb);

  VALUE (*qualifiers) (la_t *, VALUE);
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
    (*eval_string) (la_t *, const char *),
    (*qualifier_exists) (la_t *, char *);

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
