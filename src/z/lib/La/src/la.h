#ifndef LA_HDR
#define LA_HDR

#define MAX_BUILTIN_PARAMS 9
#define MAXLEN_SYMBOL      255
#define MAXLEN_MSG         511
#define MAXLEN_TYPENAME    15

typedef struct la_T la_T;
typedef struct la_t la_t;
typedef struct la_prop la_prop;
typedef struct funType funT;
typedef struct ValueType ValueType;
typedef struct sym_t sym_t;

typedef double      number;
typedef ptrdiff_t   integer;
typedef integer     pointer;

#define NULL_TYPE      0
#define BOOLEAN_TYPE   1
#define INTEGER_TYPE   2
#define NUMBER_TYPE    3
#define UFUNCTION_TYPE 4
#define CFUNCTION_TYPE 8
#define STRING_TYPE    9
#define ARRAY_TYPE     11
#define MAP_TYPE       16
#define LIST_TYPE      17
#define OBJECT_TYPE    18
#define FILEPTR_TYPE   19
#define FD_TYPE        24
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
  char           name[MAXLEN_TYPENAME + 1];
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
#define AS_STRING_ARRAY(__v__, _len_) ({                 \
  ArrayType *_ar = (ArrayType *) AS_ARRAY(__v__);        \
  if (_ar->type != STRING_TYPE)                          \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string array or null qualifier");   \
  string **_s_ar = (string **) AS_ARRAY(_ar->value);     \
  *_len_ = _ar->len;                                     \
  _s_ar;                                                 \
})

#define    ARRAY_NEW(__type__, __len__) ({               \
  ArrayType *array_ = Alloc (sizeof (ArrayType));        \
  VALUE ary_ = NULL_VALUE;                               \
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
  } else if (array_->type is LIST_TYPE) {                \
    listArrayMember **l_ar = Alloc (__len__ * sizeof (listArrayMember)); \
    for (integer i = 0; i < __len__; i++) {              \
      l_ar[i] = NULL;                                    \
    }                                                    \
    ary_ = ARRAY(l_ar);                                  \
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

#define ARRAY_INIT_WITH_LEN(__type__, __len__) ({        \
  ArrayType *array_ = Alloc (sizeof (ArrayType));        \
  VALUE ary_ = NULL_VALUE;                               \
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
    for (integer i = 0; i < __len__; i++)                \
      s_ar[i] = NULL;                                    \
    ary_ = ARRAY(s_ar);                                  \
                                                         \
  } else if (array_->type is MAP_TYPE) {                 \
    Vmap_t **m_ar = Alloc (__len__ * Vmap.size_of ());   \
    for (integer i = 0; i < __len__; i++) {              \
      m_ar[i] = NULL;                                    \
    }                                                    \
    ary_ = ARRAY(m_ar);                                  \
                                                         \
  } else if (array_->type is LIST_TYPE) {                \
    listArrayMember **l_ar = Alloc (__len__ * sizeof (listArrayMember)); \
    for (integer i = 0; i < __len__; i++) {              \
      l_ar[i] = NULL;                                    \
    }                                                    \
    ary_ = ARRAY(l_ar);                                  \
                                                         \
  } else if (array_->type is ARRAY_TYPE) {               \
    ArrayType **a_ar = Alloc (sizeof (ArrayType) * __len__);\
    for (integer i = 0; i < __len__; i++) {              \
      a_ar[i] = NULL;                                    \
    }                                                    \
    ary_ = ARRAY(a_ar);                                  \
  }                                                      \
                                                         \
  array_->value = ary_;                                  \
  array_;                                                \
})

#define    ARRAY_APPEND(__ar__, __v__) ({                \
  VALUE ary_ = __ar__->value;                            \
  int __len__ = __ar__->len + 1;                         \
  if (__ar__->type == INTEGER_TYPE) {                    \
    integer *i_ar = (integer *) AS_ARRAY(ary_);          \
    i_ar = Realloc (i_ar, __len__ * sizeof (integer));   \
    i_ar[__len__ - 1] = AS_INT(__v__);                   \
    ary_ = ARRAY(i_ar);                                  \
  } else if (__ar__->type == NUMBER_TYPE) {              \
    number *n_ar = (number *) AS_ARRAY(ary_);            \
    n_ar = Realloc (n_ar, __len__ * sizeof (number));    \
    n_ar[__len__ - 1] = AS_NUMBER(__v__);                \
    ary_ = ARRAY(n_ar);                                  \
  } else if (__ar__->type == STRING_TYPE) {              \
    string **s_ar = (string **) AS_ARRAY(ary_);          \
    s_ar = Realloc (s_ar, __len__ * sizeof (string));    \
    s_ar[__len__ - 1] = AS_STRING(__v__);                \
    ary_ = ARRAY(s_ar);                                  \
  } else if (__ar__->type is MAP_TYPE) {                 \
    Vmap_t **m_ar = (Vmap_t **) AS_ARRAY(ary_);          \
    m_ar = Realloc (m_ar, __len__ * Vmap.size_of ());    \
    m_ar[__len__ - 1] = AS_MAP(__v__);                   \
    ary_ = ARRAY(m_ar);                                  \
  } else if (__ar__->type is LIST_TYPE) {                \
    listArrayMember **l_ar = (listArrayMember **) AS_ARRAY(ary_); \
    l_ar = Realloc (l_ar, __len__ * sizeof (listArrayMember));    \
    l_ar[__len__ - 1] = LIST_ARRAY_MEMBER(__v__);        \
    ary_ = ARRAY(l_ar);                                  \
  } else if (__ar__->type is ARRAY_TYPE) {               \
    ArrayType **a_ar = (ArrayType **) AS_ARRAY(ary_);    \
    a_ar = Realloc (a_ar, __len__ * sizeof (ArrayType)); \
    a_ar[__len__ - 1] = (ArrayType *) AS_ARRAY(__v__);   \
    ary_ = ARRAY(a_ar);                                  \
  }                                                      \
  __ar__->value = ary_;                                  \
  __ar__->len = __len__;                                 \
  __ar__;                                                \
})

#define    MAP(__v__) (VALUE) {.type = MAP_TYPE, .asInteger = (pointer) __v__, .refcount = 0, .sym = NULL}
#define AS_MAP(__v__) (Vmap_t *) AS_PTR(__v__)
//#define MAPVAL(__p__) (VALUE) {.type = MAP_TYPE, .asInteger = (pointer) __p__, .refcount = __p__->refcount, .sym = __p__->sym}

#define    OBJECT(__o__) (VALUE) {.type = OBJECT_TYPE, .asInteger = (pointer) __o__, .refcount = 0, .sym = NULL}
#define AS_OBJECT(__o__) (object *) AS_PTR(__o__)

#define     LIST(__l__) (VALUE) {.type = LIST_TYPE, .asInteger = (pointer) __l__, .refcount = 0, .sym = NULL}
#define LIST_NEW() ({                                             \
  listType *_l_ = Alloc (sizeof (listType));                      \
  _l_->cur_idx = -1;                                              \
  _l_->num_items = 0;                                             \
  _l_->head = _l_->tail = _l_->current = NULL;                    \
  VALUE v_ = LIST(_l_);                                           \
  object *o = la_object_new (list_release, NULL, "ListType", v_); \
  LIST(o);                                                        \
})
#define  AS_LIST(__v__) ({                         \
  object *_o_ = AS_OBJECT(__v__);                  \
  listType *_l_ = (listType *) AS_PTR(_o_->value); \
  _l_;                                             \
})

typedef object listArrayMember;
#define LIST_ARRAY_MEMBER(__v__) ({                \
  object *_o_ = AS_OBJECT(__v__);                  \
  _o_;                                             \
})

#define FILEPTR(__fp__) (VALUE) {.type = FILEPTR_TYPE, .asInteger = (pointer) __fp__, .refcount = 0, .sym = NULL}
#define AS_FILEPTR(__v__) ({                       \
  object *_o_ = AS_OBJECT(__v__);                  \
  FILE *_fp_ = (FILE *) AS_PTR(_o_->value);        \
  _fp_;                                            \
})

#define FILEDES(__fd__) (VALUE) {.type = FD_TYPE, .asInteger = (pointer) __fd__, .refcount = 0, .sym = NULL}
#define AS_FILEDES(__v__) ({                       \
  object *_o_ = AS_OBJECT(__v__);                  \
  int _fd_ = AS_INT(_o_->value);                   \
  _fd_;                                            \
})

#define    PTR(__p__) INT((pointer) __p__)
#define AS_PTR AS_INT

#define AS_VOID_PTR(__v__) (void *) AS_PTR(__v__)
#define AS_FUNC_PTR(__v__) (funT *) AS_PTR(__v__)
#define     AS_NULL(__v__) __v__.asNull

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
#define IS_FILEDES(__v__)(__v__.type == FD_TYPE)
#define IS_PTR IS_INT
#define IS_NOTOK(__v__) (__v__.type == INTEGER_TYPE && AS_INT(v) == NOTOK)

typedef VALUE (*CFunc) (la_t *, VALUE, VALUE, VALUE, VALUE, VALUE, VALUE, VALUE, VALUE, VALUE);
typedef VALUE (*OpFunc) (la_t *, VALUE, VALUE);

typedef int  (*ModuleInit) (la_t *);
typedef void (*ModuleDeinit) (la_t *);

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
  LA_ERR_TCALLREC = 5,
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
typedef int (*LaSyntaxErrorFmt_cb) (la_t *, const char *, ...);
typedef int (*LaDefineFuns_cb) (la_t *);

#define LA_CFUNC(x) (((x) << 8) + 'B')

typedef struct la_opts {
  const char  *name;
  const char  *la_dir;
  int    name_gen;
  int    argc;
  const char **argv;
  FILE  *err_fp;
  FILE  *out_fp;

  LaPrintByte_cb print_byte;
  LaPrintBytes_cb print_bytes;
  LaPrintFmtBytes_cb print_fmt_bytes;
  LaSyntaxError_cb syntax_error;
  LaSyntaxErrorFmt_cb syntax_error_fmt;
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
  .syntax_error_fmt = NULL,     \
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

   VALUE (*qualifier) (la_t *, const char *, VALUE);

   Vmap_t *(*qualifiers) (la_t *);

   la_T *(*root) (la_t *);
} la_get_self;

typedef struct la_set_self {
  la_t *(*current) (la_T *, int);

  void
    (*Errno) (la_t *, int),
    (*la_dir) (la_t *, const char *),
    (*curMsg) (la_t *, const char *),
    (*user_data) (la_t *, void *),
    (*CFuncError) (la_t *, int),
    (*define_funs_cb) (la_t *, LaDefineFuns_cb),
    (*qualifiers) (la_t *, Vmap_t *, const char *),
    (*function_curMsg) (la_t *, const char *, const char *);

} la_set_self;

typedef struct la_object_self {
  object *(*new) (ObjectRelease, ObjectToString, const char *, VALUE);
} la_object_self;

typedef struct la_map_self {
  void
    (*release_value) (la_t *, VALUE *);

  int
    (*set_value) (la_t *, Vmap_t *, const char *, VALUE, int),
    (*reset_value) (la_t *, Vmap_t *, const char *, VALUE);
} la_map_self;

typedef struct la_self {
  la_get_self get;
  la_set_self set;
  la_map_self map;
  la_object_self object;

  void
    (*release) (la_t **),
    (*release_qualifiers) (la_t *, char *),
    (*remove_instance) (la_T *, la_t *);

  la_t
    *(*new) (la_T *),
    *(*init_instance) (la_T *, la_opts),
    *(*append_instance) (la_T *, la_t *);

  int
    (*def) (la_t *, const char *, int, VALUE),
    (*init) (la_T *, la_t *, la_opts),
    (*def_std) (la_t *, const char *, int, VALUE, int),
    (*eval_file) (la_t *, const char *),
    (*eval_expr) (la_t *, const char *, VALUE *),
    (*load_file) (la_T *, la_t *, const char *),
    (*eval_string) (la_t *, const char *),
    (*qualifier_exists) (la_t *, const char *);

  VALUE
    (*copy_value) (la_t *, VALUE),
    (*release_value) (la_t *, VALUE),
    (*print_byte) (la_t *, char),
    (*print_bytes) (la_t *, const char *);

} la_self;

struct la_T {
  la_self self;
  la_prop *prop;
};

public la_T *__init_la__ (void);
public void __deinit_la__ (la_T **);
public la_T *la_get_root (la_t *);

/* assumed a La structure */
#ifndef WITHOUT_LA_FUNCTIONS
#define THROW(__e__, __m__) do {                                          \
  La.set.CFuncError (this,  __e__);                                       \
  La.set.function_curMsg (this, __func__, __m__);                         \
  return NULL_VALUE;                                                      \
} while (0)

#define VSTRING_TO_ARRAY(__v__) ({                                        \
  ArrayType *_array = ARRAY_NEW(STRING_TYPE, __v__->num_items);           \
  string **_ar = (string **) AS_ARRAY(_array->value);                     \
  vstring_t *_it = __v__->head;                                           \
  int _idx = 0;                                                           \
  while (_it) {                                                           \
    String.replace_with_len (_ar[_idx++],                                 \
       _it->data->bytes, _it->data->num_bytes);                           \
    _it = _it->next;                                                      \
  }                                                                       \
  _array;                                                                 \
})

#define VSTRING_TO_ARRAY_AND_RELEASE(__v__) ({                            \
  ArrayType *_array = ARRAY_INIT_WITH_LEN(STRING_TYPE, __v__->num_items); \
  string **_ar = (string **) AS_ARRAY(_array->value);                     \
  vstring_t *_it = __v__->head;                                           \
  int _idx = 0;                                                           \
  while (_it) {                                                           \
    vstring_t *_tmp = _it->next;                                          \
    _ar[_idx++] = _it->data;                                              \
    free (_it);                                                           \
    _it = _tmp;                                                           \
  }                                                                       \
  free (__v__);                                                           \
  _array;                                                                 \
})

#define VSTRING_REVERSE_TO_ARRAY_AND_RELEASE(__v__) ({                    \
  ArrayType *_array = ARRAY_INIT_WITH_LEN(STRING_TYPE, __v__->num_items); \
  string **_ar = (string **) AS_ARRAY(_array->value);                     \
  vstring_t *_it = __v__->tail;                                           \
  int _idx = 0;                                                           \
  while (_it) {                                                           \
    vstring_t *_tmp = _it->prev;                                          \
    _ar[_idx++] = _it->data;                                              \
    free (_it);                                                           \
    _it = _tmp;                                                           \
  }                                                                       \
  free (__v__);                                                           \
  _array;                                                                 \
})

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
    THROW(LA_ERR_TYPE_MISMATCH, "verbose: awaiting an integer qualifier");\
  AS_INT(_v_verbose);                                                     \
})

#define GET_OPT_VERBOSE_WITH(_v_) ({                                      \
  VALUE _v_verbose = La.get.qualifier (this, "verbose", INT(_v_));        \
  ifnot (IS_INT(_v_verbose))                                              \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");         \
  AS_INT(_v_verbose);                                                     \
})

#define GET_OPT_DEBUG() ({                                                \
  VALUE _v_debug = La.get.qualifier (this, "debug", INT(0));              \
  ifnot (IS_INT(_v_debug))                                                \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");         \
  AS_INT(_v_debug);                                                       \
})

#define GET_OPT_REVERSE() ({                                              \
  VALUE _v_reverse = La.get.qualifier (this, "reverse", INT(0));          \
  ifnot (IS_INT(_v_reverse))                                              \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");         \
  AS_INT(_v_reverse);                                                     \
})

#define GET_OPT_MAX_DEPTH() ({                                            \
  VALUE _v_depth = La.get.qualifier (this, "max_depth", INT(0));          \
  ifnot (IS_INT(_v_depth))                                                \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");         \
  AS_INT(_v_depth);                                                       \
})

#define GET_OPT_MAX_DEPTH_WITH(__d__) ({                                  \
  VALUE _v_depth = La.get.qualifier (this, "max_depth", INT(__d__));      \
  ifnot (IS_INT(_v_depth))                                                \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");         \
  AS_INT(_v_depth);                                                       \
})

#define GET_OPT_FORCE() ({                                                \
  VALUE _v_force = La.get.qualifier (this, "force", INT(OPT_NO_FORCE));   \
  ifnot (IS_INT(_v_force))                                                \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");         \
  AS_INT(_v_force);                                                       \
})

#define GET_OPT_RECURSIVE() ({                                            \
  int _rec = La.qualifier_exists (this, "recursive");                     \
  if (_rec) {                                                             \
    VALUE _v_recursive = La.get.qualifier (this, "recursive", INT(OPT_NO_RECURSIVE));  \
    if (0 == IS_INT(_v_recursive)) {                                      \
      if (IS_NULL(_v_recursive))                                          \
        _rec = 1;                                                         \
      else                                                                \
        THROW(LA_ERR_TYPE_MISMATCH, "recursive, awaiting an integer qualifier"); \
    } else                                                                \
      _rec = AS_INT(_v_recursive);                                        \
  }                                                                       \
  _rec;                                                                   \
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

#define GET_OPT_YEAR() ({                                                 \
  VALUE _v_year = La.get.qualifier (this, "year", INT(OPT_NO_YEAR));      \
  ifnot (IS_INT(_v_year))                                                 \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");         \
  AS_INT(_v_year);                                                        \
})

#define GET_OPT_MONTH() ({                                                \
  VALUE _v_month = La.get.qualifier (this, "month", INT(OPT_NO_MONTH));   \
  ifnot (IS_INT(_v_month))                                                \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");         \
  AS_INT(_v_month);                                                       \
})

#define GET_OPT_DAY() ({                                                  \
  VALUE _v_day = La.get.qualifier (this, "day", INT(OPT_NO_DAY));         \
  ifnot (IS_INT(_v_day))                                                  \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");         \
  AS_INT(_v_day);                                                         \
})

#define GET_OPT_HOUR() ({                                                 \
  VALUE _v_hour = La.get.qualifier (this, "hour", INT(OPT_NO_HOUR));      \
  ifnot (IS_INT(_v_hour))                                                 \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");         \
  AS_INT(_v_hour);                                                        \
})

#define GET_OPT_MINUTES() ({                                              \
  VALUE _v_min = La.get.qualifier (this, "minutes", INT(OPT_NO_MINUTES)); \
  ifnot (IS_INT(_v_min))                                                  \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");         \
  AS_INT(_v_min);                                                         \
})

#define GET_OPT_SECONDS() ({                                              \
  VALUE _v_sec = La.get.qualifier (this, "seconds", INT(OPT_NO_SECONDS)); \
  ifnot (IS_INT(_v_sec))                                                  \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");         \
  AS_INT(_v_sec);                                                         \
})

#define GET_OPT_AS() ({                                                   \
  char * _as_ = NULL;                                                     \
  VALUE _v_as = La.get.qualifier (this, "as", NULL_VALUE);                \
  if (IS_STRING(_v_as))                                                   \
    _as_ = AS_STRING_BYTES(_v_as);                                        \
  else if (IS_NULL(_v_as) == 0)                                           \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string or null qualifier");   \
  _as_;                                                                   \
})

#define GET_OPT_PAT() ({                                                  \
  char *_pat_ = NULL;                                                     \
  VALUE _v_pat = La.get.qualifier (this, "pat", NULL_VALUE);              \
  if (IS_STRING(_v_pat))                                                  \
    _pat_ = AS_STRING_BYTES(_v_pat);                                      \
  else if (IS_NULL(_v_pat) == 0)                                          \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string or null qualifier");   \
   _pat_;                                                                 \
})

#define GET_OPT_EXCLUDE_DIRS(_len) ({                                     \
  *_len = 0;                                                              \
  string **_dirs_ = NULL;                                                 \
  VALUE _v_dirs = La.get.qualifier (this, "exclude_dirs", NULL_VALUE);    \
  if (IS_ARRAY(_v_dirs))                                                  \
    _dirs_ = AS_STRING_ARRAY(_v_dirs, _len);                              \
  else if (IS_NULL(_v_dirs) == 0)                                         \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string or null qualifier");   \
  _dirs_;                                                                 \
})

#define GET_OPT_EXCLUDE_FILES(_len) ({                                    \
  *_len = 0;                                                              \
  string **_files_ = NULL;                                                \
  VALUE _v_files = La.get.qualifier (this, "exclude_files", NULL_VALUE);  \
  if (IS_ARRAY(_v_files))                                                 \
    _files_ = AS_STRING_ARRAY(_v_files, _len);                            \
  else if (IS_NULL(_v_files) == 0)                                        \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string or null qualifier");   \
  _files_;                                                                \
})

#define GET_OPT_EXCLUDE_FILE_EXTS(_len) ({                                \
  *_len = 0;                                                              \
  string **_exts_ = NULL;                                                 \
  VALUE _v_exts = La.get.qualifier (this, "exclude_extensions", NULL_VALUE); \
  if (IS_ARRAY(_v_exts))                                                  \
    _exts_ = AS_STRING_ARRAY(_v_exts, _len);                              \
  else if (IS_NULL(_v_exts) == 0)                                         \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string or null qualifier");   \
  _exts_;                                                                 \
})

#define GET_OPT_UID() ({                                                  \
  VALUE _v_uid = La.get.qualifier (this, "uid", INT(-1));                 \
  ifnot (IS_INT(_v_uid))                                                  \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");         \
  AS_INT(_v_uid);                                                         \
})

#define GET_OPT_GID() ({                                                  \
  VALUE _v_gid = La.get.qualifier (this, "gid", INT(-1));                 \
  ifnot (IS_INT(_v_gid))                                                  \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");         \
  AS_INT(_v_gid);                                                         \
})

#define GET_OPT_EXECUTABLE() ({                                           \
  VALUE _v_exec = La.get.qualifier (this, "executable", INT(0));          \
  ifnot (IS_INT(_v_exec))                                                 \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");         \
  AS_INT(_v_exec);                                                        \
})

#define GET_OPT_HIDDEN() ({                                               \
  VALUE _v_hidden = La.get.qualifier (this, "hidden", INT(0));            \
  ifnot (IS_INT(_v_hidden))                                               \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");         \
  AS_INT(_v_hidden);                                                      \
})

#define GET_OPT_LONG_FORMAT() ({                                          \
  VALUE _v_long_format = La.get.qualifier (this, "long_format", INT(0));  \
  ifnot (IS_INT(_v_long_format))                                          \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");         \
  AS_INT(_v_long_format);                                                 \
})

#define GET_OPT_APPEND_INDICATOR() ({                                     \
  VALUE _v_append = La.get.qualifier (this, "append_indicator", INT(0));  \
  ifnot (IS_INT(_v_append))                                               \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");         \
  AS_INT(_v_append);                                                      \
})

#define GET_OPT_SORT_BY_MTIME() ({                                        \
  VALUE _v_mtime = La.get.qualifier (this, "sort_by_mtime", INT(0));      \
  ifnot (IS_INT(_v_mtime))                                                \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");         \
  AS_INT(_v_mtime);                                                       \
})

#define GET_OPT_SORT_BY_CTIME() ({                                        \
  VALUE _v_ctime = La.get.qualifier (this, "sort_by_ctime", INT(0));      \
  ifnot (IS_INT(_v_ctime))                                                \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");         \
  AS_INT(_v_ctime);                                                       \
})

#define GET_OPT_SORT_BY_ATIME() ({                                        \
  VALUE _v_atime = La.get.qualifier (this, "sort_by_atime", INT(0));      \
  ifnot (IS_INT(_v_atime))                                                \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");         \
  AS_INT(_v_atime);                                                       \
})

#define GET_OPT_SORT_BY_SIZE() ({                                         \
  VALUE _v_size = La.get.qualifier (this, "sort_by_size", INT(0));        \
  ifnot (IS_INT(_v_size))                                                 \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");         \
  AS_INT(_v_size);                                                        \
})

#define GET_OPT_WITH_LINE_NUMBER() ({                                     \
  VALUE _v_line_nr = La.get.qualifier (this, "with_line_number", INT(0)); \
  ifnot (IS_INT(_v_line_nr))                                              \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");         \
  AS_INT(_v_line_nr);                                                     \
})

#define GET_OPT_WITH_FILENAME() ({                                        \
  VALUE _v_with_fname = La.get.qualifier (this, "with_filename", INT(0)); \
  ifnot (IS_INT(_v_with_fname))                                           \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");         \
  AS_INT(_v_with_fname);                                                  \
})

#define GET_OPT_WITHOUT_FILENAME() ({                                     \
  VALUE _v_without_fname = La.get.qualifier (this, "without_filename", INT(0));\
  ifnot (IS_INT(_v_without_fname))                                        \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");         \
  AS_INT(_v_without_fname);                                               \
})

#define GET_OPT_PRINT() ({                                                \
  int _print = La.qualifier_exists (this, "print");                       \
  if (_print) {                                                           \
    VALUE _v_print = La.get.qualifier (this, "print", INT(0));            \
    if (0 == IS_INT(_v_print)) {                                          \
      if (IS_NULL(_v_print))                                              \
        _print = 1;                                                       \
      else                                                                \
        THROW(LA_ERR_TYPE_MISMATCH, "print, awaiting an integer qualifier"); \
     } else                                                               \
    _print = AS_INT(_v_print);                                            \
  }                                                                       \
  _print;                                                                 \
})

#define GET_OPT_DRYRUN() ({                                               \
  int _dryrun = La.qualifier_exists (this, "dryrun");                     \
  if (_dryrun) {                                                          \
    VALUE _vdryrun = La.get.qualifier (this, "dryrun", INT(0));           \
    if (0 == IS_INT(_vdryrun)) {                                          \
      if (IS_NULL(_vdryrun))                                              \
        _dryrun = 1;                                                      \
      else                                                                \
        THROW(LA_ERR_TYPE_MISMATCH, "dryrun, awaiting an integer qualifier"); \
     } else                                                               \
    _dryrun = AS_INT(_vdryrun);                                           \
  }                                                                       \
  _dryrun;                                                                \
})

#define IS_TERM(__v__) ({                                                 \
  int _r_ = 0;                                                            \
  if (IS_OBJECT(__v__)) {                                                 \
    object *_o_ = AS_OBJECT(__v__);                                       \
     _r_ = Cstring.eq (_o_->name, "TermType");}                           \
  _r_;                                                                    \
})

#define GET_TERM(__v__) ({                                                \
  ifnot (IS_TERM(__v__)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a term object");\
  object *_o_ = AS_OBJECT(__v__);                                         \
  term_t *_t_ = (term_t *) AS_OBJECT (_o_->value);                        \
  _t_;                                                                    \
})

#define GET_OPT_TERM() ({                                                 \
  VALUE _v_term = La.get.qualifier (this, "term", NULL_VALUE);            \
  term_t *_tt_ = NULL;                                                    \
  ifnot (IS_NULL(_v_term))                                                \
    _tt_ = GET_TERM(_v_term);                                             \
  _tt_;                                                                   \
})

#define GET_OPT_RDONLY() ({                                               \
  int _read_only = La.qualifier_exists (this, "read_only");               \
  if (_read_only) {                                                       \
    VALUE _v_read_only = La.get.qualifier (this, "read_only", INT(0));    \
    if (0 == IS_INT(_v_read_only)) {                                      \
      if (IS_NULL(_v_read_only))                                          \
        _read_only = 1;                                                   \
      else                                                                \
        THROW(LA_ERR_TYPE_MISMATCH, "read_only, awaiting an integer qualifier"); \
     } else                                                               \
    _read_only = AS_INT(_v_read_only);                                    \
  }                                                                       \
  _read_only;                                                             \
})

#define GET_OPT_BACKGROUND() ({                                            \
  int _background = La.qualifier_exists (this, "background");              \
  if (_background) {                                                       \
    VALUE _v_background = La.get.qualifier (this, "background", INT(0));   \
    if (0 == IS_INT(_v_background)) {                                      \
      if (IS_NULL(_v_background))                                          \
        _background = 1;                                                   \
      else                                                                 \
        THROW(LA_ERR_TYPE_MISMATCH, "background: awaiting an integer qualifier"); \
     } else                                                                \
    _background = AS_INT(_v_background);                                   \
  }                                                                        \
  _background;                                                             \
})

#define IS_TMPNAME(__v__)({                                               \
  int _r_ = 0;                                                            \
  if (IS_OBJECT(__v__)) {                                                 \
    object *_o_ = AS_OBJECT(__v__);                                       \
    _r_ = Cstring.eq (_o_->name, "TmpnameType");                          \
  }                                                                       \
  _r_;                                                                    \
})

#define AS_TMPNAME(__v__) ({                                              \
  object *_o_ = AS_OBJECT(__v__);                                         \
  tmpfname_t *_s_ = (tmpfname_t *) AS_OBJECT (_o_->value);                \
  _s_;                                                                    \
})

#endif /* WITHOUT_LA_FUNCTIONS */

#endif /* LA_HDR */
