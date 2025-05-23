#define REQUIRE_STDIO
#define REQUIRE_STR_EQ
#define REQUIRE_STR_EQ_N
#define REQUIRE_STR_COPY
#define REQUIRE_STR_COPY_FMT
#define REQUIRE_STR_CHR
#define REQUIRE_STR_GET_SUBSTR_AT
#define REQUIRE_STR_STR
#define REQUIRE_STRTOD
#define REQUIRE_BYTELEN
#define REQUIRE_STRING
#define REQUIRE_FILE_EXISTS
#define REQUIRE_FILE_IS_REG
#define REQUIRE_MAKE_DIR
#define REQUIRE_MAP
#define REQUIRE_TALLOC
#define REQUIRE_PATH_IS_ABSOLUTE
#define REQUIRE_PATH_BASENAME_SANS_EXTNAME
#define REQUIRE_PATH_BASENAME
#define REQUIRE_PATH_DIRNAME
#define REQUIRE_PATH_EXTNAME
#define REQUIRE_REALPATH
#define REQUIRE_GETENV
#define REQUIRE_UTF8_ENCODE
#define REQUIRE_UTF8_CHARACTER
#define REQUIRE_UTF8_CHARLEN
#define REQUIRE_UTF8_CASE
#define REQUIRE_STAT

#include <libc.h>

#define ifnot(__expr__) if (0 == (__expr__))

#include <z/list.h>
#include <z/l.h>

#define NS_GLOBAL          "global"
#define NS_GLOBAL_LEN      6
#define NS_PRIVATE         "private"
#define NS_PRIVATE_LEN     7
#define NS_STD             "std"
#define NS_STD_LEN         3
#define NS_LOOP_BLOCK      "__block_loop__"
#define NS_LOOP_BLOCK_LEN  14
#define NS_IF_BLOCK        "__block_if__"
#define NS_IF_BLOCK_LEN    12
#define NS_CHAIN_BLOCK      "__chain_block__"
#define NS_CHAIN_BLOCK_LEN  15
#define NS_ANON            "anonymous"
#define NS_ANON_LEN        9
#define L_EXTENSION       "lai"
#define L_STRING_NS       "__string__"
#define L_OPERATORS       "=+-!/*%<>&|^:"

#define MALLOCED_STRING_STATE         (1 << 0)
#define LOOP_STATE                    (1 << 1)
#define LITERAL_STRING_STATE          (1 << 2)
#define FUNC_CALL_RESULT_IS_MMT       (1 << 3)
#define LOADFILE_SILENT               (1 << 4)
#define INDEX_STATE                   (1 << 5)
#define MAP_STATE                     (1 << 6)
#define BLOCK_STATE                   (1 << 7)
#define VAR_IS_NOT_ALLOWED            (1 << 8)
#define CONSUME_STATE                 (1 << 9)
#define CLOSURE_STATE                 (1 << 10)

#define MAP_METHOD_STATE              (1 << 0)
#define OBJECT_RELEASE_STATE          (1 << 1)
#define EVAL_UNIT_STATE               (1 << 2)
#define RETURN_STATE                  (1 << 3)
#define CHAIN_STATE                   (1 << 4)

#define FMT_LITERAL                   (1 << 0)

#define OBJECT_APPEND                 (1 << 0)
#define ARRAY_MEMBER                  (1 << 1)
#define MAP_MEMBER                    (1 << 2)
#define MAP_ASSIGNMENT                (1 << 3)
#define FUNC_OVERRIDE                 (1 << 4)
#define LHS_STRING_RELEASED           (1 << 5)
#define RHS_STRING_RELEASED           (1 << 6)
#define ANNON_ARRAY                   (1 << 7)

#define ARRAY_IS_REASSIGNMENT         (1 << 0)
#define ARRAY_IS_MAP_MEMBER           (1 << 1)

#define PRIVATE_SCOPE                 0
#define PUBLIC_SCOPE                  1

#define ARRAY_LITERAL                 -10000
#define MAP_LITERAL                   -110000
#define MALLOCED_STRING               -310000
#define STRING_LITERAL                -210000
#define UNDELETABLE                   -100000
#define IS_UNCHANGEABLE               -200000

#define BINOP(_x_) (((_x_) << 8) + BINOP_TYPE)
#define CFUNC(_x_) (((_x_) << 8) + CFUNC_TYPE)

#define IS_UFUNC(_t_) ((_t_ & 0xff) == UFUNC_TYPE)
#define IS_CFUNC(_t_) ((_t_ & 0xff) == CFUNC_TYPE)

#define CFUNC_TYPE       'B'
#define UFUNC_TYPE       'f'
#define BINOP_TYPE       'o'

#define TOKEN_EOF        -1
#define TOKEN_STRING      9
#define TOKEN_ARRAY      11
#define TOKEN_MAP        16
#define TOKEN_LIST       17
#define TOKEN_OBJECT     18
#define TOKEN_FILEPTR    19
#define TOKEN_FD         24
#define TOKEN_SYMBOL     'A'
#define TOKEN_BUILTIN    'B'
#define TOKEN_CHAR       'C'
#define TOKEN_REPEAT     'D'
#define TOKEN_ELSEIF     'E'
#define TOKEN_FUNCDEF    'F'
#define TOKEN_UNTIL      'G'
#define TOKEN_FORMAT     'H'
#define TOKEN_IFNOT      'I'
#define TOKEN_ORELSE     'J'
#define TOKEN_THEN       'K'
#define TOKEN_TIMES      'L'
#define TOKEN_IN         'M'
#define TOKEN_CONTINUE   'N'
#define TOKEN_APPEND     'O'
#define TOKEN_PRINT      'P'
#define TOKEN_EVALFILE   'Q'
#define TOKEN_RETURN     'R'
#define TOKEN_AS         'S'
#define TOKEN_TYPE       'T'
#define TOKEN_NEW        'U'
#define TOKEN_VARDEF     'V'
#define TOKEN_EXIT       'X'
#define TOKEN_ARYDEF     'Y'
#define TOKEN_SYNTAX_ERR 'Z'
#define TOKEN_BREAK      'b'
#define TOKEN_CONSTDEF   'c'
#define TOKEN_NUMBER     'd'
#define TOKEN_ELSE       'e'
#define TOKEN_USRFUNC    'f'
#define TOKEN_PUBLIC     'g'
#define TOKEN_PRIVATE    'h'
#define TOKEN_IF         'i'
#define TOKEN_OVERRIDE   'j'
#define TOKEN_END        'k'
#define TOKEN_FOR        'l'
#define TOKEN_FOREVER    'm'
#define TOKEN_INTEGER    'n'
#define TOKEN_BINOP      'o'
#define TOKEN_PRINTLN    'p'
#define TOKEN_HEX_CHAR   'q'
#define TOKEN_INCLUDE    'r'
#define TOKEN_IMPORT     's'
#define TOKEN_OCTAL      't'
#define TOKEN_BINARY     'u'
#define TOKEN_VAR        'v'
#define TOKEN_WHILE      'w'
#define TOKEN_HEX_NUMBER 'x'
#define TOKEN_BLOCK      'z'
#define TOKEN_NL         '\n'
#define TOKEN_DQUOTE     '"'
#define TOKEN_COMMENT    '#'
#define TOKEN_DOLLAR     '$'
#define TOKEN_PAREN_OPEN '('
#define TOKEN_PAREN_CLOS ')'
#define TOKEN_STAR       '*'
#define TOKEN_COMMA      ','
#define TOKEN_DOT        '.'
#define TOKEN_COLON      ':'
#define TOKEN_SEMICOLON  ';'
#define TOKEN_AT         '@'
#define TOKEN_SQUOTE     '\''
#define TOKEN_INDEX_OPEN '['
#define TOKEN_SLASH      '\\'
#define TOKEN_ESCAPE_CHR TOKEN_SLASH
#define TOKEN_INDEX_CLOS ']'
#define TOKEN_BQUOTE     '`'
#define TOKEN_BLOCK_OPEN '{'
//#define TOKEN_BAR        '|'
#define TOKEN_MAP_OPEN   TOKEN_BLOCK_OPEN
#define TOKEN_BLOCK_CLOS '}'
#define TOKEN_MAP_CLOS   TOKEN_BLOCK_CLOS
#define TOKEN_UNARY      '~'
#define TOKEN_LIST_FUN    127
#define TOKEN_LIST_NEW    128
#define TOKEN_ANNOTATION  129
#define TOKEN_CALLBACK    130

//#define TOKEN_NULL       '0'
#define TOKEN_ASSIGN      1000
#define TOKEN_ASSIGN_APP  1001
#define TOKEN_ASSIGN_SUB  1002
#define TOKEN_ASSIGN_DIV  1003
#define TOKEN_ASSIGN_MUL  1004
#define TOKEN_ASSIGN_MOD  1005
#define TOKEN_ASSIGN_BAR  1006
#define TOKEN_ASSIGN_AND  1007
#define TOKEN_ASSIGN_XOR  1008
#define TOKEN_PLUS_PLUS   1009
#define TOKEN_MINUS_MINUS 1010
#define TOKEN_REASSIGN    1011
#define TOKEN_ASSIGN_LAST_VAL TOKEN_REASSIGN

#define LIST_APPEND       1
#define LIST_PREPEND      0

#define NOT_INTO_MAPDECL  0
#define INTO_MAPDECL      1

typedef struct l_string {
  uint len;
  const char *ptr;
} l_string;

typedef struct malloced_string malloced_string;

struct malloced_string {
  string *data;
  malloced_string *next;
};

typedef struct sym_t {
  int type;
  int is_const;
  VALUE value;
  funT *scope;
} sym_t;

typedef struct funstack_t funstack_t;

struct funstack_t {
  funT *f;
  funstack_t *next;
};

typedef struct fun_stack {
  funstack_t *head;
} fun_stack;

typedef struct symbolstack_t symbolstack_t;

struct symbolstack_t {
  Map_Type *symbols;
  symbolstack_t *next;
};

typedef struct symbol_stack {
  symbolstack_t *head;
} symbol_stack;

typedef struct module_so module_so;

struct module_so {
  char *name;
  void *handle;
  ModuleInit init;
  ModuleDeinit deinit;
  l_t *instance;
  module_so *next;
};

typedef struct modules {
  module_so *head;
} modules;

struct funType {
  char funname[MAXLEN_SYMBOL + 1];

  l_string body;

  int nargs;

  char argName[MAX_BUILTIN_PARAMS][MAXLEN_SYMBOL + 1];

  Map_Type *symbols;
  Map_Type *block_symbols;

  VALUE result;
  malloced_string *head;

  funT
    *root,
    *prev,
    *next;

  modules *modules;
};

typedef struct funNewArgs {
  const char *name;
  size_t namelen;
  l_string body;
  int nargs;
  int num_symbols;
  funT *parent;
  funT *root;
} funNewArgs;

#define funNew(...) (funNewArgs) {   \
  .name = NULL,                      \
  .namelen = 0,                      \
  .nargs = 0,                        \
  .num_symbols = 32,                 \
  .parent = NULL,                    \
  .root = NULL,                      \
  __VA_ARGS__}

struct l_prop {
  int name_gen;

  l_t *head;
  int num_instances;
  int current_idx;
};

typedef struct qualifier_t qualifier_t;

struct qualifier_t {
  char name[MAXLEN_SYMBOL + 1];
  Map_Type *qualifiers;
  qualifier_t *next;
};

typedef struct qualifier_stack {
  qualifier_t *head;
} qualifier_stack;

struct l_t {
  funT *function;
  funT *std;
  funT *datatypes;
  funT *curScope;
  funT *private_maps;

  fun_stack funstack[1];
  symbol_stack symbolstack[1];
  qualifier_stack qualifiers_stack[1];
  Map_Type *funRefcount;
  Map_Type *units;
  Map_Type *types;

  char name[32];
  const char *script_buffer;

  char symKey[MAXLEN_SYMBOL + 1];
  char curMapKey[MAXLEN_SYMBOL + 1];
  char curFunName[MAXLEN_SYMBOL + 1];
  char curCFunName[MAXLEN_SYMBOL + 1];
  char curMsg[MAXLEN_MSG + 1];

  funT *curFunDef;

  sym_t
    *curSym,
    *file,
    *func;

  int
    Errno,
    CFuncError,
    curToken,
    prevToken,
    curState,
    funcState,
    objectState,
    scopeState,
    fmtState,
    fmtRefcount,
    exitValue,
    tokenArgs,
    hasToReturn,
    didExit,
    loopCount,
    breakCount,
    argCount,
    byteCount,
    qualifierCount,
    exprList,
    conditionState;

  int consumed_ws;

  size_t anon_id;

  string
    *l_dir,
    *message,
    *evalFile;

  l_string
    tokenStr,
    parsePtr;

  VALUE
    appendValue,
    condValue,
    tokenVal,
    funArgs[MAX_BUILTIN_PARAMS];

  integer stackValIdx;
  VALUE stackVal[MAX_BUILTIN_PARAMS];

  sym_t
    *tokenSym;

  FILE
    *print_fp,
    *err_fp,
    *out_fp;

  l_prop *prop;

  void *user_data;

  LPrintByte_cb print_byte;
  LPrintBytes_cb print_bytes;
  LPrintFmtBytes_cb print_fmt_bytes;
  LSyntaxError_cb syntax_error;
  LSyntaxErrorFmt_cb syntax_error_fmt;
  LDefineFuns_cb define_funs_cb;

  l_t *next;

  l_T *root;
};

typedef struct tokenState {
  int curToken;
  int prevToken;
  int tokenArgs;
  l_string tokenStr;
  l_string parsePtr;
  VALUE tokenVal;
  sym_t *tokenSym;
} tokenState;

#define MAX_EXPR_LEVEL 5

#define TOKEN       this->curToken
#define TOKENVAL    this->tokenVal
#define TOKENSYM    this->tokenSym
#define TOKENARGS   this->tokenArgs
#define TOKENSTR    this->tokenStr
#define CONDVAL     this->condValue
#define APPENDVAL   this->appendValue
#define PARSEPTR    this->parsePtr
#define PREVTOKEN   this->prevToken
#define HASTORETURN this->hasToReturn

#define THROW_ERR_IF_ERR(_e_) do { if (_e_ < 0) return _e_; } while (0)
#define THROW_SYNTAX_ERR(__msg__) do { return this->syntax_error (this, __msg__); } while (0)
#define THROW_SYNTAX_ERR_IF_ERR(_e_, _msg_) \
  do { if (_e_ < 0) THROW_SYNTAX_ERR(_msg_); } while (0)
#define THROW_SYNTAX_ERR_IF(_cond_, _msg_) \
  do { if (_cond_) THROW_SYNTAX_ERR(_msg_); } while (0)
#define THROW_SYNTAX_ERR_IFNOT(_cond_, _msg_) \
  do { ifnot (_cond_) THROW_SYNTAX_ERR(_msg_); } while (0)
#define THROW_SYNTAX_ERR_FMT_IF(_cond_,_fmt_, ...) \
  do { if (_cond_) return l_syntax_error_fmt (this, _fmt_, __VA_ARGS__); } while (0)
#define THROW_SYNTAX_ERR_FMT_IFNOT(_cond_,_fmt_, ...) \
  do { if (!(_cond_)) return l_syntax_error_fmt (this, _fmt_, __VA_ARGS__); } while (0)
#define THROW_SYNTAX_ERR_FMT(_fmt_, ...) \
  do { return l_syntax_error_fmt (this, _fmt_, __VA_ARGS__); } while (0)
#define THROW_OUT_OF_BOUNDS(_fmt_, ...) do {                \
  this->print_fmt_bytes (this->err_fp, _fmt_, __VA_ARGS__); \
  this->print_bytes (this->err_fp, "\n");                   \
  return l_err_ptr (this, L_ERR_OUTOFBOUNDS);             \
} while (0)
#define THROW_OUT_OF_BOUNDS_IF(_cond_, _fmt_, ...) if (_cond_) do { \
  this->print_fmt_bytes (this->err_fp, _fmt_, __VA_ARGS__);         \
  this->print_bytes (this->err_fp, "\n");                           \
  return l_err_ptr (this, L_ERR_OUTOFBOUNDS);                     \
} while (0)
#define THROW_UNKNOWN_SYMBOL(_symname_) do {                \
  this->print_fmt_bytes (this->err_fp,                      \
    "unknown symbol: %s\n", cur_msg_str (this, _symname_)); \
  return l_err_ptr (this, L_ERR_UNKNOWN_SYM);             \
} while (0)
#define THROW_INV_ARRAY_TYPE(_tp_) do {                     \
  this->print_fmt_bytes (this->err_fp,                      \
    "invalid array type: %d\n", _tp_);                      \
  return l_err_ptr (this, L_ERR_INV_ARRAY_TYPE);          \
} while (0)
#define THROW_TOOMANY_FUNCTION_ARGS() do {                  \
  this->print_fmt_bytes (this->err_fp,                      \
    "too many function arguments\n");                       \
  return l_err_ptr (this, L_ERR_TOOMANY_FUNCTION_ARGS);   \
} while (0)
#define THROW_NUM_FUNCTION_ARGS_MISMATCH(_n_, _p_) do {         \
  this->print_fmt_bytes (this->err_fp,                          \
    "number argument mismatch, awaiting %d got %d\n", _n_, _p_);\
  return l_err_ptr (this, L_ERR_NUM_FUNCTION_ARGS_MISMATCH);  \
} while (0)
#define THROW_TYPE_MISMATCH(_g_, _a_) do {                  \
  this->print_fmt_bytes (this->err_fp,                      \
    "type mismatch, awaiting %d got %d\n", _a_, _g_);       \
  return l_err_ptr (this, L_ERR_TYPE_MISMATCH);           \
} while (0)
#define THROW_A_TYPE_MISMATCH(_g_, _m_) do {                \
  this->print_fmt_bytes (this->err_fp,                      \
    "type mismatch, got %d, awaiting %s\n", _g_, _m_);      \
  return l_err_ptr (this, L_ERR_TYPE_MISMATCH);           \
} while (0)
#define THROW_SYNTAX_ERR_IF_STD_SYM_EXISTS(_n_)             \
do {                                                        \
  THROW_SYNTAX_ERR_FMT_IF(                                  \
    (NULL != ns_lookup_symbol (this->std, _n_)),         \
    "%s: can not redefine a standard symbol", _n_);         \
} while (0)
#define THROW_SYNTAX_ERR_IF_SYM_NAME_EXCEEDS_LEN(_n_, _l_)  \
do {                                                        \
  THROW_SYNTAX_ERR_FMT_IF(_l_ > MAXLEN_SYMBOL,              \
   "%s: symbol name exceeded maximum length %d", _n_, _l_); \
} while (0)
#define THROW_UNIMPLEMENT(_msg_) do {                       \
  this->print_fmt_bytes (this->err_fp,                      \
    "%s: hasn't been implemented\n", _msg_);                \
  return l_err_ptr (this, L_ERR_SYNTAX);                    \
} while (0)

#define PRINT_ERR_CONSTANT(_e_) do {                                     \
  const char *err_msg_const[] = {                                        \
      "NO MEMORY", "SYNTAX ERROR", "UNKNOWN SYMBOL",                     \
      "UNKNOWN TYPE", "TOOMANY FUNCTION ARGUMENTS",                      \
      "NUM FUNCTION ARGUMENTS MISMATCH", "OUT OF BOUNDS",                \
      "TYPE MISMATCH", "INVALID ARRAY TYPE", "LOAD ERROR",               \
      "IMPORT ERROR", "DYNAMIC LINKING ERROR"};                          \
  this->print_fmt_bytes (this->err_fp, "%s\n", err_msg_const[-_e_ - 2]); \
} while (0)

#define SAVE_TOKENSTATE() l_save_token_state (this)
#define RESTORE_TOKENSTATE(_s_) do {l_restore_token_state (this, _s_);} while (0)

#define NEXT_TOKEN()  do { \
  this->prevToken = TOKEN; \
  if (l_do_next_token (this, 0) < TOKEN_EOF) return L_ERR_SYNTAX; \
} while (0)

#define NEXT_RAW_TOKEN()  do { \
  this->prevToken = TOKEN;     \
  if (l_do_next_token (this, 1) < TOKEN_EOF) return L_ERR_SYNTAX; \
} while (0)

#define RESET_TOKEN do {                    \
  SETSTRLEN(TOKENSTR, 0);                   \
  SETSTRPTR(TOKENSTR, GETSTRPTR(PARSEPTR)); \
} while (0)

#define RESET_PARSEPTR do { PARSEPTR.len = 0; } while (0)

#define GET_BYTE() l_get_byte (this)
#define UNGET_BYTE() do {                       \
  SETSTRLEN(PARSEPTR, GETSTRLEN(PARSEPTR) + 1); \
  SETSTRPTR(PARSEPTR, GETSTRPTR(PARSEPTR) - 1); \
  IGNORE_LAST_TOKEN;                            \
} while (0)

#define PEEK_NTH_TOKEN(_n_) l_peek_token (this, _n_)
#define PEEK_NTH_BYTE(_n_) l_peek_nth_byte (this, _n_)
#define PEEK_NTH_BYTE_NOWS_NONL(_n_) l_peek_nth_byte_nows_nonl (this, _n_)
#define PEEK_NTH_BYTE_NOWS_INLINE(_n_) l_peek_nth_byte_nows_inline (this, _n_)
#define PEEK_NUM_WS() l_peek_num_ws(this)
#define NEXT_BYTE_NOWS_NONL() l_next_byte_nows_nonl (this)
#define NEXT_BYTE_NOWS() l_next_byte_nows (this)
#define IS_NEXT_BYTE( _c_) (*GETSTRPTR(PARSEPTR) == _c_)
#define ISNOT_NEXT_BYTE( _c_) (*GETSTRPTR(PARSEPTR) != _c_)
#define IS_NEXT_BYTE_NOWS(_c_) l_is_next_byte_nows (this, _c_)

#define IGNORE_NEXT_NUM_BYTES(_n_) do {           \
  SETSTRPTR(PARSEPTR, GETSTRPTR(PARSEPTR) + _n_); \
  SETSTRLEN(PARSEPTR, GETSTRLEN(PARSEPTR) - _n_); \
} while (0)

#define IGNORE_NEXT_BYTE do {                   \
  SETSTRPTR(PARSEPTR, GETSTRPTR(PARSEPTR) + 1); \
  SETSTRLEN(PARSEPTR, GETSTRLEN(PARSEPTR) - 1); \
} while (0)

#define IGNORE_UNTIL_KNOWN_BYTE(_c_) do {       \
  int _c = *GETSTRPTR(PARSEPTR) == _c_;         \
  SETSTRPTR(PARSEPTR, GETSTRPTR(PARSEPTR) + 1); \
  SETSTRLEN(PARSEPTR, GETSTRLEN(PARSEPTR) - 1); \
  if (_c) break;                                \
} while (1)

#define IGNORE_FIRST_TOKEN do {                 \
  SETSTRPTR(TOKENSTR, GETSTRPTR(TOKENSTR) + 1); \
  SETSTRLEN(TOKENSTR, GETSTRLEN(TOKENSTR) - 1); \
} while (0)

#define IGNORE_LAST_TOKEN do {                  \
  SETSTRLEN(TOKENSTR, GETSTRLEN(TOKENSTR) - 1); \
} while (0)

#define GETSTRLEN(_s_) _s_.len
#define GETSTRPTR(_s_) _s_.ptr
#define SETSTRLEN(_s_, _l_) _s_.len = _l_
#define SETSTRPTR(_s_, _p_) _s_.ptr = _p_
#define TOKENSTRBYTES GETSTRPTR(TOKENSTR)

#define C_THROW(__e__, __m__) do {                \
  l_set_CFuncError (this,  __e__);               \
  l_set_function_curMsg (this, __func__, __m__); \
  return NULL_VALUE;                              \
} while (0)

typedef struct listNode listNode;

struct listNode {
  VALUE *value;
  listNode *next;
  listNode *prev;
};

struct listType {
  listNode *head;
  listNode *tail;
  listNode *current;

  int num_items;
  int cur_idx;
};

static int l_do_next_token (l_t *, int);
static int l_parse_iforelse (l_t *, int, VALUE *);
static int l_parse_print (l_t *);
static int l_parse_println (l_t *);
static int l_eval_string (l_t *, const char *);
static int l_parse_cond (l_t *, int);
static int l_parse_stmt (l_t *);
static int l_parse_expr (l_t *, VALUE *);
static int l_parse_primary (l_t *, VALUE *);
static int l_parse_func_def (l_t *);
static int l_eval_file (l_t *, const char *);
static int l_parse_fmt (l_t *, string *, int);
static int l_parse_string_get (l_t *, VALUE *);
static int l_parse_array_set (l_t *);
static int l_parse_string_set_char (l_t *, VALUE, int);
static inline sym_t *ns_lookup_symbol (funT *, const char *);
static sym_t *l_lookup_symbol (l_t *, l_string);
static sym_t *l_define_symbol (l_t *, funT *, const char *, int, VALUE, int);
static VALUE l_release_value (l_t *, VALUE);
static VALUE l_string_release (VALUE);
static VALUE l_copy_map (VALUE);
static VALUE l_map_release (VALUE);
static VALUE l_copy_value (VALUE v);
static VALUE l_set_errno (l_t *, VALUE);
static VALUE l_equals (l_t *, VALUE, VALUE);
static VALUE l_ne (l_t *, VALUE, VALUE);
static VALUE l_mul  (l_t *, VALUE, VALUE);
static VALUE l_add  (l_t *, VALUE, VALUE);
static VALUE l_sub  (l_t *, VALUE, VALUE);
static VALUE l_div  (l_t *, VALUE, VALUE);
static VALUE l_mod  (l_t *, VALUE, VALUE);
static VALUE l_bset (l_t *, VALUE, VALUE);
static VALUE l_bnot (l_t *, VALUE, VALUE);
static VALUE l_bitxor (l_t *, VALUE, VALUE);
static void l_release_map_val (map_type *);
static VALUE array_release (VALUE);
static ArrayType *array_copy (ArrayType *);
static int l_parse_annotation (l_t *, VALUE *);
static int l_array_set_as_array (l_t *, VALUE, integer, integer, integer);
static int l_array_assign (l_t *, VALUE *, VALUE, VALUE, int);
static int l_parse_array_def (l_t *, VALUE *, int);
static int l_parse_array_get (l_t *, VALUE *);
static int l_parse_func_call (l_t *, VALUE *, CFunc, funT *, VALUE);
static int l_parse_map_get (l_t *, VALUE *);
static int l_parse_map_set (l_t *);
static int l_map_set_value (l_t *, Map_Type *, const char *, VALUE, int);
static int l_parse_include (l_t *);
static int l_parse_chain (l_t *, VALUE *);
static void l_set_CFuncError (l_t *, int);
static void l_set_curMsg (l_t *, const char *);
static void l_set_function_curMsg (l_t *, const char *, const char *);
static void l_set_Errno (l_t *, int);
static int l_peek_nth_byte_nows_inline (l_t *, uint *);

static void l_set_message (l_t *this, int append, const char *msg) {
  if (NULL == msg) return;
  if (append)
    string_append_with (this->message, msg);
  else
    string_replace_with (this->message, msg);
}

static void l_set_message_fmt (l_t *this, int append, const char *fmt, ...) {
  size_t len = STRING_FORMAT_SIZE(fmt);
  char bytes[len + 1];

  va_list ap; va_start(ap, fmt);
  sys_vsnprintf (bytes, len + 1, fmt, ap);
  va_end(ap);

  l_set_message (this, append, bytes);
}

static inline l_string StringNewLen (const char *str, integer len) {
  l_string x;
  SETSTRLEN(x, len);
  SETSTRPTR(x, str);
  return x;
}

static inline l_string StringNew (const char *str) {
  l_string x;
  SETSTRLEN(x, bytelen (str));
  SETSTRPTR(x, str);
  return x;
}

static void l_print_lastring (l_t *this, FILE *fp, l_string s) {
  uint len = GETSTRLEN(s);
  const char *ptr = (const char *) GETSTRPTR(s);
  while (len > 0) {
    this->print_byte (fp, *ptr);
    ptr++;
    --len;
  }
}

static int l_err_ptr (l_t *this, int err) {
  const char *keep = GETSTRPTR(PARSEPTR);
  size_t len = GETSTRLEN(PARSEPTR);

  char *sp = (char *) keep;

  if (sp > this->script_buffer)
    if (*sp == '\n' || *(sp - 1) == '\n') sp--;

  while (sp > this->script_buffer &&
      0 == str_chr (";\n", *(sp - 1)) &&
      *(sp - 1) >= ' ')
    sp--;

  size_t n_len = (keep - sp);

  SETSTRPTR(PARSEPTR, sp);
  SETSTRLEN(PARSEPTR, n_len);

  sp = (char *) keep;
  int linenum = 0;
  while (*sp++)
    if (*sp == TOKEN_NL)
      if (++linenum > 9) break;

  n_len += (sp - keep);
  SETSTRLEN(PARSEPTR, n_len);

  l_print_lastring (this, this->err_fp, PARSEPTR);

  SETSTRPTR(PARSEPTR, keep);
  SETSTRLEN(PARSEPTR, len);

  this->print_bytes (this->err_fp, "\n");

  return err;
}

static void l_print_current_line (l_t *this, const char *msg, FILE *fp) {
  const char *sp = GETSTRPTR(PARSEPTR);
  const char *end = sp;
  int max = 80;
  while (max-- &&
      sp > this->script_buffer &&
      0 == str_chr (";\n", *(sp - 1)) &&
      *(sp - 1) >= ' ')
    sp--;

  while (*end) {
    if (*(end + 1))
      if (str_chr (";\n", *(end + 1)))
        break;

    end++;
  }

  if (fp == NULL) fp = this->err_fp;

  if (NULL != msg) sys_fprintf (fp, "%s", msg);

  for (int i = 0; i < end - sp; i++)
    sys_fprintf (fp, "%c", sp[i]);

  sys_fprintf (fp, "\n");
}

static int l_syntax_error (l_t *this, const char *msg) {
  this->print_fmt_bytes (this->err_fp, "%s\n", msg);
  return l_err_ptr (this, L_ERR_SYNTAX);
}

static inline char *c_funname (l_t *this, l_string x) {
  str_copy (this->curCFunName, MAXLEN_SYMBOL + 1, GETSTRPTR(x), GETSTRLEN(x));
  return this->curCFunName;
}

static int l_syntax_error_fmt (l_t *this, const char *fmt, ...) {
  size_t len = STRING_FORMAT_SIZE (fmt);
  char bytes[len + 1];

  va_list ap; va_start(ap, fmt);
  sys_vsnprintf (bytes, len + 1, fmt, ap);
  va_end(ap);

  return this->syntax_error (this, bytes);
}

static inline char *sym_key (l_t *this, l_string x) {
  str_copy (this->symKey, MAXLEN_SYMBOL + 1,
      GETSTRPTR(x), GETSTRLEN(x));
  return this->symKey;
}

static inline char *map_key (l_t *this, l_string x) {
  str_copy (this->curMapKey, MAXLEN_SYMBOL + 1,
      GETSTRPTR(x), GETSTRLEN(x));
  return this->curMapKey;
}

static inline char *cur_msg_str (l_t *this, l_string x) {
  str_copy (this->curMsg, MAXLEN_MSG + 1,
      GETSTRPTR(x), GETSTRLEN(x));
  return this->curMsg;
}

static inline int is_space (int c) {
  return (c == ' ') || (c == '\t') || (c == '\r');
}

static inline int is_digit (int c) {
  return (c >= '0' && c <= '9');
}

static inline int is_hexchar (int c) {
  return (c >= '0' && c <= '9') || str_chr ("abcdefABCDEF", c);
}

static inline int is_lower (int c) {
  return (c >= 'a' && c <= 'z');
}

static inline int is_upper (int c) {
  return (c >= 'A' && c <= 'Z');
}

static inline int is_alpha (int c) {
  return is_lower (c) || is_upper (c);
}

/*
static inline int is_idpunct (int c) {
  return NULL != str_chr (".:_", c);
}
*/

static inline int is_identifier (int c) {
  return is_alpha (c) || c == '_' || is_digit (c);
}

static inline int is_operator (int c) {
  return NULL != str_chr (L_OPERATORS, c);
}

static inline int is_operator_span (int c) {
  return NULL != str_chr ("=<>&|^+-", c);
}

static tokenState l_save_token_state (l_t *this) {
  return (tokenState) {
    .curToken  = TOKEN,
    .prevToken = PREVTOKEN,
    .tokenArgs = TOKENARGS,
    .tokenStr  = TOKENSTR,
    .tokenVal  = TOKENVAL,
    .tokenSym  = TOKENSYM,
    .parsePtr  = PARSEPTR
  };
}

static void l_restore_token_state (l_t *this, tokenState tokenstate) {
  TOKEN     = tokenstate.curToken;
  PREVTOKEN = tokenstate.prevToken;
  TOKENARGS = tokenstate.tokenArgs;
  TOKENSTR  = tokenstate.tokenStr;
  TOKENVAL  = tokenstate.tokenVal;
  TOKENSYM  = tokenstate.tokenSym;
  PARSEPTR  = tokenstate.parsePtr;
}

static int l_peek_token (l_t *this, uint n) {
  tokenState save = SAVE_TOKENSTATE();

  do {
    int err = l_do_next_token (this, 0);
    if (err < TOKEN_EOF) return err;
  } while (n--);

  int retval = TOKEN;
  RESTORE_TOKENSTATE(save);
  return retval;
}

static int l_is_next_byte_nows (l_t *this, int c) {
  const char *ptr = GETSTRPTR(PARSEPTR);
  while (is_space (*ptr)) ptr++;
  return *ptr == c;
}

static int l_peek_nth_byte (l_t *this, uint n) {
  if (GETSTRLEN(PARSEPTR) <= n) return TOKEN_EOF;
  return *(GETSTRPTR(PARSEPTR) + n);
}

static int l_get_byte (l_t *this) {
  uint len = GETSTRLEN(PARSEPTR);

  ifnot (len) return TOKEN_EOF;

  const char *ptr = GETSTRPTR(PARSEPTR);
  int c = *ptr++;

  --len;

  SETSTRPTR(PARSEPTR, ptr);
  SETSTRLEN(PARSEPTR, len);
  SETSTRLEN(TOKENSTR, GETSTRLEN(TOKENSTR) + 1);
  return c;
}

static int l_peek_num_ws (l_t *this) {
  int n = 0;
  const char *ptr = GETSTRPTR(PARSEPTR);
  while (is_space (*ptr++)) n++;
  return n;
}

static int l_ignore_ws (l_t *this) {
  int c;

  this->consumed_ws = 0;

  for (;;) {
    c = GET_BYTE();

    if (is_space (c)) {
      RESET_TOKEN;
      this->consumed_ws++;
      continue;
    }

    break;
  }

  return c;
}

static int l_peek_nth_byte_nows_nonl (l_t *this, uint *n) {
  int c;
  while (1) {
    c = PEEK_NTH_BYTE(*n);

    if (0 == is_space (c) && c != '\n')
      return c;

    *n += 1;
  }

  return TOKEN_EOF;
}

static int l_next_byte_nows_nonl (l_t *this) {
  uint n = 0;
  return PEEK_NTH_BYTE_NOWS_NONL(&n);
}

static int l_next_byte_nows (l_t *this) {
  uint n = 0;
  return PEEK_NTH_BYTE_NOWS_INLINE (&n);
}

static int l_peek_nth_byte_nows_inline (l_t *this, uint *n) {
  int c;
  while (1) {
    c = PEEK_NTH_BYTE(*n);
    ifnot (is_space (c)) return c;
    *n += 1;
  }

  return TOKEN_EOF;
}

static void l_get_span (l_t *this, int (*testfn) (int)) {
  int c;
  do
    c = GET_BYTE();
  while (testfn (c));

  if (c != TOKEN_EOF) UNGET_BYTE();
}

static int l_get_opened_block (l_t *this, const char *msg) {
  int bracket = 1;
  int c;
  int prev_c = 0;
  int in_str = 0;
  int in_com = 0;
  char str_tok = 0;

  while (bracket > 0) {
    c = GET_BYTE();

    if (c == L_NOTOK || c == TOKEN_EOF) {
      const char *keep = GETSTRPTR(PARSEPTR);
      size_t len = GETSTRLEN(PARSEPTR);

      char *sp = (char *) keep;
      while (sp > this->script_buffer) sp--;
      size_t n_len = (keep - sp);

      size_t n = 0;
      for (n = 0; n < 256 && n < n_len; n++)
        UNGET_BYTE();

      this->print_fmt_bytes (this->err_fp, "unended %s block\n",
          ((str_eq_n (this->curScope->funname, NS_IF_BLOCK, NS_IF_BLOCK_LEN))
            ? "if" :
            (str_eq_n (this->curScope->funname, NS_LOOP_BLOCK, NS_LOOP_BLOCK_LEN))
              ? "loop" :
              this->curScope->funname));

      for (size_t i = 0; i < n; i++)
        this->print_byte (this->err_fp, GET_BYTE());

      SETSTRPTR(PARSEPTR, keep);
      SETSTRLEN(PARSEPTR, len);
      THROW_SYNTAX_ERR(msg);
    }

    if (c == TOKEN_COMMENT) {
      ifnot (in_str) {
        in_com = 1;
        continue;
      }
    }

    if (c == TOKEN_NL) {
      if (in_com) {
        ifnot (in_str) {
          in_com = 0;
          continue;
        }
      }
    }

    if (in_com)
      continue;

    if ((c == TOKEN_DQUOTE || c == TOKEN_BQUOTE)) {
      if (prev_c != TOKEN_ESCAPE_CHR) {
        if (in_str) {
          if (str_tok != c) continue;
          in_str--;
        } else {
          in_str++;
          str_tok = c;
          continue;
        }
      }
    }

    prev_c = c;

    if (in_str) continue;

    switch (c) {
      case TOKEN_BLOCK_CLOS: --bracket; break;
      case TOKEN_BLOCK_OPEN: ++bracket; break;
    }
  }

  return L_OK;
}

static VALUE l_errno_string (l_t *this, VALUE v_err) {
  (void) this;
  int errnum = AS_INT(v_err);
  char *buf = errno_string (errnum);
  string *estr = string_new_with (buf);
  return STRING(estr);
}

static VALUE l_errno_name (l_t *this, VALUE v_err) {
  (void) this;
  int errnum = AS_INT(v_err);
  char *buf = errno_name (errnum);
  string *estr = string_new_with (buf);
  return STRING(estr);
}

static VALUE l_typeof (l_t *this, VALUE value) {
  (void) this;
  VALUE v;

  if (IS_UFUNC(value.type) || value.type & UFUNCTION_TYPE)
    v = INT(UFUNCTION_TYPE);
  else if (IS_CFUNC(value.type))
    v = INT(CFUNCTION_TYPE);
  else
    v = INT(value.type);
  return v;
}

static VALUE l_typeofArray (l_t *this, VALUE value) {
  (void) this;
  VALUE v = INT(value.type);
  if (value.type != ARRAY_TYPE) {
    v = INT(L_NOTOK);
    return v;
  }

  ArrayType *array = (ArrayType *) AS_ARRAY(value);
  v = INT(array->type);
  return v;
}

static VALUE l_typeAsString (l_t *this, VALUE value) {
  (void) this;
  string *buf = string_new (16);

  switch (value.type) {
    case INTEGER_TYPE: string_append_with_len (buf, "IntegerType", 11); break;
    case FILEPTR_TYPE: string_append_with_len (buf, "FilePtrType", 11); break;
    case NUMBER_TYPE:  string_append_with_len (buf, "NumberType",  10); break;
    case STRING_TYPE:  string_append_with_len (buf, "StringType",  10); break;
    case ARRAY_TYPE:   string_append_with_len (buf, "ArrayType",    9); break;
    case NULL_TYPE:    string_append_with_len (buf, "NullType",     8); break;
    case LIST_TYPE:    string_append_with_len (buf, "ListType",     8); break;
    case MAP_TYPE:     string_append_with_len (buf, "MapType",      7); break;
    case FD_TYPE:      string_append_with_len (buf, "FdType",       6); break;
    case OBJECT_TYPE: {
      object *o = AS_OBJECT(value); string_append_with (buf, o->name);
      break;
    }
    default:
      if (IS_UFUNC(value.type) || value.type & UFUNCTION_TYPE)
        string_append_with_len (buf, "FunctionType", 12);
      else if (IS_CFUNC(value.type))
        string_append_with_len (buf, "CFunctionType", 13);
      else
        string_append_with_len (buf, "UnknownType", 11);
      break;
  }

  VALUE v = STRING (buf);
  return v;
}

static VALUE l_typeArrayAsString (l_t *this, VALUE value) {
  VALUE v;
  if (value.type != ARRAY_TYPE)
    v.type = L_NOTOK;
  else {
    ArrayType *array = (ArrayType *) AS_ARRAY(value);
    v.type = array->type;
  }

  return l_typeAsString (this, v);
}

static VALUE l_is_defined (l_t *this, VALUE v_val) {
  ifnot (IS_STRING(v_val)) C_THROW(L_ERR_TYPE_MISMATCH, "awaiting a string");
  char *val = AS_STRING_BYTES(v_val);

  funT *f = this->curScope;

  while (NULL != f) {
    ifnot (NULL == ns_lookup_symbol (f, val))
      return TRUE_VALUE;

    f = f->prev;
  }

  return FALSE_VALUE;
}

static void _l_set_qualifiers (l_t *this, VALUE v_qual, funT *uf) {
  qualifier_t *q = Alloc (sizeof (qualifier_t));
  q->qualifiers = AS_MAP(v_qual);
  if (uf)
    str_copy (q->name, MAXLEN_SYMBOL + 1, uf->funname, bytelen (uf->funname));
  else
    str_copy (q->name, MAXLEN_SYMBOL + 1, this->curCFunName, bytelen (this->curCFunName));

  ListStackPush (this->qualifiers_stack, q);
}


static void l_set_qualifiers (l_t *this, Map_Type *qual, const char *funname) {
  qualifier_t *q = Alloc (sizeof (qualifier_t));
  q->qualifiers = qual;
  str_copy (q->name, MAXLEN_SYMBOL + 1, funname, bytelen (funname));

  ListStackPush (this->qualifiers_stack, q);
}

static void l_release_qualifiers (l_t *this, char *name) {
  qualifier_t *tmp = NULL;
  qualifier_t *it = this->qualifiers_stack->head;
  while (it) {
    if (str_eq (it->name, name)) {
      l_map_release (MAP(it->qualifiers));
      if (tmp)
        tmp->next = it->next;
      else
        this->qualifiers_stack->head = it->next;

      Release (it);
      break;
    }

    tmp = it;
    it = it->next;
  }
}

static VALUE l_qualifiers (l_t *this) {
  funT *scope = this->curScope;
  while (scope && str_eq_n (scope->funname, "__block_", 8))
    scope = scope->prev;
  char *funname = scope->funname;

  qualifier_t *it = this->qualifiers_stack->head;
  while (it) {
    if (str_eq (it->name, funname))
      return l_copy_map (MAP(it->qualifiers));
    it = it->next;
  }

  return NULL_VALUE;
}

static Map_Type *l_get_qualifiers (l_t *this) {
  qualifier_t *it = this->qualifiers_stack->head;
  while (it) {
    if (str_eq (it->name, this->curCFunName))
      return it->qualifiers;
    it = it->next;
  }

  return NULL;
}

static VALUE l_qualifier_exists (l_t *this, VALUE v_key) {
  ifnot (IS_STRING(v_key)) C_THROW(L_ERR_TYPE_MISMATCH, "awaiting a string");

  funT *scope = this->curScope;
  while (scope && str_eq_n (scope->funname, "__block_", 8))
    scope = scope->prev;
  char *funname = scope->funname;

  qualifier_t *it = this->qualifiers_stack->head;
  while (it) {
    if (str_eq (it->name, funname))
      return INT(map_key_exists (it->qualifiers, AS_STRING_BYTES(v_key)));

    it = it->next;
  }

  return FALSE_VALUE;
}

static int l_C_qualifier_exists (l_t *this, const char *key) {
  qualifier_t *it = this->qualifiers_stack->head;
  while (it) {
    if (str_eq (it->name, this->curCFunName))
      return map_key_exists (it->qualifiers, (char *) key);

    it = it->next;
  }

  return 0;
}

static VALUE l_qualifier (l_t *this, VALUE v_key, VALUE v_defval) {
  ifnot (IS_STRING(v_key)) C_THROW(L_ERR_TYPE_MISMATCH, "awaiting a string");

  funT *scope = this->curScope;
  while (scope && str_eq_n (scope->funname, "__block_", 8))
    scope = scope->prev;
  char *funname = scope->funname;

  qualifier_t *it = this->qualifiers_stack->head;
  while (it) {
    if (str_eq (it->name, funname)) {
      VALUE *v = (VALUE *) map_get_value (it->qualifiers, AS_STRING_BYTES(v_key));
      if (v == NULL)
        return l_copy_value (v_defval);
      return l_copy_value ((*v));
    }

    it = it->next;
  }

  return v_defval;
}

static VALUE l_get_qualifier (l_t *this, const char *key, VALUE v_defval) {
  qualifier_t *it = this->qualifiers_stack->head;
  while (it) {
    if (str_eq (it->name, this->curCFunName)) {
      VALUE *v = (VALUE *) map_get_value (it->qualifiers, key);
      if (NULL == v) return v_defval;
      return *v;
    }

    it = it->next;
  }

  return v_defval;
}

static object *l_object_new (ObjectRelease o_release, ObjectToString o_tostr, const char *name, VALUE value) {
  object *o = Alloc (sizeof (object));
  o->release = o_release;
  o->toString = o_tostr;
  ifnot (NULL == name)
    str_copy (o->name, MAXLEN_TYPENAME + 1, name, MAXLEN_TYPENAME);
  else
    str_copy (o->name, MAXLEN_TYPENAME + 1, "ObjectType", MAXLEN_TYPENAME);
  o->value = value;
  return o;
}

static VALUE l_fclose (l_t *this, VALUE fp_val) {
  VALUE result = NOTOK_VALUE;

  if (fp_val.type == NULL_TYPE) return result;

  if (fp_val.refcount < 0) return OK_VALUE;

  FILE *fp = AS_FILEPTR(fp_val);

  if (NULL == fp) return result;

  ifnot (NULL == this)
    l_set_Errno (this, 0);

  if (0 != sys_fclose (fp)) {
    ifnot (NULL == this)
      l_set_Errno (this, sys_errno);
  } else
    result = OK_VALUE;

  ifnot (NULL == this) {
    ifnot (this->funcState & OBJECT_RELEASE_STATE) {
      object *o = AS_OBJECT(fp_val);
      Release (o);
    }
  }

  fp = NULL;
  ifnot (NULL == fp_val.sym)
    fp_val.sym->value = NULL_VALUE;

  fp_val.sym = NULL;
  fp_val = NULL_VALUE;

  return result;
}

static VALUE l_fflush (l_t *this, VALUE fp_val) {
  VALUE result = NOTOK_VALUE;

  if (fp_val.type == NULL_TYPE) return result;

  ifnot (IS_FILEPTR(fp_val)) C_THROW(L_ERR_TYPE_MISMATCH, "awaiting a file pointer");

  FILE *fp = AS_FILEPTR(fp_val);
  if (NULL == fp) return result;

  l_set_Errno (this, 0);

  int retval = sys_fflush (fp);
  if (retval) {
    l_set_Errno (this, sys_errno);
    return result;
  }

  return OK_VALUE;
}

static VALUE l_fopen (l_t *this, VALUE fn_value, VALUE mod_value) {
  ifnot (IS_STRING(fn_value))  C_THROW(L_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_STRING(mod_value)) C_THROW(L_ERR_TYPE_MISMATCH, "awaiting a string");

  l_set_Errno (this, 0);

  char *fn = AS_STRING_BYTES(fn_value);
  char *mode = AS_STRING_BYTES(mod_value);
  FILE *fp = sys_fopen (fn, mode);

  if (NULL == fp) {
    l_set_Errno (this, sys_errno);
    return NULL_VALUE;
  }

  VALUE v = FILEPTR(fp);
  object *o = l_object_new (l_fclose, NULL, "FilePtrType", v);
  return FILEPTR(o);
}

static VALUE list_release (l_t *this, VALUE v_list) {
  ifnot (IS_LIST(v_list)) C_THROW(L_ERR_TYPE_MISMATCH, "awaiting a list");
  listType *list = AS_LIST(v_list);
  listNode *it = list->head;

  while (it) {
    listNode *tmp = it->next;
    l_release_value (this, *(it->value));
    Release (it->value);
    Release (it);
    it = tmp;
  }

  Release (list);
  return OK_VALUE; // not really needed (only the test on null can be meaningfull here,
                   // as it is dictated (at least for now) by the C_THROW macro)
}

static VALUE list_set (l_t *this, VALUE v_list, VALUE v_item, int what) {
  ifnot (IS_LIST(v_list)) C_THROW(L_ERR_TYPE_MISMATCH, "awaiting a list");
  listType *list = AS_LIST(v_list);

  listNode *node = Alloc (sizeof (listNode));

  VALUE v;
  if (v_item.sym != NULL)
    v = l_copy_value (v_item);
  else
    v = v_item;

  VALUE *vp = Alloc (sizeof (VALUE));
  *vp = v;
  node->value = vp;
  if (what == LIST_APPEND)
    DListAppend(list, node);
  else
    DListPrepend(list, node);

  return v_list;
}

static VALUE list_prepend (l_t *this, VALUE v_list, VALUE v_item) {
  return list_set (this, v_list, v_item, LIST_PREPEND);
}

static VALUE list_append (l_t *this, VALUE v_list, VALUE v_item) {
  return list_set (this, v_list, v_item, LIST_APPEND);
}

static VALUE list_insert_at (l_t *this, VALUE v_list, VALUE v_idx, VALUE v_v) {
  ifnot (IS_LIST(v_list)) C_THROW(L_ERR_TYPE_MISMATCH, "awaiting a list");
  ifnot (IS_INT(v_idx)) C_THROW(L_ERR_TYPE_MISMATCH, "awaiting an integer");

  listType *list = AS_LIST(v_list);
  int idx = AS_INT(v_idx);
  if (0 > idx)
    idx += list->num_items;

  if (idx <= -1 || idx > list->num_items) {
    char msg[256]; str_copy_fmt (msg, 256,
      "given index %d is > than %d list length, or less than or equal with -1", AS_INT(v_idx), list->num_items);
    C_THROW(L_ERR_OUTOFBOUNDS, msg);
  }

  if (idx == 0)               return list_set (this, v_list, v_v, LIST_PREPEND);
  if (idx == list->num_items) return list_set (this, v_list, v_v, LIST_APPEND);

  idx--;

  listNode *node = Alloc (sizeof (listNode));
  VALUE v = l_copy_value (v_v);
  VALUE *vp = Alloc (sizeof (VALUE));
  *vp = v;
  node->value = vp;
  DListInsertAt(list, node, idx);

  return v_list;
}

static int l_parse_list_get (l_t *this, VALUE *vp) {
  VALUE v_list = TOKENVAL;

  NEXT_TOKEN();
  int c = TOKEN;

  if (c != TOKEN_INDEX_OPEN) {
    if (this->exprList && this->objectState & MAP_ASSIGNMENT)
      *vp = l_copy_value (v_list);
    else
      *vp = v_list;

    return L_OK;
  }

  NEXT_TOKEN();

  VALUE ix;
  int err = l_parse_expr (this, &ix);
  THROW_ERR_IF_ERR(err);
  THROW_SYNTAX_ERR_IF(TOKEN != TOKEN_INDEX_CLOS, "list get: awaiting ]");
  THROW_SYNTAX_ERR_IF(ix.type != INTEGER_TYPE, "awaiting an integer expression, when getting list index");

  listType *list = AS_LIST(v_list);
  int idx = AS_INT(ix);
  if (0 > idx) idx += list->num_items;
  THROW_OUT_OF_BOUNDS_IF(idx <= -1 || idx >= list->num_items, "list index [%d] == out of bounds, actual list length [%d]",
    AS_INT(ix), list->num_items);

  listNode *node = DListGetAt(list, listNode, idx);

  VALUE *val = node->value;

  NEXT_TOKEN();

  if (TOKEN == TOKEN_DOT) {
    THROW_SYNTAX_ERR_IF(val->type != MAP_TYPE, "value is not a map type");
    TOKENVAL = *val;
    UNGET_BYTE();
    return l_parse_map_get (this, vp);
  }

  if (TOKEN == TOKEN_INDEX_OPEN) {
    switch (val->type) {
      case LIST_TYPE:
      case ARRAY_TYPE:
      case STRING_TYPE:
        break;
      default:
        THROW_SYNTAX_ERR("awaiting an array or a list or a string");
    }

    TOKENVAL = *val;
    UNGET_BYTE();

    if (val->type == LIST_TYPE)
      return l_parse_list_get (this, vp);

    if (val->type == ARRAY_TYPE)
      return l_parse_array_get (this, vp);

    return l_parse_string_get (this, vp);
  }

  if (TOKEN == TOKEN_PLUS_PLUS || TOKEN == TOKEN_MINUS_MINUS) {
    int peek = PEEK_NTH_BYTE(0);
    switch (peek) {
      case TOKEN_INDEX_CLOS:
      case TOKEN_SEMICOLON:
      case TOKEN_PAREN_CLOS:
      case TOKEN_NL:
      case TOKEN_COLON:
      case ' ':
      case '\t':
      case TOKEN_EOF:
        break;

      default:
        THROW_SYNTAX_ERR_FMT("unexpected token after %s",
          (TOKEN == TOKEN_PLUS_PLUS ? "++" : "--"));
    }

    *vp = *val; // get current value
    switch (val->type) {
      case INTEGER_TYPE: {
        AS_INT((*val)) = AS_INT((*val)) + (TOKEN == TOKEN_PLUS_PLUS ? 1 : -1);
        NEXT_TOKEN();
        return L_OK;
      }

      case NUMBER_TYPE: {
        AS_NUMBER((*val)) = AS_NUMBER((*val)) + (TOKEN == TOKEN_PLUS_PLUS ? 1 : -1);
        NEXT_TOKEN();
        return L_OK;
     }

      default:
        THROW_SYNTAX_ERR("awaiting an integer or a number");
    }
  }

  ifnot (IS_NULL(APPENDVAL)) {
    switch (val->type) {
      case INTEGER_TYPE:
        if (IS_INT(APPENDVAL)) {
          AS_INT((*val)) += AS_INT(APPENDVAL);
          *vp = *val;
          APPENDVAL = NULL_VALUE;
        }
        return L_OK;

      case NUMBER_TYPE:
        if (IS_NUMBER(APPENDVAL)) {
          AS_NUMBER((*val)) += AS_NUMBER(APPENDVAL);
          *vp = *val;
          APPENDVAL = NULL_VALUE;
        }
        return L_OK;

      case STRING_TYPE:
        *vp = *val;
        return L_OK;
    }
  }

  if (this->exprList && 0 == this->fmtRefcount) {
    *vp = *val;
    if (vp->type == STRING_TYPE || vp->type == LIST_TYPE   ||
        vp->type == MAP_TYPE    || vp->type == ARRAY_TYPE)
      vp->refcount++;
  } else if (this->fmtRefcount) {
    *vp = *val;
    vp->refcount++;
  } else
    *vp = l_copy_value (*val);

  return L_OK;
}

static int l_parse_list_set (l_t *this) {
  VALUE v_list = TOKENVAL;

  int c = NEXT_BYTE_NOWS_NONL();

  if (c == '=') {
    sym_t *sym = v_list.sym;

    THROW_SYNTAX_ERR_IF(sym == NULL, "unexpected error, variable is not associated with a symbol");
    THROW_SYNTAX_ERR_IF(sym->is_const, "can not reassign a new value, to a constant declared list");
    sym->type = NULL_TYPE;
    sym->value = NULL_VALUE;

    this->curMsg[0] = '\0';

    VALUE rv = l_release_value (this, v_list);
    THROW_SYNTAX_ERR_FMT_IF(IS_NULL(rv), "%s\n", this->curMsg);
    return L_MMT_REASSIGN;
  }

  int err;

  NEXT_TOKEN();

  if (TOKEN == TOKEN_REASSIGN) {
    VALUE v = v_list;
    sym_t *sym = v.sym;
    THROW_SYNTAX_ERR_IF(sym->is_const, "can not reassign a new value, to a constant declared list");

    sym->value = v;

    err = l_parse_chain (this, &v);
    THROW_ERR_IF_ERR(err);

    ifnot (v.refcount) // this condition almost? guarrantees that this is not our list
      l_release_value (this, v_list); // so release it

    v.refcount = 0;   // the chain function increments refcount, so zero it 
    sym->value = v;
    sym->type = v.type;
    return err;
  }

  THROW_SYNTAX_ERR_IF(TOKEN != TOKEN_INDEX_OPEN, "list set, awaiting [");

  NEXT_TOKEN();

  VALUE ix;
  err = l_parse_expr (this, &ix);
  THROW_ERR_IF_ERR(err);
  THROW_SYNTAX_ERR_IF(ix.type != INTEGER_TYPE, "awaiting an integer expression, when getting list index");
  THROW_SYNTAX_ERR_IF(TOKEN != TOKEN_INDEX_CLOS, "list set: awaiting ]");

  listType *list = AS_LIST(v_list);
  int idx = AS_INT(ix);
  if (0 > idx) idx += list->num_items;
  THROW_OUT_OF_BOUNDS_IF(idx <= -1 || idx >= list->num_items, "list index [%d] == out of bounds [%d]", idx, list->num_items);

  listNode *node = DListGetAt(list, listNode, idx);

  NEXT_TOKEN();

  if (TOKEN > TOKEN_ASSIGN) {
    if (TOKEN == TOKEN_PLUS_PLUS || TOKEN == TOKEN_MINUS_MINUS) {
      VALUE *val = node->value;

      int peek = PEEK_NTH_BYTE(0);
      switch (peek) {
        case TOKEN_INDEX_CLOS:
        case TOKEN_SEMICOLON:
        case TOKEN_PAREN_CLOS:
        case TOKEN_NL:
        case TOKEN_COLON:
        case ' ':
        case '\t':
        case TOKEN_EOF:
          break;

        default:
          THROW_SYNTAX_ERR_FMT("unexpected token after %s",
            (TOKEN == TOKEN_PLUS_PLUS ? "++" : "--"));
      }

      switch (val->type) {
        case INTEGER_TYPE:
          AS_INT((*val)) = AS_INT((*val)) + (TOKEN == TOKEN_PLUS_PLUS ? 1 : -1);
          NEXT_TOKEN();
          return L_OK;

        case NUMBER_TYPE:
          AS_NUMBER((*val)) = AS_NUMBER((*val)) + (TOKEN == TOKEN_PLUS_PLUS ? 1 : -1);
          NEXT_TOKEN();
          return L_OK;

        default:
          THROW_SYNTAX_ERR("awaiting an integer or a number");
      }
    }

    int token = TOKEN;

    NEXT_TOKEN();

    int is_un = 0;

    if (TOKEN == TOKEN_UNARY) {
      is_un = 1;
      NEXT_TOKEN();
    }

    VALUE v;
    err = l_parse_expr (this, &v);
    THROW_ERR_IF_ERR(err);

    if (is_un) {
      if (v.type == INTEGER_TYPE)
        AS_INT(v) = ~AS_INT(v);
      else
        THROW_SYNTAX_ERR("error while setting an unary object, awaiting an integer");
    }

    VALUE val = *node->value;

    switch (val.type) {
      case STRING_TYPE:
      case INTEGER_TYPE:
      case NUMBER_TYPE:
        break;

      default:
        THROW_A_TYPE_MISMATCH(val.type, "is not allowd for a binary operation");
    }

    VALUE result;
    switch (token) {
      case TOKEN_ASSIGN_APP:
        this->objectState |= OBJECT_APPEND;
        result = l_add (this, val, v);
        this->objectState &= ~OBJECT_APPEND;
        break;

      case TOKEN_ASSIGN_SUB:
        result = l_sub (this, val, v); break;
      case TOKEN_ASSIGN_DIV:
        result = l_div (this, val, v); break;
      case TOKEN_ASSIGN_MUL:
        result = l_mul (this, val, v); break;
      case TOKEN_ASSIGN_MOD:
        result = l_mod (this, val, v); break;
      case TOKEN_ASSIGN_BAR:
        result = l_bset (this, val, v); break;
      case TOKEN_ASSIGN_AND:
        result = l_bnot (this, val, v); break;
      case TOKEN_ASSIGN_XOR:
        result = l_bitxor (this, val, v); break;
    }

    if (result.type == NULL_TYPE)
       THROW_SYNTAX_ERR("unxpected operation");

    switch (val.type) {
      case STRING_TYPE: {
        *node->value = result;
        break;
      }

      case INTEGER_TYPE: {
        *node->value = result;
        break;
      }

      case NUMBER_TYPE: {
        *node->value = result;
        break;
      }
    }

    return L_OK;
  }

  if (TOKEN == TOKEN_DOT) {
    VALUE val = *node->value;
    THROW_SYNTAX_ERR_IF(val.type != MAP_TYPE,
      "value is not a map type");

    TOKENVAL = val;
    UNGET_BYTE();
    return l_parse_map_set (this);
  }

  if (TOKEN == TOKEN_INDEX_OPEN) {
    VALUE val = *node->value;
    switch (val.type) {
      case LIST_TYPE:
      case ARRAY_TYPE:
      case STRING_TYPE:
        break;
      default:
        THROW_SYNTAX_ERR("awaiting an array or a list or a string");
    }

    TOKENVAL = val;
    UNGET_BYTE();

    if (val.type == LIST_TYPE)
      return l_parse_list_set (this);

    if (val.type == ARRAY_TYPE)
      return l_parse_array_set (this);

    return l_parse_string_set_char (this, val, 0);
  }

  THROW_SYNTAX_ERR_IF(TOKEN != TOKEN_ASSIGN, "syntax error while setting list, awaiting =");

  NEXT_TOKEN();

  VALUE v;
  err = l_parse_expr (this, &v);
  THROW_ERR_IF_ERR(err);

  l_release_value (this, *node->value);

  switch (v.refcount) {
    case STRING_LITERAL:
      v.refcount = 0;
      // fallthrough

    case 0:
      *node->value = v;
      break;

    default:
      *node->value = l_copy_value (v);
  }

  return L_OK;
}

static VALUE list_copy (VALUE v) {
  listType *list = AS_LIST(v);
  int num = list->num_items;

  VALUE v_list = LIST_NEW();
  listType *l_new = AS_LIST(v_list);

  for (int i = 0; i < num; i++) {
    listNode *new = Alloc (sizeof (listNode));
    listNode *node = DListGetAt(list, listNode, i);
    VALUE *vp = Alloc (sizeof (VALUE));
    VALUE cpv = l_copy_value (*node->value);
    *vp = cpv;
    new->value = vp;
    DListAppend(l_new, new);
  }

  return v_list;
}

static VALUE list_delete_at (l_t *this, VALUE v_list, VALUE v_idx) {
  ifnot (IS_LIST(v_list)) C_THROW(L_ERR_TYPE_MISMATCH, "awaiting a list");
  ifnot (IS_INT(v_idx)) C_THROW(L_ERR_TYPE_MISMATCH, "awaiting an integer");

  listType *list = AS_LIST(v_list);

  int idx = AS_INT(v_idx);
  if (0 > idx)
    idx += list->num_items;

  if (idx <= -1 || idx >= list->num_items) {
    char msg[256]; str_copy_fmt (msg, 256,
      "given index %d >= than %d list length, or less than or equal with -1", AS_INT(v_idx), list->num_items);
    C_THROW(L_ERR_OUTOFBOUNDS, msg);
  }

  listNode *node = DListPopAt(list, listNode, idx);
  l_release_value (this, *node->value);
  Release (node->value);
  Release (node);

  return v_list;
}

static VALUE list_clear (l_t *this, VALUE v_list) {
  ifnot (IS_LIST(v_list)) C_THROW(L_ERR_TYPE_MISMATCH, "awaiting a list");

  listType *list = AS_LIST(v_list);

  listNode *node = NULL;
  while (1) {
    node = DListPopTail(list, listNode);
    if (NULL == node)
      break;

    l_release_value (this, *node->value);
    Release (node->value);
    Release (node);
  }

  return v_list;
}

static VALUE list_pop_at (l_t *this, VALUE v_list, VALUE v_idx) {
  ifnot (IS_LIST(v_list)) C_THROW(L_ERR_TYPE_MISMATCH, "awaiting a list");
  ifnot (IS_INT(v_idx)) C_THROW(L_ERR_TYPE_MISMATCH, "awaiting an integer");

  listType *list = AS_LIST(v_list);

  int idx = AS_INT(v_idx);
  if (0 > idx)
    idx += list->num_items;

  if (idx <= -1 || idx >= list->num_items) {
    char msg[256]; str_copy_fmt (msg, 256,
      "given index %d >= than %d list length, or less than or equal with -1", AS_INT(v_idx), list->num_items);
    C_THROW(L_ERR_OUTOFBOUNDS, msg);
  }

  listNode *node = DListPopAt(list, listNode, idx);

  VALUE v = l_copy_value (*node->value);
  l_release_value (this, *node->value);
  Release (node->value);
  Release (node);
  return v;
}

static int l_parse_list (l_t *this, VALUE *vp, int is_fun) {
  int err;
  NEXT_TOKEN();
  ifnot (is_fun)
    THROW_SYNTAX_ERR_IF(TOKEN != TOKEN_INDEX_OPEN,  "parsing list: awaiting [");
  else
    THROW_SYNTAX_ERR_IF(TOKEN != TOKEN_PAREN_OPEN,  "parsing list: awaiting (");

  VALUE v_list = LIST_NEW();
  if (this->exprList)
    v_list.refcount--;

  NEXT_TOKEN();

  if (TOKEN == TOKEN_PAREN_CLOS) {
    THROW_SYNTAX_ERR_IFNOT(is_fun, "parsing list: unexpected token )");
    *vp = v_list;
    NEXT_TOKEN ();
    return L_OK;
  } else if (TOKEN == TOKEN_INDEX_CLOS) {
    THROW_SYNTAX_ERR_IF(is_fun, "parsing list: unxpected token ]");
    *vp = v_list;
    NEXT_RAW_TOKEN ();
    THROW_SYNTAX_ERR_IF(TOKEN != TOKEN_BLOCK_CLOS, "parsing list: awaiting }");
    NEXT_TOKEN ();
    return L_OK;
  }

  while (1) {
    VALUE v;
    err = l_parse_expr (this, &v);
    THROW_ERR_IF_ERR(err);

    list_append (this, v_list, v);

    if (TOKEN == TOKEN_COMMA) {
      NEXT_TOKEN();
      continue;
    }

    break;
  }

  *vp = v_list;

  if (is_fun) {
    THROW_SYNTAX_ERR_IF(TOKEN != TOKEN_PAREN_CLOS,  "parsing list: awaiting )");
  } else {
    THROW_SYNTAX_ERR_IF(TOKEN != TOKEN_INDEX_CLOS,  "parsing list: awaiting ]");
    NEXT_RAW_TOKEN();
    THROW_SYNTAX_ERR_IF(TOKEN != TOKEN_BLOCK_CLOS, "parsing list: awaiting }");
  }

  NEXT_TOKEN();

  return L_OK;
}


static int l_parse_append (l_t *this, VALUE *vp) {
  APPENDVAL = NULL_VALUE;
  VALUE v;
  NEXT_TOKEN();
  int err = l_parse_expr (this, &v);
  THROW_ERR_IF_ERR(err);

  THROW_SYNTAX_ERR_IFNOT(TOKEN == TOKEN_IN, "error parsing append, awaiting in");

  NEXT_TOKEN();

  APPENDVAL = v;

  err = l_parse_expr (this, vp);
  THROW_ERR_IF_ERR(err);

  err = L_OK;

  switch (vp->type) {
    case ARRAY_TYPE: {
      ArrayType *array = (ArrayType *) AS_ARRAY((*vp));
      THROW_SYNTAX_ERR_IFNOT(v.type == array->type,
        "appended value type in array, is not the same of the array type");

      if (v.type == STRING_TYPE || v.sym != NULL) {
        if (v.refcount == STRING_LITERAL) {
          v.refcount = 0;
        } else {
          VALUE t = l_copy_value (v);
          v = t;
        }
      }

      array = ARRAY_APPEND(array, v);
      *vp = ARRAY(array);
      break;
    }

    case LIST_TYPE:
      list_set (this, *vp, v, LIST_APPEND); // always succeeds
      break;

    case MAP_TYPE: {
      ifnot (str_eq ("as", sym_key (this, TOKENSTR)))
        THROW_SYNTAX_ERR("error while appending to map, awaiting as");

      this->curState |= MALLOCED_STRING_STATE;
      VALUE vkey;
      NEXT_TOKEN();
      err = l_parse_expr (this, &vkey);
      this->curState &= ~MALLOCED_STRING_STATE;
      THROW_SYNTAX_ERR_IF(vkey.type != STRING_TYPE, "error while appending to map, awaiting a string as a key");

      string *s = AS_STRING(vkey);
      char *key = s->bytes;
      Map_Type *map = AS_MAP((*vp));
      err = l_map_set_value (this, map, key, v, 1);
      NEXT_TOKEN();
      break;
    }

    case STRING_TYPE: {
      if (str_eq ("as", sym_key (this, TOKENSTR)))
        TOKEN = TOKEN_AS;

      if (TOKEN != TOKEN_AS ||
          vp->refcount == STRING_LITERAL)
        this->objectState |= OBJECT_APPEND;

      VALUE rv = l_add (this, *vp, v);
      THROW_SYNTAX_ERR_IF(IS_NULL(rv), "error appending to a string");

      if (TOKEN == TOKEN_AS) {
        NEXT_RAW_TOKEN();
        THROW_SYNTAX_ERR_IF(TOKEN != TOKEN_SYMBOL, "awaiting a symbol after an as");
        sym_t * sym = l_lookup_symbol (this, TOKENSTR);
        THROW_SYNTAX_ERR_IF(sym != NULL, "can not reassign to a symbol with the append method");
        TOKENSYM = l_define_symbol (this, this->curScope, sym_key (this, TOKENSTR), STRING_TYPE, rv, 0);
        *vp = rv;
        NEXT_TOKEN();
      }
      break;
    }

    default: {
      if (IS_NULL(APPENDVAL)) break;
      VALUE rv = l_add (this, *vp, v);
      THROW_SYNTAX_ERR_IF(IS_NULL(rv), "append error");

      if (vp->sym != NULL)
        vp->sym->value = rv;
      *vp = rv;
    }
  }

  APPENDVAL = NULL_VALUE;
  return err;
}

static int l_parse_format_array (l_t *this, VALUE *vp, int release_vp) {
  THROW_SYNTAX_ERR_IFNOT(TOKEN == TOKEN_PAREN_CLOS, "array format(), awaiting ')'");
  ArrayType *array = (ArrayType *) AS_ARRAY((*vp));
  THROW_SYNTAX_ERR_IF(array->type != STRING_TYPE, "format() awaiting a StringType array");

  ArrayType *nar = ARRAY_NEW(STRING_TYPE, (integer) array->len);
  string **n_ar = (string **) AS_ARRAY(nar->value);

  VALUE ary = array->value;
  string **s_ar = (string **) AS_ARRAY(ary);

  l_string savepc = PARSEPTR;

  int err;
  for (int i = 0; i < array->len; i++) {
    string *s = s_ar[i];
    string *n = n_ar[i];
    PARSEPTR = StringNew(s->bytes);
    err = l_parse_fmt (this, n, 1);
    THROW_ERR_IF_ERR(err);
    n_ar[i] = n;
  }

  if (release_vp && vp->sym == NULL)
    l_release_value (this, *vp);

  *vp = ARRAY(nar);
  PARSEPTR = savepc;
  NEXT_TOKEN();
  return L_OK;
}

static int l_parse_format (l_t *, VALUE *);
static int l_parse_format (l_t *this, VALUE *vp) {
  int err;
  string *str = NULL;

  int c = l_ignore_ws (this);

  if (c != TOKEN_PAREN_OPEN) {
    this->print_bytes (this->err_fp, "format error, awaiting (\n");
    l_err_ptr (this, L_NOTOK);
    return L_NOTOK;
  }

  int is_expr = 0;

  c= l_ignore_ws (this);

  if (c != TOKEN_DQUOTE) {
    UNGET_BYTE();
    NEXT_TOKEN();
    c = TOKEN;
    VALUE v;
    this->curState |= MALLOCED_STRING_STATE;
    err = l_parse_expr (this, &v);
    this->curState &= ~MALLOCED_STRING_STATE;
    THROW_ERR_IF_ERR(err);

    if (v.type == ARRAY_TYPE) {
      *vp = v;
      err = l_parse_format_array (this, vp, 1);
      THROW_ERR_IF_ERR(err);
      return L_OK;
    }

    if (v.type == STRING_TYPE) {
      str = string_new (32);
      c = TOKEN;
      l_string savepc = PARSEPTR;
      string *v_s = AS_STRING(v);
      /* very crude algorithm (but for a minor case (evaluation of a string
       * that might include embedded interpolation syntax)) */
      /* Update: mind change. Always include the required characters for the
         evaluation, and remove ambiguities and false interpretation. */

      size_t len = v_s->num_bytes + 4;
      char buf[len + 1];
      buf[0] = '('; buf[1] = '"';
      str_copy (buf + 2, len + 1, v_s->bytes, v_s->num_bytes);
      buf[len - 2] = '"'; buf[len - 1] = ')'; buf[len] = '\0';

      if (v.sym == NULL &&
          v.refcount != MALLOCED_STRING &&
          0 == (this->objectState & (ARRAY_MEMBER|MAP_MEMBER)))
         string_release (v_s);

      PARSEPTR = StringNew (buf);
      err = l_parse_format (this, &v);
      THROW_ERR_IF_ERR(err);

      PARSEPTR = savepc;

      string *vs = AS_STRING(v);

      string_append_with_len (str, vs->bytes, vs->num_bytes);
      string_release (vs);

      is_expr = 1;
      goto theend;
    }
  }

  if (c != TOKEN_DQUOTE)
    THROW_SYNTAX_ERR("format error, awaiting \"");

  str = string_new (32);

  err = l_parse_fmt (this, str, 0);
  THROW_ERR_IF_ERR(err);

theend:
  ifnot (NULL == str)
    *vp = STRING(str);

  ifnot (is_expr) {
    NEXT_TOKEN();
    c = TOKEN;
  }

  if (c != TOKEN_PAREN_CLOS)
    THROW_SYNTAX_ERR("format error, awaiting )");

  NEXT_TOKEN();
  c = TOKEN;

  ifnot (NULL == str) {
    if (this->fmtRefcount)
      this->fmtState |= FMT_LITERAL;
    else
      if (this->exprList)
        vp->refcount = STRING_LITERAL;
   //     vp->refcount = -1;
   // CHANGE      vp->refcount = STRING_LITERAL;
  }

  return L_OK;
}

static VALUE l_fileno (l_t *this, VALUE fp_val) {
  (void) this;
  ifnot (IS_FILEPTR(fp_val)) C_THROW(L_ERR_TYPE_MISMATCH, "awaiting a file pointer");
  FILE *fp = AS_FILEPTR(fp_val);
  l_set_Errno (this, 0);
  int fd = sys_fileno (fp);
  VALUE v = INT(fd);
  if (fd == -1)
    l_set_Errno (this, sys_errno);

  return v;
}

static VALUE l_len (l_t *this, VALUE value) {
  (void) this;
  VALUE result = INT(0);

  switch (value.type) {
    case STRING_TYPE: {
      string *str = AS_STRING(value);
      result = INT(str->num_bytes);
      break;
    }

    case ARRAY_TYPE: {
      ArrayType *array = (ArrayType *) AS_ARRAY(value);
      result = INT(array->len);
      break;
    }

    case MAP_TYPE: {
      Map_Type *map = AS_MAP(value);
      result = INT(map->num_keys);
      break;
    }

    case LIST_TYPE: {
      listType *list = AS_LIST(value);
      result = INT(list->num_items);
      break;
    }
  }

  return result;
}

static VALUE l_string_release (VALUE value) {
  VALUE result = INT(L_OK);

  if (value.refcount > 0) goto theend;

  if (value.refcount < 0)
    if (value.refcount != STRING_LITERAL)
      return result;

  string_release (AS_STRING(value));

  value = NULL_VALUE;

theend:
  value.refcount--;
  return result;
}

static VALUE object_release (l_t *this, VALUE value) {
  VALUE result = INT(L_OK);

  if (value.refcount < 0)
    return result;

  if (value.refcount > 0) goto theend;

  if (value.type == NULL_TYPE) goto theend;

  object *o = AS_OBJECT(value);

  ifnot (NULL == o->release) {
    ifnot (NULL == this) this->funcState |= OBJECT_RELEASE_STATE;
    o->release (this, value);
    ifnot (NULL == this) this->funcState &= ~OBJECT_RELEASE_STATE;
  }

  Release (o);

  value = NULL_VALUE;

theend:
  value.refcount--;
  return result;
}

static VALUE l_release_value (l_t *this, VALUE value) {
  (void) this;
  VALUE result = INT(L_OK);
  if (value.type < STRING_TYPE) return result;

  void *obj = NULL;

  switch (value.type) {
    case   POINTER_TYPE: obj = AS_VOID_PTR(value); break;
    case     ARRAY_TYPE: return array_release (value);
    case    STRING_TYPE: return l_string_release (value);
    case       MAP_TYPE: return l_map_release (value);
    case        FD_TYPE:
    case   FILEPTR_TYPE:
    case      LIST_TYPE:
    case    OBJECT_TYPE: return object_release (this, value);
    default:
      return result;
  }

  ifnot (NULL == obj) {
    Release (obj);
    value = NULL_VALUE;
  }

  obj = NULL;
  return result;
}

static void ns_release_malloced_strings (funT *this) {
  malloced_string *item = this->head;
  while (item) {
    malloced_string *tmp = item->next;
    string_release (item->data);
    Release (item);
    item = tmp;
  }

  this->head = NULL;
}

static void fun_release (funT **thisp) {
  if (*thisp == NULL) return;
  funT *this = *thisp;

  map_release (&this->symbols);
  ns_release_malloced_strings (this);
  if (this->modules != NULL) {

  #ifndef STATIC
    module_so *it = this->modules->head;
    while (it) {
      module_so *tmp = it->next;
      Release (it->name);
      it->deinit (it->instance);
//      sys_dlclose (it->handle);
      Release (it);
      it = tmp;
    }
  #endif

    Release (this->modules);
  }

  Release (this);
  *thisp = NULL;
}

static void l_release_sym (map_type *sym) {
  if (sym == NULL) return;

  sym_t *this = (sym_t *) sym;

  VALUE v = this->value;

  if (IS_UFUNC(this->type)) {
    ifnot (v.type & UFUNCTION_TYPE) {
      funT *f = AS_FUNC_PTR(v);
      fun_release (&f);
    }
  } else
    l_release_value (NULL, v);

  Release (this);
  this = NULL;
}

static void l_release_unit (map_type *map) {
  string *str = (string *) map;
  string_release (str);
}

static funT *fun_copy (funT *f, char *name, size_t len) {
  funT *uf = Alloc (sizeof (funT));
  str_copy (uf->funname, MAXLEN_SYMBOL + 1, name, len);
  uf->body = f->body;
  uf->nargs = f->nargs;
  uf->prev = f->prev;
  uf->root = f->root;
  uf->next = f->next;
  for (int i = 0; i < uf->nargs; i++)
    str_copy (uf->argName[i], MAXLEN_SYMBOL + 1, f->argName[i], bytelen (f->argName[i]));
  uf->symbols = map_new (32);
  uf->block_symbols = NULL;
  uf->modules = NULL;
  return uf;
}

static funT *fun_new (funNewArgs options) {
  funT *uf = Alloc (sizeof (funT));
  str_copy (uf->funname, MAXLEN_SYMBOL + 1, options.name, options.namelen);
  uf->body = options.body;
  uf->nargs = options.nargs;
  uf->prev = options.parent;
  uf->root = options.root;
  uf->next = NULL;
  uf->symbols = map_new (options.num_symbols);
  uf->block_symbols = NULL;
  uf->head = NULL;

  return uf;
}

static funT *Fun_new (l_t *this, funNewArgs options) {
  funT *f = fun_new (options);

  funT *parent = options.parent;

  if (parent == NULL) {
    this->function = this->curScope = f->root = f;
    this->function->modules = Alloc (sizeof (modules));
    return f;
  }

  f->modules = NULL;
  f->prev = parent;
  f->root = this->function;
  return f;
}

static sym_t *l_define_block_symbol (l_t *this, funT *f, const char *key, int typ, VALUE value, int is_const) {
  (void) this;
  ifnot (key) return NULL;

  sym_t *sym = Alloc (sizeof (sym_t));
  sym->type = typ;
  sym->value = value;
  sym->value.sym = sym;
  sym->is_const = is_const;
  sym->scope = f;

  if (-1 == map_set (f->block_symbols, (char *) key, sym, l_release_sym)) {
    Release (sym);
    return NULL;
  }

  return sym;
}

static sym_t *l_define_symbol (l_t *this, funT *f, const char *key, int typ, VALUE value, int is_const) {
  if (this->curState & BLOCK_STATE)
    return l_define_block_symbol (this, f, key, typ, value, is_const);

  ifnot (key) return NULL;

  sym_t *sym = Alloc (sizeof (sym_t));

  sym->type = typ;
  sym->value = value;
  sym->value.sym = sym;
  sym->is_const = is_const;
  sym->scope = f;

  if (-1 == map_set (f->symbols, (char *) key, sym, l_release_sym)) {
    Release (sym);
    return NULL;
  }

  return sym;
}

static int l_define (l_t *this, const char *key, int typ, VALUE val) {
  sym_t *sym = l_define_symbol (this, this->std, (char *) key, typ, val, 1);
  return (NULL == sym ? L_NOTOK : L_OK);
}

static int l_define_type (l_t *this, const char *key, int typ, VALUE val) {
  sym_t *sym = l_define_symbol (this, this->datatypes, (char *) key, typ, val, 1);
  return (NULL == sym ? L_NOTOK : L_OK);
}

static inline sym_t *ns_lookup_symbol (funT *scope, const char *key) {
  return map_get_value (scope->symbols, key);
}

static sym_t *l_lookup_symbol (l_t *this, l_string name) {
  char *key = sym_key (this, name);

  sym_t *sym = ns_lookup_symbol (this->std, key);
  ifnot (NULL == sym) return sym;

  sym = ns_lookup_symbol (this->datatypes, key);
  ifnot (NULL == sym) return sym;

  funT *f = this->curScope;

  while (NULL != f) {
    ifnot (NULL == f->block_symbols) {
      sym = map_get_value (f->block_symbols, key);
      ifnot (NULL == sym) return sym;
    }

    sym = ns_lookup_symbol (f, key);

    ifnot (NULL == sym) return sym;

    f = f->prev;
  }

  return NULL;
}

static int l_parse_anon_func (l_t *this, VALUE *vp) {
  int err;

  str_copy_fmt
    (this->curFunName, MAXLEN_SYMBOL + 1, NS_ANON "_%d", this->anon_id++);

  this->curFunDef = NULL;

  err = l_parse_func_def (this);
  THROW_ERR_IF_ERR(err);

  this->curFunName[0] = '\0';

  sym_t *sym = this->curSym;
  sym->value = NULL_VALUE;
  funT *fun = this->curFunDef;
  TOKENARGS = fun->nargs;

  THROW_SYNTAX_ERR_IFNOT(TOKEN == TOKEN_PAREN_OPEN,
    "anonymous function error, awaiting (");

  UNGET_BYTE();

  err = l_parse_func_call (this, vp, NULL, fun, TOKENVAL);

  fun_release (&fun);

  NEXT_TOKEN();

  return err;
}

static int l_consume_string (l_t *this, char chr) {
  int pc;
  int c = 0;

  while (1) {
    pc = c;
    c = GET_BYTE();

    if (c == TOKEN_EOF)
      THROW_SYNTAX_ERR_FMT("error while getting literal string, awaiting %s quote, found EOF",
          (chr == '`' ? "back" : "double"));

    if (c == chr) {
      if (pc == TOKEN_ESCAPE_CHR) continue;
      break;
    }
  }

  TOKEN = TOKEN_STRING;
  return TOKEN;
}

static int l_get_string (l_t *this, char chr) {
  string *str = string_new (8);

  size_t ptrlen = GETSTRLEN(PARSEPTR);
  const char *ptr = GETSTRPTR(PARSEPTR);
  const char *save_ptr = ptr;

  int c = 0;
  int pc;

  if (chr == TOKEN_BQUOTE) {
    if (*ptr == '[') {
      ptr++;
      int depth = 1;

      while (1) {
        THROW_SYNTAX_ERR_IF(*ptr == '\0',
          "error while getting literal string, awaiting ]`");

        c = *ptr++;

        if (c == ']') {
          if (*ptr && *ptr == TOKEN_BQUOTE) {
            depth--;
            ptr++;

            ifnot (depth) break;

            string_append_with_len (str, "]`", 2);
            continue;
          }
        }

        if (c == '`') {
          if (*ptr && *ptr == '[') {
            depth++;
            ptr++;
            string_append_with_len (str, "`[", 2);
            continue;
          }
        }

        string_append_byte (str, c);
      }

      goto attributes;
    }

    while (1) {
      pc = c;
      c = *ptr++;

      THROW_SYNTAX_ERR_IF(c == TOKEN_EOF, "error while getting literal string, awaiting back quote");

      if (c == TOKEN_BQUOTE) {
        if (pc == TOKEN_ESCAPE_CHR) {
          string_clear_at (str, -1);
          string_append_byte (str, c);
          continue;
        }

        break;
      }

      string_append_byte (str, c);
    }

    attributes:
    c = *ptr;

    if (c == 'F') {
      ifnot (*str->bytes == '"' + (str->bytes[str->num_bytes - 1] == '"')) {
        string_prepend_byte (str, '"');
        string_append_byte (str, '"');
      }

      string_prepend_byte (str, '(');
      string_append_byte (str, ')');

      l_string savepc = PARSEPTR;
      PARSEPTR = StringNew (str->bytes);
      VALUE v;
      int err = l_parse_format (this, &v);
      THROW_ERR_IF_ERR(err);

      PARSEPTR = savepc;
      ptr++;
      c = *(ptr + 1);
      if (c == ':') { ptr++; c = *ptr; }
      string_replace_with (str, AS_STRING_BYTES(v));
    }

    if (c == 'S') {
      ptr++;

      if (*ptr == '0') {
        ptr++;
        goto theend;
      }

      int num_ws = 1024;

      if ('1' <= *ptr && *ptr <= '9') {
        c = *ptr++;

        num_ws = c - '0';

        while ((c = *ptr)) {
          if ('0' <= c && c <= '9') {
            num_ws = 10 * num_ws + (c - '0');
            ptr++;
            continue;
          }
          break;
        }
      }

      size_t len = str->num_bytes;
      char sptr[len + 1];
      str_copy (sptr, len + 1, str->bytes, len);

      string_clear (str);

      char *p = sptr;

      while ((c = *p++)) {
        if (c != TOKEN_NL) {
          string_append_byte (str, c);
          continue;
        }

        string_append_byte (str, c);
        int num_stripped = 0;

        while (*p && num_stripped < num_ws) {
          c = *p++;

          if (is_space (c)) {
            num_stripped++;
            continue;
          }

          string_append_byte (str, c);
          if (c == TOKEN_NL) {
            num_stripped = 0;
            continue;
          }

          break;
        }
      }

      if (*(ptr + 1) == ':') { ptr++; goto attributes; }

    }

  } else {
    while (1) {
      pc = c;
      c = *ptr++;

      if (c == TOKEN_EOF)
        THROW_SYNTAX_ERR("error while getting literal string, awaiting double quote");

      if (pc == TOKEN_ESCAPE_CHR) {
        switch (c) {
          case 'a': string_append_byte (str, '\a'); break;
          case 'b': string_append_byte (str, '\b'); break;
          case 'f': string_append_byte (str, '\f'); break;
          case 't': string_append_byte (str, '\t'); break;
          case 'r': string_append_byte (str, '\r'); break;
          case 'n': string_append_byte (str, '\n'); break;
          case 'v': string_append_byte (str, '\v'); break;
          case 'e': string_append_byte (str,  033); break;
          case '"' :string_append_with (str, "\""); break;
          case '\\':string_append_byte (str, '\\'); c = 0; break;
          default:
           THROW_SYNTAX_ERR_FMT("unknow escape sequence \\\\%c", c);
        }
        continue;
      }

      if (c == TOKEN_ESCAPE_CHR) continue;

      if (c == TOKEN_DQUOTE) break;

      string_append_byte (str, c);
    }
    c = *(ptr + 1);
    if (c == 'F' || c == 'S') {
      ptr++;
      goto attributes;
    }
  }

theend:
  SETSTRPTR(PARSEPTR, ptr);
  SETSTRLEN(PARSEPTR, ptrlen - (ptr - save_ptr));

  VALUE v = STRING(str);
  if (this->curState & MALLOCED_STRING_STATE) {
    malloced_string *mbuf = Alloc (sizeof (malloced_string));
    v.refcount = MALLOCED_STRING;
    mbuf->data = str;
    ListStackPush (this->curScope, mbuf);
  } else {
    v.refcount = STRING_LITERAL;
    this->curState |= LITERAL_STRING_STATE;
  }

  TOKENVAL = v;
  TOKEN = TOKEN_STRING;
  return TOKEN;
}

static inline int parse_number (l_t *this, int c, int *token_type) {
  *token_type = TOKEN_INTEGER;

  int dot_found = 0;
  int plus_found = 0;
  int minus_found = 0;

  int is_octOrbin = c == '0';

  c = GET_BYTE();

  if (is_octOrbin) {
    ifnot (is_digit (c)) {
      if (c == 'b') {
        *token_type = TOKEN_BINARY;
        RESET_TOKEN;
        c = GET_BYTE();
      }
    } else
      *token_type = TOKEN_OCTAL;
  }

  if (c == '-' || c == '+')
    return L_NOTOK;

  goto parse;

  for (;;) {
    c = GET_BYTE();

    parse:
    if (c == '-' || '+' == c) {
      if (*token_type != TOKEN_NUMBER) return L_NOTOK;
      if (c == '-') {
        if (minus_found++) return L_NOTOK;
        else if (plus_found++) return L_NOTOK;
      }

      int cc = PEEK_NTH_BYTE(0);
      ifnot (is_digit (cc)) return L_NOTOK;
      continue;
    }

    if (c == '.') {
      *token_type = TOKEN_NUMBER;

      if (dot_found++) return L_NOTOK;
      ifnot (is_digit (PEEK_NTH_BYTE(0))) return L_NOTOK;
      continue;
    }

    if (c == 'e' || c == 'E') {
      *token_type = TOKEN_NUMBER;

      int cc = PEEK_NTH_BYTE(0);
      if (0 == is_digit (cc) ||
          cc != '-' ||
          cc != '.' ||
          cc != '+') {
        return L_NOTOK;
      }

      continue;
    }

    ifnot (is_digit (c)) break;

    if (*token_type == TOKEN_OCTAL)
      if (c > '7')
        THROW_SYNTAX_ERR("not an octal number");

    if (*token_type == TOKEN_BINARY)
      if (c > '1')
        THROW_SYNTAX_ERR("not a binary number");
  }

  if (c != TOKEN_EOF) UNGET_BYTE();

  return L_OK;
}

static int l_get_annotated_block (l_t *this, string *s) {
  int c = GET_BYTE();

  THROW_SYNTAX_ERR_IF(c != ' ' && c != TOKEN_NL, "error while getting annotated block: awaiting a space");

  int prev_c;

  while (1) {
    prev_c = c;
    c = GET_BYTE();
    THROW_SYNTAX_ERR_IF(c == TOKEN_EOF, "unended annotated block");

    if (c == TOKEN_COMMENT && prev_c == TOKEN_NL)
      break;

    if (s)
      string_append_byte (s, c);
  }

  c = GET_BYTE();

  if (c == ' ') {
    int orig_len = GETSTRLEN(PARSEPTR);
    const char *ptr = GETSTRPTR(PARSEPTR);

    if (str_eq_n (ptr, "as ", 3)) {
      const char *p = ptr + 3;
      int idx = 0;
      while (is_identifier (*p)) {
        if (idx++ == MAXLEN_SYMBOL)
          THROW_SYNTAX_ERR("annotated identifier exceeded maximum length");

        p++;
        if (*p == ' ') {
          while (*p != TOKEN_NL) p++;
          THROW_SYNTAX_ERR_IF(c == TOKEN_EOF, "unended annotated block");
          break;
        }

        if (*p == TOKEN_NL) break;
      }

      if (s && idx)
        for (int i = 0; i < idx + 3; i++)
          string_append_byte (s, ptr[i]);

      SETSTRPTR(PARSEPTR, p);
      SETSTRLEN(PARSEPTR, orig_len - (p - ptr));
    }
  }

  return c;
}

static int l_do_next_token (l_t *this, int israw) {
  int err;

  TOKENSYM = NULL;
  int token = TOKEN;

  RESET_TOKEN;

  int c = l_ignore_ws (this);

  if (c == TOKEN_COMMENT) {
    c = GET_BYTE();

    switch (c) {
      case TOKEN_COMMENT:
        c = GET_BYTE();
        if (c != TOKEN_INDEX_OPEN)
          goto consume_comment;

        int num = 1;
        while (num) {
          c = GET_BYTE();

          if (c == TOKEN_COMMENT && ((c = GET_BYTE()) == TOKEN_COMMENT)) {
            c = GET_BYTE();
            if (c == TOKEN_INDEX_CLOS) { num--; continue; }
            if (c == TOKEN_INDEX_OPEN) { num++; continue; }
          }

          THROW_SYNTAX_ERR_IF(c == TOKEN_EOF, "awaiting ##]");
        }

        c = GET_BYTE();
        goto consume_comment;

      case TOKEN_AT:
        c = l_get_annotated_block (this, NULL);
        THROW_ERR_IF_ERR(c);
        return l_do_next_token (this, israw);
    }

    consume_comment:
    while (c >= 0 && c != TOKEN_NL)
      c = GET_BYTE();

    TOKEN = c;
    return TOKEN;
  }

  if (is_alpha (c) || c == '_') {
    l_get_span (this, is_identifier);

    TOKEN = TOKEN_SYMBOL;

    if (GETSTRLEN(TOKENSTR) > MAXLEN_SYMBOL)
      THROW_SYNTAX_ERR_FMT("%s: exceeds maximum length (%d) of an identifier",
         cur_msg_str (this, TOKENSTR), MAXLEN_SYMBOL);

    if (israw)
      return TOKEN;

    TOKENSYM = l_lookup_symbol (this, TOKENSTR);

    ifnot (TOKENSYM)
      return TOKEN;

    TOKEN = TOKENSYM->type & 0xff;

    if (TOKEN == TOKEN_BINOP) { /* is, isnot, and, or */
      TOKEN = TOKENSYM->type;
      TOKENVAL = TOKENSYM->value;
      return TOKEN;
    }

    TOKENARGS = (TOKENSYM->type >> 8) & 0xff;
    TOKENSYM->value.sym = TOKENSYM;
    TOKENVAL = TOKENSYM->value;

    if (AS_VOID_PTR(TOKENVAL) == l_parse_array_def)
      TOKEN = TOKEN_ARYDEF;

    return TOKEN;
  }

  if (is_digit (c) || (c == '-' && is_digit (PEEK_NTH_BYTE(0)) && token != INTEGER_TYPE)) {
    if (c == '0' && NULL != str_chr ("xX", PEEK_NTH_BYTE(0))
        && is_hexchar (PEEK_NTH_BYTE(1))) {
      GET_BYTE();
      RESET_TOKEN;
      l_get_span (this, is_hexchar);
      TOKEN = TOKEN_HEX_NUMBER;
      return TOKEN;
    }

    int r;
    err = parse_number (this, c, &r);
    THROW_SYNTAX_ERR_IF_ERR(err, "error while tokenizing a number");
    TOKEN = r;
    return TOKEN;
  }

  if (is_operator (c)) {
    l_get_span (this, is_operator_span);

    TOKENSYM = ns_lookup_symbol (this->std, sym_key (this, TOKENSTR));

    if (TOKENSYM) {
      TOKEN = TOKENSYM->type;
      TOKENVAL = TOKENSYM->value;
    } else {
      if (c == TOKEN_COLON) {
        TOKEN = c;
        return TOKEN;
      }

      TOKEN = TOKEN_SYNTAX_ERR;
    }

    return TOKEN;
  }

  if (c == TOKEN_BQUOTE || c == TOKEN_DQUOTE) {
    if (this->curState & CONSUME_STATE) {
      err = l_consume_string (this, c);
      THROW_ERR_IF_ERR(err);
      return TOKEN;
    }

    return l_get_string (this, c);
  }

  if (c == TOKEN_BLOCK_OPEN) {
    if (IS_NEXT_BYTE(TOKEN_INDEX_OPEN)) {
      TOKEN = TOKEN_LIST_NEW;
      return TOKEN;
    }

    RESET_TOKEN;
    err = l_get_opened_block (this, "unended opened block");
    THROW_ERR_IF_ERR(err);

    IGNORE_LAST_TOKEN;
    TOKEN = TOKEN_BLOCK;
    return TOKEN;
  }

  if (c == TOKEN_COLON || c == TOKEN_DOLLAR) {
    TOKEN = c;
    return TOKEN;
  }

  if (c == TOKEN_SQUOTE) {
    c = GET_BYTE();
    if (c == TOKEN_ESCAPE_CHR) {
      if (IS_NEXT_BYTE('x')) {
        GET_BYTE();

        THROW_SYNTAX_ERR_IF(ISNOT_NEXT_BYTE('{'),
          "error while parsing char, awaiting {");

        GET_BYTE();
        RESET_TOKEN;

        l_get_span (this, is_hexchar);
        c = GET_BYTE();
        THROW_SYNTAX_ERR_IFNOT(c == '}', "error while parsing char, awaiting }");

        IGNORE_LAST_TOKEN;

        c = GET_BYTE();
        THROW_SYNTAX_ERR_IF(c != TOKEN_SQUOTE, "error while parsing char, awaiting '");

        IGNORE_LAST_TOKEN;
        TOKEN = TOKEN_HEX_CHAR;
        return TOKEN;
      }

      GET_BYTE();
    }

    int max = 4;

    do {
      c = GET_BYTE();
      if (c == TOKEN_SQUOTE) {
        IGNORE_FIRST_TOKEN;
        IGNORE_LAST_TOKEN;
        TOKEN = TOKEN_CHAR;
        return TOKEN;
      }
    } while (--max != 0);

    RESET_TOKEN;
    return L_ERR_SYNTAX;
  }

  TOKEN = c;
  return TOKEN;
}

static void stack_push (l_t *this, VALUE x) {
  this->stackVal[++this->stackValIdx] = x;
}

static VALUE stack_pop (l_t *this) {
  return this->stackVal[this->stackValIdx--];
}

static void l_fun_stack_push (l_t *this, funT *f) {
  funstack_t *item = Alloc (sizeof (funstack_t));
  item->f = f;
  ListStackPush (this->funstack, item);
}

static funT *l_fun_stack_pop (l_t *this) {
  funstack_t *item = ListStackPop (this->funstack, funstack_t);
  funT *f = item->f;
  Release (item);
  return f;
}

static VALUE l_copy_value (VALUE v) {
  VALUE new;

  switch (v.type) {
    case STRING_TYPE:
      new = STRING(string_dup (AS_STRING(v)));
      break;

    case ARRAY_TYPE:
      new = ARRAY(array_copy ((ArrayType *) AS_ARRAY(v)));
      break;

    case MAP_TYPE:
      new = l_copy_map (v);
      break;

    case LIST_TYPE:
      new = list_copy (v);
      break;

    default:
      new = v;
  }

  return new;
}

static VALUE _l_copy_value (l_t *this, VALUE v) {
  (void) this;
  return l_copy_value (v);
}

static void *l_clone_sym_item (void *item, void *obj) {
  (void) obj;
  sym_t *sym = (sym_t *) item;

  sym_t *new = Alloc (sizeof (sym_t));

  new->type = sym->type;
  new->is_const = sym->is_const;

  new->value = l_copy_value (sym->value);
  return new;

#if 0
  if ((new->type & 0xff) == ARRAY_TYPE) {
    VALUE ar = sym->value;
    ArrayType *array = (ArrayType *) AS_ARRAY(ar);
    ArrayType *new_array = array_copy (array);
    new->value = ARRAY(new_array);
    return new;

  } else if ((new->type & 0xff) == STRING_TYPE) {
    string *old_str = AS_STRING(sym->value);
    new->value = STRING_NEW_WITH_LEN(old_str->bytes, old_str->num_bytes);
    return new;
  }

  new->value = sym->value;
  return new;
#endif
}

static Map_Type *l_symbol_stack_push (l_t *this, Map_Type *symbols) {
  symbolstack_t *item = Alloc (sizeof (symbolstack_t));
  item->symbols = map_clone (symbols, l_clone_sym_item, NULL);
  ListStackPush (this->symbolstack, item);
  return item->symbols;
}

static Map_Type *l_symbol_stack_pop (l_t *this) {
  symbolstack_t *item = ListStackPop (this->symbolstack, symbolstack_t);
  Map_Type *symbols = item->symbols;
  Release (item);
  return symbols;
}

static VALUE l_string_to_dec (l_string s) {
  integer r = 0;
  int c;
  const char *ptr = GETSTRPTR(s);
  int len = GETSTRLEN(s);
  int is_sign = *ptr == '-';
  if (is_sign) ptr++;

  while (len-- > 0) {
    c = *ptr++;
    ifnot (is_digit (c)) break;
    r = 10 * r + (c - '0');
  }

  if (is_sign) r = -r;

  return INT(r);
}

static VALUE l_HexStringToNum (l_string s) {
  integer r = 0;
  int c;
  const char *ptr = GETSTRPTR(s);
  int len = GETSTRLEN(s);
  while (len-- > 0) {
    c = *ptr++;
    ifnot (is_hexchar (c)) break;
    if (c <= '9')
      r = 16 * r + (c - '0');
    else if (c <= 'F')
      r = 16 * r + (c - 'A' + 10);
    else
      r = 16 * r + (c - 'a' + 10);
  }

  return INT(r);
}

static VALUE l_OctalStringToNum (l_string s) {
  integer r = 0;
  int c;
  const char *ptr = GETSTRPTR(s);
  int len = GETSTRLEN(s);
  while (len-- > 0) {
    c = *ptr++;
    r = 8 * r + (c - '0');
  }

  return INT(r);
}

static VALUE l_BinaryStringToNum (l_string s) {
  integer r = 0;
  int c;
  const char *ptr = GETSTRPTR(s);
  int len = GETSTRLEN(s);

  while (len-- > 0) {
    c = *ptr++;
    r = 2 * r + (c - '0');
  }

  return INT(r);
}

static int  l_parse_string_get (l_t *this, VALUE *vp) {
  int err;
  int c = PEEK_NTH_BYTE(0);

  if (c == TOKEN_INDEX_OPEN) {
    string *str = AS_STRING(TOKENVAL);

    NEXT_TOKEN();

    VALUE v;
    this->curState |= INDEX_STATE;
    err = l_parse_primary (this, &v);
    this->curState &= ~INDEX_STATE;
    THROW_ERR_IF_ERR(err);

    THROW_SYNTAX_ERR_IFNOT(v.type == INTEGER_TYPE,
      "awaiting an integer expression, when getting string index");

    integer idx = AS_INT(v);

    if (0 > idx) idx += str->num_bytes;

    if (idx < 0 || (size_t) idx >= str->num_bytes)
      THROW_OUT_OF_BOUNDS("string index %d >= than %d length, or less than zero", idx, str->num_bytes);

    *vp = INT(str->bytes[idx]);

    if (this->curState & LITERAL_STRING_STATE) {
      this->curState &= ~LITERAL_STRING_STATE;
      string_release (str);
    }

  } else {
    *vp = TOKENVAL;
    NEXT_TOKEN();
  }

  return L_OK;
}

static int l_parse_string_set_char (l_t *this, VALUE value, int is_const) {
  int err;

  THROW_SYNTAX_ERR_IF(is_const, "can not modify constant string");

  string *str = AS_STRING(value);

  NEXT_TOKEN();

  VALUE v;

  this->curState |= INDEX_STATE;
  err = l_parse_primary (this, &v);
  this->curState &= ~INDEX_STATE;
  THROW_ERR_IF_ERR(err);

  THROW_SYNTAX_ERR_IFNOT(v.type == INTEGER_TYPE,
    "awaiting an integer expression, when setting string index");

  integer idx = AS_INT(v);

  if (0 > idx) idx += str->num_bytes;
  if (idx < 0 || (size_t) idx >= str->num_bytes)
    THROW_OUT_OF_BOUNDS("string index %d >= than %d length, or less than zero", idx, str->num_bytes);

  THROW_SYNTAX_ERR_IFNOT(TOKEN == TOKEN_ASSIGN,
    "syntax error while setting string, awaiting =");

  NEXT_TOKEN();
  err = l_parse_expr (this, &v);
  THROW_ERR_IF_ERR(err);

  THROW_SYNTAX_ERR_IFNOT(v.type == INTEGER_TYPE,
    "awaiting an integer expression, when setting string index");

  integer chr = AS_INT(v);
  // PROBLEMATIC as idx can be anywhere - FIXME
  // fixing this probably means changing indexing semantics, to mean character 
  // instead of byte and probably std string functions needs to adapt
  int num_bytes = utf8_charlen ((uchar) *str->bytes + idx);
  char buf[8];
  utf8_character (chr, buf, 8);
  string_replace_numbytes_at_with (str, num_bytes, idx, buf);
  return L_OK;
}

static VALUE array_release (VALUE value) {
  VALUE result = INT(L_OK);

  if (value.refcount > 0) goto theend;

  if (value.refcount < 0)
    if (value.refcount != ARRAY_LITERAL)
      return result;

  ArrayType *array = (ArrayType *) AS_ARRAY(value);
  if (NULL == array) return result;

  VALUE ary = array->value;

  if (array->type == STRING_TYPE) {
    string **s_ar = (string **) AS_ARRAY(ary);
    if (s_ar == NULL) return result;
    for (int i = 0; i < array->len; i++) {
      ifnot (NULL == s_ar[i])
        string_release (s_ar[i]);
    }
    Release (s_ar);
    s_ar = NULL;
  } else if (array->type == MAP_TYPE) {
    Map_Type **m_ar = (Map_Type **) AS_ARRAY(ary);
    if (m_ar == NULL) return result;
    for (int i = 0; i < array->len; i++)
      map_release (&m_ar[i]);
    Release (m_ar);
    m_ar = NULL;
  } else if (array->type == LIST_TYPE) {
    listArrayMember **l_ar = (listArrayMember **) AS_ARRAY(ary);
    if (l_ar == NULL) return result;
    for (int i = 0; i < array->len; i++)
      object_release (NULL, LIST(l_ar[i]));
    Release (l_ar);
    l_ar = NULL;
  } else if (array->type == ARRAY_TYPE) {
    ArrayType **a_ar = (ArrayType **) AS_ARRAY(ary);
    if (a_ar == NULL) return result;
    for (int i = 0; i < array->len; i++)
      array_release (ARRAY(a_ar[i]));
    Release (a_ar);
    a_ar = NULL;
  } else {
    char *ar = (char *) AS_ARRAY(ary);
    if (ar == NULL) return result;
    Release (ar);
  }

  Release (array);
  array = NULL;

  value = NULL_VALUE;

theend:
  value.refcount--;
  return result;
}

static int l_parse_anon_array (l_t *this, VALUE *vp) {
  int err;

  THROW_SYNTAX_ERR_IF(NEXT_BYTE_NOWS_NONL() == TOKEN_INDEX_CLOS,
    "inline empty array is not supported");

  int instr = 0;
  int strtype = '"';
  int inmap = 0;
  int indtokopen = 1;
  int indtokclos = 0;
  int parenopen = 0;
  int inparen = 0;
  int inar = 0;
  int inlist = 0;
  int is_array_of_array = 0;
  int num_elem = 1;

  uint n = 0;
  if (PEEK_NTH_BYTE_NOWS_NONL(&n) == TOKEN_INDEX_OPEN)
    is_array_of_array = 1;

  int pc;
  int c = 0;

  n = 0;
  const char *ptr = GETSTRPTR(PARSEPTR);

  while (1) {
    pc = c;
    c = *ptr++;

    THROW_SYNTAX_ERR_IF(c == TOKEN_EOF, "unended array");

    if (c == TOKEN_INDEX_CLOS && 0 == instr + inparen) {
      if (inmap)
        continue;

      if (*ptr == TOKEN_MAP_CLOS && inlist) {
        ptr++;
        inlist--;
        continue;
      }

      indtokclos++;

      ifnot (indtokopen - indtokclos)
        break;

      if (1 == indtokopen - indtokclos)
        inar = 0;

      continue;
    }

    if (c == TOKEN_INDEX_OPEN && 0 == instr + inparen) {
      if (inmap)
        continue;

      indtokopen++;

      if (indtokopen - indtokclos == 2)
        inar = 1;

     continue;
    }

    if (c == TOKEN_PAREN_OPEN) {
      if (instr)
        continue;

      parenopen++;
      inparen = 1;
      continue;
    }

    if (c == TOKEN_PAREN_CLOS) {
      if (instr)
        continue;

      parenopen--;
      inparen = parenopen;
      continue;
    }

    if (c == TOKEN_DQUOTE || c == TOKEN_BQUOTE) {
      ifnot (inparen) {
        if (instr) {
          if (c == strtype && pc != TOKEN_ESCAPE_CHR) {
            instr = 0;
            continue;
          }
        } else {
          instr = 1;
          strtype = c;
          continue;
        }
      }
    }

    if (c == TOKEN_MAP_OPEN && 0 == instr + inparen) {
      if (*ptr == TOKEN_INDEX_OPEN) {
        ptr++;
        inlist++;
      } else
        inmap++;

      continue;
    }

    if (c == TOKEN_MAP_CLOS && 0 == instr + inparen) {
      inmap--;

      continue;
    }

    //if ((c == TOKEN_COMMA || c == TOKEN_NL) && 0 == instr && 0 == inmap && 0 == inar)
    if (c == TOKEN_COMMA && 0 == instr + inmap + inar + inparen + inlist)
      num_elem++;
  }

  if (is_array_of_array) {
    VALUE ary = ARRAY(ARRAY_NEW(ARRAY_TYPE, num_elem));
    ArrayType *array = (ArrayType *) AS_ARRAY(ary);
    VALUE ar = array->value;

    err = l_array_set_as_array (this, ar, num_elem, 0, num_elem - 1);
    *vp = ary;
    return err;
  }

  VALUE v;
  this->curState |= MALLOCED_STRING_STATE;
  NEXT_TOKEN();
  err = l_parse_expr (this, &v);
  this->curState &= ~MALLOCED_STRING_STATE;
  THROW_ERR_IF_ERR(err);

  int type = v.type;

  VALUE ary = ARRAY(ARRAY_NEW(type, num_elem));
  ArrayType *array = (ArrayType *) AS_ARRAY(ary);
  VALUE ar = array->value;

  switch (type) {
    case STRING_TYPE: {
      string **s_ar = (string **) AS_ARRAY(ar);
      string *item = s_ar[0];
      string *val = AS_STRING(v);
      string_replace_with_len (item, val->bytes, val->num_bytes);

      if (v.sym == NULL &&
          v.refcount != MALLOCED_STRING &&
          0 == (this->objectState & (ARRAY_MEMBER|MAP_MEMBER))) {
        l_release_value (this, v);
        this->objectState &= ~(ARRAY_MEMBER|MAP_MEMBER);
      }

      break;
    }

    case MAP_TYPE: {
      Map_Type **m_ar = (Map_Type **) AS_ARRAY(ar);
      Map_Type *item = m_ar[0];
      ifnot (NULL == item)
        map_release (&item);
      m_ar[0] = AS_MAP(v);
      break;
    }

    case LIST_TYPE: {
      listArrayMember **l_ar = (listArrayMember **) AS_ARRAY(ar);
      listArrayMember *item = l_ar[0];
      ifnot (NULL == item)
        l_release_value (this, LIST(item));

      l_ar[0] = LIST_ARRAY_MEMBER(v);
      break;
    }

    case INTEGER_TYPE: {
      integer *i_ar = (integer *) AS_ARRAY(ar);
      i_ar[0] = AS_INT(v);
      break;
    }

    case NUMBER_TYPE: {
      number *n_ar = (number *) AS_ARRAY(ar);
      n_ar[0] = AS_NUMBER(v);
      break;
    }

    default:
      THROW_SYNTAX_ERR("unsupported array type");
  }

  *vp = ary;

  if (this->exprList) {
    vp->refcount = ARRAY_LITERAL;
    this->objectState |= ANNON_ARRAY;
  }

  VALUE fidx = INT(1);
  VALUE lidx = INT(num_elem - 1);

  ifnot (1 == num_elem)
    return l_array_assign (this, &ary, fidx, lidx, 1);

  return err;
}

static ArrayType *array_copy (ArrayType *array) {
  ArrayType *new_array = Alloc (sizeof (ArrayType));

  int len = array->len;
  integer type = array->type;

  new_array->len = len;
  new_array->type = type;

  switch (type) {
    case NUMBER_TYPE: {
      number *ary = (number *) AS_ARRAY(array->value);

      number *n_ar = Alloc (len * sizeof (number));
      for (int i = 0; i < len; i++)
        n_ar[i] = ary[i];

      new_array->value = ARRAY(n_ar);
      break;
    }

    case STRING_TYPE: {
      string **ary = (string **) AS_ARRAY(array->value);

      string **s_ar = Alloc (len * sizeof (string));
      for (int i = 0; i < len; i++) {
        string *item = ary[i];
        s_ar[i] = string_new_with_len (item->bytes, item->num_bytes);
      }
      new_array->value = ARRAY(s_ar);
      break;
    }

    case MAP_TYPE: {
      Map_Type **ary = (Map_Type **) AS_ARRAY(array->value);

      Map_Type **m_ar = Alloc (len * sizeof (Map_Type));
      for (int i = 0; i < len; i++) {
        Map_Type *item = ary[i];
        m_ar[i] = AS_MAP(l_copy_map (MAP(item)));
      }
      new_array->value = ARRAY(m_ar);
      break;
    }

    default: {
      integer *ary = (integer *) AS_ARRAY(array->value);

      integer *i_ar = Alloc (len * sizeof (integer));
      for (int i = 0; i < len; i++)
        i_ar[i] = ary[i];

      new_array->value = ARRAY(i_ar);
      break;
    }
  }

  return new_array;
}

static int l_array_set_as_array (l_t *this, VALUE ar, integer len, integer idx, integer last_idx) {
  int err;
  VALUE val;

  ArrayType **a_ar = (ArrayType **) AS_ARRAY(ar);

  do {
    if (idx < 0 || idx >= len || idx > last_idx)
      THROW_OUT_OF_BOUNDS("array index %d >= than %d length, less than zero or %d > than last index %d",
        idx, len, idx, last_idx);

    NEXT_TOKEN();

    err = l_parse_expr (this, &val);
    THROW_ERR_IF_ERR(err);

    THROW_SYNTAX_ERR_IF(val.type != ARRAY_TYPE && val.type != NULL_TYPE,
      "error while setting Array of an array, awaiting an array or null");

    if (a_ar[idx] != NULL)
      array_release (val);

    if (ar.type != ARRAY_TYPE)
      a_ar[idx] = NULL;
    else
      a_ar[idx] = (ArrayType *) AS_ARRAY(val);

    idx++;
  } while (TOKEN == TOKEN_COMMA);

  if (idx - 1 != last_idx)
    THROW_OUT_OF_BOUNDS("array index %d - 1 is not last index %d", idx - 1, last_idx);

  return L_OK;
}

static int l_array_set_as_map (l_t *this, VALUE ar, integer len, integer idx, integer last_idx) {
  int err;
  VALUE val;

  Map_Type **m_ar = (Map_Type **) AS_ARRAY(ar);

  do {
    if (idx < 0 || idx >= len || idx > last_idx)
      THROW_OUT_OF_BOUNDS("array index %d >= than %d length, less than zero or %d > than last index %d",
         idx, len, idx, last_idx);

    NEXT_TOKEN();

    err = l_parse_expr (this, &val);
    THROW_ERR_IF_ERR(err);

    THROW_SYNTAX_ERR_IF(val.type != MAP_TYPE && val.type != NULL_TYPE,
      "error while setting Map array, awaiting a map or null");

    m_ar[idx] = (val.type == MAP_TYPE) ? AS_MAP(val) : NULL;
    idx++;
  } while (TOKEN == TOKEN_COMMA);

  if (idx - 1 != last_idx)
    THROW_OUT_OF_BOUNDS("array index %d - 1 is not last index %d", idx - 1, last_idx);

  if (TOKEN == TOKEN_NL && PEEK_NTH_TOKEN(0) == TOKEN_INDEX_CLOS)
    NEXT_TOKEN();

  return L_OK;
}

static int l_array_set_as_list (l_t *this, VALUE ar, integer len, integer idx, integer last_idx) {
  int err;
  VALUE val;

  listArrayMember **l_ar = (listArrayMember **) AS_ARRAY(ar);

  do {
    if (idx < 0 || idx >= len || idx > last_idx)
      THROW_OUT_OF_BOUNDS("array index %d >= than %d length, less than zero or %d > than last index %d",
         idx, len, idx, last_idx);

    NEXT_TOKEN();

    err = l_parse_expr (this, &val);
    THROW_ERR_IF_ERR(err);

    THROW_SYNTAX_ERR_IF(val.type != LIST_TYPE && val.type != NULL_TYPE,
      "error while setting List array, awaiting a list or null");

    l_ar[idx] = (val.type == LIST_TYPE) ? LIST_ARRAY_MEMBER(val) : NULL;
    idx++;
  } while (TOKEN == TOKEN_COMMA);

  if (idx - 1 != last_idx)
    THROW_OUT_OF_BOUNDS("array index %d - 1 is not last index %d", idx - 1, last_idx);

  if (TOKEN == TOKEN_NL && PEEK_NTH_TOKEN(0) == TOKEN_INDEX_CLOS)
    NEXT_TOKEN();

  return L_OK;
}

static int l_array_set_as_string (l_t *this, VALUE ar, integer len, integer idx, integer last_idx) {
  int err;
  VALUE val;

  string **s_ar = (string **) AS_ARRAY(ar);

  do {
    if (idx < 0 || idx >= len || idx > last_idx)
      THROW_OUT_OF_BOUNDS("array index %d >= than %d length, less than zero or %d > than last index %d",
        idx, len, idx, last_idx);

    this->curState |= MALLOCED_STRING_STATE;
    NEXT_TOKEN();

    err = l_parse_expr (this, &val);
    this->curState &= ~MALLOCED_STRING_STATE;
    THROW_ERR_IF_ERR(err);

    switch (val.type) {
      case STRING_TYPE: {
        ifnot (NULL == s_ar[idx]) {
          string *item = s_ar[idx];
          string *s_val = AS_STRING(val);
          string_replace_with_len (item, s_val->bytes, s_val->num_bytes);
        } else {
          string *s_val = AS_STRING(val);
          s_ar[idx] = string_new_with_len (s_val->bytes, s_val->num_bytes);
        }

        if (val.sym == NULL &&
            val.refcount != MALLOCED_STRING &&
            0 == (this->objectState & (ARRAY_MEMBER|MAP_MEMBER))) {
          l_release_value (this, val);
          this->objectState &= ~(ARRAY_MEMBER|MAP_MEMBER);
        }

        break;
      }

      case NULL_TYPE:
        ifnot (NULL == s_ar[idx]) {
          string *item = s_ar[idx];
          string_release (item);
        }

        s_ar[idx] = NULL;
        break;

      default:
        THROW_SYNTAX_ERR("error while setting string array, awaiting a string or null");
    }

    idx++;
  } while (TOKEN == TOKEN_COMMA);
  // } while (TOKEN == TOKEN_COMMA || (TOKEN == TOKEN_NL && PEEK_NTH_TOKEN(0) != TOKEN_INDEX_CLOS));

  if (idx - 1 != last_idx)
    THROW_OUT_OF_BOUNDS("array index %d - 1 is not last index %d", idx - 1, last_idx);

  return L_OK;
}

static int l_array_set_as_number (l_t *this, VALUE ar, integer len, integer idx, integer last_idx) {
  int err;
  VALUE val;

  number *n_ar = (number *) AS_ARRAY(ar);

  do {
    if (idx < 0 || idx >= len || idx > last_idx)
      THROW_OUT_OF_BOUNDS("array index %d >= than %d length, less than zero or %d > than last index %d",
        idx, len, idx, last_idx);

    NEXT_TOKEN();

    err = l_parse_expr (this, &val);
    THROW_ERR_IF_ERR(err);

    n_ar[idx] = AS_NUMBER(val);

    idx++;
  } while (TOKEN == TOKEN_COMMA);

  if (idx - 1 != last_idx)
    THROW_OUT_OF_BOUNDS("array index %d - 1 is not last index %d", idx - 1, last_idx);

  return L_OK;
}

static int l_array_set_as_int (l_t *this, VALUE ar, integer len, integer idx, integer last_idx) {
  int err;
  VALUE val;

  integer *s_ar = (integer *) AS_ARRAY(ar);

  do {
    if (idx < 0 || idx >= len || idx > last_idx)
      THROW_OUT_OF_BOUNDS("array index %d >= than %d length, less than zero or %d > than last index %d",
        idx, len, idx, last_idx);

    NEXT_TOKEN();

    err = l_parse_expr (this, &val);
    THROW_ERR_IF_ERR(err);

    s_ar[idx] = AS_INT(val);

    idx++;
  } while (TOKEN == TOKEN_COMMA);

  if (idx - 1 != last_idx)
    THROW_OUT_OF_BOUNDS("array index %d - 1 is not last index %d", idx - 1, last_idx);

  return L_OK;
}

static int l_array_set_with_expr (l_t *this, VALUE ar, integer len, integer idx, integer last_idx, int type) {
  int err;
  VALUE val;

  switch (type) {
    case MAP_TYPE:
      THROW_UNIMPLEMENT("set an array with a function call for maps");
    case LIST_TYPE:
      THROW_UNIMPLEMENT("set an array with a function call for lists");
    case ARRAY_TYPE:
      THROW_UNIMPLEMENT("set an array with a function call for arrays");
  }

  err = l_parse_expr (this, &val);
  THROW_ERR_IF_ERR(err);

  ifnot (IS_ARRAY(val))
    THROW_A_TYPE_MISMATCH(val.type, "an array");

  ArrayType *n_array = (ArrayType *) AS_ARRAY(val);
  integer n_len = n_array->len;

  if (type != n_array->type)
    THROW_TYPE_MISMATCH(type, n_array->type);

  if (idx < 0 || idx >= len || idx > last_idx || (idx - 1) + n_len > last_idx)
    THROW_OUT_OF_BOUNDS("array index %d >= than %d length, less than zero or %d > than last index %d",
        idx, len, idx, last_idx);

  switch (type) {
    case INTEGER_TYPE: {
      integer *src_i_ar = (integer *) AS_ARRAY(n_array->value);
      integer *dest_i_ar = (integer *) AS_ARRAY(ar);
      for (int i = idx, d = 0; i <= last_idx; i++, d++)
         dest_i_ar[i] = src_i_ar[d];

      l_release_value (this, val);
      break;
    }

    case NUMBER_TYPE: {
      number *src_n_ar = (number *) AS_ARRAY(n_array->value);
      number *dest_n_ar = (number *) AS_ARRAY(ar);
      for (int i = idx, d = 0; i <= last_idx; i++, d++)
         dest_n_ar[i] = src_n_ar[d];

      l_release_value (this, val);
      break;
    }

    case STRING_TYPE: {
      string **src_s_ar = (string **) AS_ARRAY(n_array->value);
      string **dest_s_ar = (string **) AS_ARRAY(ar);
      for (int i = idx, d = 0; i <= last_idx; i++, d++) {
         string *dest_s_item = dest_s_ar[i];
         string *src_s_item = src_s_ar[d];
         string_replace_with_len (dest_s_item, src_s_item->bytes, src_s_item->num_bytes);
      }

      l_release_value (this, val);
      break;
    }
  }

  return L_OK;
}

/* Initial primitive Interface by MickeyDelp <mickey at delptronics dot com> */
static int l_array_assign (l_t *this, VALUE *ar, VALUE ix, VALUE last_ix, int is_single) {
  int err;

  ArrayType *array = (ArrayType *) AS_ARRAY((*ar));
  integer len = array->len;

  integer idx = AS_INT(ix);
  if (0 > idx)
    idx += len;

  integer last_idx = AS_INT(last_ix);
  if (0 > last_idx)
    last_idx += len;

  if (last_idx < 0 || last_idx >= len)
    THROW_OUT_OF_BOUNDS("array index %d >= %d len, or less than zero", last_idx, len);

  VALUE ary = array->value;

  ifnot (is_single) {
    NEXT_TOKEN();
    if (TOKEN != TOKEN_INDEX_OPEN)
      return l_array_set_with_expr (this, ary, len, idx, last_idx, array->type);
  }

  switch (array->type) {
    case INTEGER_TYPE:
      err = l_array_set_as_int (this, ary, len, idx, last_idx);
      break;

    case STRING_TYPE:
      err = l_array_set_as_string (this, ary, len, idx, last_idx);
      break;

    case MAP_TYPE:
      err = l_array_set_as_map (this, ary, len, idx, last_idx);
      break;

    case ARRAY_TYPE:
      err = l_array_set_as_array (this, ary, len, idx, last_idx);
      break;

    case LIST_TYPE:
      err = l_array_set_as_list (this, ary, len, idx, last_idx);
      break;

    default:
      err = l_array_set_as_number (this, ary, len, idx, last_idx);
  }

  THROW_ERR_IF_ERR(err);

  ifnot (is_single) {
    if (TOKEN == TOKEN_NL)
      NEXT_TOKEN();

    THROW_SYNTAX_ERR_IFNOT(TOKEN == TOKEN_INDEX_CLOS,
      "array assignment: awaiting ]");

    NEXT_TOKEN();
  }

  return L_OK;
}

static int l_parse_array_def (l_t *this, VALUE *vp, int flags) {
  int err;
  int type = TOKENSYM->type;

  NEXT_TOKEN();

  THROW_SYNTAX_ERR_IFNOT(TOKEN == TOKEN_INDEX_OPEN, "array assignment: awaiting [");

  int c = TOKEN;

  NEXT_TOKEN();

  VALUE len;
  err = l_parse_expr (this, &len);
  THROW_ERR_IF_ERR(err);
  THROW_SYNTAX_ERR_IFNOT(TOKEN == TOKEN_INDEX_CLOS, "array assignment: awaiting ]");
  THROW_SYNTAX_ERR_IFNOT(len.type == INTEGER_TYPE, "awaiting an integer expression, when getting array length");

  NEXT_TOKEN();

  VALUE ar;
  if (flags & (ARRAY_IS_MAP_MEMBER|ARRAY_IS_REASSIGNMENT)) {
    c = TOKEN;
    integer nlen = AS_INT(len);
    ar = ARRAY(ARRAY_NEW(type, nlen));
    *vp = ar;
    TOKENVAL = ar;
    goto assign;
  }

  THROW_SYNTAX_ERR_IF(TOKEN != TOKEN_SYMBOL && TOKEN != TOKEN_ARRAY,
    "syntax error, awaiting an identifier for the array declaration");

  l_string name = TOKENSTR;

  integer nlen = AS_INT(len);

  ar = ARRAY(ARRAY_NEW(type, nlen));

  funT *scope = (this->scopeState == PUBLIC_SCOPE ? this->function : this->curScope);
  TOKENSYM = l_define_symbol (this, scope, sym_key (this, name), ARRAY_TYPE,
      ar, 0);
  this->scopeState = 0;

  NEXT_TOKEN();
  c = TOKEN;

assign:
  if (c == TOKEN_ASSIGN) {
    VALUE at_idx = INT(0);
    VALUE last_idx = INT(-1);
    return l_array_assign (this, &ar, at_idx, last_idx, 0);
  }

  return L_OK;
}

static int l_parse_array_set (l_t *this) {
  int c = NEXT_BYTE_NOWS_NONL();

  if (c == '=') {
    VALUE v = TOKENVAL;
    sym_t *sym = v.sym;

    THROW_SYNTAX_ERR_IF(sym == NULL, "unexpected error, variable is not associated with a symbol");
    THROW_SYNTAX_ERR_IF(sym->is_const, "can not reassign a new value, to a constant declared list");
    sym->type = NULL_TYPE;
    sym->value = NULL_VALUE;

    array_release (v);

    return L_MMT_REASSIGN;
  }

  VALUE ary = TOKENVAL;

  int err;

  NEXT_TOKEN();

  if (TOKEN == TOKEN_REASSIGN) {
    VALUE v = ary;
    sym_t *sym = v.sym;
    THROW_SYNTAX_ERR_IF(sym->is_const, "can not reassign a new value, to a constant declared array");
    ary.sym = NULL;
    err = l_parse_chain (this, &ary);
    THROW_ERR_IF_ERR(err);
    ary.sym = sym;
    sym->value = ary;
    sym->type = ary.type;
    return err;
  }

  VALUE ix = INT(0);

  c = TOKEN;

  VALUE last_ix = INT(-1);

  int is_index = c == TOKEN_INDEX_OPEN;

  if (is_index) {
    uint n = 0;
    c = PEEK_NTH_BYTE_NOWS_NONL(&n);
    if (c == TOKEN_STAR) {
      for (uint i = 0; i <= n; i++)
        IGNORE_NEXT_BYTE;

      NEXT_TOKEN();

      if (TOKEN != TOKEN_INDEX_CLOS)
        THROW_SYNTAX_ERR("array set: awaiting ]");

      NEXT_TOKEN();

      if (TOKEN < TOKEN_ASSIGN)
        THROW_SYNTAX_ERR("syntax error while setting array, awaiting =");

      VALUE ar_val;
      NEXT_TOKEN();
      err = l_parse_expr (this, &ar_val);
      THROW_ERR_IF_ERR(err);

      ArrayType *array = (ArrayType *) AS_ARRAY(ary);
      if (array->type != ar_val.type)
        THROW_TYPE_MISMATCH(array->type, ar_val.type);

      switch (array->type) {
        case STRING_TYPE: {
          string **s_ar = (string **) AS_ARRAY(array->value);
          string *s_val = AS_STRING(ar_val);
          for (int i = 0; i < array->len; i++) {
            string *item = s_ar[i];
            string_replace_with_len (item, s_val->bytes, s_val->num_bytes);
          }

          string_release (s_val);
          return L_OK;
        }

        case MAP_TYPE: {
          Map_Type **m_ar = (Map_Type **) AS_ARRAY(array->value);
          for (int i = 0; i < array->len; i++) {
            ifnot (NULL == m_ar[i])
              map_release (&m_ar[i]);
            m_ar[i] = AS_MAP(l_copy_value (ar_val));
          }

          if (NULL == ar_val.sym)
            l_release_value (this, ar_val);

          return L_OK;
        }

        case LIST_TYPE: {
          listArrayMember **l_ar = (listArrayMember **) AS_ARRAY(array->value);
          for (int i = 0; i < array->len; i++) {
            ifnot (NULL == l_ar[i])
               l_release_value (this, LIST(l_ar[i]));
            l_ar[i] = LIST_ARRAY_MEMBER(l_copy_value (ar_val));
          }

          if (NULL == ar_val.sym)
            l_release_value (this, ar_val);

          return L_OK;
        }

        case INTEGER_TYPE: {
          int i_val = AS_INT(ar_val);
          integer *i_ar = (integer *) AS_ARRAY(array->value);
          for (int i = 0; i < array->len; i++)
            i_ar[i] = i_val;
          return L_OK;
        }

        case NUMBER_TYPE: {
          number n_val = AS_NUMBER(ar_val);
          number *n_ar = (number *) AS_ARRAY(array->value);
          for (int i = 0; i < array->len; i++)
            n_ar[i] = n_val;
          return L_OK;
        }

        default:
          THROW_INV_ARRAY_TYPE(array->type);
      }
    }

    this->curState |= INDEX_STATE;
    err = l_parse_primary (this, &ix);
    this->curState &= ~INDEX_STATE;
    THROW_ERR_IF_ERR(err);

    if (TOKEN == TOKEN_COLON) {
      NEXT_TOKEN();
      c = TOKEN;
      if (c && c != TOKEN_INDEX_CLOS) {
        err = l_parse_expr (this, &last_ix);
        THROW_ERR_IF_ERR(err);
      }

      NEXT_TOKEN();
      is_index = 0;
    }
  }

  c = TOKEN;

  if (c == TOKEN_INDEX_OPEN) {
    ArrayType *array = (ArrayType *) AS_ARRAY(ary);
    VALUE ar = array->value;

    switch (array->type) {
      case STRING_TYPE: {
        string **s_ar = (string **) AS_ARRAY(ar);
        VALUE v = STRING(s_ar[AS_INT(ix)]);
        UNGET_BYTE();
        return l_parse_string_set_char (this, v, 0);
      }

      case LIST_TYPE:
        return l_parse_list_set (this);

      default:
        THROW_SYNTAX_ERR("Arrays of arrays haven't been implemented");
    }
  }

  if (c == TOKEN_DOT) {
    ArrayType *array = (ArrayType *) AS_ARRAY(ary);
    VALUE ar = array->value;

    switch (array->type) {
      case MAP_TYPE: {
        Map_Type **m_ar = (Map_Type **) AS_ARRAY(ar);
        VALUE v = MAP(m_ar[AS_INT(ix)]);
        TOKENVAL = v;
        UNGET_BYTE();
        err = l_parse_map_set (this);
        return err;
      }

      default:
        THROW_SYNTAX_ERR("not a map");
    }
  }

  if (c == TOKEN_MAP) {
    err = l_parse_map_get (this, &ix);
    THROW_ERR_IF_ERR(err);

    if (TOKEN == TOKEN_INDEX_CLOS) {
      NEXT_TOKEN();
      is_index = 1;
    }
  }

  int token = TOKEN;
  int is_un = 0;

  if (token > TOKEN_ASSIGN) {
    if (token == TOKEN_PLUS_PLUS || token == TOKEN_MINUS_MINUS) {
      int peek = PEEK_NTH_BYTE(0);
      switch (peek) {
        case TOKEN_INDEX_CLOS:
        case TOKEN_SEMICOLON:
        case TOKEN_PAREN_CLOS:
        case TOKEN_NL:
        case TOKEN_COLON:
        case ' ':
        case '\t':
        case TOKEN_EOF:
          break;

        default:
          THROW_SYNTAX_ERR_FMT("unexpected token after %s",
            (token == TOKEN_PLUS_PLUS ? "++" : "--"));
      }

      int idx = AS_INT(ix);
      ArrayType *array = (ArrayType *) AS_ARRAY(ary);

      switch (array->type) {
        case INTEGER_TYPE: {
          integer *i_ar = (integer *) AS_ARRAY(array->value);
          ifnot (is_index) {
            for (int i = 0; i < array->len; i++)
              i_ar[i] += (token == TOKEN_PLUS_PLUS ? 1 : -1);
          } else {
            i_ar[idx] += (token == TOKEN_PLUS_PLUS ? 1 : -1);
          }

          NEXT_TOKEN();
          return L_OK;
        }

        case NUMBER_TYPE: {
          number *n_ar = (number *) AS_ARRAY(array->value);
          ifnot (is_index) {
            for (int i = 0; i < array->len; i++)
              n_ar[i] += (token == TOKEN_PLUS_PLUS ? 1 : -1);
          } else {
            n_ar[idx] += (token == TOKEN_PLUS_PLUS ? 1 : -1);
          }

          NEXT_TOKEN();
          return L_OK;
        }

        default:
          THROW_SYNTAX_ERR("awaiting an array of Integer or a Number Type");
      }
    }

    NEXT_TOKEN();

    if (TOKEN == TOKEN_UNARY) {
      is_un = 1;
      NEXT_TOKEN();
    }

    VALUE v;
    err = l_parse_expr (this, &v);
    THROW_ERR_IF_ERR(err);

    if (is_un) {
      if (v.type == INTEGER_TYPE)
        AS_INT(v) = ~AS_INT(v);
      else
        THROW_SYNTAX_ERR("error while setting an unary object, awaiting an integer");
    }

    ArrayType *array = (ArrayType *) AS_ARRAY(ary);
    int idx = AS_INT(ix);
    VALUE val;

    ifnot (is_index) {
      switch (array->type) {
        case INTEGER_TYPE: {
          THROW_SYNTAX_ERR_IF(v.type != INTEGER_TYPE, "binary operation error in array, awaiting an integer");
          integer *i_ar = (integer *) AS_ARRAY(array->value);
          int vi = AS_INT(v);

          for (int i = 0; i < array->len; i++) {
            switch (token) {
              case TOKEN_ASSIGN_APP:
                i_ar[i] += vi; break;
              case TOKEN_ASSIGN_SUB:
                i_ar[i] -= vi; break;
              case TOKEN_ASSIGN_DIV:
                i_ar[i] /= vi; break;
              case TOKEN_ASSIGN_MUL:
                i_ar[i] *= vi; break;
              case TOKEN_ASSIGN_MOD:
                i_ar[i] %= vi; break;
              case TOKEN_ASSIGN_BAR:
                i_ar[i] |= vi; break;
              case TOKEN_ASSIGN_AND:
                i_ar[i] &= vi; break;
              case TOKEN_ASSIGN_XOR:
                i_ar[i] ^= vi; break;
            }
          }
          return L_OK;
        }

        case NUMBER_TYPE: {
          THROW_SYNTAX_ERR_IF(v.type != NUMBER_TYPE, "binary operation error in array, awaiting a number");
          number *n_ar = (number *) AS_ARRAY(array->value);
          double vn = AS_NUMBER(v);
          for (int i = 0; i < array->len; i++) {
            switch (token) {
              case TOKEN_ASSIGN_APP:
                n_ar[i] += vn; break;
              case TOKEN_ASSIGN_SUB:
                n_ar[i] -= vn; break;
              case TOKEN_ASSIGN_DIV:
                n_ar[i] /= vn; break;
              case TOKEN_ASSIGN_MUL:
                n_ar[i] *= vn; break;
              default:
                THROW_SYNTAX_ERR("only adding/substracting/division/multiplication implemented for number arrays");
            }
          }
          return L_OK;
        }

        case STRING_TYPE: {
          THROW_SYNTAX_ERR_IF(v.type != STRING_TYPE && v.type != INTEGER_TYPE, "binary operation error in a string array, awaiting a string || an integer");
          THROW_SYNTAX_ERR_IF(token != TOKEN_ASSIGN_APP, "binary operation error in string array, only append == implemented");

          string **s_ar = (string **) AS_ARRAY(array->value);
          if (v.type == STRING_TYPE) {
            string *s = AS_STRING(v);
            for (int i = 0; i < array->len; i++)
              if (s_ar[i] != NULL)
                string_append_with_len (s_ar[i], s->bytes, s->num_bytes);

            if (v.sym == NULL && v.refcount != MALLOCED_STRING &&
              0 == (this->objectState & (ARRAY_MEMBER|MAP_MEMBER))) {
              l_string_release (v);
              this->objectState |= RHS_STRING_RELEASED;
              this->objectState &= ~(ARRAY_MEMBER|MAP_MEMBER);
            }

          } else {
            char buf[8];
            int len = utf8_character (AS_INT(v), buf, 8);
            for (int i = 0; i < array->len; i++)
              if (s_ar[i] != NULL)
                string_append_with_len (s_ar[i], buf, len);
          }

          return L_OK;
        }

        default:
          THROW_SYNTAX_ERR("binary operation error in array, only implemented for string, integer && number types");
      }

    } else {
      switch (array->type) {
        case STRING_TYPE: {
          string **s_ar = (string **) AS_ARRAY(array->value);
          val = STRING(s_ar[idx]);
          break;
        }

        case INTEGER_TYPE: {
          integer *i_ar = (integer *) AS_ARRAY(array->value);
          val = INT(i_ar[idx]);
          break;
        }

        case NUMBER_TYPE: {
          number *n_ar = (number *) AS_ARRAY(array->value);
          val = NUMBER(n_ar[idx]);
          break;
        }

        default:
          THROW_A_TYPE_MISMATCH(array->type, " == not allowed for a binary operation for arrays");
      }
    }

    VALUE result;
    switch (token) {
      case TOKEN_ASSIGN_APP:
        this->objectState |= OBJECT_APPEND;
        result = l_add (this, val, v);
        this->objectState &= ~OBJECT_APPEND;
        break;

      case TOKEN_ASSIGN_SUB:
        result = l_sub (this, val, v); break;
      case TOKEN_ASSIGN_DIV:
        result = l_div (this, val, v); break;
      case TOKEN_ASSIGN_MUL:
        result = l_mul (this, val, v); break;
      case TOKEN_ASSIGN_MOD:
        result = l_mod (this, val, v); break;
      case TOKEN_ASSIGN_BAR:
        result = l_bset (this, val, v); break;
      case TOKEN_ASSIGN_AND:
        result = l_bnot (this, val, v); break;
      case TOKEN_ASSIGN_XOR:
        result = l_bitxor (this, val, v); break;
    }

    if (result.type == NULL_TYPE)
       THROW_SYNTAX_ERR("unxpected array operation");

    switch (array->type) {
      case STRING_TYPE: {
        string **s_ar = (string **) AS_ARRAY(array->value);
        s_ar[idx] = AS_STRING(result);
        break;
      }

      case INTEGER_TYPE: {
        integer *i_ar = (integer *) AS_ARRAY(array->value);
        i_ar[idx] = AS_INT(result);
        break;
      }

      case NUMBER_TYPE: {
        number *n_ar = (number *) AS_ARRAY(array->value);
        n_ar[idx] = AS_NUMBER(result);
        break;
      }
    }

    return L_OK;
  }

  if (TOKEN < TOKEN_ASSIGN)
    THROW_SYNTAX_ERR("syntax error while setting array, awaiting =");

  ifnot (is_index)
    return l_array_assign (this, &ary, ix, last_ix, is_index);

  last_ix = INT(AS_INT(ix));
  return l_array_assign (this, &ary, ix, last_ix, is_index);
}

static int l_array_from_array (l_t *this, ArrayType *src_ar, VALUE v_iar, VALUE *vp) {
  (void) this;
  ArrayType *ary = (ArrayType *) AS_ARRAY(v_iar);

  if (ary->type != INTEGER_TYPE)
    THROW_SYNTAX_ERR("awaiting an integer type array");

  ArrayType *array = ARRAY_NEW(src_ar->type, (integer) ary->len);

  integer *x_ar = (integer *) AS_ARRAY(ary->value);

  switch (src_ar->type) {
    case STRING_TYPE: {
      string **s_ar = (string **) AS_ARRAY(array->value);
      string **s_ar_src = (string **) AS_ARRAY(src_ar->value);
      for (int i = 0; i < ary->len; i++) {
        int idx = x_ar[i];
        if (idx >= src_ar->len)
          THROW_OUT_OF_BOUNDS("array index %d >= than %d length", idx, src_ar->len);

        string_replace_with_len (s_ar[i], s_ar_src[idx]->bytes, s_ar_src[idx]->num_bytes);
      }
      break;
    }

    case INTEGER_TYPE: {
      integer *i_ar = (integer *) AS_ARRAY(array->value);
      integer *i_ar_src = (integer *) AS_ARRAY(src_ar->value);
      for (int i = 0; i < ary->len; i++) {
        int idx = x_ar[i];
        if (idx >= src_ar->len)
          THROW_OUT_OF_BOUNDS("array index %d >= than %d length", idx, src_ar->len);

        i_ar[i] = i_ar_src[idx];
      }
      break;
    }

    case NUMBER_TYPE: {
      double *d_ar = (double *) AS_ARRAY(array->value);
      double *d_ar_src = (double *) AS_ARRAY(src_ar->value);
      for (int i = 0; i < ary->len; i++) {
        int idx = x_ar[i];
        if (idx >= src_ar->len)
          THROW_OUT_OF_BOUNDS("array index %d >= than %d length", idx, src_ar->len);

        d_ar[i] = d_ar_src[idx];
      }
      break;
    }
  }

  *vp = ARRAY(array);

  if (v_iar.sym == NULL) array_release (v_iar);

  return L_OK;
}

static int l_parse_array_get (l_t *this, VALUE *vp) {
  VALUE ar = TOKENVAL;
  ArrayType *array = (ArrayType *) AS_ARRAY(ar);

  NEXT_TOKEN();
  int c = TOKEN;

  if (c != TOKEN_INDEX_OPEN) {
    *vp = ar;
    return L_OK;
  }

  int err;
  if (NEXT_BYTE_NOWS_NONL() == TOKEN_INDEX_OPEN) {
    NEXT_TOKEN();
    VALUE v;
    err = l_parse_anon_array (this, &v);
    THROW_ERR_IF_ERR(err);

    NEXT_TOKEN();

    THROW_SYNTAX_ERR_IFNOT(TOKEN == TOKEN_INDEX_CLOS, "array get, awaiting ]");

    err = l_array_from_array (this, array, v, vp);

    THROW_SYNTAX_ERR_IFNOT(TOKEN == TOKEN_INDEX_CLOS, "array get, awaiting ]");

    NEXT_TOKEN();

    return err;
  }

  VALUE ix;
  this->curState |= INDEX_STATE;
  err = l_parse_primary (this, &ix);
  this->curState &= ~INDEX_STATE;
  THROW_ERR_IF_ERR(err);

  if (ix.type == ARRAY_TYPE)
    return l_array_from_array (this, array, ix, vp);

  THROW_SYNTAX_ERR_IF(ix.type != INTEGER_TYPE, "array get, awaiting an integer expression");

  integer len = array->len;
  integer idx = AS_INT(ix);

  if (0 > idx)
    idx += len;

  if (idx <= -1 || idx >= len)
    THROW_OUT_OF_BOUNDS("array index %d >= than %d length, or less or equal than -1",
        idx, len);

  switch (array->type) {
    case INTEGER_TYPE: {
      integer *ary = (integer *) AS_ARRAY(array->value);
      if (IS_INT(APPENDVAL)) {
        ary[idx] += AS_INT(APPENDVAL);
        APPENDVAL = NULL_VALUE;
      }

      *vp = INT(ary[idx]);
      break;
    }

    case NUMBER_TYPE: {
      number *ary = (number *) AS_ARRAY(array->value);
      if (IS_NUMBER(APPENDVAL)) {
        ary[idx] += AS_NUMBER(APPENDVAL);
        APPENDVAL = NULL_VALUE;
      }

      *vp = NUMBER(ary[idx]);
      break;
    }

    case STRING_TYPE: {
      string **ary = (string **) AS_ARRAY(array->value);
      if (ary[idx] != NULL) {
        *vp = STRING(ary[idx]);
        vp->sym = ar.sym;
        this->objectState |= ARRAY_MEMBER;
      } else
        *vp = NULL_VALUE;
      break;
    }

    case MAP_TYPE: {
      Map_Type **ary = (Map_Type **) AS_ARRAY(array->value);
      if (ary[idx] != NULL) {
        *vp = MAP(ary[idx]);
      } else
        *vp = NULL_VALUE;
      break;
    }

    case LIST_TYPE: {
      listArrayMember **ary = (listArrayMember **) AS_ARRAY(array->value);
      if (ary[idx] != NULL) {
        this->objectState |= ARRAY_MEMBER;
        *vp = LIST(ary[idx]);
      } else
        *vp = NULL_VALUE;
      break;
    }

    case ARRAY_TYPE: {
      ArrayType **ary = (ArrayType **) AS_ARRAY(array->value);
      if (ary[idx] != NULL) {
        this->objectState |= ARRAY_MEMBER;
        *vp = ARRAY(ary[idx]);
      } else
        *vp = NULL_VALUE;
      break;
    }

    default:
      THROW_INV_ARRAY_TYPE(array->type);
  }

  c = TOKEN;

  if (c == TOKEN_DOT) {
    VALUE v = *vp;
    THROW_SYNTAX_ERR_IF(v.type != MAP_TYPE,
      "value is not a map type");

    TOKENVAL = v;
    UNGET_BYTE();
    return l_parse_map_get (this, vp);
  }

  if (c == TOKEN_PLUS_PLUS || c == TOKEN_MINUS_MINUS) {
    int peek = PEEK_NTH_BYTE(0);
    switch (peek) {
      case TOKEN_INDEX_CLOS:
      case TOKEN_SEMICOLON:
      case TOKEN_PAREN_CLOS:
      case TOKEN_NL:
      case TOKEN_COLON:
      case ' ':
      case '\t':
      case TOKEN_EOF:
        break;

      default:
        THROW_SYNTAX_ERR_FMT("unexpected token after %s",
          (c == TOKEN_PLUS_PLUS ? "++" : "--"));
    }

    switch (vp->type) {
      case INTEGER_TYPE: {
        integer *ary = (integer *) AS_ARRAY(array->value);
        ary[idx] += (c == TOKEN_PLUS_PLUS ? 1 : -1);
        NEXT_TOKEN();
        return L_OK;
      }

      case NUMBER_TYPE: {
        number *ary = (number *) AS_ARRAY(array->value);
        ary[idx] += (c == TOKEN_PLUS_PLUS ? 1 : -1);
        NEXT_TOKEN();
        return L_OK;
     }

      default:
        THROW_SYNTAX_ERR("awaiting an integer or a number");
    }
  }

  if (c == TOKEN_INDEX_OPEN) {
    VALUE v = *vp;
    switch (v.type) {
      case STRING_TYPE:
        TOKENVAL = v;
  //      this->objectState |= ARRAY_MEMBER;
        UNGET_BYTE();
        err = l_parse_string_get (this, vp);
        THROW_SYNTAX_ERR_IF(TOKEN == TOKEN_INDEX_OPEN, "unsupported indexing");

        return err;

      case ARRAY_TYPE:
        TOKENVAL = v;
        UNGET_BYTE();
        return l_parse_array_get (this, vp);

      case LIST_TYPE:
        TOKENVAL = v;
        UNGET_BYTE();
        return l_parse_list_get (this, vp);

      default:
        THROW_SYNTAX_ERR("Arrays of arrays haven't been implemented");
    }
  }

  return L_OK;
}

static int l_array_eq (VALUE x, VALUE y) {
  ArrayType *xa = (ArrayType *) AS_ARRAY(x);
  ArrayType *ya = (ArrayType *) AS_ARRAY(y);

  if (xa == ya) return 1;

  if (xa->len != ya->len) return 0;

  switch (xa->type) {
    case INTEGER_TYPE:
      switch (ya->type) {
        case INTEGER_TYPE: {
          integer *x_ar = (integer *) AS_ARRAY(xa->value);
          integer *y_ar = (integer *) AS_ARRAY(ya->value);
          for (int i = 0; i < xa->len; i++)
            if (x_ar[i] != y_ar[i]) return 0;

          return 1;
        }
      }
      return 0;

    case NUMBER_TYPE:
      switch (ya->type) {
        case NUMBER_TYPE: {
          number *x_ar = (number *) AS_ARRAY(xa->value);
          number *y_ar = (number *) AS_ARRAY(ya->value);
          for (int i = 0; i < xa->len; i++)
            if (x_ar[i] != y_ar[i]) return 0;

          return 1;
        }
      }
      return 0;

    case STRING_TYPE:
      switch (ya->type) {
        case STRING_TYPE: {
          string **x_ar = (string **) AS_ARRAY(xa->value);
          string **y_ar = (string **) AS_ARRAY(ya->value);
          for (int i = 0; i < xa->len; i++)
            ifnot (str_eq (x_ar[i]->bytes, y_ar[i]->bytes)) return 0;

          return 1;
        }
      }
      return 0;

    default:
      return 0;
  }
  return 0;
}

static void l_release_map_val (map_type *v) {
  VALUE *val = (VALUE *) v;
  Release (val->sym);
  l_release_value (NULL, *val);
  Release (val);
}

static void l_map_release_value (l_t *this, VALUE *v) {
  (void) this;
  l_release_map_val ((map_type *) v);
}

static int l_map_set_value (l_t *this, Map_Type *map, const char *key, VALUE v, int scope) { 
  VALUE *val = Alloc (sizeof (VALUE));
  val->refcount = v.refcount;
  val->type = v.type;

  sym_t *sym = Alloc (sizeof (sym_t));
  sym->scope = (scope ? this->function : NULL);
  sym->value = MAP(map);
  val->sym = sym;

  switch (val->type) {
    case STRING_TYPE:
      if (this->objectState & (ARRAY_MEMBER|MAP_MEMBER)) {
        val->asString = AS_STRING(l_copy_value (v));
        val->refcount = 0;
        this->objectState &= ~(ARRAY_MEMBER|MAP_MEMBER);
      } else {
        if (v.sym == NULL) {
          val->asString  = v.asString;
          if (val->refcount != MALLOCED_STRING)
              val->refcount = 0;
          this->curState &= ~LITERAL_STRING_STATE;
        } else {
          val->asString = AS_STRING(l_copy_value (v));
          val->refcount = 0;
        }
      }
      break;

    case NUMBER_TYPE: val->asNumber  = v.asNumber;  break;
    case NULL_TYPE  : val->asNull    = v.asNull;    break;

    case MAP_TYPE:
      if (v.sym != NULL) {
        val->asInteger = (pointer) AS_MAP(l_copy_map (v));
        break;
      }
      // fall through

    default: val->asInteger = v.asInteger; break;
  }

  if (-1 == map_set (map, (char *) key, val, l_release_map_val)) {
    this->print_bytes (this->err_fp, "Map.set() internal error\n");
    return L_NOTOK;
  }

  return L_OK;
}

static int l_map_reset_value (l_t *this, Map_Type *map, const char *key, VALUE v) {
  VALUE *val = map_pop_value (map, key);
  int scope = PUBLIC_SCOPE;

  if (val != NULL) {
    scope = val->sym->scope != NULL;
    l_release_map_val ((map_type *) val);
  }

  return l_map_set_value (this, map, key, v, scope);
}

static int map_set_append_rout (l_t *this, Map_Type *map, char *key, int token) {
  int err;
  VALUE *val = map_get_value (map, key);

  if (NULL == val)
    THROW_SYNTAX_ERR_FMT("%s key doesn't exists", key);

  if (val->type > STRING_TYPE || IS_UFUNC(val->type))
    THROW_A_TYPE_MISMATCH(val->type, "illegal map type");

  if (token == TOKEN_PLUS_PLUS || token == TOKEN_MINUS_MINUS) {
    int peek = PEEK_NTH_BYTE(0);
    switch (peek) {
      case TOKEN_INDEX_CLOS:
      case TOKEN_SEMICOLON:
      case TOKEN_PAREN_CLOS:
      case TOKEN_NL:
      case TOKEN_COLON:
      case ' ':
      case '\t':
      case TOKEN_EOF:
        break;

      default:
        THROW_SYNTAX_ERR_FMT("unexpected token after %s",
          (token == TOKEN_PLUS_PLUS ? "++" : "--"));
    }

    switch (val->type) {
      case INTEGER_TYPE:
        AS_INT((*val)) = AS_INT((*val)) + (token == TOKEN_PLUS_PLUS ? 1 : -1);
        NEXT_TOKEN();
        return L_OK;

      case NUMBER_TYPE:
        AS_NUMBER((*val)) = AS_NUMBER((*val)) + (token == TOKEN_PLUS_PLUS ? 1 : -1);
        NEXT_TOKEN();
        return L_OK;

      default:
        THROW_SYNTAX_ERR("awaiting an integer or a number");
    }
  }

  VALUE v;
  NEXT_TOKEN();
  int is_un = TOKEN == TOKEN_UNARY;
  if (is_un)
    NEXT_TOKEN();

  err = l_parse_expr (this, &v);
  THROW_ERR_IF_ERR(err);

  if (is_un) {
    if (v.type == INTEGER_TYPE)
      AS_INT(v) = ~AS_INT(v);
    else
      THROW_SYNTAX_ERR("error while setting an unary object, awaiting an integer");
  }

  VALUE result;
  switch (token) {
    case TOKEN_ASSIGN_APP:
      this->objectState |= OBJECT_APPEND;
      result = l_add (this, *val, v);
      this->objectState &= ~OBJECT_APPEND;
      break;

    case TOKEN_ASSIGN_SUB:
      result = l_sub (this, *val, v); break;
    case TOKEN_ASSIGN_DIV:
      result = l_div (this, *val, v); break;
    case TOKEN_ASSIGN_MUL:
      result = l_mul (this, *val, v); break;
    case TOKEN_ASSIGN_MOD:
      result = l_mod (this, *val, v); break;
    case TOKEN_ASSIGN_BAR:
      result = l_bset (this, *val, v); break;
    case TOKEN_ASSIGN_AND:
      result = l_bnot (this, *val, v); break;
    case TOKEN_ASSIGN_XOR:
      result = l_bitxor (this, *val, v); break;
  }

  if (result.type == NULL_TYPE)
     THROW_SYNTAX_ERR("unxpected operation");

  sym_t *sym = val->sym;
  *val = result;
  val->sym = sym;

  return L_OK;
}

static int map_set_rout (l_t *this, Map_Type *map, char *key, int scope, int into_map_decl) {
  int err;
  VALUE v;

  NEXT_TOKEN();

  while (TOKEN == TOKEN_NL) NEXT_TOKEN();

  int is_priv = 0;
  if (TOKEN == TOKEN_FUNCDEF) {
    str_copy_fmt (this->curFunName, MAXLEN_SYMBOL + 1, NS_ANON "_%d_%s", this->anon_id++, key);

    this->scopeState = PUBLIC_SCOPE; // this is for map.f = func ... cases

    err = l_parse_func_def (this);
    this->curFunName[0] = '\0';
    THROW_ERR_IF_ERR(err);

    if (TOKEN == TOKEN_PAREN_OPEN) {
      funT *uf = this->curFunDef;

      if (into_map_decl) {
        sym_t *sym = l_define_symbol (this, uf, "this", MAP_TYPE, MAP(map), 0);
        THROW_SYNTAX_ERR_IF(NULL == sym, "unknown error on `this` declaration");
        this->funcState |= MAP_METHOD_STATE;
      }

      UNGET_BYTE();

      err = l_parse_func_call (this, &v, NULL, uf, v);
      THROW_ERR_IF_ERR(err);
      NEXT_TOKEN();
      goto assign;
    }

    v = PTR(this->curFunDef);
    v.type = UFUNCTION_TYPE;

    ifnot (into_map_decl) is_priv = 1;

    goto assign;
  }

  if (NULL != TOKENSYM &&
      IS_INT(TOKENSYM->value) &&
      AS_VOID_PTR(TOKENSYM->value) == l_parse_array_def) {
  /* moved from do_next_token (),
   * first commit after 8ed84cdc92c074e4dba4c1d38e72cae6b8236178
   * if (IS_INT(symbol->value) && AS_VOID_PTR(symbol->value) == l_parse_array_def) {
   *   r = TOKEN_ARYDEF;
   *   goto theend;
   * }
   */

    err = l_parse_array_def (this, &v, ARRAY_IS_MAP_MEMBER);
    THROW_ERR_IF_ERR(err);

    goto assign;
  }

  if (TOKEN == TOKEN_ARRAY) {
    sym_t *sym = TOKENSYM;
    v =  ARRAY(array_copy ((ArrayType *) AS_ARRAY(sym->value)));
    NEXT_TOKEN();

    goto assign;
  }

  this->objectState |= MAP_ASSIGNMENT;
  err = l_parse_expr (this, &v);
  this->objectState &= ~MAP_ASSIGNMENT;
  THROW_ERR_IF_ERR(err);

  if (IS_UFUNC(v.type) || v.type & UFUNCTION_TYPE) {
    if (TOKEN == TOKEN_AS) {
      NEXT_TOKEN();
      THROW_SYNTAX_ERR_IF(TOKEN != TOKEN_CALLBACK, "awaiting callback");
      size_t len = str_copy_fmt
        (this->curFunName, MAXLEN_SYMBOL + 1, "__G_" NS_ANON "_%d", this->anon_id++);
      funT *uf = fun_copy (AS_FUNC_PTR(v), this->curFunName, len);
      this->curFunName[0] = '\0';
      v = PTR(uf);
      v.type = UFUNCTION_TYPE;
      sym_t *sym = l_define_symbol (this, this->function, uf->funname, (UFUNC_TYPE | (uf->nargs << 8)), PTR(uf), 0);
      THROW_SYNTAX_ERR_IF(NULL == sym, "unknown error on declaration");
      NEXT_TOKEN();
    }
  }

  assign:

  err = l_map_set_value (this, map, key, v, scope);
  THROW_ERR_IF_ERR(err);

  if ((v.type == ARRAY_TYPE || v.type == LIST_TYPE) && TOKEN == TOKEN_INDEX_CLOS)
    NEXT_TOKEN();

  if (v.type == UFUNCTION_TYPE && is_priv) {
    VALUE *val = (VALUE *) map_get_value (map, key);
    val->sym->scope = this->private_maps;
  }

  return err;
}

static VALUE l_map_release (VALUE value) {
  VALUE result = INT(L_OK);

  if (value.sym != NULL) {
    if (value.refcount > 0) goto theend;

    //if (value.refcount < 0)
    //  if (value.refcount != MAP_LITERAL)
    //    return result;
  }

  Map_Type *m = AS_MAP(value);
  map_release (&m);

theend:
  value.refcount--;
  return result;
}

static void *l_copy_map_cb (void *value, void *mapval) {
  Map_Type *map = (Map_Type *) mapval;
  VALUE *v = (VALUE *) value;
  VALUE *val = Alloc (sizeof (VALUE));
  val->refcount = 0;  // v->refcount;
  val->type = v->type;

  sym_t *sym = Alloc (sizeof (sym_t));
  sym->scope = v->sym->scope;
  sym->value = MAP(map);
  val->sym = sym;

  switch (val->type) {
    case MAP_TYPE   :
    case LIST_TYPE  :
    case ARRAY_TYPE : val->asInteger = AS_PTR(l_copy_value (*v));    break;
    case STRING_TYPE: val->asString  = AS_STRING(l_copy_value (*v)); break;
    case NUMBER_TYPE: val->asNumber  = v->asNumber;                   break;
    case NULL_TYPE  : val->asNull    = v->asNull;                     break;
    default         : val->asInteger = v->asInteger;                  break;
  }

  return val;
}

static VALUE l_copy_map (VALUE mapval) {
  Map_Type *map = AS_MAP(mapval);
  Map_Type *new = map_clone (map, l_copy_map_cb, map);
  VALUE nval = MAP(new);
  return nval;
}

static int l_get_map_key (l_t *this, char ident[MAXLEN_SYMBOL + 1]) {
  int idx = 0;
  int num_comma = 0;
  int c;

  get_byte:
  c = GET_BYTE();

  while (is_space (c)) c = GET_BYTE();

  if (c == TOKEN_NL) goto get_byte;

  if (c == TOKEN_COMMENT) {
    do c = GET_BYTE(); while (c >= 0 && c != TOKEN_NL);
    goto get_byte;
  }

  if (c == TOKEN_COMMA) {
    if (num_comma++)
      THROW_SYNTAX_ERR("error while getting map field, found two consecutive commas");
    goto get_byte;
  }

  if (is_alpha (c) || c == '_' || is_digit (c)) {

    identifier:
    THROW_SYNTAX_ERR_IF(idx == MAXLEN_SYMBOL, "map key identifier exceeded maximum length");

    ident[idx++] = c;

    c = GET_BYTE();
    if (c == TOKEN_EOF)
      goto theend;

    if (0 == is_alpha (c) && c != '_' && 0 == is_digit (c)) {
      UNGET_BYTE();
      goto theend;
    }

    goto identifier;
  }

  if (c == TOKEN_DOLLAR) {
    VALUE v_key;

    this->curState |= (MALLOCED_STRING_STATE|CLOSURE_STATE);
    NEXT_TOKEN();
    int err = l_parse_expr (this, &v_key);
    this->curState &= ~(MALLOCED_STRING_STATE|CLOSURE_STATE);
    THROW_ERR_IF_ERR(err);

    THROW_SYNTAX_ERR_IF(TOKEN != TOKEN_PAREN_CLOS, "awaiting ')'");

    if (v_key.type != STRING_TYPE)
      THROW_SYNTAX_ERR("map get, awaiting a string as a key");

    string *s_key = AS_STRING(v_key);
    if (s_key->num_bytes >= MAXLEN_SYMBOL)
      THROW_SYNTAX_ERR("identifier exceeded maximum length");

    str_copy (ident, MAXLEN_SYMBOL + 1, s_key->bytes, s_key->num_bytes);
    return L_OK;
  }

  if (c != TOKEN_DQUOTE)
    return c;

  get_char:
  c = GET_BYTE();
  THROW_SYNTAX_ERR_IF(c == TOKEN_EOF,
    "error while getting string as a map key, found end of input");
  THROW_SYNTAX_ERR_IF(c == TOKEN_NL,
    "error while getting string as a map key, found a new line");

  if (c == TOKEN_DQUOTE) goto theend;

  THROW_SYNTAX_ERR_IF(idx == MAXLEN_SYMBOL, "map key identifier exceeded maximum length");

  ident[idx++] = c;
  goto get_char;

theend:
  ident[idx] = '\0';
  if ('p' == *ident) {
    if (idx >= 6) {
      if (str_eq (ident, "private"))
        return TOKEN_PRIVATE;
      if (str_eq (ident, "public"))
        return TOKEN_PUBLIC;
   }
  }

  return L_OK;
}

static int l_parse_map_members (l_t *this, VALUE map, int into_map_decl) {
  int err;
  this->curState |= MAP_STATE;
  this->scopeState = PUBLIC_SCOPE;
  int scope = this->scopeState;

  for (;;) {
    char key[MAXLEN_SYMBOL + 1];
    TOKEN = l_get_map_key (this, key);

    if (TOKEN == TOKEN_EOF) break;

    THROW_ERR_IF_ERR(TOKEN);

    switch (TOKEN) {
      case L_OK:
        break;

      case TOKEN_PRIVATE:
        scope = this->scopeState = PRIVATE_SCOPE;
        continue;

      case TOKEN_PUBLIC:
        scope = this->scopeState = PUBLIC_SCOPE;
        continue;

      case TOKEN_PAREN_CLOS:
        if (this->exprList) goto theend;

      // fall through
      default:
        THROW_SYNTAX_ERR(
          "error while getting a map field, awaiting a double quoted string or an identifier token");
    }

    if (PEEK_NTH_TOKEN(0) != TOKEN_COLON) {
      l_map_set_value (this, AS_MAP(map), key, NULL_VALUE, scope);
      continue;
    }

    IGNORE_UNTIL_KNOWN_BYTE(TOKEN_COLON);

    err = map_set_rout (this, AS_MAP(map), key, this->scopeState == PUBLIC_SCOPE, into_map_decl);
    this->scopeState = scope;
    THROW_ERR_IF_ERR(err);

    switch (TOKEN) {
      case TOKEN_EOF:
        goto theend;

      case TOKEN_NL:
      case TOKEN_COMMA:
        continue;

      case TOKEN_PAREN_CLOS:
        if (this->exprList)
          goto theend;

      // fall through
      default:
        THROW_SYNTAX_ERR("awaiting a new line or a comma");
    }
  }

theend:
  this->curState &= ~MAP_STATE;
  this->scopeState = PRIVATE_SCOPE;
  return L_OK;
}

static int l_parse_map (l_t *this, VALUE *vp) {
  l_string saved_ptr = PARSEPTR;
  PARSEPTR = TOKENSTR;

  Map_Type *map = map_new (32);
  funT *scope = this->curScope;
  while (scope && str_eq_n (scope->funname, "__block_", 8))
    scope = scope->prev;

#if 0
  VALUE *vscope = Alloc (sizeof (VALUE));
  *vscope = STRING_NEW(scope->funname);
  sym_t *sym = Alloc (sizeof (sym_t));
  sym->scope = NULL;
  sym->value = MAP(map);
  vscope->sym = sym;

  if (-1 == map_set (map, "__MAP_SCOPE__", vscope, l_release_map_val)) {
    this->print_bytes (this->err_fp, "Map.set() internal error\n");
    return L_NOTOK;
  }
#endif

  int err = l_parse_map_members (this, MAP(map), INTO_MAPDECL);
  THROW_ERR_IF_ERR(err);

  PARSEPTR = saved_ptr;

  VALUE v = MAP(map);
  if (this->exprList) v.refcount--;
  *vp = v;

  NEXT_TOKEN();
  return L_OK;
}

static int l_parse_map_get (l_t *this, VALUE *vp) {
  int err;

  int is_this = (GETSTRLEN(TOKENSTR) == 4 &&
      str_eq_n (GETSTRPTR(TOKENSTR), "this", 4));

  VALUE m = TOKENVAL;

  NEXT_TOKEN();

  if (TOKEN != TOKEN_DOT) {
    if (this->objectState & MAP_ASSIGNMENT)
      *vp = l_copy_map (m);
    else
      *vp = m;

    return L_OK;
  }

  err = 0;
  TOKENVAL = m;
  VALUE map_parent = TOKENVAL;

  int submap = 0;
  VALUE *v = NULL;

  redo: {}

  VALUE save_map = TOKENVAL;

  char key[MAXLEN_SYMBOL + 1];
  TOKEN = l_get_map_key (this, key);
  THROW_ERR_IF_ERR(TOKEN);

  TOKENVAL = save_map;

  if (submap) {
    map_parent = TOKENVAL;
    map_parent.sym->value = (*v);
  }

  Map_Type *map = AS_MAP(TOKENVAL);

  v = map_get_value (map, key);

  if (v == NULL)
    THROW_SYNTAX_ERR_FMT("%s, not a key", key);

  if (v->sym->scope == NULL) {
    ifnot (is_this)
      THROW_SYNTAX_ERR_FMT("%s, symbol has private scope", key);
  }

  *vp = *v;

  NEXT_TOKEN();

  if (TOKEN == TOKEN_PLUS_PLUS || TOKEN == TOKEN_MINUS_MINUS) {
    int peek = PEEK_NTH_BYTE(0);
    switch (peek) {
      case TOKEN_INDEX_CLOS:
      case TOKEN_SEMICOLON:
      case TOKEN_PAREN_CLOS:
      case TOKEN_NL:
      case TOKEN_COLON:
      case ' ':
      case '\t':
      case TOKEN_EOF:
        break;

      default:
        THROW_SYNTAX_ERR_FMT("unexpected token after %s",
          (TOKEN == TOKEN_PLUS_PLUS ? "++" : "--"));
    }

    switch (v->type) {
      case INTEGER_TYPE:
        AS_INT((*v)) = AS_INT((*v)) + (TOKEN == TOKEN_PLUS_PLUS ? 1 : -1);
        NEXT_TOKEN();
        return L_OK;

      case NUMBER_TYPE:
        AS_NUMBER((*v)) = AS_NUMBER((*v)) + (TOKEN == TOKEN_PLUS_PLUS ? 1 : -1);
        NEXT_TOKEN();
        return L_OK;

      default:
        THROW_SYNTAX_ERR("awaiting an integer or a number");
    }
  }

  if (TOKEN == TOKEN_DOT || TOKEN == TOKEN_COLON) {
    if (v->type != MAP_TYPE) {
      if (TOKEN == TOKEN_COLON)
        goto theend;
      else
        THROW_SYNTAX_ERR_FMT("%s, not a map", key);
    }

    if (TOKEN == TOKEN_COLON) {
      const char *ptr = GETSTRPTR(PARSEPTR);
      while (is_space (*ptr)) ptr++;
      char k[MAXLEN_SYMBOL + 1];
      int idx = 0;
      while (is_identifier (*ptr)) {
        if (idx == MAXLEN_SYMBOL)
          THROW_SYNTAX_ERR("identifier exceeded maximum length");
        k[idx++] = *ptr;
        ptr++;
      }

      k[idx] = '\0';

      Map_Type *vmap = AS_MAP((*v));

      ifnot (map_key_exists (vmap, k))
        goto theend;
    }

    TOKENVAL = (*v);

    submap = (TOKEN == TOKEN_COLON);

    goto redo;
  }

  if (TOKEN == TOKEN_INDEX_OPEN) {
    THROW_SYNTAX_ERR_IF(v->type != ARRAY_TYPE && v->type != LIST_TYPE &&
      v->type != STRING_TYPE,  "map get: not an array nor a list or a string");

    TOKENVAL = *v;
    UNGET_BYTE();

    if (v->type == ARRAY_TYPE)
      return l_parse_array_get (this, vp);

    if (v->type == STRING_TYPE)
      return l_parse_string_get (this, vp);

    return l_parse_list_get (this, vp);
  }

  if (TOKEN == TOKEN_PAREN_OPEN) {
    if (NULL == v)
      THROW_SYNTAX_ERR_FMT("%s, method doesn't exists", key);

    if (v->sym->scope == NULL && 0 == is_this)
      THROW_SYNTAX_ERR_FMT("%s, symbol has private scope", key);

    int type;
    if (v->type & UFUNCTION_TYPE)
      type = UFUNCTION_TYPE;
    else if ((v->type & 0x77) == TOKEN_BUILTIN)
      type = CFUNCTION_TYPE;
    else
      THROW_SYNTAX_ERR_FMT("%s, not a method", key);

    UNGET_BYTE();

    if (type == UFUNCTION_TYPE) {
      funT *uf = AS_FUNC_PTR((*v));

      VALUE thismap = map_parent.sym->value;

      if (str_eq_n (uf->funname, "__G_" NS_ANON, 4 + NS_ANON_LEN)) {
        str_copy_fmt (this->curFunName, MAXLEN_SYMBOL + 1, NS_ANON "_%zd", this->anon_id++);
        funT *fun = Fun_new (this, funNew (
          .name = this->curFunName, .namelen = bytelen (this->curFunName),
          .parent = this->function));
        this->curFunName[0] = '\0';
        funT *curScope = this->curScope;
        this->curScope = fun;
        stack_push (this, thismap);
        this->argCount = 1;
        err = l_parse_func_call (this, vp, NULL, uf, *v);
        fun_release (&fun);
        this->curScope = curScope;
      } else {
        ifnot (v->sym->scope == this->private_maps) {
          sym_t *sym = l_define_symbol (this, uf, "this", MAP_TYPE, thismap, 0);
          THROW_SYNTAX_ERR_IF(NULL == sym, "unknown error on declaration");
          this->funcState |= MAP_METHOD_STATE;
        }

        err = l_parse_func_call (this, vp, NULL, uf, *v);
      }

      NEXT_TOKEN();
    } else {
      CFunc op = (CFunc) AS_PTR((*v));
      TOKENARGS = ((*v).type >> 8) & 0xff;
      size_t len = bytelen (key);
      str_copy (this->curCFunName, MAXLEN_SYMBOL + 1, key, len);
      err = l_parse_func_call (this, vp, op, NULL, *v);
    }

    return err;
  }

  if (v->type == MAP_TYPE) {
    if (0 == is_this || this->objectState & MAP_ASSIGNMENT) {
      *vp = l_copy_map (*v);
    } else {
      if (v->sym != NULL && is_this && this->funcState & RETURN_STATE
           && 0 == this->exprList) {
        vp->refcount++;
        this->objectState |= MAP_MEMBER;
      }
    }
  }

  ifnot (IS_NULL(APPENDVAL)) {
    switch (v->type) {
      case INTEGER_TYPE:
        if (IS_INT(APPENDVAL)) {
          AS_INT((*v)) += AS_INT(APPENDVAL);
          APPENDVAL = NULL_VALUE;
        }
        break;

      case NUMBER_TYPE:
        if (IS_NUMBER(APPENDVAL)) {
          AS_NUMBER((*v)) += AS_NUMBER(APPENDVAL);
          APPENDVAL = NULL_VALUE;
        }
        break;
    }
  }

theend:
  if (vp->type == STRING_TYPE || vp->type == ARRAY_TYPE)
    this->objectState |= MAP_MEMBER;

  return L_OK;
}

static int l_parse_map_set (l_t *this) {
  int is_this = (GETSTRLEN(TOKENSTR) == 4 &&
      str_eq_n (GETSTRPTR(TOKENSTR), "this", 4));

  VALUE map_parent = TOKENVAL;
  Map_Type *map = AS_MAP(TOKENVAL);

  int override = this->objectState & FUNC_OVERRIDE;
  this->objectState &= ~FUNC_OVERRIDE;

  int c = NEXT_BYTE_NOWS_NONL();

  if (c == '=') {
    VALUE v = TOKENVAL;
    sym_t *sym = v.sym;
    THROW_SYNTAX_ERR_IF(sym == NULL, "unexpected error, variable is not associated with a symbol");
    THROW_SYNTAX_ERR_IF(sym->is_const, "can not reassign a new value, to a constant declared list");
    sym->type = NULL_TYPE;
    sym->value = NULL_VALUE;

    map_release (&map);

    return L_MMT_REASSIGN;
  }

  int err;

  NEXT_TOKEN();

  if (TOKEN == TOKEN_REASSIGN) {
    VALUE v = map_parent;
    sym_t *sym = v.sym;
    THROW_SYNTAX_ERR_IF(sym->is_const, "can not reassign a new value, to a constant declared map");
    map_parent.sym = NULL;
    err = l_parse_chain (this, &map_parent);
    THROW_ERR_IF_ERR(err);
    sym->value = map_parent;
    map_parent.sym = sym;
    sym->type = map_parent.type;
    return err;
  }

  c = TOKEN;

  if (c != TOKEN_DOT)
    THROW_SYNTAX_ERR("awaiting .");

  redo: {}

  char key[MAXLEN_SYMBOL + 1];
  TOKEN = l_get_map_key (this, key);
  THROW_ERR_IF_ERR(TOKEN);

  NEXT_TOKEN();
  c = TOKEN;

  VALUE *v;

  if (c == TOKEN_PAREN_OPEN) {
    v = map_get_value (map, key);
    if (NULL == v)
      THROW_SYNTAX_ERR_FMT("error while seting map: %s method doesn't exists", key);

    if (v->sym->scope == NULL && 0 == is_this)
      THROW_SYNTAX_ERR_FMT("%s, symbol has private scope", key);

    int type;
    if (v->type & UFUNCTION_TYPE)
      type = UFUNCTION_TYPE;
    else if (IS_CFUNC(v->type))
      type = CFUNCTION_TYPE;
    else
      THROW_SYNTAX_ERR_FMT("%s, not a method", key);

    UNGET_BYTE();

    if (type == UFUNCTION_TYPE) {
      funT *uf = AS_FUNC_PTR((*v));

      VALUE thismap = map_parent.sym->value;

      if (str_eq_n (uf->funname, "__G_" NS_ANON, 4 + NS_ANON_LEN)) {
        str_copy_fmt (this->curFunName, MAXLEN_SYMBOL + 1, NS_ANON "_%zd", this->anon_id++);
        funT *fun = Fun_new (this, funNew (
          .name = this->curFunName, .namelen = bytelen (this->curFunName),
          .parent = this->function));
        this->curFunName[0] = '\0';
        funT *curScope = this->curScope;
        this->curScope = fun;
        stack_push (this, thismap);
        this->argCount = 1;
        VALUE vp;
        err = l_parse_func_call (this, &vp, NULL, uf, *v);
        fun_release (&fun);
        this->curScope = curScope;
      } else {
        sym_t *sym = l_define_symbol (this, uf, "this", MAP_TYPE, thismap, 0);
        THROW_SYNTAX_ERR_IF(NULL == sym, "unknown error on declaration");
        this->funcState |= MAP_METHOD_STATE;
        VALUE vp;
        err = l_parse_func_call (this, &vp, NULL, uf, *v);
      }

      NEXT_TOKEN();
    } else {
      CFunc op = (CFunc) AS_PTR((*v));
      TOKENARGS = ((*v).type >> 8) & 0xff;
      size_t len = bytelen (key);
      str_copy (this->curCFunName, MAXLEN_SYMBOL + 1, key, len);
      VALUE vp;
      err = l_parse_func_call (this, &vp, op, NULL, *v);
    }

    return err;
  }

  if (c < TOKEN_ASSIGN) {
    ifnot (map_key_exists (map, key))
      THROW_SYNTAX_ERR_FMT("%s: map key couldn't been found", key);

    if (c == TOKEN_DOT || c == TOKEN_COLON) {
      v = map_get_value (map, key);
      if (v->type != MAP_TYPE)
        THROW_SYNTAX_ERR_FMT("%s, not a map", key);

      map = AS_MAP((*v));

      is_this = 0;

      if (c == TOKEN_COLON) {
        map_parent = (*v);
        map_parent.sym->value = (*v);
      }

      goto redo;
    }

    if (c == TOKEN_INDEX_OPEN) {
      v = map_get_value (map, key);
        THROW_SYNTAX_ERR_IF(v->type != ARRAY_TYPE && v->type != LIST_TYPE,
          "map set: not an array nor a list");

      TOKENVAL = *v;
      UNGET_BYTE();

      if (v->type == ARRAY_TYPE)
        return l_parse_array_set (this);

      return l_parse_list_set (this);
    }
  }

  v = map_get_value (map, key);

  if (v != NULL) {
    if (v->sym->scope == NULL && 0 == is_this)
      THROW_SYNTAX_ERR_FMT("%s, symbol has private scope", key);

    if (v->type & UFUNCTION_TYPE ||
       (v->type & 0x77) == TOKEN_BUILTIN) {
      if (c == TOKEN_ASSIGN && 0 == override)
        THROW_SYNTAX_ERR("you can not override a method");
    }
  }

  int scope = PUBLIC_SCOPE;
  if (c <= TOKEN_ASSIGN) {
    if (v != NULL) {
      scope = v->sym->scope != NULL;
      l_release_map_val (map_pop_value (map, key));
    }
  } else
    return map_set_append_rout (this, map, key, c);

  return map_set_rout (this, map, key, scope, NOT_INTO_MAPDECL);
}

static int l_parse_new (l_t *this, VALUE *vp) {
  int err;
  NEXT_RAW_TOKEN();
  int c = TOKEN;

  THROW_SYNTAX_ERR_IF(c != TOKEN_SYMBOL, "awaiting a type name");

  char type[MAXLEN_SYMBOL + 1];
  str_copy (type, MAXLEN_SYMBOL + 1, GETSTRPTR(TOKENSTR),
      GETSTRLEN(TOKENSTR));

  ifnot (map_key_exists (this->types, type))
    THROW_SYNTAX_ERR_FMT("%s, not such type", type);

  string *block = (string *) map_get_value (this->types, type);

  TOKENSTR = StringNew (block->bytes);

  err = l_parse_map (this, vp);
  THROW_ERR_IF_ERR(err);

  THROW_SYNTAX_ERR_IF(vp->type != MAP_TYPE, "expression not a type");

  Map_Type *map = AS_MAP((*vp));

  THROW_SYNTAX_ERR_IF(TOKEN != TOKEN_PAREN_OPEN, "awaiting (");

  VALUE *val = map_get_value (map, "init");

  THROW_SYNTAX_ERR_IF(NULL == val, "init method doesn't exists");

  ifnot (val->type & UFUNCTION_TYPE)
   THROW_SYNTAX_ERR("init, not a function method");

  UNGET_BYTE();

  funT *uf = AS_FUNC_PTR((*val));
  VALUE th = val->sym->value;
  sym_t *sym = l_define_symbol (this, uf, "this", MAP_TYPE, th, 0);
  THROW_SYNTAX_ERR_IF(NULL == sym, "unknown error on declaration");
  this->funcState |= MAP_METHOD_STATE;
  VALUE v;

  err = l_parse_func_call (this, &v, NULL, uf, *val);
  THROW_ERR_IF_ERR(err);
  THROW_SYNTAX_ERR_FMT_IF(IS_NOTOK(v), "init() method of %s type returned an error", type);

  NEXT_TOKEN();
  return err;
}

static int l_parse_type (l_t *this) {
  NEXT_RAW_TOKEN();

  THROW_SYNTAX_ERR_IF(TOKEN != TOKEN_SYMBOL, "awaiting a type name");

  char type[MAXLEN_SYMBOL + 1];
  str_copy (type, MAXLEN_SYMBOL + 1, GETSTRPTR(TOKENSTR),
      GETSTRLEN(TOKENSTR));

  THROW_SYNTAX_ERR_IF('Z' < type[0] || type[0] < 'A',
    "type names begin with a capital");

  NEXT_TOKEN();

  THROW_SYNTAX_ERR_IF(TOKEN != TOKEN_BLOCK, "awaiting block");

  string *block = string_new_with_len (GETSTRPTR(TOKENSTR),
      GETSTRLEN(TOKENSTR));

  map_set (this->types, type, block, l_release_unit);

  NEXT_TOKEN();

  return L_OK;
}

static char EXPR_NAMES[20][128];

static int l_parse_expr_list (l_t *this, funT *uf, int expectargs) {
  int err;
  l_string saved_ptr;
  int count = this->argCount;
  this->argCount = 0;
  VALUE v;

  int c = TOKEN;
  if (c == TOKEN_SEMICOLON) {
    if (NEXT_BYTE_NOWS() == TOKEN_NL) NEXT_TOKEN();
    goto parse_qualifiers;
  }

  this->exprList++;
  do {
    THROW_SYNTAX_ERR_FMT_IF(expectargs == count,
      "error while parsing expression list, expected %d arguments", expectargs);

    this->curState |= MALLOCED_STRING_STATE;
    this->objectState &= ~ANNON_ARRAY;
    this->exprList++;

    l_string tokenstr = TOKENSTR;
    int len = GETSTRLEN(tokenstr);
    if (len < 128) {
      char *key = sym_key (this, TOKENSTR);
      str_copy (EXPR_NAMES[this->argCount], 128, key, len);
    }

    err = l_parse_expr (this, &v);

    this->exprList--;
    THROW_ERR_IF_ERR(err);

    if (v.refcount == STRING_LITERAL)
      // if (v.sym != NULL && 0 == (this->funcState & MAP_METHOD_STATE)) {
      if (v.sym != NULL) {
        VALUE t = l_copy_value (v);
        v = t;
        v.refcount--;
      }

    if (v.refcount == ARRAY_LITERAL)
      ifnot (this->objectState & ANNON_ARRAY)
        v.refcount--;

    if (this->curState & FUNC_CALL_RESULT_IS_MMT)
      if (v.sym == NULL)
        if (v.refcount > -1)
          v.refcount--;

    stack_push (this, v);

    count++;

    c = TOKEN;

    if (c == TOKEN_COMMA) {
      this->objectState &= ~ANNON_ARRAY;
      this->curState |= MALLOCED_STRING_STATE;
      NEXT_TOKEN();
      THROW_SYNTAX_ERR_IF(TOKEN == TOKEN_COMMA, "error while parsing expression list, found two consecutive commas");
    }

  } while (c == TOKEN_COMMA);

  this->exprList--;

  this->objectState &= ~(ARRAY_MEMBER|MAP_MEMBER|ANNON_ARRAY);

  if (c != TOKEN_PAREN_CLOS) {
    ifnot (c == TOKEN_SEMICOLON) {
      ifnot (NEXT_BYTE_NOWS_NONL() == TOKEN_SEMICOLON)
        goto theend;

      NEXT_TOKEN();
      if (TOKEN == TOKEN_NL)
        NEXT_TOKEN();

    } else
      if (NEXT_BYTE_NOWS() == TOKEN_NL)
        NEXT_TOKEN();

parse_qualifiers:
    saved_ptr = PARSEPTR;

    NEXT_TOKEN();

    THROW_SYNTAX_ERR_IF(TOKEN == TOKEN_NL, "error while parsing qualifiers of an expression list, found two consecutive new lines");

    Map_Type *map = NULL;

    if (TOKEN == TOKEN_BLOCK) {
      err = l_parse_map (this, &v);
      THROW_ERR_IF_ERR(err);

      _l_set_qualifiers (this, v, uf);

    } else if (TOKEN == TOKEN_MAP) {
      this->exprList++;
      err = l_parse_map_get (this, &v);
      this->exprList--;
      THROW_ERR_IF_ERR(err);

      VALUE val = l_copy_map (v);
      v = val;

      _l_set_qualifiers (this, v, uf);

    } else {
      if (PEEK_NTH_TOKEN(0) == TOKEN_COLON) {
parse_members:
        map = map_new (32);
        PARSEPTR = saved_ptr;
        RESET_TOKEN;

        this->exprList++;
        err = l_parse_map_members (this, MAP(map), NOT_INTO_MAPDECL);
        this->exprList--;
        THROW_ERR_IF_ERR(err);

        THROW_SYNTAX_ERR_IF(TOKEN != TOKEN_PAREN_CLOS,
          "error while getting qualifiers, awaiting )");

        _l_set_qualifiers (this, MAP(map), uf);

      } else {
        if (NULL == TOKENSYM) goto parse_members;
        if (IS_UFUNC(TOKENSYM->type) || IS_CFUNC(TOKENSYM->type)) {
          if (PEEK_NTH_TOKEN(0) == TOKEN_PAREN_OPEN) {
            this->exprList++;
            err = l_parse_primary (this, &v);
            this->exprList--;
            THROW_ERR_IF_ERR(err);

            if (v.type == MAP_TYPE) {
              _l_set_qualifiers (this, v, uf);
              goto theend;
            }

            if (v.type != NULL_TYPE)
              THROW_SYNTAX_ERR("awaiting a map as qualifiers");
            goto theend;
          }
        }

        goto parse_members;
      }
    }
  }

theend:
  return count;
}

static int l_parse_char (l_t *this, VALUE *vp, l_string token) {
  VALUE v = INT(0);
  const char *ptr = GETSTRPTR(token);

  if (ptr[0] == TOKEN_SQUOTE) THROW_SYNTAX_ERR("error while getting a char token ");

  if (ptr[0] == TOKEN_ESCAPE_CHR) {
    if (ptr[1] == 'n')  { v = INT('\n'); goto theend; }
    if (ptr[1] == 't')  { v = INT('\t'); goto theend; }
    if (ptr[1] == 'r')  { v = INT('\r'); goto theend; }
    if (ptr[1] == '\\') { v = INT('\\'); goto theend; }
    if (ptr[1] == TOKEN_SQUOTE) { v = INT(TOKEN_SQUOTE); goto theend; }
    THROW_SYNTAX_ERR("unknown escape sequence");
  }

  if (ptr[0] >= ' ' && ptr[0] <= '~') {
    if (ptr[1] == TOKEN_SQUOTE) {
      v = INT (ptr[0]);
      goto theend;
    } else {
      THROW_SYNTAX_ERR("error while taking character literal");
    }
  }

  utf8_char u;
  utf8_encode (&u, ptr, 8);

  utf8 c = u.code;

  if (TOKEN_SQUOTE != ptr[u.len])
    THROW_SYNTAX_ERR("error while taking character literal");

  v = INT(c);

theend:
  *vp = v;
  return L_OK;
}

static int l_parse_string (l_t *this, l_string str) {
  int c,  r;
  l_string savepc = PARSEPTR;

  PARSEPTR = str;

  for (;;) {
    NEXT_TOKEN();

    while (TOKEN == TOKEN_NL || TOKEN == TOKEN_SEMICOLON)
      NEXT_TOKEN();

    if (TOKEN < 0) break;

    do_token:
    if (TOKEN == UFUNC_TYPE) {
      c = NEXT_BYTE_NOWS_NONL();
      if (c != TOKEN_PAREN_OPEN)
        TOKEN = TOKEN_VAR;
    }

    r = l_parse_stmt (this);

    if (r != L_OK) return r;

    if (TOKEN == TOKEN_NL || TOKEN == TOKEN_SEMICOLON ||
        TOKEN < 0)
      continue;
    //else
    goto do_token; // this is experimental (allow multiply statements in a single line)
    //return this->syntax_error_fmt (this, "%s(), unknown token |%c| |%d|", __func__, c, c);
  }

  PARSEPTR = savepc;

  return L_OK;
}

static void *l_fun_refcount_incr (void *count) {
  return (void *) ((intptr_t) count + 1);
}

static void *l_fun_refcount_decr (void *count) {
  return (void *) ((intptr_t) count - 1);
}

static void l_fun_release_symbols (funT *uf, int clear, int is_method) {
  if (NULL == uf->symbols) return;

  if (is_method) {
    sym_t *sym = map_pop_value (uf->symbols, "this");
    Release (sym);
  }

  if (clear) {
    map_clear (uf->symbols);
    return;
  }

  map_release (&uf->symbols);
}

static void l_fun_release_symbols_only (funT *uf, int is_method) {
  if (NULL == uf->symbols) return;

  if (is_method) {
    sym_t *sym = map_pop_value (uf->symbols, "this");
    Release (sym);
  }

  char **keys = map_keys (uf->symbols);
  int num_keys = uf->symbols->num_keys;

  for (int i = 0; i < num_keys; i++) {
    sym_t *sym = map_pop_value (uf->symbols, keys[i]);
    if (sym->type == STRING_TYPE) { // or sym->type is ARRAY_TYPE) {
      VALUE v = sym->value;
      if (0 == v.refcount || v.refcount == MALLOCED_STRING)
        string_release (AS_STRING(v));
    }
    Release (sym);
  }

  for (int i = 0; i < num_keys; i++)
    Release (keys[i]);
  Release (keys);
}

static int l_parse_func_call (l_t *this, VALUE *vp, CFunc op, funT *uf, VALUE value) {
  NEXT_TOKEN();
  int c = TOKEN;

  if (c != TOKEN_PAREN_OPEN) {
    VALUE v;
    if (uf != NULL) {
      int l = GETSTRLEN(TOKENSTR);
      while (l-- > 0) UNGET_BYTE();
      v = PTR(uf);
      v.type |= UFUNCTION_TYPE;
    } else {
      v = PTR(op);
      v.type = value.sym->type;
    }

    v.sym = value.sym;
    *vp = v;
    return L_OK;
  }

  int expectargs = (uf ? uf->nargs : TOKENARGS);

  int paramCount = 0;

  int is_method = this->funcState & MAP_METHOD_STATE;
  this->funcState &= ~MAP_METHOD_STATE;

  this->curState |= MALLOCED_STRING_STATE;

  NEXT_TOKEN();
  c = TOKEN;

  if (c != TOKEN_PAREN_CLOS) {
    paramCount = l_parse_expr_list (this, uf, expectargs);

    c = TOKEN;
    if (paramCount < 0)
      return paramCount;
  } else {
    paramCount = this->argCount;
    this->argCount = 0;
  }

  this->curState &= ~(MALLOCED_STRING_STATE);

  do {
    if (c != TOKEN_PAREN_CLOS) {
      if (c == TOKEN_NL) {
        if (PEEK_NTH_TOKEN(0) == TOKEN_PAREN_CLOS) {
          NEXT_TOKEN();
          break;
        }
      }

      THROW_SYNTAX_ERR("error while parsing expression list: expected closed parentheses");
    }
  } while (0);
  if (expectargs != paramCount)
    THROW_NUM_FUNCTION_ARGS_MISMATCH(expectargs, paramCount);

  while (paramCount > 0) {
    --paramCount;
    this->funArgs[paramCount] = stack_pop (this);
  }

  int err = L_OK;

  if (uf) {
    int refcount = (intptr_t) map_set_by_callback (this->funRefcount, uf->funname, NULL, l_fun_refcount_incr);
    if (refcount > 1) {
      if (str_eq (this->curScope->funname, uf->funname))
        l_symbol_stack_push (this, this->curScope->symbols);
      else
        l_symbol_stack_push (this, uf->symbols);

      for (int i = 0; i < expectargs; i++)
        this->funArgs[i] = l_copy_value (this->funArgs[i]);

      l_fun_release_symbols_only (uf, is_method);
    }

    sym_t *uf_argsymbols[expectargs];

    for (int i = 0; i < expectargs; i++) {
      VALUE v = this->funArgs[i];
      if (v.type & UFUNCTION_TYPE) {
        funT *f = AS_FUNC_PTR(v);
        uf_argsymbols[i] = l_define_symbol (this, uf, uf->argName[i], (UFUNC_TYPE | (f->nargs << 8)), v, 0);
      } else {
        v.refcount += (v.refcount > -2 ? (refcount < 2) : 0);
        uf_argsymbols[i] = l_define_symbol (this, uf, uf->argName[i], v.type, v, 0);
      }
    }

    int hastoreturn = HASTORETURN;
    HASTORETURN = 0;

    uf->result = NULL_VALUE;

    l_fun_stack_push (this, this->curScope);

    this->curScope = uf;

    string *func = AS_STRING(this->func->value);
    string_replace_with (func, uf->funname);

    int varisnotallowed = this->curState & VAR_IS_NOT_ALLOWED;
    this->curState &= ~VAR_IS_NOT_ALLOWED;

    int loopcount = this->loopCount;
    this->loopCount = 0;

    l_string savepc = PARSEPTR;
again:

    err = l_parse_string (this, uf->body);

    if (err == L_ERR_TCALLREC) {
      NEXT_TOKEN();
      THROW_SYNTAX_ERR_IF(TOKEN != TOKEN_PAREN_OPEN,
        "error while parsing function call, awaiting (");

      this->curState |= MALLOCED_STRING_STATE;
      NEXT_TOKEN();
      paramCount = l_parse_expr_list (this, uf, expectargs);
      this->curState &= ~MALLOCED_STRING_STATE;

      THROW_SYNTAX_ERR_IF(TOKEN != TOKEN_PAREN_CLOS,
        "error while parsing function call, awaiting )");

      int peek_tok = PEEK_NTH_TOKEN(0);

      if ((peek_tok & 0xff) == TOKEN_BINOP)
        THROW_SYNTAX_ERR ("awaiting a tail call");

      if (paramCount < 0)
        return paramCount;

      if (expectargs != paramCount)
        THROW_NUM_FUNCTION_ARGS_MISMATCH(expectargs, paramCount);

      while (paramCount > 0) {
        this->funArgs[--paramCount] = stack_pop (this);
      }

      for (int i = 0; i < expectargs; i++) {
        VALUE v = this->funArgs[i];
        VALUE val = l_copy_value (v);
        val.refcount = 0;
        this->funArgs[i] = val;
      }

      l_fun_release_symbols (uf, 1, is_method);

      for (int i = 0; i < expectargs; i++) {
        VALUE v = this->funArgs[i];
        if (v.type & UFUNCTION_TYPE) {
          funT *f = AS_FUNC_PTR(v);
          uf_argsymbols[i] = l_define_symbol (this, uf, uf->argName[i], (UFUNC_TYPE | (f->nargs << 8)), v, 0);
        } else {
          uf_argsymbols[i] = l_define_symbol (this, uf, uf->argName[i], v.type, v, 0);
        }
      }

      goto again;
    }

    PARSEPTR = savepc;

    this->loopCount = loopcount;
    if (this->loopCount)
      this->curState |= LOOP_STATE;

    if (varisnotallowed) this->curState |= VAR_IS_NOT_ALLOWED;

    this->curScope = l_fun_stack_pop (this);

    string_replace_with (func, this->curScope->funname);

    HASTORETURN = hastoreturn;

    l_release_qualifiers (this, uf->funname);

    this->curState &= ~FUNC_CALL_RESULT_IS_MMT;
    if (uf->result.type >= UFUNCTION_TYPE) {
      sym_t *sym = uf->result.sym;
      this->curState |= FUNC_CALL_RESULT_IS_MMT;

      ifnot (NULL == sym) {
        if (uf->result.type == STRING_TYPE &&
            uf->result.sym->type == MAP_TYPE) {
          VALUE v = STRING(string_dup(AS_STRING(uf->result)));
          uf->result = v;
        }

        sym->value = NULL_VALUE;
        uf->result.sym = NULL;

      } else {
        ifnot (this->exprList)
          if (uf->result.refcount == MALLOCED_STRING) {
            VALUE v = STRING(string_dup(AS_STRING(uf->result)));
            uf->result = v;
          }
      }
    }

    for (int i = 0; i < expectargs; i++) {
      if (refcount > 0) continue;
      VALUE v = this->funArgs[i];
      if (v.type >= UFUNCTION_TYPE) {
        sym_t *uf_sym = uf_argsymbols[i];
        VALUE uf_val = uf_sym->value;
        sym_t *sym = uf_val.sym;

        if (sym != NULL && uf_sym->scope != sym->scope) {
          sym->value = uf_val;
          uf_sym->value = NULL_VALUE;
        } else {
          if (v.type == STRING_TYPE && v.refcount <= MALLOCED_STRING)
            uf_sym->value = NULL_VALUE;
          else if  (v.type == MAP_TYPE && v.sym != NULL)
            uf_sym->value = NULL_VALUE;
        }
      }
    }

    refcount = (intptr_t) map_set_by_callback (this->funRefcount, uf->funname, NULL, l_fun_refcount_decr);

    ifnot (refcount)
      l_fun_release_symbols (uf, 1, is_method);
    else {
      l_fun_release_symbols (uf, 0, is_method);

      if (str_eq (this->curScope->funname, uf->funname))
        this->curScope->symbols = l_symbol_stack_pop (this);
      else
        uf->symbols = l_symbol_stack_pop (this);
    }

    *vp = uf->result;

    return err;
  }

  this->CFuncError = L_OK;
  this->curMsg[0] = '\0';

  *vp = op (this, this->funArgs[0], this->funArgs[1], this->funArgs[2],
                  this->funArgs[3], this->funArgs[4], this->funArgs[5],
                  this->funArgs[6], this->funArgs[7], this->funArgs[8]);

  for (int i = 0; i < expectargs; i++) {
    VALUE v = this->funArgs[i];
    if (v.type == STRING_TYPE)
      if (v.refcount <= MALLOCED_STRING)
        continue;

    if (v.sym == NULL)
      l_release_value (this, v);
  }

  l_release_qualifiers (this, this->curCFunName);

  this->curState &= ~FUNC_CALL_RESULT_IS_MMT;

  if (vp->type >= UFUNCTION_TYPE)
    this->curState |= FUNC_CALL_RESULT_IS_MMT;

  err = this->CFuncError;

  if (err != L_OK)
    if (*this->curMsg)
      this-> print_fmt_bytes (this->err_fp, "C function error: %s\n", this->curMsg);

  NEXT_TOKEN();

  return err;
}

static int l_parse_if_in_chain (l_t *, VALUE *);
static int l_parse_chain (l_t *this, VALUE *vp) {
  int err, c;

  sym_t *sym = NULL;
  char method[MAXLEN_SYMBOL * 2];
  this->funcState |= CHAIN_STATE;

  do {
    NEXT_RAW_TOKEN();
    while (TOKEN == TOKEN_NL) NEXT_RAW_TOKEN();
    c = TOKEN;

    VALUE save_v;

    this->objectState &= ~(ARRAY_MEMBER|MAP_MEMBER);

    if (c != TOKEN_SYMBOL)
      THROW_SYNTAX_ERR("awaiting a method");

    char *key = map_key (this, TOKENSTR);

    switch (vp->type) {
      case STRING_TYPE:
        str_copy_fmt (method, MAXLEN_SYMBOL * 2, "string_%s", key);
        break;

      case INTEGER_TYPE:
        str_copy_fmt (method, MAXLEN_SYMBOL * 2, "integer_%s", key);
        break;

      case NUMBER_TYPE:
        str_copy_fmt (method, MAXLEN_SYMBOL * 2, "number_%s", key);
        break;

      case ARRAY_TYPE:
        str_copy_fmt (method, MAXLEN_SYMBOL * 2, "array_%s", key);
        break;

      case MAP_TYPE:
        str_copy_fmt (method, MAXLEN_SYMBOL * 2, "map_%s", key);
        break;

      case LIST_TYPE:
        str_copy_fmt (method, MAXLEN_SYMBOL * 2, "list_%s", key);
        break;

      case NULL_TYPE:
      default:
          method[0] = '\0';
        break;

      // default: allow
      //   THROW_SYNTAX_ERR("unsupported datatype");
    }

    sym = ns_lookup_symbol (this->std, method);

    if (sym == NULL) {
      str_copy (method, MAXLEN_SYMBOL + 1, key, bytelen (key));
      sym = ns_lookup_symbol (this->std, method);
    }

    if (sym == NULL || sym->value.type == 0) {
      sym = l_lookup_symbol (this, TOKENSTR);

      if (sym == NULL)
        THROW_SYNTAX_ERR_FMT("%s: unknown function", method);

      switch (sym->type) {
        case TOKEN_FORMAT: {
          if (vp->type == ARRAY_TYPE) {
            NEXT_TOKEN();
            THROW_SYNTAX_ERR_IFNOT(TOKEN == TOKEN_PAREN_OPEN, "array format(), awaiting '('");
            NEXT_TOKEN();
            err = l_parse_format_array (this, vp, 0);
            THROW_ERR_IF_ERR(err);
            goto release_value;
          }

          THROW_SYNTAX_ERR_IF(vp->type != STRING_TYPE, "format(): awaiting a string type");

          string *s = AS_STRING ((*vp));
          size_t len = s->num_bytes + 4;
          char buf[len + 1];
          int idx = 0;
          buf[idx++] = '(';
          char *ptr = s->bytes;
          if (*ptr != '\"') {
            char *evptr = str_chr (ptr, '$');
            if (NULL != evptr && (evptr == ptr ||
                *(evptr - 1) != '\\')) {
              buf[idx++] = '"';
              str_copy (buf + idx, len, ptr, s->num_bytes);
              idx += s->num_bytes;
              buf[idx++] = '"';
            } else {
              str_copy (buf + idx, len, ptr, s->num_bytes);
              idx += s->num_bytes;
            }
          } else {
            str_copy (buf + idx, len, ptr, s->num_bytes);
            idx += s->num_bytes;
          }
          buf[idx++] = ')';
          buf[idx] = '\0';

          l_string savepc = PARSEPTR;
          PARSEPTR = StringNew (buf);

          save_v = *vp;
          if (save_v.refcount == STRING_LITERAL)
            save_v.refcount = 0;

          err = l_parse_format (this, vp);
          THROW_ERR_IF_ERR(err);
          PARSEPTR = savepc;

          NEXT_TOKEN();
          c = TOKEN;

          THROW_SYNTAX_ERR_IFNOT(c == TOKEN_PAREN_OPEN, "awaiting (");

          NEXT_TOKEN();
          c = TOKEN;
          THROW_SYNTAX_ERR_IFNOT(c == TOKEN_PAREN_CLOS, "awaiting )");
          NEXT_TOKEN();
          goto release_value;
        }

        default:
          if (IS_UFUNC(sym->type) || IS_CFUNC(sym->type))
            break;

          THROW_SYNTAX_ERR_FMT("%s: unknown function", method);
      }
    }

    if (sym->type == TOKEN_IF || sym->type == TOKEN_IFNOT) {
      save_v = *vp;
      if (save_v.refcount == STRING_LITERAL)
        save_v.refcount = 0;

      TOKEN = sym->type;

      err = l_parse_if_in_chain (this, vp);

      goto release_value;

    }

    int type;
    if (IS_UFUNC(sym->type))
      type = UFUNCTION_TYPE;
    else if (IS_CFUNC(sym->type))
      type = CFUNCTION_TYPE;
    else
      THROW_SYNTAX_ERR("not a function type");

    VALUE val = sym->value;
    save_v = *vp;

    if (vp->refcount > -1)
      vp->refcount++;

    stack_push (this, (*vp));
    this->argCount = 1;

    if (type == UFUNCTION_TYPE) {
      funT *uf = AS_FUNC_PTR(val);
      err = l_parse_func_call (this, vp, NULL, uf, *vp);

      if (save_v.refcount == ARRAY_LITERAL)
        save_v.refcount--;

      NEXT_TOKEN();

    } else {
      CFunc op = (CFunc) AS_PTR(sym->value);
      TOKENARGS = (sym->type >> 8) & 0xff;
      str_copy (this->curCFunName, MAXLEN_SYMBOL + 1, method, bytelen (method));
      err = l_parse_func_call (this, vp, op, NULL, val);

      if (save_v.type == MAP_TYPE || save_v.refcount == ARRAY_LITERAL)
        save_v.refcount--;
    }

    release_value:

    if (save_v.sym == NULL) {
      if (save_v.refcount > -1) {
        l_release_value (this, save_v);
      } else if (save_v.refcount == ARRAY_LITERAL) {
        if (vp->refcount == ARRAY_LITERAL + 1) // care as ARRAY_LITERAL < 0
          vp->refcount--; // though it is the opposite, it matches with the code above
        else              // and with the how the if_in_chain() handles it
          l_release_value (this, save_v);
      }
    }

  } while (TOKEN == TOKEN_COLON);

  this->funcState &= ~CHAIN_STATE;
  return err;
}

static int l_parse_prefix (l_t *this, VALUE *vp, int op_token) {
  int err;

  NEXT_TOKEN();

  switch (TOKEN) {
    case INTEGER_TYPE:
    case NUMBER_TYPE: {
      err = l_parse_primary (this, vp);
      THROW_ERR_IF_ERR(err);
      THROW_SYNTAX_ERR_IF(vp->type != INTEGER_TYPE && vp->type != NUMBER_TYPE,
         "awaiting an integer or a number");

      sym_t *sym = NULL;

      sym = vp->sym;
      THROW_SYNTAX_ERR_IF(sym == NULL, "awaiting a symbol");

      switch (vp->type) {
        case INTEGER_TYPE:
          sym->value = INT(AS_INT((*vp)) + (op_token == TOKEN_PLUS_PLUS ? 1 : -1));
          break;

        case NUMBER_TYPE:
          sym->value = NUMBER(AS_NUMBER((*vp)) + (op_token == TOKEN_PLUS_PLUS ? 1 : -1));
      }

      *vp = sym->value;
      return L_OK;
    }

    case TOKEN_LIST:
    case TOKEN_ARRAY:
    // fall through
    case TOKEN_MAP: {
      int token = TOKEN;
      VALUE tokenval = TOKENVAL;

      const char *ptr = GETSTRPTR(PARSEPTR);
      err = l_parse_primary (this, vp);
      THROW_ERR_IF_ERR(err);
      THROW_SYNTAX_ERR_IF(vp->type != INTEGER_TYPE && vp->type != NUMBER_TYPE,
        "awaiting an integer or a number");

      const char *cptr = GETSTRPTR(PARSEPTR);
      integer len = cptr - ptr;
      char buf[len + 128];
      integer idx = 0;
      for (integer i = 0; i < len; i++) {
        if (ptr[i] == '-') {
          if (ptr[i+1] == '-') { // there is room: if last then it is a syntax error
            THROW_SYNTAX_ERR_IF(i + 2 == len ||
              (i + 3 == len && (ptr[i+2] == '\n' || ptr[i+2] == ';')),
              "prefixed and postfixed operations on the same object, is not allowed");
              /* the above should be catched before and should count and for
               * the opposite operators
               */

            if (i && (ptr[i - 1] != ' ' && ptr[i - 1] != TOKEN_INDEX_OPEN && ptr[i - 1] != TOKEN_PAREN_OPEN)) {
              buf[idx++] = ' '; buf[idx++] = '+';
              buf[idx++] = ' '; buf[idx++] = '1';
            }

            i++;
            continue;
          }
        }

        if (ptr[i] == '+') {
          if (ptr[i+1] == '+') {
            THROW_SYNTAX_ERR_IF(i + 2 == len ||
              (i + 3 == len && (ptr[i+2] == '\n' || ptr[i+2] == ';')),
              "prefixed && postfixed operations on the same object, is not allowed");

            if (i && (ptr[i - 1] != ' ' && ptr[i - 1] != TOKEN_INDEX_OPEN && ptr[i - 1] != TOKEN_PAREN_OPEN)) {
              buf[idx++] = ' '; buf[idx++] = '-';
              buf[idx++] = ' '; buf[idx++] = '1';
            }

            i++;
            continue;
          }
        }

        buf[idx++] = ptr[i];
      }

      if (buf[idx - 1] == '\n' || buf[idx - 1] == ';') {
        idx--;
      } else {
        int toklen = GETSTRLEN(TOKENSTR);
        if (buf[idx - toklen - 1] == ' ')
          idx -= (toklen + 1);
        else
          THROW_SYNTAX_ERR("unexpected token on a prefix operation");
      }

      op_token = (op_token == TOKEN_PLUS_PLUS ? '+' : '-');

      buf[idx++] = op_token; buf[idx++] = op_token;
      buf[idx] = '\0';

      int curtok = TOKEN;
      VALUE curtokenval = TOKENVAL;
      l_string savepc = PARSEPTR;

      PARSEPTR = StringNew(buf);
      TOKEN = token;
      TOKENVAL = tokenval;

      err = l_parse_primary (this, vp);
      THROW_ERR_IF_ERR(err);

      PARSEPTR = savepc;
      TOKEN = curtok;
      TOKENVAL = curtokenval;

      VALUE v;
      switch (vp->type) {
        case INTEGER_TYPE:
          v = INT(AS_INT((*vp)) + (op_token == '+' ? 1 : -1));
          *vp = v;
          return L_OK;

        case NUMBER_TYPE:
          v = NUMBER(AS_NUMBER((*vp)) + (op_token == '-' ? 1 : -1));
          *vp = v;
          return L_OK;
      }
    }
    break;

    default:
      THROW_SYNTAX_ERR("awaiting a variable, a map or a list or an array");
  }

  return L_NOTOK;
}

static int l_parse_primary (l_t *this, VALUE *vp) {
  int err = L_OK;
  int c = TOKEN;

  switch (c) {
    case TOKEN_INDEX_OPEN:
      ifnot (this->curState & INDEX_STATE) {
        err = l_parse_anon_array (this, vp);
        if (err == L_OK) {

          if (TOKEN != TOKEN_INDEX_CLOS) {
            if (TOKEN != TOKEN_NL)
              THROW_SYNTAX_ERR("array expression, awaiting ]");
            else
              GET_BYTE();
          }

          c = NEXT_BYTE_NOWS_NONL();

          if (c == TOKEN_INDEX_OPEN) {
            VALUE saved_val = *vp;
            TOKEN = TOKEN_ARRAY;
            TOKENVAL = *vp;

            err = l_parse_primary (this, vp);
            THROW_ERR_IF_ERR(err);

            if (vp->type == STRING_TYPE) {
              string *str = AS_STRING((*vp));
              *vp = STRING_NEW_WITH_LEN(str->bytes, str->num_bytes);
            }

            l_release_value (this, saved_val);
            return err;
          }

          NEXT_TOKEN();
          c = TOKEN;

          if (c == TOKEN_COLON && 0 == (this->curState & INDEX_STATE)) {
            vp->refcount = ARRAY_LITERAL;
            TOKENVAL = *vp;
            return l_parse_chain (this, vp);
          }

          return L_OK;
        }

        return err;
      }
      /* fall through */

    case TOKEN_PAREN_OPEN: {
      int close_token = (c == TOKEN_PAREN_OPEN
         ? TOKEN_PAREN_CLOS : TOKEN_INDEX_CLOS);

      NEXT_TOKEN();

      err = l_parse_expr (this, vp);
      THROW_ERR_IF_ERR(err);

      if (TOKEN == close_token && 0 == (this->curState & CLOSURE_STATE)) {
        NEXT_TOKEN();

        if (TOKEN == TOKEN_COLON && 0 == (this->curState & INDEX_STATE)) {
          TOKENVAL = *vp;
          return l_parse_chain (this, vp);
        }

        return L_OK;
      }

      return err;
    }

    case TOKEN_IFNOT:
    case TOKEN_IF:

      err = l_parse_cond (this, c == TOKEN_IFNOT);
      THROW_ERR_IF_ERR(err);

      if (TOKEN != TOKEN_THEN) {
        if (TOKEN == TOKEN_NL || TOKEN == TOKEN_SEMICOLON) {
          if (PEEK_NTH_TOKEN(0) != TOKEN_THEN) {
            if (this->funcState & RETURN_STATE) {
              HASTORETURN = AS_INT(CONDVAL);
              *vp = NULL_VALUE;
              return L_OK;
            }
            else
              THROW_SYNTAX_ERR("if expression: awaiting then");
          } else
            NEXT_TOKEN();
        } else
          THROW_SYNTAX_ERR("if expression: awaiting then");
      }

      return l_parse_iforelse (this, AS_INT(CONDVAL), vp);

    case TOKEN_INTEGER: {
      VALUE val = l_string_to_dec (TOKENSTR);
      *vp = val;

      NEXT_TOKEN();
      c = TOKEN;

      if (c == TOKEN_COLON && 0 == (this->curState & INDEX_STATE)) {
        TOKENVAL = *vp;
        return l_parse_chain (this, vp);
      }

      return L_OK;
    }

    case TOKEN_NUMBER: {
      char *endptr; char str[32];
      str_copy (str, 32, GETSTRPTR(TOKENSTR), GETSTRLEN(TOKENSTR));
      double val = str_to_double (str, &endptr);
      *vp = NUMBER(val);

      NEXT_TOKEN();
      c = TOKEN;

      if (c == TOKEN_COLON) {
        TOKENVAL = *vp;
        return l_parse_chain (this, vp);
      }
      return L_OK;
    }

    case TOKEN_OCTAL:
      *vp = l_OctalStringToNum (TOKENSTR);

      NEXT_TOKEN();
      c = TOKEN;

      if (c == TOKEN_COLON) {
        TOKENVAL = *vp;
        return l_parse_chain (this, vp);
      }

      return L_OK;

    case TOKEN_BINARY:
      *vp = l_BinaryStringToNum (TOKENSTR);

      NEXT_TOKEN();
      c = TOKEN;

      if (c == TOKEN_COLON && 0 == (this->curState & INDEX_STATE)) {
        TOKENVAL = *vp;
        return l_parse_chain (this, vp);
      }
      return L_OK;

    case TOKEN_HEX_CHAR:
    case TOKEN_HEX_NUMBER:
      *vp = l_HexStringToNum (TOKENSTR);

      NEXT_TOKEN();
      c = TOKEN;

      if (c == TOKEN_COLON && 0 == (this->curState & INDEX_STATE)) {
        TOKENVAL = *vp;
        return l_parse_chain (this, vp);
      }
      return L_OK;

    case TOKEN_CHAR:
      err = l_parse_char (this, vp, TOKENSTR);
      THROW_ERR_IF_ERR(err);

      NEXT_TOKEN();

      if (TOKEN == TOKEN_COLON && 0 == (this->curState & INDEX_STATE)) {
        TOKENVAL = *vp;
        return l_parse_chain (this, vp);
      }

      return err;

    case TOKEN_PRINTLN:
    case TOKEN_PRINT:
      if (c == TOKEN_PRINT)
        err = l_parse_print (this);
      else
        err = l_parse_println (this);
      THROW_ERR_IF_ERR (err);

      *vp = INT(this->byteCount);
      return L_OK;

    case TOKEN_FORMAT:
      err = l_parse_format (this, vp);

      if (TOKEN == TOKEN_COLON) {
        vp->refcount = STRING_LITERAL;
        TOKENVAL = *vp;
        return l_parse_chain (this, vp);
      }
      return err;

    case TOKEN_LIST:
      err = l_parse_list_get (this, vp);
      THROW_ERR_IF_ERR(err);

      switch (TOKEN) {
        case TOKEN_COLON:
          if (this->curState & INDEX_STATE) return L_OK;
          return l_parse_chain (this, vp);

        case TOKEN_INDEX_OPEN:
          switch (vp->type) {
            case ARRAY_TYPE:
              UNGET_BYTE();
              TOKEN = TOKEN_ARRAY;
              TOKENVAL = *vp;
              return l_parse_primary (this, vp);

            case STRING_TYPE:
              UNGET_BYTE();
              TOKEN = TOKEN_STRING;
              TOKENVAL = *vp;
              return l_parse_primary (this, vp);

            case LIST_TYPE:
              UNGET_BYTE();
              TOKEN = TOKEN_LIST;
              TOKENVAL = *vp;
              return l_parse_primary (this, vp);

            default:
              THROW_SYNTAX_ERR("awaiting an array or a string or a list");
          }

        case TOKEN_DOT:
          switch (vp->type) {
            case MAP_TYPE:
              UNGET_BYTE();
              TOKEN = TOKEN_MAP;
              TOKENVAL = *vp;
              return l_parse_primary (this, vp);

            default:
              THROW_SYNTAX_ERR("awaiting a map");
          }

        default:
          return L_OK;
      }

    case  TOKEN_OBJECT:
    case TOKEN_FILEPTR:
    case      TOKEN_FD:
    case       0 ... 4:
    case     TOKEN_VAR:
      *vp = TOKENVAL;

      NEXT_TOKEN();

      if (TOKEN == TOKEN_PLUS_PLUS || TOKEN == TOKEN_MINUS_MINUS) {
        int peek = PEEK_NTH_BYTE(0);
        switch (peek) {
          case TOKEN_INDEX_CLOS:
          case TOKEN_SEMICOLON:
          case TOKEN_PAREN_CLOS:
          case TOKEN_NL:
          case TOKEN_COLON:
          case ' ':
          case '\t':
          case TOKEN_EOF:
            break;

          default:
            THROW_SYNTAX_ERR_FMT("unexpected token after %s",
              (TOKEN == TOKEN_PLUS_PLUS ? "++" : "--"));
        }

        VALUE v = *vp;
        sym_t *sym = vp->sym;

        switch (vp->type) {
          case INTEGER_TYPE:
            sym->value = INT(AS_INT((*vp)) + (TOKEN == TOKEN_PLUS_PLUS ? 1 : -1));
            NEXT_TOKEN();
            *vp = v;
            break;

          case NUMBER_TYPE:
            sym->value = NUMBER(AS_NUMBER((*vp)) + (TOKEN == TOKEN_PLUS_PLUS ? 1 : -1));
            NEXT_TOKEN();
            *vp = v;
            break;

          default:
            THROW_SYNTAX_ERR("awaiting an integer or a number");
        }
      }

      if (TOKEN != TOKEN_COLON || (this->curState & INDEX_STATE))
        return L_OK;

      return l_parse_chain (this, vp);

    case TOKEN_ARRAY:
      err = l_parse_array_get (this, vp);
      THROW_ERR_IF_ERR(err);

      if (TOKEN != TOKEN_DOT) {
        if (TOKEN == TOKEN_COLON && 0 == (this->curState & INDEX_STATE)) {
          TOKENVAL = *vp;
          return l_parse_chain (this, vp);
        }

        return err;
      }

      THROW_SYNTAX_ERR_IF(vp->type != MAP_TYPE,
        "awaiting a map type");

      UNGET_BYTE();
      TOKENVAL = *vp;
      /* fall through */

    case TOKEN_MAP:
      err = l_parse_map_get (this, vp);
      THROW_ERR_IF_ERR(err);

      /* UNUSED
      int type;
      if (vp->type & UFUNCTION_TYPE)
        type = UFUNCTION_TYPE;
      else if (IS_CFUNC(vp->type))
        type = CFUNCTION_TYPE;
      else {
      */
      if (TOKEN == TOKEN_COLON) {
        TOKENVAL = *vp;
        return l_parse_chain (this, vp);
      }

      break;

#if 0
      /* probably unused code, but leave it here for a while in case */
      ifnot (TOKEN == TOKEN_PAREN_OPEN)
        break;

      UNGET_BYTE();

      if (type == UFUNCTION_TYPE) {
        funT *uf = AS_FUNC_PTR((*vp));
        VALUE th = vp->sym->value;
        l_define_symbol (this, uf, "this", MAP_TYPE, th, 0);
        this->funcState |= MAP_METHOD_STATE;
        err = l_parse_func_call (this, vp, NULL, uf, *vp);
        NEXT_TOKEN();
        return err;
      } else {
        CFunc op = (CFunc) AS_PTR((*vp));
        TOKENARGS = ((*vp).type >> 8) & 0xff;
        return l_parse_func_call (this, vp, op, NULL, *vp);
      }
#endif

    case TOKEN_PLUS_PLUS:
    case TOKEN_MINUS_MINUS:
      return l_parse_prefix (this, vp, c);

    case TOKEN_APPEND:
      return l_parse_append (this, vp);

    case TOKEN_ANNOTATION:
      return l_parse_annotation (this, vp);

    case TOKEN_EVALFILE: {
      this->funcState |= EVAL_UNIT_STATE;
      err = l_parse_include (this);
      this->funcState &= ~EVAL_UNIT_STATE;
      THROW_ERR_IF_ERR(err);

      VALUE curFile = this->file->value;
      this->file->value = STRING(this->evalFile);

      funT *uf = this->curFunDef;
      VALUE ufv = PTR(uf);
      l_define_symbol (this, this->function, uf->funname, (UFUNC_TYPE | (uf->nargs << 8)), ufv, 0);
      l_string savepc = PARSEPTR;
      PARSEPTR = StringNew ("()");
      err = l_parse_func_call (this, vp, NULL, uf, ufv);
      this->file->value = curFile;
      PARSEPTR = savepc;
      return err;
    }

    case TOKEN_USRFUNC: {
      sym_t *symbol = TOKENSYM;
      THROW_SYNTAX_ERR_IF(NULL == symbol, "user defined function, not declared");

      funT *uf = AS_FUNC_PTR(symbol->value);

      err = l_parse_func_call (this, vp, NULL, uf, symbol->value);
      THROW_ERR_IF_ERR(err);

      NEXT_TOKEN();

      if (TOKEN == TOKEN_COLON) {
        TOKENVAL = *vp;
        return l_parse_chain (this, vp);
      }

      return err;
    }

    case TOKEN_BUILTIN: {
      CFunc op = (CFunc) AS_PTR(TOKENVAL);
      c_funname (this, TOKENSTR);
      err = l_parse_func_call (this, vp, op, NULL, TOKENSYM->value);
      THROW_ERR_IF_ERR(err);

      if (TOKEN == TOKEN_COLON) {
        TOKENVAL = *vp;
        return l_parse_chain (this, vp);
      }

      return err;
    }

    case TOKEN_FUNCDEF:
      return l_parse_anon_func (this, vp);

    case TOKEN_STRING:
      err = l_parse_string_get (this, vp);
      THROW_ERR_IF_ERR(err);

      if (TOKEN == TOKEN_COLON) {
        if (vp->sym == NULL &&
            vp->refcount != MALLOCED_STRING &&
            0 == (this->objectState & (ARRAY_MEMBER|MAP_MEMBER)))
          vp->refcount = STRING_LITERAL;

        this->objectState &= ~(ARRAY_MEMBER|MAP_MEMBER);
        TOKENVAL = *vp;
        return l_parse_chain (this, vp);
      }

      return err;

    case TOKEN_NEW:
      err = l_parse_new (this, vp);
      THROW_ERR_IF_ERR(err);

      while (TOKEN == TOKEN_DOT) {
        UNGET_BYTE();

        TOKENVAL = *vp;
        VALUE mapval = TOKENVAL;

        TOKEN = TOKEN_MAP;
        err = l_parse_primary (this, vp);
        THROW_ERR_IF_ERR(err);

        if (mapval.sym == NULL) {
          VALUE v = *vp;

          if (v.type != MAP_TYPE) {
            *vp = l_copy_value (v);
            this->objectState &= ~MAP_MEMBER;
          }

          l_release_value (this, mapval);
        }
      }

      return err;

    case TOKEN_BLOCK:
      err = l_parse_map (this, vp);
      THROW_ERR_IF_ERR(err);

      if (TOKEN == TOKEN_COLON) {
        vp->refcount = MAP_LITERAL;
        TOKENVAL = *vp;
        return l_parse_chain (this, vp);
      }

      while (TOKEN == TOKEN_DOT) {
        UNGET_BYTE();

        TOKENVAL = *vp;
        VALUE mapval = TOKENVAL;

        TOKEN = TOKEN_MAP;
        err = l_parse_primary (this, vp);
        THROW_ERR_IF_ERR(err);

        if (mapval.sym == NULL) {
          VALUE v = *vp;

          if (v.type != MAP_TYPE) {
            *vp = l_copy_value (v);
            this->objectState &= ~MAP_MEMBER;
          }

          l_release_value (this, mapval);
        }
      }

      return err;

    case TOKEN_LIST_NEW:
    case TOKEN_LIST_FUN:
      err = l_parse_list (this, vp, c == TOKEN_LIST_FUN);
      THROW_ERR_IF_ERR(err);

      if (TOKEN == TOKEN_COLON) {
        TOKENVAL = *vp;
        return l_parse_chain (this, vp);
      }

      return err;

    case TOKEN_ARYDEF:
      err = l_parse_array_def (this, vp, ARRAY_IS_REASSIGNMENT);
      THROW_ERR_IF_ERR(err);

      if (TOKEN == TOKEN_COLON) {
        TOKENVAL = *vp;
        vp->refcount = ARRAY_LITERAL;
        return l_parse_chain (this, vp);
      }

      return err;

    default:
      /* if ((c & 0xff) == TOKEN_BINOP) {
        OpFunc op = (OpFunc) AS_PTR(TOKENVAL);
        NEXT_TOKEN();
        VALUE v;
        err = l_parse_expr (this, &v);
        THROW_ERR_IF_ERR(err);

        *vp = op (this, INT(0), v);
        return err;
      }
      */

      if (c == TOKEN_NL) {
        NEXT_TOKEN();
        return l_parse_primary (this, vp);
      }

      if (c != TOKEN_EOF)
        return this->syntax_error_fmt (this, "%s(), syntax error, unknown token |%c| |%d|", __func__, c, c);
  }

  return L_OK;
}

static int l_parse_boolean_stmt (l_t *this, int tok) {
  int err;

  size_t len = GETSTRLEN(PARSEPTR);
  const char *Ptr = GETSTRPTR(PARSEPTR);
  const char *ptr = Ptr;

  while (*ptr && is_space (*ptr)) ptr++;

  if (len - (ptr - Ptr) < 5) return tok;

  if ('i' == *ptr && 'f' == *(++ptr)) {
    ptr++;
    int isspace = 0;
    int isif = -1;

    if ('(' == *ptr || (isspace = is_space (*ptr)) || *ptr == TOKEN_NL) {
      isif = 1;
    } else if (len - (ptr - Ptr) > 5 && (
      'n' == *ptr && 'o' == *(++ptr) && 't' == *(++ptr))) {
      ptr++;
      if ('(' == *ptr || (isspace = is_space (*ptr)) || *ptr == TOKEN_NL)
        isif = 0;
    }

    if (-1 == isif) return tok;

    if (isspace) {
      ptr++;
      while (is_space (*ptr)) ptr++;
    }

    if (*ptr == TOKEN_NL) ptr++;

    len -= ptr - Ptr;
    PARSEPTR = StringNew (ptr);
    SETSTRLEN(PARSEPTR, len);

    err = l_parse_cond (this, isif == 0);
    THROW_ERR_IF_ERR(err);

    if (AS_INT(CONDVAL))
      return tok;
    return L_OK;
  }

  return tok;
}

static int l_handle_break (l_t *this) {
  ifnot (this->curState & LOOP_STATE)
    THROW_SYNTAX_ERR("break is not in a loop");

  int c;
  uint nth = 0;
  int level = 0;
  int num = PEEK_NTH_BYTE_NOWS_NONL(&nth);
  if ('1' <= num && num <= '9') {
    nth++;
    c = PEEK_NTH_BYTE_NOWS_INLINE(&nth);

    if (c != TOKEN_SEMICOLON && c != TOKEN_NL &&
        c != TOKEN_BLOCK_CLOS && c != TOKEN_EOF &&
        c != 'i' && c != 'e')
      THROW_SYNTAX_ERR("unexpected token after a break statement");

    num -= ('0' + 1);

    if (num >= this->loopCount)
      THROW_SYNTAX_ERR("break statement: too many loop count levels");

    const char *ptr = GETSTRPTR(PARSEPTR) + 2;
    uint len = GETSTRLEN(PARSEPTR) - 2;
    SETSTRPTR(PARSEPTR, ptr);
    SETSTRLEN(PARSEPTR, len);
    level = num;
  }

  int err = l_parse_boolean_stmt (this, L_ERR_BREAK);
  THROW_ERR_IF_ERR(err);
  if (err == L_ERR_BREAK) this->breakCount = level;
  return err;
}

static int l_parse_stmt (l_t *this) {
  int err = L_OK;
  int c;
  int prev_token = 0;
  l_string name;
  funT *scope = this->curScope;
  VALUE val;

  do_token:
  c = TOKEN;

  switch (c) {
    case TOKEN_BREAK:
      return l_handle_break (this);

    case TOKEN_CONTINUE:
      ifnot (this->curState & LOOP_STATE)
        THROW_SYNTAX_ERR("continue is not in a loop");

      return l_parse_boolean_stmt (this, L_ERR_CONTINUE);

    case TOKEN_VARDEF:
    case TOKEN_CONSTDEF: {
      if (this->curState & VAR_IS_NOT_ALLOWED)
        THROW_SYNTAX_ERR("var declaration is not allowed");

      prev_token = c;
      int is_const = c == TOKEN_CONSTDEF;

      NEXT_RAW_TOKEN(); c = TOKEN;

      if (c == TOKEN_NL) {
        NEXT_RAW_TOKEN(); c = TOKEN;
      }

      THROW_SYNTAX_ERR_IF(c != TOKEN_SYMBOL, "var definition: expected a symbol");

      name = TOKENSTR;

      char *key = sym_key (this, name);

      THROW_SYNTAX_ERR_IF(str_eq(key, "this"), "can not define this");

      sym_t *sym = ns_lookup_symbol (this->std, key);
      ifnot (NULL == sym) {
        THROW_SYNTAX_ERR_FMT_IFNOT(str_eq (key, "func"), "%s, can not redefine a standard symbol", key);
        err = l_parse_func_def (this);
        THROW_ERR_IF_ERR(err);

        if (is_const)
         this->curSym->is_const = 1;
        return L_OK;
      }

      sym = ns_lookup_symbol (this->datatypes, key);

      ifnot (NULL == sym) {
        TOKENSYM = sym;
        VALUE v;
        int (*func) (l_t *, VALUE *, int) = AS_VOID_PTR(sym->value);
        err = (*func) (this, &v, 0);
        THROW_ERR_IF_ERR(err);

        if (TOKEN == TOKEN_COMMA) {
          uint n = 0;
          c = PEEK_NTH_BYTE_NOWS_INLINE(&n);
          if (c == TOKEN_NL) {
            for (uint i = 0; i <= n; i++)
              IGNORE_NEXT_BYTE;
          }

          TOKEN = prev_token;
          goto do_token;
        }

        return err;
      }

      scope = (this->scopeState == PUBLIC_SCOPE ? this->function : this->curScope);

      sym = ns_lookup_symbol (scope, key);

      ifnot (NULL == sym) {
        VALUE v = sym->value;
        if (is_const && (v.type & UFUNCTION_TYPE || IS_CFUNC(v.type))) {
          NEXT_TOKEN();
          if (TOKEN == TOKEN_ASSIGN) {
            NEXT_TOKEN();
            VALUE vv = TOKENVAL;
            switch (TOKEN) {
              case TOKEN_USRFUNC:
              case TOKEN_BUILTIN:
              if (AS_PTR(v) == AS_PTR(vv)) {
                NEXT_TOKEN();
               if (TOKEN == TOKEN_COMMA) {
                  uint n = 0;
                  c = PEEK_NTH_BYTE_NOWS_INLINE(&n);
                  if (c == TOKEN_NL) {
                    for (uint i = 0; i <= n; i++)
                      IGNORE_NEXT_BYTE;
                  }

                  TOKEN = prev_token;
                  goto do_token;
                }
                return L_OK;
              }
            }
          }
        }

        THROW_SYNTAX_ERR("can not redeclare a symbol in this scope");
      }

      VALUE v = NULL_VALUE;

      TOKENSYM = l_define_symbol (this, scope, key, v.type, v, is_const);

      THROW_SYNTAX_ERR_IF(NULL == TOKENSYM, "unknown error on declaration");

      int token = PEEK_NTH_TOKEN(0);
      if (token != TOKEN_ASSIGN) {
        if (token == TOKEN_COMMA) {
          NEXT_TOKEN();
          TOKEN = prev_token;
          goto do_token;
        }

        if (token == TOKEN_NL) {
          NEXT_TOKEN();
          return L_OK;
        }
      }

      this->scopeState = 0;
      TOKEN = TOKEN_VAR;
      goto do_token;
    }

    case TOKEN_STRING:
      if (IS_NEXT_BYTE(TOKEN_INDEX_OPEN)) {
        sym_t *symbol = TOKENSYM;
        return l_parse_string_set_char (this, symbol->value, symbol->is_const);
      }
      /* fall through */

    case  TOKEN_OBJECT:
    case TOKEN_FILEPTR:
    case      TOKEN_FD:
    case       0 ... 4:
    case     TOKEN_VAR: {
      name = TOKENSTR;
      sym_t *symbol = TOKENSYM;

      ifnot (symbol)
        THROW_UNKNOWN_SYMBOL(name);

      val = symbol->value;

      NEXT_TOKEN();

      int token = TOKEN;

      if (token < TOKEN_ASSIGN &&
          token > TOKEN_ASSIGN_LAST_VAL)
        THROW_SYNTAX_ERR("expected assignment operator");

      if (token == TOKEN_PLUS_PLUS || token == TOKEN_MINUS_MINUS) {
        int peek = PEEK_NTH_BYTE(0);
        switch (peek) {
          case TOKEN_INDEX_CLOS:
          case TOKEN_SEMICOLON:
          case TOKEN_PAREN_CLOS:
          case TOKEN_NL:
          case TOKEN_COLON:
          case ' ':
          case '\t':
          case TOKEN_EOF:
            break;

          default:
            THROW_SYNTAX_ERR_FMT("unexpected token after %s",
              (token == TOKEN_PLUS_PLUS ? "++" : "--"));
        }

        switch (val.type) {
          case INTEGER_TYPE:
            symbol->value = INT(AS_INT(val) + (token == TOKEN_PLUS_PLUS ? 1 : -1));
            NEXT_TOKEN();
            return L_OK;

          case NUMBER_TYPE:
            symbol->value = NUMBER(AS_NUMBER(val) + (token == TOKEN_PLUS_PLUS ? 1 : -1));
            NEXT_TOKEN();
            return L_OK;

          default:
            THROW_SYNTAX_ERR("awaiting an integer or a number");
        }
      }

      int is_const = symbol->is_const;
      if (is_const)
          THROW_SYNTAX_ERR_FMT_IFNOT(symbol->value.type == NULL_TYPE,
            "%s: can not reassign a value to a constant declared symbol",
             cur_msg_str (this, name));

      if (token == TOKEN_REASSIGN) {
        symbol->value.sym = NULL; // let the function to handle resources
        err = l_parse_chain (this, &symbol->value);
        THROW_ERR_IF_ERR(err);
        symbol->value.sym = symbol;
        return L_OK;
      }

      NEXT_TOKEN();

      int is_un = TOKEN == TOKEN_UNARY;
      if (is_un) NEXT_TOKEN();

      val.refcount = IS_UNCHANGEABLE;
      err = l_parse_expr (this, &val);
      THROW_ERR_IF_ERR(err);

      switch (val.refcount) {
        case IS_UNCHANGEABLE:
          return L_OK;
        case STRING_LITERAL:
          val.refcount = 0;
      }

      if (this->objectState & (ARRAY_MEMBER|MAP_MEMBER)) {
        switch (val.type) {
          case LIST_TYPE:
          case ARRAY_TYPE:
          case STRING_TYPE: {
            VALUE v = val;
            val = l_copy_value (v);
          }
        }
        this->objectState &= ~(ARRAY_MEMBER|MAP_MEMBER);
      }

      if (is_un) {
        if (val.type == INTEGER_TYPE)
          AS_INT(val) = ~AS_INT(val);
        else
          THROW_SYNTAX_ERR("error while setting an unary object, awaiting an integer");
      }

      if (val.type & UFUNCTION_TYPE) {
        funT *f = AS_FUNC_PTR(val);
        symbol->type = (UFUNC_TYPE | (f->nargs << 8));
        str_copy (f->funname, MAXLEN_SYMBOL + 1, GETSTRPTR(name), GETSTRLEN(name));
        f->prev = scope->prev;
      } else
        symbol->type = val.type;

      if (token == TOKEN_ASSIGN) {
        switch (symbol->value.type) {
          case STRING_TYPE:
            //if (str_eq (symbol->scope->funname, scope->funname))
            l_release_value (this, symbol->value);
            break;

          default:
            if (symbol->type & UFUNCTION_TYPE) {
              funT *f = AS_FUNC_PTR(symbol->value);
              fun_release (&f);
              break;
            }

            l_release_value (this, symbol->value);
        }

        if (val.sym != NULL) {
          switch (val.type) {
            case STRING_TYPE:
            case ARRAY_TYPE:
            case LIST_TYPE:
            case MAP_TYPE: {
              VALUE v = val;
              val = l_copy_value (v);
            }
          }
        }
      }

      VALUE result;

      switch (token) {
        case TOKEN_ASSIGN:
          val.sym = symbol;
          result = val;

          goto assign_and_return;

        case TOKEN_ASSIGN_APP:
          this->objectState |= OBJECT_APPEND;
          result = l_add (this, symbol->value, val);
          this->objectState &= ~OBJECT_APPEND;
          break;

        case TOKEN_ASSIGN_SUB: result = l_sub  (this, symbol->value, val); break;
        case TOKEN_ASSIGN_DIV: result = l_div  (this, symbol->value, val); break;
        case TOKEN_ASSIGN_MUL: result = l_mul  (this, symbol->value, val); break;
        case TOKEN_ASSIGN_MOD: result = l_mod  (this, symbol->value, val); break;
        case TOKEN_ASSIGN_BAR: result = l_bset (this, symbol->value, val); break;
        case TOKEN_ASSIGN_AND: result = l_bnot (this, symbol->value, val); break;
        case TOKEN_ASSIGN_XOR: result = l_bitxor (this, symbol->value, val); break;
        default: THROW_SYNTAX_ERR("unknown operator");
      }

      if (result.type == NULL_TYPE)
        THROW_SYNTAX_ERR("unexpected operation");

      assign_and_return:
      symbol->value = result;

      this->curState &= ~LITERAL_STRING_STATE;

      if (prev_token != TOKEN_VARDEF && prev_token != TOKEN_CONSTDEF)
        return L_OK;

      if (TOKEN == TOKEN_COMMA) {
        uint n = 0;
        c = PEEK_NTH_BYTE_NOWS_INLINE(&n);
        if (c == TOKEN_NL) {
          for (uint i = 0; i <= n; i++)
            IGNORE_NEXT_BYTE;
        }

        TOKEN = prev_token;
        goto do_token;
      }

      return L_OK;
    }

    case TOKEN_LIST:
      err = l_parse_list_set (this);
      THROW_ERR_IF_ERR(err);

      if (err == L_MMT_REASSIGN) {
        TOKEN = TOKEN_VAR;
        goto do_token;
      }

      return err;

    case TOKEN_ARRAY:
      err = l_parse_array_set (this);
      THROW_ERR_IF_ERR(err);

      if (err == L_MMT_REASSIGN) {
        TOKEN = TOKEN_VAR;
        goto do_token;
      }

      return err;

    case TOKEN_MAP:
      err = l_parse_map_set (this);
      THROW_ERR_IF_ERR(err);

      if (err == L_MMT_REASSIGN) {
        TOKEN = TOKEN_VAR;
        goto do_token;
      }

      return err;

    case TOKEN_OVERRIDE:
      NEXT_TOKEN();
      c = TOKEN;

      THROW_SYNTAX_ERR_IFNOT(c == TOKEN_MAP,
        "override works only for maps");

      this->objectState |= FUNC_OVERRIDE;
      TOKEN = TOKEN_MAP;
      goto do_token;

    case TOKEN_BUILTIN:
    case UFUNC_TYPE:
      err = l_parse_primary (this, &val);
      THROW_ERR_IF_ERR(err);

      if (val.sym == NULL) // plain function call without reason
        l_release_value (this, val);

      return err;

    case TOKEN_APPEND:
      err = l_parse_append (this, &val);
      THROW_ERR_IF_ERR(err);
      return err;

    case TOKEN_MINUS_MINUS:
    case TOKEN_PLUS_PLUS: {
      VALUE v;
      return l_parse_prefix (this, &v, c);
    }

    case TOKEN_COMMA:
      NEXT_TOKEN();
      return l_parse_stmt (this);

    default:
      if (TOKENSYM && AS_INT(TOKENVAL)) {
        int (*func) (l_t *) = AS_VOID_PTR(TOKENVAL);
        err = (*func) (this);
        if (err == TOKEN_PUBLIC)
          goto do_token;
        return err;
      }

      return this->syntax_error_fmt (this, "%s(), unknown token |%c| |%d|", __func__, c, c);
  }

  return err;
}

static int l_consume_binop (l_t *this) {
  int is_index_open = 0;
  int is_paren_open = 0;
  int is_if = 0;
  int is_binop = 0;

  NEXT_TOKEN();

next:
  is_binop = (TOKEN & 0xff) == TOKEN_BINOP;
  NEXT_TOKEN();

  switch (TOKEN) {
    case TOKEN_INDEX_OPEN:
      is_index_open++;
      goto next;

    case TOKEN_PAREN_OPEN:
      is_paren_open++;
      goto next;

    case TOKEN_IF:
    case TOKEN_IFNOT:
      is_if++;
      goto next;

    case TOKEN_INDEX_CLOS:
      if (is_index_open) {
        is_index_open--;
        goto next;
      }

    // fall through
    case TOKEN_PAREN_CLOS:
      if (is_paren_open) {
        is_paren_open--;
        goto next;
      }

    // fall through
    case TOKEN_BLOCK:
      ifnot (is_binop)
        break;
      goto next;

    case TOKEN_THEN:
      if (is_if) {
        is_if--;
        goto next;
      }

    // fall through
    case TOKEN_NL:
      if (is_binop)
        goto next;

    case TOKEN_SEMICOLON:
    // case TOKEN_COLON: cannot happen as end?
      break;

    case TOKEN_EOF:
      return L_NOTOK;
    default:
      goto next;
  }

  return L_OK;
}

static int l_parse_expr_level (l_t *this, int max_level, VALUE *vp) {
  int err;
  int c = TOKEN;

  ifnot ((c & 0xff) == TOKEN_BINOP)
    return L_OK;

  err = L_OK;
  VALUE lhs = *vp;
  VALUE rhs;

  string *x = NULL;
  if (lhs.type == STRING_TYPE && lhs.sym == NULL) {
    if (this->curState & LITERAL_STRING_STATE) {
      if (lhs.refcount != MALLOCED_STRING)
        x = AS_STRING(lhs);
    } else if (lhs.refcount == STRING_LITERAL) {
      x = AS_STRING(lhs);
    } else {
      if (0 == (this->objectState & (ARRAY_MEMBER|MAP_MEMBER)) &&
          lhs.refcount != MALLOCED_STRING &&
          0 == lhs.refcount) {
        x = AS_STRING(lhs);
      }
    }
  }

  int num_iter = 0;
  int lhs_released = 0;

  do {
    int level = (c >> 8) & 0xff;
    if (level > max_level) break;

    OpFunc op = (OpFunc) AS_PTR(TOKENVAL);
    int isbtype = TOKENSYM->type == BINOP(4);

    this->curState |= MALLOCED_STRING_STATE;
    NEXT_TOKEN();
    c = TOKEN;

    err = l_parse_primary (this, &rhs);
    THROW_ERR_IF_ERR(err);

    c = TOKEN;

    while ((c & 0xff) == TOKEN_BINOP) {
      int nextlevel = (c >> 8) & 0xff;
      if (level <= nextlevel) break;

      this->objectState &= ~(LHS_STRING_RELEASED|RHS_STRING_RELEASED);
      err = l_parse_expr_level (this, nextlevel, &rhs);
      THROW_ERR_IF_ERR(err);

      c = TOKEN;
    }

    this->CFuncError = L_OK;
    this->curMsg[0] = '\0';
    VALUE sv_lhs = lhs;

    if (num_iter++ || (lhs.type == STRING_TYPE && lhs.sym == NULL && lhs.refcount == STRING_LITERAL))
      this->objectState |= OBJECT_APPEND;
    this->objectState &= ~(LHS_STRING_RELEASED|RHS_STRING_RELEASED);

    const char *ptr = GETSTRPTR(TOKENSTR);
    lhs = op (this, lhs, rhs);

    if (isbtype) {
      ifnot (AS_INT(lhs)) {
        if (str_eq_n (ptr, "and", 3) || str_eq_n (ptr, "&&", 2)) {
          err = l_consume_binop (this);
          THROW_SYNTAX_ERR_IF(err == L_NOTOK, "awaiting the end of a binary operation");
          lhs = FALSE_VALUE;
          goto theend;
        }
      } else {
        if (str_eq_n (ptr, "or", 2) || str_eq_n (ptr, "||", 2)) {
          err = l_consume_binop (this);
          THROW_SYNTAX_ERR_IF(err == L_NOTOK, "awaiting the end of a binary operation");
          lhs = TRUE_VALUE;
          goto theend;
        }
      }
    }

    ifnot (num_iter - 1)
      lhs_released = (this->objectState & LHS_STRING_RELEASED) + (lhs.sym == NULL && lhs.refcount == STRING_LITERAL);

    if (this->CFuncError != L_OK) {
      if (this->curMsg[0] != '\0')
        this-> print_fmt_bytes (this->err_fp, "binary operation: %s\n", this->curMsg);
      return this->CFuncError;
    }

    if (sv_lhs.type != STRING_TYPE) {
      if (sv_lhs.sym == NULL)
        l_release_value (this, sv_lhs);
    }

    if (rhs.sym == NULL) {
      if (rhs.type != STRING_TYPE) {
        l_release_value (this, rhs);
      } else {
        ifnot (this->objectState & RHS_STRING_RELEASED) {
          if (rhs.refcount == STRING_LITERAL) {
            l_string_release (rhs);
          } else if (rhs.refcount != MALLOCED_STRING &&
              0 == (this->objectState & (ARRAY_MEMBER|MAP_MEMBER)))
            l_string_release (rhs);
         }
       }
    }

    this->curState &= ~LITERAL_STRING_STATE;
  } while ((c & 0xff) == TOKEN_BINOP);

theend:
  this->curState &= ~(MALLOCED_STRING_STATE|LITERAL_STRING_STATE);
  this->objectState &= ~(ARRAY_MEMBER|MAP_MEMBER|OBJECT_APPEND|LHS_STRING_RELEASED|RHS_STRING_RELEASED);

  *vp = lhs;

  ifnot (lhs_released)
    string_release (x);

  return err;
}

static int l_parse_expr (l_t *this, VALUE *vp) {
  int err = l_parse_primary (this, vp);
  THROW_ERR_IF_ERR(err);

  err = l_parse_expr_level (this, MAX_EXPR_LEVEL, vp);
  return err;
}

static int l_parse_block (l_t *this, const char *descr) {
  if (TOKEN == TOKEN_BLOCK)
    return L_OK;

  THROW_SYNTAX_ERR_FMT_IF(TOKEN == TOKEN_EOF, "unended %s block", descr);

  int leading_ws = 0;

  if (TOKEN == TOKEN_NL) {
    leading_ws = PEEK_NUM_WS();
    NEXT_TOKEN();
  }

  THROW_SYNTAX_ERR_FMT_IF(TOKEN == TOKEN_NL, "%s: expecting a block, found two consecutive new lines", descr);

  int parens = 0;
  int in_str = 0;
  int c = 0;
  int prev_c;
  char str_tok = 0;
  int num_block = 1;

  while (1) {
    prev_c = c;
    c = GET_BYTE();

    switch (c) {
      case TOKEN_PAREN_OPEN:
        ifnot (in_str) parens++;
         continue;

      case TOKEN_PAREN_CLOS:
        ifnot (in_str) parens--;
         continue;

      case TOKEN_DQUOTE:
      case TOKEN_BQUOTE:
        if (prev_c == TOKEN_ESCAPE_CHR) continue;

        if (in_str) {
          if (str_tok != c) continue;
           in_str--;
        } else {
          str_tok = c;
          in_str++;
        }
        continue;

      case TOKEN_SEMICOLON:
        if (parens || in_str)
          continue;
        break;

      case TOKEN_BLOCK_OPEN:
        if (parens || in_str)
          continue;
        num_block++;
        continue;

      case TOKEN_BLOCK_CLOS:
        if (parens || in_str)
          continue;

        num_block--;
        ifnot (num_block)
         break;

        continue;

      case TOKEN_NL: {
          int n = PEEK_NUM_WS();
          if (leading_ws && n >= leading_ws)
            continue;

          if (TOKEN_BLOCK_CLOS == *(GETSTRPTR(PARSEPTR) + n)) {
            GET_BYTE();
            continue;
          }
        }

        UNGET_BYTE();

      case TOKEN_EOF:
        break;

      default:
        continue;
    }

    break;
  }

  return L_OK;
}

static int l_consume_ifelse (l_t *this) {
  int err;
  int c;

  while (1) {
    NEXT_TOKEN();
    c = TOKEN;

    if (c == TOKEN_EOF)
      THROW_SYNTAX_ERR("unended conditional block");

    while ((c = GET_BYTE())) {
      if (c == TOKEN_EOF)
        THROW_SYNTAX_ERR("unended conditional block");

      if (c == TOKEN_BLOCK_OPEN) {
        err = l_get_opened_block (this, "unended conditional block");
        THROW_ERR_IF_ERR(err);

        NEXT_TOKEN();
        c = TOKEN;

        if (c != TOKEN_ELSE)
          return L_OK;
      }
    }
  }

  return L_OK;
}

static int l_consume_iforelse (l_t *this, int break_at_orelse, int leading_ws) {
  int levels = 1;
  this->curState |= CONSUME_STATE;

  int paren_open = 0;
  int index_open = TOKEN == TOKEN_INDEX_OPEN;

  while (1) {
    NEXT_TOKEN();

    check:

    switch (TOKEN) {
      case TOKEN_COLON:
        ifnot (this->curState & INDEX_STATE)
          continue;

      // fall through
      case TOKEN_NL:
        if (leading_ws) {  // we play with fire here
          int n = PEEK_NUM_WS();

          if (TOKEN_NL == *(GETSTRPTR(PARSEPTR) + (n + 1)))
            goto theend;

          if (n >= leading_ws) {
            IGNORE_NEXT_NUM_BYTES(n);
            continue;
          }

          goto theend;
        }

      // fall through
      case TOKEN_SEMICOLON:
        if (paren_open)
          continue;

      // fall through
      case TOKEN_EOF:
        if (this->funcState & CHAIN_STATE)
          continue;
        goto theend;

      case TOKEN_END:
        NEXT_TOKEN();
        levels--;
        goto theend;

      case TOKEN_ORELSE:
        if (break_at_orelse)
          if (levels == 1) goto theend;
        continue;

      case TOKEN_THEN:
        levels++;
        NEXT_TOKEN();
        if (TOKEN == TOKEN_NL) NEXT_TOKEN();
        goto check;

      case TOKEN_PAREN_OPEN:
        paren_open++;
        continue;

      case TOKEN_PAREN_CLOS:
        ifnot (paren_open) {
          ifnot (index_open) goto theend;
          continue;
        }

        paren_open--;
        continue;

      case TOKEN_INDEX_OPEN:
        index_open++;
        continue;

      case TOKEN_INDEX_CLOS:
        ifnot (index_open) goto theend;
        index_open--;
        continue;
    }

  }

theend:
  this->curState &= ~CONSUME_STATE;
  return levels;
}

static int l_parse_iforelse (l_t *this, int cond, VALUE *vp) {
  int err;
  this->conditionState = 0;
  tokenState save;
  int isorelse = TOKEN == TOKEN_ORELSE;

  this->curState |= MALLOCED_STRING_STATE;
  NEXT_TOKEN();

  int leading_ws = 0;

  if (TOKEN == TOKEN_NL) {
    NEXT_TOKEN();
    leading_ws = this->consumed_ws;
  }

  this->curState &= ~MALLOCED_STRING_STATE;

  THROW_SYNTAX_ERR_IF(TOKEN == TOKEN_EOF, "unended if/orelse");

  ifnot (cond) {
    if (TOKEN == TOKEN_ORELSE)
      return l_parse_iforelse (this, 1, vp);

    err = l_consume_iforelse (this, 1, leading_ws);
    THROW_ERR_IF_ERR(err);

    if (TOKEN == TOKEN_NL && PEEK_NTH_TOKEN(0) == TOKEN_ORELSE)
      NEXT_TOKEN();

    if (TOKEN == TOKEN_ORELSE)
      return l_parse_iforelse (this, 1, vp);

    save = SAVE_TOKENSTATE();

    this->curState |= CONSUME_STATE;

    NEXT_TOKEN();

    if (TOKEN == TOKEN_ORELSE) {
      this->curState &= ~CONSUME_STATE;
      if (err > 1)
        goto consume;
      else
        return l_parse_iforelse (this, 1, vp);
    }

    this->curState &= ~CONSUME_STATE;

    RESTORE_TOKENSTATE(save);
    return L_OK;
  }

  ifnot (NULL == vp) {
    err = l_parse_expr (this, vp);

  } else {
    this->curState |= VAR_IS_NOT_ALLOWED;
    err = l_parse_stmt (this);
    if (err == L_ERR_BREAK || err == L_ERR_CONTINUE)
      if (TOKEN == TOKEN_BREAK || TOKEN == TOKEN_CONTINUE)
        NEXT_TOKEN();

    this->curState &= ~VAR_IS_NOT_ALLOWED;
  }

  this->conditionState = 1;
  THROW_ERR_IF_ERR(err);

  if (isorelse || TOKEN == TOKEN_END) {
    if (TOKEN == TOKEN_END) NEXT_TOKEN();
    goto theend;
  }

  if (TOKEN != TOKEN_NL &&
      TOKEN != TOKEN_SEMICOLON && TOKEN != TOKEN_EOF) {
    if (TOKEN == TOKEN_ORELSE) {
      goto consume;

    } else if (
        TOKEN == TOKEN_PAREN_CLOS ||
        TOKEN == TOKEN_INDEX_CLOS ||
        TOKEN == TOKEN_COMMA ||
       (TOKEN == TOKEN_COLON && this->curState == INDEX_STATE)) {
      goto theend;

    } else {
      THROW_SYNTAX_ERR("awaiting a new line || ;");
    }

  } else {
    int p0 = PEEK_NTH_TOKEN(0);
    if (p0 == TOKEN_ORELSE) {
      NEXT_TOKEN();
      goto consume;
    }

    if (p0 == TOKEN_NL)
      if (PEEK_NTH_TOKEN(1) == TOKEN_ORELSE) {
        NEXT_TOKEN();
        NEXT_TOKEN();
        goto consume;
      }
  }

  save = SAVE_TOKENSTATE();

  consume_orelse:
  this->curState |= CONSUME_STATE;
  int nc = PEEK_NTH_TOKEN(0);
  if (nc == TOKEN_PAREN_CLOS || nc == TOKEN_INDEX_CLOS) {
    RESTORE_TOKENSTATE(save);
    goto theend;
  }

  NEXT_TOKEN();

  if (TOKEN == TOKEN_ORELSE) {

    consume:
    this->curState |= CONSUME_STATE;

    NEXT_TOKEN();

    if (TOKEN == TOKEN_NL) NEXT_TOKEN();

    int lerr = l_consume_iforelse (this, 0, leading_ws);

    THROW_ERR_IF_ERR(lerr);

    // if (lerr > 1) {
    //   lerr = l_consume_iforelse (this, 0, leading_ws);

    //   THROW_ERR_IF_ERR(lerr);
    // }

    save = SAVE_TOKENSTATE();

    goto consume_orelse;
  } else {
    RESTORE_TOKENSTATE(save);
  }

theend:
  this->curState &= ~CONSUME_STATE;
  return err;
}

static int l_parse_cond (l_t *this, int nottrue) {
  int err;

  NEXT_TOKEN();
  THROW_SYNTAX_ERR_IF(TOKEN == TOKEN_EOF, "unended conditional expression");

  this->curState |= MALLOCED_STRING_STATE;
  err = l_parse_expr (this, &CONDVAL);
  this->curState &= ~MALLOCED_STRING_STATE;
  THROW_ERR_IF_ERR(err);

  if (IS_INT(CONDVAL))
    CONDVAL = INT((AS_INT(CONDVAL) ? 0 == nottrue : nottrue));
  else if (IS_NULL(CONDVAL))
    CONDVAL = INT((nottrue ? 0 : 1));
  else if (IS_NUMBER(CONDVAL))
    CONDVAL = INT((AS_NUMBER(CONDVAL) == 0.0 ? nottrue: 0 == nottrue));

  return L_OK;
}

static int l_parse_if (l_t *this) {
  int err = l_parse_cond (this, TOKEN == TOKEN_IFNOT);
  THROW_ERR_IF_ERR(err);

  int is_true = AS_INT(CONDVAL);

  if (TOKEN == TOKEN_THEN)
    return l_parse_iforelse (this, is_true, NULL);

  err = l_parse_block (this, "if");
  THROW_ERR_IF_ERR(err);

  l_string ifpart = TOKENSTR;
  l_string elsepart;

  NEXT_TOKEN();

  int haveelse = 0;
  int haveelif = 0;

  l_string savepc;

  if (TOKEN == TOKEN_ELSE) {
    savepc = PARSEPTR;

    NEXT_TOKEN();

    if (TOKEN != TOKEN_IF && TOKEN != TOKEN_IFNOT) {
      if (TOKEN != TOKEN_BLOCK)
        THROW_SYNTAX_ERR("parsing else, not a block string");

      elsepart = TOKENSTR;
      haveelse = 1;

      NEXT_TOKEN();
    } else {
      haveelif = TOKEN;
      IGNORE_LAST_TOKEN;
    }
  }

  funT *save_scope = this->curScope;

  funT *fun = Fun_new (this, funNew (
    .name = NS_IF_BLOCK, .namelen = NS_IF_BLOCK_LEN, .parent = this->curScope
  ));

  this->curScope = fun;

  if (is_true) {
    err = l_parse_string (this, ifpart);
    if (haveelif) {
      PARSEPTR = savepc;
      int e = l_consume_ifelse (this);
      if (e != L_OK) err = e;
    }

  } else if (haveelse) {
    err = l_parse_string (this, elsepart);

  } else if (haveelif) {
    TOKEN = haveelif;
    err = l_parse_if (this);
  }

  if (HASTORETURN) RESET_PARSEPTR;

  this->curScope = save_scope;
  fun_release (&fun);
  return err;
}

static int l_parse_if_in_chain (l_t *this, VALUE *vp) {
  int err;
  int token = TOKEN;

  l_string savepc = PARSEPTR;

  NEXT_RAW_TOKEN();

  if (TOKEN != TOKEN_SYMBOL)
    THROW_SYNTAX_ERR("expected a symbol");

  char *key = sym_key (this, TOKENSTR);
  sym_t *sym = ns_lookup_symbol (this->std, key);
  ifnot (NULL == sym)
    THROW_SYNTAX_ERR("can not redefine a standard symbol");

  funT *fun = Fun_new (this, funNew (
    .name = NS_CHAIN_BLOCK, .namelen = NS_CHAIN_BLOCK_LEN, .parent = this->curScope
  ));

  funT *save_scope = this->curScope;
  this->curScope = fun;

  VALUE stackval = l_copy_value ((*vp));
  if (stackval.type == STRING_TYPE)
    stackval.refcount = STRING_LITERAL;

  sym = l_define_symbol (this, fun, key, stackval.type, stackval, 0);

  PARSEPTR = savepc;

  err = l_parse_cond (this, token == TOKEN_IFNOT);
  THROW_ERR_IF_ERR(err);

  err = l_parse_iforelse (this, AS_INT(CONDVAL), vp);
  THROW_ERR_IF_ERR(err);

  if (this->conditionState == 0 ||
      AS_VOID_PTR((*vp)) == AS_VOID_PTR(stackval)) {
    sym->value.refcount = -1;
    stackval.sym = NULL;
    *vp = stackval;
    if (vp->refcount == ARRAY_LITERAL)
      vp->refcount++;

  } else {
    if (vp->refcount == MALLOCED_STRING) {
      VALUE v = STRING(string_dup(AS_STRING((*vp))));
      *vp = v;
    }
  }

  this->curScope = save_scope;

  fun_release (&fun);

  return err;
}

static int l_parse_while (l_t *this) {
  int err;
  this->loopCount++;

  l_string exprpptr = PARSEPTR;

  NEXT_TOKEN();
  VALUE v;
  err = l_parse_expr (this, &v);
  THROW_ERR_IF_ERR(err);

  funT *save_scope = this->curScope;

  funT *fun = Fun_new (this, funNew (
    .name = NS_LOOP_BLOCK, .namelen = NS_LOOP_BLOCK_LEN, .parent = this->curScope
  ));

  fun->block_symbols = map_new (8);

  this->curScope = fun;

  err = l_parse_block (this, "while");
  THROW_ERR_IF_ERR(err);

  int is_single = TOKEN != TOKEN_BLOCK;

  const char *tmp_ptr = GETSTRPTR(TOKENSTR);

  integer bodylen = GETSTRLEN(TOKENSTR);
  char body[bodylen + 1];
  for (integer i = 0; i < bodylen; i++)
    body[i] = tmp_ptr[i];
  body[bodylen] = '\0';

  l_string body_str = StringNewLen (body, bodylen);

  l_string savepc = PARSEPTR;

  int count = 0;

  for (;;) {
    if (count++) {
      PARSEPTR = exprpptr;
      NEXT_TOKEN();
      err = l_parse_expr (this, &v);
      THROW_ERR_IF_ERR(err);
    }

    ifnot (AS_INT(v)) goto theend;

    this->curState |= LOOP_STATE;
    if (is_single) {
      PARSEPTR = body_str;
      NEXT_TOKEN();
      err = l_parse_stmt (this);
      if (TOKEN > TOKEN_EOF && TOKEN != TOKEN_SEMICOLON) {
        l_print_current_line (this,
          "[WARNING]: extra tokens detected after the end of a while statement\n",
          NULL);
      }

    } else {
      err = l_parse_string (this, body_str);
    }

    this->curState &= ~LOOP_STATE;
    THROW_ERR_IF_ERR(err);

    if (err == L_ERR_BREAK) goto theend;

    if (HASTORETURN) {
      this->curState &= ~LOOP_STATE;
      RESET_PARSEPTR;
      map_release (&fun->block_symbols);
      fun_release (&fun);
      return L_OK;
    }

    if (err == L_ERR_CONTINUE) {
      l_fun_release_symbols (fun, 1, 0);
      continue;
    }

    l_fun_release_symbols (fun, 1, 0);

    THROW_ERR_IF_ERR(err);
  }

theend:
  this->curScope = save_scope;
  map_release (&fun->block_symbols);
  fun_release (&fun);

  this->loopCount--;
  ifnot (this->loopCount)
    this->curState &= ~LOOP_STATE;
  else
    this->curState |= LOOP_STATE;

  PARSEPTR = savepc;

  TOKEN = TOKEN_SEMICOLON;

  if (err == L_ERR_BREAK && this->breakCount) {
    this->breakCount--;
    return L_ERR_BREAK;
  }

  return L_OK;
}

static int l_parse_repeat (l_t *this) {
  int err;
  this->loopCount++;

  NEXT_TOKEN();
  err = l_parse_block (this, "repeat/until");
  THROW_ERR_IF_ERR(err);

  integer bodylen = GETSTRLEN(TOKENSTR);
  const char *tmp_ptr = GETSTRPTR(TOKENSTR);

  int is_single = TOKEN != TOKEN_BLOCK;

  if (is_single) {
    int idx = 0;
    const char *ptr = tmp_ptr;
    while (idx++ < bodylen - 4) {
      if (*ptr == 'u' && str_eq_n (ptr, "until", 5)) {
        bodylen = (ptr - tmp_ptr) - 1;
        PARSEPTR = StringNew (ptr);
        break;
      }
      ptr++;
    }
  }

  char body[bodylen + 1];
  for (integer i = 0; i < bodylen; i++)
    body[i] = tmp_ptr[i];
  body[bodylen] = '\0';

  l_string body_str = StringNewLen (body, bodylen);

  NEXT_TOKEN();

  if (TOKEN != TOKEN_UNTIL)
    THROW_SYNTAX_ERR("error while parsing repeat/until loop, awaiting until");

  const char *orig_ptr = GETSTRPTR(PARSEPTR);
  tmp_ptr = orig_ptr;

  integer orig_len = GETSTRLEN(PARSEPTR);

  int is_paren_open = PEEK_NTH_TOKEN(0) == TOKEN_PAREN_OPEN;
  int has_a_nl = 0;
  if (is_paren_open) {
    while (*tmp_ptr != TOKEN_PAREN_OPEN) tmp_ptr++;
    tmp_ptr++;
  } else {
    while (*tmp_ptr == ' ') tmp_ptr++;
    if (*tmp_ptr == TOKEN_NL) {
      tmp_ptr++;
      has_a_nl = 1;
    }
  }

  const char *ptr = tmp_ptr;

  int parenopen = is_paren_open;
  int in_str = 0;
  int prev_c = *ptr;
  char str_tok = 0;

  THROW_SYNTAX_ERR_IF(has_a_nl && *ptr == TOKEN_NL, "error while parsing repeat/until condition: found two consecutive new lines");

  while (*ptr) {
    if (*ptr == TOKEN_NL) {
      if (is_paren_open || in_str)
        goto next;

      break;
    }

    if (*ptr == TOKEN_SEMICOLON) {
      if (is_paren_open || in_str)
        goto next;

      break;
    }

    if (*ptr == TOKEN_PAREN_OPEN && 1 == in_str + is_paren_open) {
      parenopen++;
      goto next;
    }

    if (*ptr == TOKEN_PAREN_CLOS && 1 == in_str + is_paren_open) {
      parenopen--;
      ifnot (parenopen)
        break;

      goto next;
    }

    if ((*ptr == TOKEN_DQUOTE || *ptr == TOKEN_BQUOTE)) {
      if (prev_c != TOKEN_ESCAPE_CHR) {
        if (in_str) {
          if (str_tok != *ptr) goto next;
           in_str--;
        } else {
          str_tok = *ptr;
          in_str++;
        }
      }
    }

    next: prev_c = *ptr; ptr++;
  }

  ifnot (*ptr)
    THROW_SYNTAX_ERR("unended repeat/until loop");

  TOKEN = TOKEN_PAREN_OPEN;

  integer cond_len = ptr - tmp_ptr;

  char cond[cond_len + 3];
  cond[0] = TOKEN_PAREN_OPEN;
  for (integer i = 0; i < cond_len; i++)
    cond[i + 1] = tmp_ptr[i];
  cond[cond_len + 1] = TOKEN_PAREN_CLOS;
  cond[cond_len + 2] = '\0';

  l_string cond_str = StringNewLen (cond, cond_len + 2);

  funT *save_scope = this->curScope;

  funT *fun = Fun_new (this, funNew (
    .name = NS_LOOP_BLOCK, .namelen = NS_LOOP_BLOCK_LEN, .parent = this->curScope
  ));

  fun->block_symbols = map_new (8);

  this->curScope = fun;

  SETSTRPTR(PARSEPTR, ptr + 1);
  SETSTRLEN(PARSEPTR, orig_len - ((ptr + 1) - orig_ptr));

  l_string savepc = PARSEPTR;

  VALUE v;
  for (;;) {
    this->curState |= LOOP_STATE;
    if (is_single) {
      PARSEPTR = body_str;
      NEXT_TOKEN();
      err = l_parse_stmt (this);
      if (TOKEN > TOKEN_EOF && TOKEN != TOKEN_SEMICOLON) {
        l_print_current_line (this,
          "[WARNING]: extra tokens detected after the end of a repeat/until statement\n",
          NULL);
      }

    } else {
      err = l_parse_string (this, body_str);
    }

    this->curState &= ~LOOP_STATE;
    THROW_ERR_IF_ERR(err);

    if (err == L_ERR_BREAK) goto theend;

    if (HASTORETURN) {
      this->curState &= ~LOOP_STATE;
      RESET_PARSEPTR;
      map_release (&fun->block_symbols);
      fun_release (&fun);
      return L_OK;
    }

    if (err == L_ERR_CONTINUE) {
      l_fun_release_symbols (fun, 1, 0);
      continue;
    }

    THROW_ERR_IF_ERR(err);

    PARSEPTR = cond_str;

    TOKEN = TOKEN_PAREN_OPEN;
    this->curState |= BLOCK_STATE;
    err = l_parse_expr (this, &v);
    this->curState &= ~BLOCK_STATE;
    THROW_ERR_IF_ERR(err);

    ifnot (AS_INT(v)) goto theend;
    l_fun_release_symbols (fun, 1, 0);
  }

theend:
  this->curScope = save_scope;
  map_release (&fun->block_symbols);
  fun_release (&fun);

  this->loopCount--;
  ifnot (this->loopCount)
    this->curState &= ~LOOP_STATE;
  else
    this->curState |= LOOP_STATE;

  PARSEPTR = savepc;

  TOKEN = TOKEN_SEMICOLON;

  if (err == L_ERR_BREAK && this->breakCount) {
    this->breakCount--;
    return L_ERR_BREAK;
  }

  return L_OK;
}

static int l_parse_init_foreach (l_t *this, char *aident, char *bident, char *cident) {
  int num = 0;
  int len = GETSTRLEN(TOKENSTR);
  SETSTRLEN(PARSEPTR, GETSTRLEN(PARSEPTR) + len);
  SETSTRPTR(PARSEPTR, GETSTRPTR(PARSEPTR) - len);

  NEXT_RAW_TOKEN();
  THROW_SYNTAX_ERR_IF(TOKEN != TOKEN_SYMBOL, "parsing for each: awaiting identifier");

  num++;
  len = GETSTRLEN(TOKENSTR);
  str_copy (aident, len + 1, GETSTRPTR(TOKENSTR), len);

  NEXT_RAW_TOKEN();
  if (TOKEN == TOKEN_COMMA)
    NEXT_RAW_TOKEN();
  else {
    THROW_SYNTAX_ERR_IF(GETSTRLEN(TOKENSTR) != 2 || (TOKENSTRBYTES[0] != 'i' && TOKENSTRBYTES[1] != 'n'),
      "parsing for each: awaiting in");
    return num;
  }

  THROW_SYNTAX_ERR_IF(TOKEN != TOKEN_SYMBOL, "parsing for each: awaiting identifier");

  num++;
  len = GETSTRLEN(TOKENSTR);
  str_copy (bident, len + 1, GETSTRPTR(TOKENSTR), len);

  NEXT_RAW_TOKEN();

  if (TOKEN == TOKEN_COMMA)
    NEXT_RAW_TOKEN();
  else {
    THROW_SYNTAX_ERR_IF(GETSTRLEN(TOKENSTR) != 2 || (TOKENSTRBYTES[0] != 'i' && TOKENSTRBYTES[1] != 'n'),
      "parsing for each: awaiting in");
    return num;
  }

  THROW_SYNTAX_ERR_IF(TOKEN != TOKEN_SYMBOL, "parsing for each: awaiting identifier");

  num++;
  len = GETSTRLEN(TOKENSTR);
  str_copy (cident, len + 1, GETSTRPTR(TOKENSTR), len);

  NEXT_RAW_TOKEN();
  THROW_SYNTAX_ERR_IF(GETSTRLEN(TOKENSTR) != 2 || (TOKENSTRBYTES[0] != 'i' && TOKENSTRBYTES[1] != 'n'),
    "parsing for each: awaiting in");

  return num;
}

static int l_parse_foreach (l_t *this) {
  int err = L_OK;

  char aident[MAXLEN_SYMBOL + 1];
  char bident[MAXLEN_SYMBOL + 1];
  char cident[MAXLEN_SYMBOL + 1];

  int num = l_parse_init_foreach (this, aident, bident, cident);
  THROW_ERR_IF_ERR(num);

  VALUE v;
  NEXT_TOKEN();

  l_string sym_str = TOKENSTR;
  int symlen = GETSTRLEN (TOKENSTR);

  err = l_parse_primary (this, &v);
  THROW_ERR_IF_ERR(err);

  int type = v.type;

  switch (type) {
    case MAP_TYPE:
    case ARRAY_TYPE:
    case STRING_TYPE:
    case LIST_TYPE:
      break;

    default:
      THROW_A_TYPE_MISMATCH(type, "awaiting a map or an array or a string or a list: illegal foreach type");
  }

  err = l_parse_block (this, "for each");
  THROW_ERR_IF_ERR(err);

  int len = GETSTRLEN(TOKENSTR);
  char body[len+1];
  str_copy (body, len + 1, GETSTRPTR(TOKENSTR), len);

  this->loopCount++;

  funT *fun = Fun_new (this, funNew (
    .name = NS_LOOP_BLOCK, .namelen = NS_LOOP_BLOCK_LEN, .parent = this->curScope
  ));

  fun->block_symbols = map_new (8);

  l_string savepc = PARSEPTR;
  funT *save_scope = this->curScope;
  this->curScope = fun;

  if (type == MAP_TYPE) {
    THROW_SYNTAX_ERR_IF(num != 2, "error parsing for each for maps: awaiting two identifiers");

    Map_Type *map = AS_MAP(v);

    int is_this = str_eq_n (GETSTRPTR(sym_str), "this", 4) && symlen == 4;

    char *key = aident;
    char *val = bident;

    v = STRING_NEW_WITH_LEN("", 0);
    sym_t *key_sym = l_define_block_symbol (this, fun, key, STRING_TYPE, v, 0);
    v = INT(0);
    sym_t *val_sym = l_define_block_symbol (this, fun, val, INTEGER_TYPE, v, 0);

    num = map->num_keys;
    char **keys = map_keys (map);

    l_string body_str = StringNew (body);

    for (int i = 0; i < num; i++) {
      VALUE *value = (VALUE *) map_get_value (map, keys[i]);
      if (value->sym->scope == NULL && 0 == is_this)
        continue;
      // if (str_eq (keys[i]->bytes, "__MAP_SCOPE__")) continue;

      string *v_k = AS_STRING(key_sym->value);
      string_replace_with (v_k, keys[i]);
      val_sym->value = *value;
      val_sym->type = value->type;

      this->curState |= LOOP_STATE;
      err = l_parse_string (this, body_str);
      this->curState &= ~LOOP_STATE;

      val_sym->value = NULL_VALUE;

      if (err == L_ERR_BREAK) {
        for (int j = 0; j < num; j++)
          Release (keys[j]);
        Release (keys);
        goto theend;
      }

      if (HASTORETURN) {
        this->curState &= ~LOOP_STATE;
        RESET_PARSEPTR;
        map_release (&fun->block_symbols);
        fun_release (&fun);
        for (int j = 0; j < num; j++)
          Release (keys[j]);
        Release (keys);
        return L_OK;
      }

      if (err == L_ERR_CONTINUE) {
        l_fun_release_symbols (fun, 1, 0);
        continue;
      }

      THROW_ERR_IF_ERR(err);

      l_fun_release_symbols (fun, 1, 0);
    }

    for (int j = 0; j < num; j++)
      Release (keys[j]);
    Release (keys);

    goto theend;
  }

  if (type == ARRAY_TYPE) {
    THROW_SYNTAX_ERR_IF(num != 2 && num != 1, "error parsing for each for arrays: awaiting one || two identifiers");

    ArrayType *array = (ArrayType *) AS_ARRAY(v);

    char *index = NULL;
    char *elem = NULL;

    if (num == 2) {
      index = aident;
      elem =  bident;
    } else {
      bident[0] = '_'; bident[1] = '\0';
      index = bident;
      elem = aident;
    }

    string **s_ar = NULL;
    integer *i_ar = NULL;
    number *n_ar = NULL;
    Map_Type **m_ar = NULL;
    ArrayType **a_ar = NULL;

    v = INT(-1);
    sym_t *index_sym = l_define_block_symbol (this, fun, index, INTEGER_TYPE, v, 0);

    sym_t *elem_sym = NULL;

    switch (array->type) {
      case INTEGER_TYPE:
        i_ar = (integer *) AS_ARRAY(array->value);
        v = INT(0);
        elem_sym = l_define_block_symbol (this, fun, elem, INTEGER_TYPE, v, 0);
        break;

      case NUMBER_TYPE:
        n_ar = (number *) AS_ARRAY(array->value);
        v = NUMBER(0);
        elem_sym = l_define_block_symbol (this, fun, elem, NUMBER_TYPE, v, 0);
        break;

      case STRING_TYPE:
        s_ar = (string **) AS_ARRAY(array->value);
        v = NULL_VALUE;
        elem_sym = l_define_block_symbol (this, fun, elem, STRING_TYPE, v, 0);
        break;

      case MAP_TYPE:
        m_ar = (Map_Type **) AS_ARRAY(array->value);
        v = NULL_VALUE;
        elem_sym = l_define_block_symbol (this, fun, elem, MAP_TYPE, v, 0);
        break;

      case ARRAY_TYPE:
        a_ar = (ArrayType **) AS_ARRAY(array->value);
        v = NULL_VALUE;
        elem_sym = l_define_block_symbol (this, fun, elem, ARRAY_TYPE, v, 0);
        break;
    }

    num = array->len;

    l_string body_str = StringNew (body);

    for (int i = 0; i < num; i++) {
      integer v_idx = AS_INT(index_sym->value);
      v_idx++;
      if (v_idx >= num) break;
      index_sym->value = INT(v_idx);

      switch (array->type) {
        case INTEGER_TYPE:
          elem_sym->value = INT(i_ar[v_idx]);
          break;

        case NUMBER_TYPE:
          elem_sym->value = NUMBER(n_ar[v_idx]);
          break;

        case STRING_TYPE:
          if (NULL == s_ar[v_idx])
            elem_sym->value = NULL_VALUE;
          else
            elem_sym->value = STRING(s_ar[v_idx]);
          elem_sym->value.refcount++;
          break;

        case MAP_TYPE:
          if (NULL == m_ar[v_idx])
            elem_sym->value = NULL_VALUE;
          else
            elem_sym->value = MAP(m_ar[v_idx]);
          elem_sym->value.refcount++;
          break;

        case ARRAY_TYPE:
          if (NULL == a_ar[v_idx])
            elem_sym->value = NULL_VALUE;
          else
            elem_sym->value = ARRAY(a_ar[v_idx]);
          elem_sym->value.refcount++;
          break;
      }

      this->curState |= LOOP_STATE;
      err = l_parse_string (this, body_str);
      this->curState &= ~LOOP_STATE;
      THROW_ERR_IF_ERR(err);

      elem_sym->value = NULL_VALUE;

      if (err == L_ERR_BREAK) goto theend;

      if (HASTORETURN) {
        this->curState &= ~LOOP_STATE;
        RESET_PARSEPTR;
        map_release (&fun->block_symbols);
        fun_release (&fun);
        return L_OK;
      }

      if (err == L_ERR_CONTINUE) {
        l_fun_release_symbols (fun, 1, 0);
        continue;
      }

      THROW_ERR_IF_ERR(err);

      l_fun_release_symbols (fun, 1, 0);
    }

    goto theend;
  }

  if (v.type == LIST_TYPE) {
    THROW_SYNTAX_ERR_IF(num != 2 && num != 1, "error parsing for each for lists: awaiting one || two identifiers");

    listType *list = AS_LIST(v);

    char *index = NULL;
    char *elem = NULL;

    if (num == 2) {
      index = aident;
      elem =  bident;
    } else {
      bident[0] = '_'; bident[1] = '\0';
      index = bident;
      elem = aident;
    }

    VALUE vidx = INT(-1);
    sym_t *index_sym = l_define_block_symbol (this, fun, index, INTEGER_TYPE, vidx, 0);

    VALUE elem_value = NULL_VALUE;

    sym_t *elem_sym = l_define_block_symbol (this, fun, elem, NULL_TYPE, elem_value, 0);
    num = list->num_items;

    l_string body_str = StringNew (body);

    for (int i = 0; i < num; i++) {
      integer v_idx = AS_INT(index_sym->value);
      v_idx++;
      if (v_idx >= num) break;
      index_sym->value = INT(v_idx);

      listNode *node = DListGetAt(list, listNode, v_idx);

      elem_value = *node->value;
      elem_sym->value = elem_value;
      elem_sym->type = elem_value.type;
      elem_value.refcount++;
      this->curState |= LOOP_STATE;
      err = l_parse_string (this, body_str);
      this->curState &= ~LOOP_STATE;
      THROW_ERR_IF_ERR(err);
      elem_value.refcount--;
      elem_sym->value = NULL_VALUE;

      if (err == L_ERR_BREAK) goto theend;

      if (HASTORETURN) {
        this->curState &= ~LOOP_STATE;
        RESET_PARSEPTR;
        map_release (&fun->block_symbols);
        fun_release (&fun);
        return L_OK;
      }

      if (err == L_ERR_CONTINUE) {
        l_fun_release_symbols (fun, 1, 0);
        continue;
      }

      THROW_ERR_IF_ERR(err);

      l_fun_release_symbols (fun, 1, 0);
    }

    goto theend;
  }

  THROW_SYNTAX_ERR_IF(num != 1 && num != 3, "error parsing for each for strings: awaiting one || three identifiers");

  string *str = AS_STRING(v);
  ifnot (str->num_bytes)
    goto theend;

  char *ci = aident;
  char *s  = NULL;
  char *w  = NULL;

  if (num == 3) {
    s  = bident;
    w  = cident;

    v = INT(0);
    sym_t *c_sym = l_define_block_symbol (this, fun, ci, INTEGER_TYPE, v, 0);
    v = STRING_NEW_WITH_LEN("", 0);
    sym_t *s_sym = l_define_block_symbol (this, fun, s, STRING_TYPE, v, 0);
    v = INT(0);
    sym_t *w_sym = l_define_block_symbol (this, fun, w, INTEGER_TYPE, v, 0);

    l_string body_str = StringNew (body);

    utf8_char u;
    char *sp = str->bytes;

    for (;;) {
      utf8_encode (&u, sp, 8);

      ifnot (u.len)
        goto theend;

      sp += u.len;

      c_sym->value = INT(u.code);
      string *v_s = AS_STRING(s_sym->value);
      string_replace_with (v_s, u.buf);
      w_sym->value = INT(u.width);

      this->curState |= LOOP_STATE;
      err = l_parse_string (this, body_str);
      this->curState &= ~LOOP_STATE;
      THROW_ERR_IF_ERR(err);

      if (err == L_ERR_BREAK)
        goto theend;

      if (HASTORETURN) {
        this->curState &= ~LOOP_STATE;
        RESET_PARSEPTR;
        map_release (&fun->block_symbols);
        fun_release (&fun);
        return L_OK;
      }

      if (err == L_ERR_CONTINUE) {
        l_fun_release_symbols (fun, 1, 0);
        continue;
      }

      THROW_ERR_IF_ERR(err);

      l_fun_release_symbols (fun, 1, 0);
    }

    goto theend;
  }

  v = INT(0);
  sym_t *c_sym = l_define_block_symbol (this, fun, ci, INTEGER_TYPE, v, 0);
  num = str->num_bytes;

  l_string body_str = StringNew (body);

  for (int i = 0; i < num; i++) {
    c_sym->value = INT(str->bytes[i]);

    this->curState |= LOOP_STATE;
    err = l_parse_string (this, body_str);
    this->curState &= ~LOOP_STATE;
    THROW_ERR_IF_ERR(err);

    if (err == L_ERR_BREAK) goto theend;

    if (HASTORETURN) {
      this->curState &= ~LOOP_STATE;
      RESET_PARSEPTR;
      map_release (&fun->block_symbols);
      fun_release (&fun);
      return L_OK;
    }

    if (err == L_ERR_CONTINUE) {
      l_fun_release_symbols (fun, 1, 0);
      continue;
    }

    THROW_ERR_IF_ERR(err);

    l_fun_release_symbols (fun, 1, 0);
  }

theend:
  this->curScope = save_scope;
  map_release (&fun->block_symbols);
  fun_release (&fun);

  this->loopCount--;
  ifnot (this->loopCount)
    this->curState &= ~LOOP_STATE;
  else
    this->curState |= LOOP_STATE;

  PARSEPTR = savepc;

  TOKEN = TOKEN_SEMICOLON;

  if (err == L_ERR_BREAK && this->breakCount) {
    this->breakCount--;
    return L_ERR_BREAK;
  }

  return L_OK;
}

static char *find_end_for_stmt (const char *str) {
  int in_str = 0;
  int op_paren = 0;
  int cl_paren = 0;

  char *ptr = (char *) str;
  char prev_c = *ptr;
  char str_tok = 0;

  while (cl_paren <= op_paren) {
    if (*ptr == TOKEN_EOF)
      return NULL;

    if (*ptr == TOKEN_PAREN_CLOS) {
      ifnot (in_str) cl_paren++;
      goto next;
    }

    if (*ptr == TOKEN_PAREN_OPEN) {
      ifnot (in_str) op_paren++;
      goto next;
    }

    if ((*ptr == TOKEN_DQUOTE || *ptr == TOKEN_BQUOTE)) {
      if (prev_c != TOKEN_ESCAPE_CHR) {
        if (in_str) {
          if (str_tok != *ptr) goto next;
           in_str--;
        } else {
          str_tok = *ptr;
          in_str++;
        }
      }
    }

   next: prev_c = *ptr; ptr++;
  }

  return ptr;
}

static int l_parse_loop_for_n_times (l_t *this, int n) {
  int err;
  this->loopCount++;

  funT *save_scope = this->curScope;

  funT *fun = Fun_new (this, funNew (
    .name = NS_LOOP_BLOCK, .namelen = NS_LOOP_BLOCK_LEN, .parent = this->curScope
  ));

  fun->block_symbols = map_new (8);

  err = l_parse_block (this, "loop for nth times");
  THROW_ERR_IF_ERR(err);

  int is_single = TOKEN != TOKEN_BLOCK;

  const char *tmp_ptr = GETSTRPTR(TOKENSTR);

  integer bodylen = GETSTRLEN(TOKENSTR);
  char body[bodylen + 1];
  for (integer i = 0; i < bodylen; i++)
    body[i] = tmp_ptr[i];
  body[bodylen] = '\0';

  l_string body_str = StringNew (body);

  l_string savepc = PARSEPTR;

  this->curScope = fun;

  for (integer i = 0; i < n; i++) {
    this->curState |= LOOP_STATE;
    if (is_single) {
      PARSEPTR = body_str;
      NEXT_TOKEN();
      err = l_parse_stmt (this);
      if (TOKEN > TOKEN_EOF && TOKEN != TOKEN_SEMICOLON) {
        l_print_current_line (this,
          "[WARNING]: extra tokens detected after the end of a loop statement\n",
          NULL);
      }

    } else {
      err = l_parse_string (this, body_str);
    }

    this->curState &= ~LOOP_STATE;
    THROW_ERR_IF_ERR(err);

    if (err == L_ERR_BREAK) goto theend;

    if (HASTORETURN) {
      this->curState &= ~LOOP_STATE;
      RESET_PARSEPTR;
      map_release (&fun->block_symbols);
      fun_release (&fun);
      return L_OK;
    }

    if (err == L_ERR_CONTINUE) {
      l_fun_release_symbols (fun, 1, 0);
      continue;
    }

    l_fun_release_symbols (fun, 1, 0);

    THROW_ERR_IF_ERR(err);
  }

theend:
  this->curScope = save_scope;
  map_release (&fun->block_symbols);
  fun_release (&fun);

  this->loopCount--;
  ifnot (this->loopCount)
    this->curState &= ~LOOP_STATE;
  else
    this->curState |= LOOP_STATE;

  PARSEPTR = savepc;

  TOKEN = TOKEN_SEMICOLON;

  if (err == L_ERR_BREAK && this->breakCount) {
    this->breakCount--;
    return L_ERR_BREAK;
  }

  return L_OK;
}

static int l_parse_for (l_t *this) {
  int err;
  int c;
  VALUE v;

  NEXT_TOKEN();

  switch (TOKEN) {
    case TOKEN_PAREN_OPEN:
      goto cfor;

    case TOKEN_INTEGER:
      err = l_parse_primary (this, &v);
      THROW_ERR_IF_ERR(err);

      NEXT_TOKEN();

      return l_parse_loop_for_n_times (this, AS_INT(v));
  }


  c = PEEK_NTH_TOKEN(0);

  if (c == TOKEN_TIMES) {
    v = TOKENVAL;
    NEXT_RAW_TOKEN();
    NEXT_TOKEN();
    return l_parse_loop_for_n_times (this, AS_INT(v));
  }

  if (c != TOKEN_COMMA && c != TOKEN_IN)
    THROW_SYNTAX_ERR("error while parsing for loop, awaiting ( or a comma or in or times[s]");

  return l_parse_foreach (this);

cfor:
  c = TOKEN;

  this->loopCount++;

  if (c != TOKEN_PAREN_OPEN)
    THROW_SYNTAX_ERR("error while parsing for loop, awaiting (");

  funT *save_scope = this->curScope;

  funT *fun = Fun_new (this, funNew (
    .name = NS_LOOP_BLOCK, .namelen = NS_LOOP_BLOCK_LEN, .parent = this->curScope
  ));
  fun->block_symbols = map_new (8);

  this->curScope = fun;

  NEXT_TOKEN();

  this->curState |= BLOCK_STATE;
  do {
    err = l_parse_stmt (this);
    THROW_ERR_IF_ERR(err);
  } while (TOKEN == TOKEN_COMMA);
  this->curState &= ~BLOCK_STATE;

  const char *tmp_ptr = GETSTRPTR(PARSEPTR);
  char *ptr = str_chr (tmp_ptr, TOKEN_SEMICOLON);
  if (NULL == ptr)
    THROW_SYNTAX_ERR("error while parsing for loop, awaiting ;");

  integer orig_len = GETSTRLEN(PARSEPTR);
  integer cond_len = ptr - tmp_ptr;
  integer advanced_len = cond_len;

  char cond[cond_len + 3];
  cond[0] = TOKEN_PAREN_OPEN;
  for (integer i = 0; i < cond_len; i++)
    cond[i + 1] = tmp_ptr[i];
  cond[cond_len + 1] = TOKEN_PAREN_CLOS;
  cond[cond_len + 2] = '\0';

  l_string cond_str = StringNewLen (cond, cond_len + 2);

  advanced_len++;

  tmp_ptr = ptr + 1;
  ptr = find_end_for_stmt (tmp_ptr);
  if (NULL == ptr)
    THROW_SYNTAX_ERR("error while parsing for loop, awaiting )");

  integer stmt_len = ptr - tmp_ptr;
  advanced_len += stmt_len;

  char stmt[stmt_len + 2];
  for (integer i = 0; i < stmt_len; i++)
    stmt[i] = tmp_ptr[i];
  stmt[stmt_len] = TOKEN_SEMICOLON;
  stmt[stmt_len + 1] = '\0';

  l_string stmt_str = StringNewLen (stmt, stmt_len + 1);

  advanced_len++;

  SETSTRPTR(PARSEPTR, ptr + 1);
  SETSTRLEN(PARSEPTR, orig_len - advanced_len);

  if (*ptr == TOKEN_NL) UNGET_BYTE();

  NEXT_TOKEN();

  err = l_parse_block (this, "for");
  THROW_ERR_IF_ERR(err);

  int is_single = TOKEN != TOKEN_BLOCK;

  tmp_ptr = GETSTRPTR(TOKENSTR);

  integer bodylen = GETSTRLEN(TOKENSTR);
  char body[bodylen + 1];
  for (integer i = 0; i < bodylen; i++)
    body[i] = tmp_ptr[i];
  body[bodylen] = '\0';

  l_string body_str = StringNewLen (body, bodylen);

  l_string savepc = PARSEPTR;

  for (;;) {
    PARSEPTR = cond_str;

    TOKEN = TOKEN_PAREN_OPEN;
    this->curState |= BLOCK_STATE;
    err = l_parse_expr (this, &v);
    this->curState &= ~BLOCK_STATE;
    THROW_ERR_IF_ERR(err);

    ifnot (AS_INT(v)) goto theend;

    this->curState |= LOOP_STATE;
    if (is_single) {
      PARSEPTR = body_str;
      NEXT_TOKEN();
      err = l_parse_stmt (this);
      if (TOKEN > TOKEN_EOF && TOKEN != TOKEN_SEMICOLON) {
        l_print_current_line (this,
          "[WARNING]: extra tokens detected after the end of a for statement\n",
          NULL);
      }

    } else {
      err = l_parse_string (this, body_str);
    }

    this->curState &= ~LOOP_STATE;
    THROW_ERR_IF_ERR(err);

    if (err == L_ERR_BREAK) goto theend;

    if (HASTORETURN) {
      this->curState &= ~LOOP_STATE;
      TOKEN = TOKEN_SEMICOLON;
      RESET_PARSEPTR;
      map_release (&fun->block_symbols);
      fun_release (&fun);
      return L_OK;
    }

    if (err == L_ERR_CONTINUE)
      goto ctl_stmt;

    THROW_ERR_IF_ERR(err);

    ctl_stmt:
      PARSEPTR = stmt_str;
      this->curState |= BLOCK_STATE;
      do {
        NEXT_TOKEN();
        err = l_parse_stmt (this);
        THROW_ERR_IF_ERR(err);
      } while (TOKEN == TOKEN_COMMA);
      this->curState &= ~BLOCK_STATE;

      l_fun_release_symbols (fun, 1, 0);
  }

theend:
  this->curScope = save_scope;
  map_release (&fun->block_symbols);
  fun_release (&fun);

  this->loopCount--;
  ifnot (this->loopCount)
    this->curState &= ~LOOP_STATE;
  else
    this->curState |= LOOP_STATE;

  PARSEPTR = savepc;

  TOKEN = TOKEN_SEMICOLON;

  if (err == L_ERR_BREAK && this->breakCount) {
    this->breakCount--;
    return L_ERR_BREAK;
  }

  return L_OK;
}

static int l_parse_forever (l_t *this) {
  int err;
  this->loopCount++;

  funT *save_scope = this->curScope;

  funT *fun = Fun_new (this, funNew (
    .name = NS_LOOP_BLOCK, .namelen = NS_LOOP_BLOCK_LEN, .parent = this->curScope
  ));

  fun->block_symbols = map_new (8);

  this->curScope = fun;

  NEXT_TOKEN();

  err = l_parse_block (this, "forever");
  THROW_ERR_IF_ERR(err);

  int is_single = TOKEN != TOKEN_BLOCK;

  const char *tmp_ptr = GETSTRPTR(TOKENSTR);

  integer bodylen = GETSTRLEN(TOKENSTR);
  char body[bodylen + 1];
  for (integer i = 0; i < bodylen; i++)
    body[i] = tmp_ptr[i];
  body[bodylen] = '\0';

  l_string body_str = StringNewLen (body, bodylen);

  l_string savepc = PARSEPTR;

  for (;;) {
    this->curState |= LOOP_STATE;
    if (is_single) {
      PARSEPTR = body_str;
      NEXT_TOKEN();
      err = l_parse_stmt (this);
      if (TOKEN > TOKEN_EOF && TOKEN != TOKEN_SEMICOLON) {
        l_print_current_line (this,
          "[WARNING]: extra tokens detected after the end of a forever statement\n",
          NULL);
      }

    } else {
      err = l_parse_string (this, body_str);
    }

    this->curState &= ~LOOP_STATE;
    THROW_ERR_IF_ERR(err);

    if (err == L_ERR_BREAK) goto theend;

    if (HASTORETURN) {
      this->curState &= ~LOOP_STATE;
      RESET_PARSEPTR;
      map_release (&fun->block_symbols);
      fun_release (&fun);
      return L_OK;
    }

    if (err == L_ERR_CONTINUE) {
      l_fun_release_symbols (fun, 1, 0);
      continue;
    }

    l_fun_release_symbols (fun, 1, 0);

    THROW_ERR_IF_ERR(err);
  }

theend:
  this->curScope = save_scope;
  map_release (&fun->block_symbols);
  fun_release (&fun);

  this->loopCount--;
  ifnot (this->loopCount)
    this->curState &= ~LOOP_STATE;
  else
    this->curState |= LOOP_STATE;

  PARSEPTR = savepc;
  TOKEN = TOKEN_SEMICOLON;

  if (err == L_ERR_BREAK && this->breakCount) {
    this->breakCount--;
    return L_ERR_BREAK;
  }

  return L_OK;
}

static int l_parse_visibility (l_t *this) {
  this->scopeState = 0;
  int token = str_eq_n (GETSTRPTR(TOKENSTR), "public", 6)
      ? TOKEN_PUBLIC : TOKEN_PRIVATE;
  int scope = (token == TOKEN_PUBLIC ? PUBLIC_SCOPE : PRIVATE_SCOPE);

  NEXT_TOKEN();
  int c = TOKEN;

  switch (c) {
    case TOKEN_ARYDEF:
    case TOKEN_VARDEF:
    case TOKEN_CONSTDEF:
    case TOKEN_FUNCDEF:
      this->scopeState = scope;
      break;

    default:
      if (this->curState & MAP_STATE) {
        this->scopeState = scope;
        break;
      }

      if (c == TOKEN_EOF)
        THROW_SYNTAX_ERR("unended statement");
      THROW_SYNTAX_ERR("unsupported visibility/scope attribute");
  }

  return token;
}

static int l_parse_arg_list (l_t *this, funT *uf) {
  int nargs = 0;

  NEXT_RAW_TOKEN();

  for (;;) {
    if (TOKEN == TOKEN_PAREN_CLOS) break;

    if (nargs == MAX_BUILTIN_PARAMS) THROW_TOOMANY_FUNCTION_ARGS();

    THROW_SYNTAX_ERR_IF(TOKEN != TOKEN_SYMBOL, "argument list definition, awaiting a symbol name");

    size_t len = GETSTRLEN(TOKENSTR);

    if (len > MAXLEN_SYMBOL)
      THROW_SYNTAX_ERR_FMT("%s: exceeds maximum length (%d) of an identifier",
         cur_msg_str (this, TOKENSTR), MAXLEN_SYMBOL);

    str_copy (uf->argName[nargs], MAXLEN_SYMBOL + 1, GETSTRPTR(TOKENSTR), len);

    THROW_SYNTAX_ERR_IF(str_eq (uf->argName[nargs], "this"), "declaring this in an argument list is not allowed");

    nargs++;

    NEXT_RAW_TOKEN();

    if (TOKEN == TOKEN_COMMA) {
      NEXT_RAW_TOKEN();
      if (TOKEN == TOKEN_NL) NEXT_RAW_TOKEN();
      THROW_SYNTAX_ERR_IF(TOKEN == TOKEN_NL || TOKEN == TOKEN_COMMA,
          "awaiting a symbol, while parsing argument list");
    }
  }

  uf->nargs = nargs;
  return nargs;
}

static int l_parse_func_def (l_t *this) {
  l_string name;
  int nargs = 0;
  size_t len = 0;

  char *fn = NULL;

  ifnot (this->curFunName[0]) {
    NEXT_RAW_TOKEN();

    THROW_SYNTAX_ERR_IF(TOKEN != TOKEN_SYMBOL,  "function definition, not a symbol");

    name = TOKENSTR;
    len = GETSTRLEN(name);
    fn = sym_key (this, name);
    THROW_SYNTAX_ERR_IF_STD_SYM_EXISTS(fn);

  } else {
    name = StringNew (this->curFunName);
    len = bytelen (this->curFunName);
    THROW_SYNTAX_ERR_IF_SYM_NAME_EXCEEDS_LEN(name, len);
    fn = this->curFunName;
  }

  sym_t *sym = l_lookup_symbol (this, name);
  ifnot (NULL == sym)
    THROW_SYNTAX_ERR_IF(sym->is_const, "can not redefine a constant function");

  funT *uf = Fun_new (this, funNew (
    .name = fn, .namelen = len, .parent = this->curScope
  ));

  NEXT_TOKEN();

  if (TOKEN == TOKEN_PAREN_OPEN) {
    l_fun_stack_push (this, this->curScope);

    this->curScope = uf;

    nargs = l_parse_arg_list (this, uf);

    this->curScope = l_fun_stack_pop (this);

    if (nargs < 0) return nargs;

    NEXT_TOKEN();
  }

  int err = l_parse_block (this, uf->funname);
  THROW_ERR_IF_ERR(err);

  uf->body = TOKENSTR;

  VALUE v = PTR(uf);

  funT *scope = (this->scopeState == PUBLIC_SCOPE ? this->function : this->curScope);
  this->curSym = l_define_symbol (this, scope, uf->funname, (UFUNC_TYPE | (nargs << 8)), v, 0);
  this->scopeState = 0;

  this->curFunDef = uf;

  NEXT_TOKEN();
  return L_OK;
}

static int l_parse_fmt (l_t *this, string *str, int break_at_eof) {
  int err = L_NOTOK;
  VALUE value;
  char directive = 0;

  this->fmtRefcount++;

  int pc = 0;
  int c;

  for (;;) {
    get_byte:
    c = GET_BYTE();

    if (c == TOKEN_EOF && break_at_eof)
      break;

    if (c == TOKEN_DQUOTE) {
      if (pc != TOKEN_ESCAPE_CHR)
        break;

      string_append_byte (str, '\\');
      string_append_byte (str, TOKEN_DQUOTE);
      pc = TOKEN_DQUOTE;
      c = GET_BYTE();
    }

    if (c == TOKEN_ESCAPE_CHR) {
      if (pc == TOKEN_ESCAPE_CHR) {
        pc = str->bytes[str->num_bytes - 1];
        string_append_byte (str, TOKEN_ESCAPE_CHR);
      } else {
        int cc = PEEK_NTH_BYTE(0);
        switch (cc) {
          case 'a' : string_append_byte (str, '\a'); break;
          case 'b' : string_append_byte (str, '\b'); break;
          case 'f' : string_append_byte (str, '\f'); break;
          case 't' : string_append_byte (str, '\t'); break;
          case 'r' : string_append_byte (str, '\r'); break;
          case 'n' : string_append_byte (str, '\n'); break;
          case 'v' : string_append_byte (str, '\v'); break;
          case 'e' : string_append_byte (str,  033); break;
          case '$' :
            if (PEEK_NTH_BYTE(1) == TOKEN_BLOCK_OPEN) {
              string_append_byte (str, '$');
              break;
            }
            // fall through

          default:
            string_append_byte (str, TOKEN_ESCAPE_CHR);
            string_append_byte (str, cc);
        }

        GET_BYTE(); pc = 0;
        goto get_byte;
      }

      c = GET_BYTE();
    }

    if (c == '$' && PEEK_NTH_BYTE(0) == TOKEN_BLOCK_OPEN) {
      c = GET_BYTE();

      pc = c;
      NEXT_TOKEN();
      c = TOKEN;

      if (*(GETSTRPTR(TOKENSTR)) == '%' &&
            GETSTRLEN(TOKENSTR) == 1) {
        c = GET_BYTE();
        if (c != 's' && c != 'p' && c != 'd' &&
            c != 'o' && c != 'x' && c != 'f' && c != 'b') {
          this->print_fmt_bytes (this->err_fp, "string fmt error, unsupported directive [%c]\n", c);
          l_err_ptr (this, L_NOTOK);
          err = L_ERR_SYNTAX;
          goto theend;
        } else
          directive = c;

        NEXT_TOKEN();
        c = TOKEN;

        if (c != TOKEN_COMMA) {
          this->print_fmt_bytes (this->err_fp, "string fmt error, awaiting a comma\n");
          l_err_ptr (this, L_NOTOK);
          err = L_ERR_SYNTAX;
          goto theend;
        }

        pc = TOKEN_COMMA;
        NEXT_TOKEN();
        c = TOKEN;
      }

      this->exprList++;
      err = l_parse_expr (this, &value);
      this->exprList--;
      if (err != L_OK) goto theend;

      if (TOKEN != TOKEN_BLOCK_CLOS) {
        this->print_fmt_bytes (this->err_fp, "string fmt error, awaiting '}'\n");
        l_err_ptr (this, L_NOTOK);
        err = L_ERR_SYNTAX;
        goto theend;
      }

      ifnot (directive) {
        switch (value.type) {
          case STRING_TYPE:  directive = 's'; break;
          case INTEGER_TYPE: directive = 'd'; break;
          case NUMBER_TYPE:  directive = 'f'; break;
          case NULL_TYPE:    directive = 's'; break;
          default: directive = 'p';
        }
      }

      switch (directive) {
        case 's':
          switch (value.type) {
            case STRING_TYPE:
              string_append_with_fmt (str, "%s", AS_STRING_BYTES(value));
              if ((this->fmtState & FMT_LITERAL) || (value.sym == NULL &&
                  value.refcount != MALLOCED_STRING &&
                  0 == (this->objectState & (MAP_MEMBER|ARRAY_MEMBER)))) {
                l_release_value (this, value);
              }

              this->fmtState &= ~FMT_LITERAL;
              this->objectState &= ~(ARRAY_MEMBER|MAP_MEMBER);
              break;

            case NULL_TYPE:
              string_append_with_len (str, "(null)", 6);
              break;

            case ARRAY_TYPE: {
              VALUE v;
              TOKENVAL = value;
              err = l_parse_array_get (this, &v);
              if (err != L_OK) {
                this->print_bytes (this->err_fp, "string fmt error, awaiting an array\n");
                err = L_ERR_SYNTAX;
                goto theend;
              }

              switch (v.type) {
                case STRING_TYPE:
                  string_append_with_fmt (str, "%s", AS_STRING_BYTES(v));
                  break;

                default:
                  this->print_bytes (this->err_fp, "string fmt error, awaiting a string\n");
                  err = L_ERR_SYNTAX;
                  goto theend;

              }
              break;
            }

            default:
              this->print_bytes (this->err_fp, "string fmt error, awaiting a string\n");
              err = L_ERR_SYNTAX;
              goto theend;
          }

          break;

        case 'p':
          string_append_with_fmt (str, "%p", AS_PTR(value));
          if (value.type == STRING_TYPE) {
            if ((this->fmtState & FMT_LITERAL) || (value.sym == NULL &&
                value.refcount != MALLOCED_STRING &&
               0 == (this->objectState & (ARRAY_MEMBER|MAP_MEMBER)))) {
                l_release_value (this, value);
            }
            this->fmtState &= ~FMT_LITERAL;
            this->objectState &= ~(ARRAY_MEMBER|MAP_MEMBER);
          }
          break;

        case 'b':
          switch (value.type) {
            case INTEGER_TYPE:
              string_append_with_fmt (str, "0b%b", AS_INT(value));
              break;

            default:
              this->print_bytes (this->err_fp, "string fmt error, awaiting an integer\n");
              err = L_ERR_SYNTAX;
              goto theend;
           }
           break;

        case 'o':
          switch (value.type) {
            case INTEGER_TYPE:
              string_append_with_fmt (str, "0%o", AS_INT(value));
              break;

            default:
              this->print_bytes (this->err_fp, "string fmt error, awaiting an integer\n");
              err = L_ERR_SYNTAX;
              goto theend;
          }

          break;

        case 'x':
          switch (value.type) {
            case INTEGER_TYPE:
              string_append_with_fmt (str, "0x%x", AS_INT(value));
              break;

            default:
              this->print_bytes (this->err_fp, "string fmt error, awaiting an integer\n");
              err = L_ERR_SYNTAX;
              goto theend;
           }
           break;

        case 'f':
          switch (value.type) {
            case ARRAY_TYPE: {
              VALUE v;
              TOKENVAL = value;
              err = l_parse_array_get (this, &v);
              if (err != L_OK) {
                this->print_bytes (this->err_fp, "string fmt error, awaiting an array\n");
                err = L_ERR_SYNTAX;
                goto theend;
              }

              switch (v.type) {
                case NUMBER_TYPE:
                  string_append_with_fmt (str, "%.15f", AS_NUMBER(v));
                  break;

                default:
                  this->print_bytes (this->err_fp, "string fmt error, awaiting a number\n");
                  err = L_ERR_SYNTAX;
                  goto theend;
              }
              break;
            }

          case NUMBER_TYPE:
            string_append_with_fmt (str, "%.15f", AS_NUMBER(value));
            break;

            default:
              this->print_bytes (this->err_fp, "string fmt error, awaiting a number\n");
              err = L_ERR_SYNTAX;
              goto theend;
          }

          break;

        case 'd':
        default:
          switch (value.type) {
            case ARRAY_TYPE: {
              VALUE v;
              TOKENVAL = value;
              err = l_parse_array_get (this, &v);
              if (err != L_OK) {
                this->print_bytes (this->err_fp, "string fmt error, awaiting array\n");
                err = L_ERR_SYNTAX;
                goto theend;
              }

              switch (v.type) {
                case INTEGER_TYPE:
                  string_append_with_fmt (str, "%" PRId64, AS_INT(v));
                  break;

                default:
                  this->print_bytes (this->err_fp, "string fmt error, awaiting an integer\n");
                  err = L_ERR_SYNTAX;
                  goto theend;

              }
              break;
            }

            case STRING_TYPE: {
              VALUE v;
              TOKENVAL = value;
              err = l_parse_string_get (this, &v);
              if (err != L_OK) {
                this->print_bytes (this->err_fp, "string fmt error, awaiting a string\n");
                err = L_ERR_SYNTAX;
                goto theend;
              }

              switch (v.type) {
                case INTEGER_TYPE:
                  string_append_with_fmt (str, "%" PRId64, AS_INT(v));
                  break;

                default:
                  this->print_bytes (this->err_fp, "string fmt error, awaiting an integer\n");
                  err = L_ERR_SYNTAX;
                  goto theend;
              }
              break;
            }

          case INTEGER_TYPE:
            string_append_with_fmt (str, "%" PRId64, AS_INT(value));
            break;

          default:
            this->print_bytes (this->err_fp, "string fmt error, awaiting an integer\n");
            err = L_ERR_SYNTAX;
            goto theend;
        }
      }

      directive = 0;
      continue;
    }

    string_append_byte (str, c);
  }

  err = L_OK;

theend:
  this->fmtRefcount--;
  return err;
}

static int l_parse_print (l_t *this) {
  int err = L_NOTOK;

  this->byteCount = 0;

  string *str = string_new (32);

  int c = l_ignore_ws (this);

  if (c != TOKEN_PAREN_OPEN) {
    this->print_bytes (this->err_fp, "print string error, awaiting (\n");
    l_err_ptr (this, L_NOTOK);
    goto theend;
  }

  c = l_ignore_ws (this);

  FILE *fp = this->out_fp;
  int is_add_operation = 0;
  int num_iterations = 0;

  VALUE v;

  if (c != TOKEN_DQUOTE) {
    UNGET_BYTE();

    NEXT_TOKEN();
    c = TOKEN;

    this->curState |= MALLOCED_STRING_STATE;
    this->fmtRefcount++;
    err = l_parse_expr (this, &v);
    this->curState &= ~MALLOCED_STRING_STATE;
    this->fmtRefcount--;
    THROW_ERR_IF_ERR(err);

    check_expr:
    if (err != L_OK) goto theend;
    num_iterations++;

    c = TOKEN;

    switch (v.type) {
      case STRING_TYPE: {
          string *vs = AS_STRING(v);

          ifnot (is_add_operation)
            string_append_with_len (str, vs->bytes, vs->num_bytes);
          else
            string_replace_with_len (str, vs->bytes, vs->num_bytes);

          ifnot (this->objectState & RHS_STRING_RELEASED)
            if (v.sym == NULL &&
                v.refcount != MALLOCED_STRING &&
                0 == (this->objectState & (ARRAY_MEMBER|MAP_MEMBER)))
              l_release_value (this, v);

          this->objectState &= ~(ARRAY_MEMBER|MAP_MEMBER);
        }

        goto print_str;

      case INTEGER_TYPE:
        string_append_with_fmt (str, "%" PRId64, AS_INT(v));
        goto print_str;

      case NUMBER_TYPE:
        string_append_with_fmt (str, "%f", AS_NUMBER(v));
        goto print_str;

      case NULL_TYPE:
        string_append_with_len (str, "(null)", 6);
        goto print_str;

      case FILEPTR_TYPE:
        if (1 == num_iterations)
          break;

      // fall through
      default:
        string_append_with_fmt (str, "%p", AS_PTR(v));
        goto print_str;
    }

    fp = AS_FILEPTR(v);
    IGNORE_NEXT_BYTE;
    c = l_ignore_ws (this);

    if (c != TOKEN_DQUOTE) {
      UNGET_BYTE();

      NEXT_TOKEN();
      c = TOKEN;

      err = l_parse_expr (this, &v);
      THROW_ERR_IF_ERR(err);
      goto check_expr;
    }
  }

  if (c != TOKEN_DQUOTE) {
    this->print_bytes (this->err_fp, "print string error, awaiting double quote\n");
    l_err_ptr (this, L_NOTOK);
    goto theend;
  }

  err = l_parse_fmt (this, str, 0);
  THROW_ERR_IF_ERR(err);

  NEXT_TOKEN();
  c = TOKEN;

print_str:
  this->print_fp = fp;

  if (c != TOKEN_PAREN_CLOS) {
    if ((c & 0xff) == TOKEN_BINOP) {
      if (AS_VOID_PTR(TOKENVAL) == l_add) {
        is_add_operation = 1;
        TOKEN = c;
        v = STRING(str);
        if (v.refcount > -1)
          v.refcount++;
        this->curState |= MALLOCED_STRING_STATE;
        err = l_parse_expr_level (this, MAX_EXPR_LEVEL, &v);
        THROW_ERR_IF_ERR(err);

        if (v.refcount > 0)
          v.refcount--;
        this->curState &= ~MALLOCED_STRING_STATE;
        goto check_expr;
      }
    }

    this->print_fmt_bytes (this->err_fp, "string fmt error, awaiting ) found %c\n", c);
    l_err_ptr (this, L_NOTOK);
    err = L_ERR_SYNTAX;
    goto theend;
  }

  int num_bts = this->print_bytes (fp, str->bytes);
  if (num_bts < 0) {
    this->print_bytes (this->err_fp, "error while printing string\n");
    this->print_fmt_bytes (this->err_fp, "%s\n", str->bytes);
    this->print_fmt_bytes (this->err_fp, "%s|n", errno_string (sys_errno));
    goto theend;
  }

  NEXT_TOKEN();

  this->byteCount = num_bts;
  err = L_OK;

theend:
  string_release (str);
  return err;
}

static int l_parse_println (l_t *this) {
  int err = l_parse_print (this);
  if (err > L_NOTOK) {
    sys_fprintf (this->print_fp, "\n");
    this->byteCount++;
  }

  return err;
}

static int l_parse_exit (l_t *this) {
  NEXT_TOKEN();

  VALUE v;
  l_parse_expr (this, &v);
  this->exitValue = AS_INT(v);

  RESET_PARSEPTR;
  HASTORETURN = 1;

  return L_ERR_EXIT;
}

static int l_parse_return (l_t *this) {
  int err = L_OK, type;
  funT *scope = this->curScope;

  NEXT_TOKEN();

  switch (TOKEN) {
    case TOKEN_SEMICOLON:
    case       TOKEN_EOF:
    case        TOKEN_NL:
      HASTORETURN = 1;
      RESET_PARSEPTR;
      return L_OK;
  }

  while (scope && str_eq_n (scope->funname, "__block_", 8))
    scope = scope->prev;

  if (NULL == scope)
    THROW_SYNTAX_ERR("error while parsing return, unknown scope");

  this->curState |= MALLOCED_STRING_STATE;
  int token = PEEK_NTH_TOKEN(0);
  this->curState &= ~MALLOCED_STRING_STATE;

  if (token == TOKEN_IF || token == TOKEN_IFNOT ) {
    tokenState save = SAVE_TOKENSTATE();

    NEXT_TOKEN();

    if (IS_NEXT_BYTE_NOWS(TOKEN_NL))
      NEXT_TOKEN();

    err = l_parse_cond (this, token == TOKEN_IFNOT);
    THROW_ERR_IF_ERR(err);

    ifnot (AS_INT(CONDVAL)) return L_OK;

    tokenState save_again = SAVE_TOKENSTATE();

    RESTORE_TOKENSTATE(save);
    SETSTRLEN(PARSEPTR, GETSTRLEN(TOKENSTR));

    err = l_parse_primary (this, &scope->result);
    THROW_ERR_IF_ERR(err);

    RESTORE_TOKENSTATE(save_again);

    HASTORETURN = 1;
    goto check_value;
  }

  if (TOKEN == TOKEN_USRFUNC)
    if (str_eq ("self", sym_key (this, TOKENSTR)))
      return L_ERR_TCALLREC;

  HASTORETURN = 1;

  this->funcState |= RETURN_STATE;
  err = l_parse_expr (this, &scope->result);
  this->funcState &= ~RETURN_STATE;
  THROW_ERR_IF_ERR(err);

  ifnot (HASTORETURN) return L_OK;

  check_value:
  type = scope->result.type;

  switch (type) {
    case ARRAY_TYPE:
    case STRING_TYPE:
      if (this->objectState & MAP_MEMBER) {
        scope->result.refcount++;
        if (scope->result.sym != NULL) {
          ifnot (scope->result.sym->type)
            scope->result.sym->type = MAP_TYPE;
        }

        this->objectState &= ~MAP_MEMBER;
        break;
      }
  }

  if (TOKEN != TOKEN_SEMICOLON &&
      TOKEN != TOKEN_NL &&
      TOKEN != TOKEN_EOF)
    if (PREVTOKEN != TOKEN_EOF ||
        PREVTOKEN != TOKEN_NL ||
        PREVTOKEN != TOKEN_SEMICOLON)
      THROW_SYNTAX_ERR("awaiting a semicolon or a new line after return");

  RESET_PARSEPTR;
  return err;
}

static int l_import_file (l_t *this, const char *module, const char *err_msg) {
  (void) this;
  (void) module;
  (void) err_msg;
  return L_OK;
#if 0
  int err = L_NOTOK;

  char *mname = path_basename_sans_extname ((char *) module);

  module_so *it = this->function->modules->head;
  while (it) {
    if (str_eq (it->name, mname)) {
      Release (mname);
      return L_OK;
    }

    it = it->next;
  }

  void *handle = sys_dlopen (module, RTLD_NOW|RTLD_GLOBAL);

  if (handle == NULL) {
    err_msg = sys_dlerror ();
    l_set_message_fmt (this, 1, "%s, %s\n", module, err_msg);

    ifnot (this->curState & LOADFILE_SILENT)
      this->print_fmt_bytes (this->err_fp, "import error, %s\n", err_msg);

    Release (mname);
    return L_ERR_IMPORT;
  }

  size_t len = bytelen (mname) - 7;
  char tmp[len + 1];
  str_get_substr_at (tmp, len, mname, len + 7, 0);

  string *init_fun = string_new_with_fmt ("__init_%s_module__", tmp);
  string *deinit_fun = string_new_with_fmt ("__deinit_%s_module__", tmp);

  sys_dlerror ();
  ModuleInit init_sym = (ModuleInit) sys_dlsym (handle, init_fun->bytes);
  err_msg = sys_dlerror ();
  if (err_msg != NULL) {
    l_set_message_fmt (this, 1, "%s, while getting %s symbol symbol address %s\n", module, init_fun->bytes, err_msg);
    this->print_fmt_bytes (this->err_fp, "import error while getting %s symbol symbol address, %s\n", init_fun->bytes, err_msg);
    err = L_ERR_DYNLINK;
    goto theend;
  }

  sys_dlerror ();
  ModuleDeinit deinit_sym = (ModuleDeinit) sys_dlsym (handle, deinit_fun->bytes);
  err_msg = sys_dlerror ();

  if (err_msg != NULL) {
    l_set_message_fmt (this, 1, "%s, while getting %s symbol symbol address %s\n", module, deinit_fun->bytes, err_msg);
    this->print_fmt_bytes (this->err_fp, "import error while getting %s symbol symbol address, %s\n", deinit_fun->bytes, err_msg);
    err = L_ERR_DYNLINK;
    goto theend;
  }

  int retval = init_sym (this);

  if (retval != L_OK) {
    sys_dlclose (handle);
    err = L_ERR_DYNLINK;
    goto theend;
  }

  module_so *modl = Alloc (sizeof (module_so));
  modl->name = mname;
  modl->handle = handle;
  modl->init = init_sym;
  modl->deinit = deinit_sym;
  modl->instance = this;
  ListStackPush(this->function->modules, modl);
  err = L_OK;

theend:
  ifnot (err == L_OK)
    Release (mname);

  string_release (init_fun);
  string_release (deinit_fun);
  return err;
#endif
}

static int l_parse_import (l_t *this) {
  int err;
  const char *err_msg = "";

  NEXT_TOKEN();
  int c = TOKEN;

  ifnot (c == TOKEN_PAREN_OPEN)
    THROW_SYNTAX_ERR("error while parsing import(), awaiting (");

  VALUE v;
  err = l_parse_expr (this, &v);
  THROW_SYNTAX_ERR_IF_ERR(err, "error while parsing import()");

  THROW_SYNTAX_ERR_IF (v.type != STRING_TYPE,
    "error while parsing import(), awaiting a string");

  string *fname = AS_STRING(v);
  string *fn = NULL;

  if (this->curState & LITERAL_STRING_STATE)
    this->curState &= ~LITERAL_STRING_STATE;
  else
    fname = string_dup (fname);

#ifdef STATIC
  char *mname = path_basename (fname->bytes);

  if ('a' <= *mname && *mname <= 'z') {
    utf8 chr = utf8_to_upper (*mname);
    *mname = chr;
  }

  err = L_OK;

  if (NULL == ns_lookup_symbol (this->std, mname))
    err =  this->syntax_error_fmt (this, "%s module hasn't been initialized", mname);

  Release (fname);
  return err;
#endif

  string_append_with (fname, "-module.so");

  string *ns = NULL;
  funT *load_ns = this->curScope;
  funT *prev_ns = load_ns;

  if (TOKEN == TOKEN_COMMA) {
    NEXT_TOKEN();
    err = l_parse_expr (this, &v);
    THROW_ERR_IF_ERR(err);

    ifnot (v.type == STRING_TYPE)
      THROW_SYNTAX_ERR("error while parsing import() ns, awaiting a string");

    ns = AS_STRING(v);

    if (this->curState & LITERAL_STRING_STATE)
      this->curState &= ~LITERAL_STRING_STATE;
    else
      ns = string_dup (ns);

    ifnot (ns->num_bytes)
      goto theload;

    l_string x = StringNew (ns->bytes);
    sym_t *symbol = l_lookup_symbol (this, x);
    if (symbol != NULL) {
      v = symbol->value;
      if (v.type & UFUNCTION_TYPE) {
        funT *f = AS_FUNC_PTR(v);
        load_ns = f;
        goto theload;
      }

      this->print_fmt_bytes (this->err_fp, "import(): %s is not a namespace\n", ns->bytes);
      err = L_ERR_SYNTAX;
      goto theend;
    }

    this->print_bytes (this->err_fp, "import(), functionality hasn't been implemented\n");
    err = L_ERR_SYNTAX;
    goto theend;
  }

theload:
  this->curScope = load_ns;

   l_set_message (this, 0, "import: ");

  ifnot (path_is_absolute (fname->bytes)) {
    ifnot (str_eq (L_STRING_NS, AS_STRING_BYTES(this->file->value))) {
      fn = string_dup (fname);
      string_prepend_byte (fn, DIR_SEP);
      char *dname = path_dirname (AS_STRING_BYTES(this->file->value));
      string_prepend_with (fn, dname);
      Release (dname);
      this->curState |= LOADFILE_SILENT;
      err = l_import_file (this, fn->bytes, err_msg);
      this->curState &= ~LOADFILE_SILENT;
      if (err == L_OK || err == L_ERR_DYNLINK)
        goto theend;
    }

    this->curState |= LOADFILE_SILENT;
    err = l_import_file (this, fname->bytes, err_msg);
    this->curState &= ~LOADFILE_SILENT;

    if (err == L_OK || err == L_ERR_DYNLINK)
      goto theend;

    sym_t *symbol = l_lookup_symbol (this, StringNew ("__importpath"));
    ArrayType *p_ar = (ArrayType *) AS_ARRAY(symbol->value);
    string **s_ar = (string **) AS_ARRAY(p_ar->value);
    for (int i = 0; i < p_ar->len; i++) {
      string *p = s_ar[i];
      ifnot (p->num_bytes) continue;

      string_release (fn);
      fn = string_dup (fname);
      string_prepend_with_fmt (fn, "%s/", p->bytes);
      this->curState |= LOADFILE_SILENT;
      err = l_import_file (this, fn->bytes, err_msg);
      this->curState &= ~LOADFILE_SILENT;
      if (err == L_OK || err == L_ERR_DYNLINK)
        goto theend;
    }
    goto theend;
  }

  err = l_import_file (this, fname->bytes, err_msg);

theend:
  if (err != L_OK) {
    if (err != L_ERR_DYNLINK)
      err = L_ERR_IMPORT;

    this->print_bytes (this->err_fp, this->message->bytes);
  }

  this->curScope = prev_ns;
  string_release (fname);
  string_release (fn);
  string_release (ns);

  return err;
}

static int l_search_for_annotation (l_t *this, string *evalbuf, VALUE *vp) {
  ifnot (evalbuf->num_bytes) {
    *vp = NULL_VALUE;
    return 0;
  }

  int c;

again:
  while ((c = GET_BYTE()))
    if (c == TOKEN_COMMENT || c <= 0)
      break;

  if (c != TOKEN_COMMENT) {
    *vp = NULL_VALUE;
    return 0;
  }

  c = GET_BYTE();

  if (c != TOKEN_AT) {
    if (c > 0)
      goto again;
    else {
      *vp = NULL_VALUE;
      return c;
    }
  }

  return 1;
}

static int l_parse_annotation (l_t *this, VALUE *vp) {
  int c = GET_BYTE();
  THROW_SYNTAX_ERR_IF(c != ' ', "parsing annotation: awaiting a space");

  c = GET_BYTE();

  switch (c) {
    case 'g':
      c = GET_BYTE();
      switch (c) {
        case 'e':
          c = GET_BYTE();
          THROW_SYNTAX_ERR_IF(c != 't', "parsing annotation: awaiting get");
          break;

        default:
          THROW_SYNTAX_ERR("parsing annotation: unknown method");
      }
      break;

    default:
      THROW_SYNTAX_ERR("parsing annotation: unknown method");
  }

  NEXT_TOKEN();

  VALUE v;
  int err = l_parse_expr (this, &v);
  THROW_SYNTAX_ERR_IF_ERR(err, "error while parsing annotation, awaiting an expression");

  THROW_SYNTAX_ERR_IF (v.type != STRING_TYPE,
    "error while parsing annotation, awaiting a string");

  string *evalbuf = AS_STRING(v);

  l_string saved_ptr = PARSEPTR;
  PARSEPTR = StringNewLen (evalbuf->bytes, evalbuf->num_bytes);

  err = l_search_for_annotation (this, evalbuf, vp);
  THROW_ERR_IF_ERR(err);

  ifnot (err) {
    *vp = NULL_VALUE;
    return L_OK;
  }

  string *s = string_new (32);

  c = l_get_annotated_block (this, s);
  THROW_ERR_IF_ERR(c);

  *vp = STRING(s);

  PARSEPTR = saved_ptr;

  return c;
}

static VALUE l_annotate_get (l_t *this, VALUE v_buf) {
  ifnot (IS_STRING(v_buf))  C_THROW(L_ERR_TYPE_MISMATCH, "awaiting a string");

  string *buf = AS_STRING(v_buf);

  l_string saved_ptr = PARSEPTR;
  PARSEPTR = StringNewLen (buf->bytes, buf->num_bytes);

  VALUE v;

  int err = l_search_for_annotation (this, buf, &v);

  if (err < 0)
    C_THROW(err, "error while getting annotation");

  ifnot (err) {
    PARSEPTR = saved_ptr;
    return STRING_NEW ("");
  }

  string *s = string_new (32);

  err = l_get_annotated_block (this, s);

  PARSEPTR = saved_ptr;

  if (err < 0) {
    string_release (s);
    C_THROW(err, "error while getting annotation");
  }

  return STRING(s);
}

static int l_parse_include (l_t *this) {
  NEXT_TOKEN();
  int c = TOKEN;
  int err;

  THROW_SYNTAX_ERR_IFNOT(c == TOKEN_PAREN_OPEN, "evaluation error, awaiting (");

  VALUE v;
  err = l_parse_expr (this, &v);
  THROW_SYNTAX_ERR_IF_ERR(err, "error while evaluating file");

  THROW_SYNTAX_ERR_IF (v.type != STRING_TYPE, "error while evaluating file, awaiting a string");

  string *fname = AS_STRING(v);
  string *fn = NULL;

  if (this->curState & LITERAL_STRING_STATE)
    this->curState &= ~LITERAL_STRING_STATE;
  else {
    string *tmp = fname;
    fname = string_dup (tmp);
    if (v.sym == NULL && v.refcount != MALLOCED_STRING &&
        0 == (this->objectState & (ARRAY_MEMBER|MAP_MEMBER)))
      string_release (tmp);
  }

  if (file_exists (fname->bytes) && file_is_reg (fname->bytes))
    goto loadfile;

  char *extname = path_extname (fname->bytes);

  size_t exlen = bytelen (extname);
  if (exlen) {
    if (exlen != fname->num_bytes) {
      char *p = fname->bytes + fname->num_bytes - 1;
      if (exlen == 4 && (str_eq (p - 4, "." L_EXTENSION)))
        goto loadfile;

      string_append_byte (fname, '.');
    } else  // .file
      string_append_byte (fname, '.');
  } else
    string_append_byte (fname, '.');

  string_append_with (fname, L_EXTENSION);

loadfile: {}

  string *ns = NULL;
  funT *load_ns = this->curScope;
  funT *prev_ns = load_ns;

  while (load_ns && str_eq_n (load_ns->funname, "__block_", 8))
    load_ns = load_ns->prev;

  if (TOKEN == TOKEN_COMMA) {
    NEXT_TOKEN();
    err = l_parse_expr (this, &v);
    THROW_ERR_IF_ERR(err);

    THROW_SYNTAX_ERR_IFNOT (v.type == STRING_TYPE, "namespace error while evaluating file, awaiting a string");

    ns = AS_STRING(v);

    if (this->curState & LITERAL_STRING_STATE)
      this->curState &= ~LITERAL_STRING_STATE;
    else
      ns = string_dup (ns);

    ifnot (ns->num_bytes)
      goto theload;

    l_string x = StringNew (ns->bytes);
    sym_t *symbol = l_lookup_symbol (this, x);
    if (symbol != NULL) {
      v = symbol->value;
      if (v.type & UFUNCTION_TYPE) {
        funT *f = AS_FUNC_PTR(v);
        load_ns = f;
        goto theload;
      }

      this->print_fmt_bytes (this->err_fp, "loadfile(): %s is not a namespace\n", ns->bytes);
      err = L_ERR_SYNTAX;
      goto theend;
    }

    this->print_bytes (this->err_fp, "loadfile(), functionality hasn't been implemented\n");
    err = L_ERR_SYNTAX;
    goto theend;
  }

theload:
  this->curScope = load_ns;

  ifnot (path_is_absolute (fname->bytes)) {
    ifnot (str_eq (L_STRING_NS, AS_STRING_BYTES(this->file->value))) {
      fn = string_dup (fname);
      string_prepend_byte (fn, DIR_SEP);
      char *dname = path_dirname (AS_STRING_BYTES(this->file->value));
      string_prepend_with (fn, dname);
      Release (dname);
      this->curState |= LOADFILE_SILENT;
      err = l_eval_file (this, fn->bytes);
      this->curState &= ~LOADFILE_SILENT;
      if (err != L_ERR_LOAD)
        goto theend;
    }

    this->curState |= LOADFILE_SILENT;
    err = l_eval_file (this, fname->bytes);
    this->curState &= ~LOADFILE_SILENT;
    if (err != L_ERR_LOAD)
      goto theend;

    if (this->l_dir->num_bytes) {
      string_release (fn);
      fn = string_dup (fname);
      string_prepend_with_fmt (fn, "%s/", this->l_dir->bytes);
      this->curState |= LOADFILE_SILENT;
      err = l_eval_file (this, fn->bytes);
      this->curState &= ~LOADFILE_SILENT;
      if (err != L_ERR_LOAD) goto theend;

      string_release (fn);
      fn = string_dup (fname);
      string_prepend_with_fmt (fn, "%s/scripts/", this->l_dir->bytes);
      this->curState |= LOADFILE_SILENT;
      err = l_eval_file (this, fn->bytes);
      this->curState &= ~LOADFILE_SILENT;
      if (err != L_ERR_LOAD) goto theend;
    }

    sym_t *symbol = l_lookup_symbol (this, StringNew ("__loadpath"));
    ArrayType *p_ar = (ArrayType *) AS_ARRAY(symbol->value);
    string **s_ar = (string **) AS_ARRAY(p_ar->value);
    for (int i = 0; i < p_ar->len; i++) {
      string *p = s_ar[i];
      ifnot (p->num_bytes) continue;

      string_release (fn);
      fn = string_dup (fname);
      string_prepend_with_fmt (fn, "%s/", p->bytes);
      this->curState |= LOADFILE_SILENT;
      err = l_eval_file (this, fn->bytes);
      this->curState &= ~LOADFILE_SILENT;
      if (err != L_ERR_LOAD)
        goto theend;
    }

    goto theend;
  }

  err = l_eval_file (this, fname->bytes);

theend:
  this->curScope = prev_ns;
  string_release (fname);
  string_release (fn);
  string_release (ns);
  return err;
}

static VALUE l_equals (l_t *this, VALUE x, VALUE y) {
  VALUE result = FALSE_VALUE;

  switch (x.type) {
    case NUMBER_TYPE:
      switch (y.type) {
        case NUMBER_TYPE:
          return INT(AS_NUMBER(x) == AS_NUMBER(y));
        case INTEGER_TYPE:
          return INT(AS_NUMBER(x) == AS_INT(y));
        case NULL_TYPE: return result;
        default:
          this->CFuncError = L_ERR_TYPE_MISMATCH;
          str_copy_fmt (this->curMsg, MAXLEN_MSG + 1,
              "NumberType == %s is not possible",
              AS_STRING_BYTES(l_typeAsString (this, y)));
          return result;
      }
      return result;

    case INTEGER_TYPE:
      switch (y.type) {
        case NUMBER_TYPE:
          return INT(AS_INT(x) == AS_NUMBER(y));
        case INTEGER_TYPE:
          return INT(AS_INT(x) == AS_INT(y));
        case NULL_TYPE: return result;
        default:
          this->CFuncError = L_ERR_TYPE_MISMATCH;
          str_copy_fmt (this->curMsg, MAXLEN_MSG + 1,
              "IntegerType == %s is not possible",
              AS_STRING_BYTES(l_typeAsString (this, y)));
          return result;
      }
      return result;

    case STRING_TYPE:
      switch (y.type) {
        case STRING_TYPE:
          return INT(str_eq (AS_STRING_BYTES(x), AS_STRING_BYTES(y)));

        case NULL_TYPE: return result;
        default:
          this->CFuncError = L_ERR_TYPE_MISMATCH;
          str_copy_fmt (this->curMsg, MAXLEN_MSG + 1,
              "StringType == %s is not possible",
              AS_STRING_BYTES(l_typeAsString (this, y)));
          return result;
      }
      return result;

    case ARRAY_TYPE:
      switch (y.type) {
        case ARRAY_TYPE:
          return INT(l_array_eq (x, y));
        case NULL_TYPE: return result;
        default:
          this->CFuncError = L_ERR_TYPE_MISMATCH;
          str_copy_fmt (this->curMsg, MAXLEN_MSG + 1,
              "ArrayType == %s is not possible",
              AS_STRING_BYTES(l_typeAsString (this, y)));
          return result;
      }
      return result;

    case MAP_TYPE:
      switch (y.type) {
        case NULL_TYPE: return result;
        case MAP_TYPE: {
          Map_Type *xmap = AS_MAP(x);
          Map_Type *ymap = AS_MAP(y);
          return INT (xmap == ymap);
        }

        default:
          this->CFuncError = L_ERR_TYPE_MISMATCH;
          str_copy_fmt (this->curMsg, MAXLEN_MSG + 1,
              "MapType == %s is not possible",
              AS_STRING_BYTES(l_typeAsString (this, y)));
          return result;
      }

    case LIST_TYPE:
      switch (y.type) {
        case NULL_TYPE: return FALSE_VALUE;
        default:
          str_copy_fmt (this->curMsg, MAXLEN_MSG + 1,
              "ListType == %s is not possible",
              AS_STRING_BYTES(l_typeAsString (this, y)));
          return result;
      }

    case NULL_TYPE:
      switch (y.type) {
        case NULL_TYPE: return TRUE_VALUE;
        default: return result;
      }

    case      FD_TYPE:
    case FILEPTR_TYPE:
    case  OBJECT_TYPE:
      switch (y.type) {
        case NULL_TYPE: return result;
        default:
          this->CFuncError = L_ERR_TYPE_MISMATCH;
          str_copy_fmt (this->curMsg, MAXLEN_MSG + 1,
              "%s == %s is not possible",
              AS_STRING_BYTES(l_typeAsString (this, x)),
              AS_STRING_BYTES(l_typeAsString (this, y)));
          return result;
      }

    default:
      if (x.type > OBJECT_TYPE) {
        switch (y.type) {
          case NULL_TYPE: return result;
          default:
            this->CFuncError = L_ERR_TYPE_MISMATCH;
            str_copy_fmt (this->curMsg, MAXLEN_MSG + 1,
                "%s == %s is not possible",
                AS_STRING_BYTES(l_typeAsString (this, x)),
                AS_STRING_BYTES(l_typeAsString (this, y)));
            return result;
        }
      }

      switch (y.type) {
        case NULL_TYPE: return result;
      }

      this->CFuncError = L_ERR_TYPE_MISMATCH;
      str_copy_fmt (this->curMsg, MAXLEN_MSG + 1,
         "%s == %s is not possible",
         AS_STRING_BYTES(l_typeAsString (this, x)),
         AS_STRING_BYTES(l_typeAsString (this, y)));
      return result;
  }

  return result;
}

static VALUE l_ne (l_t *this, VALUE x, VALUE y) {
  VALUE result = l_equals (this, x, y);
  return INT(0 == AS_INT(result));
}

static VALUE l_lt (l_t *this, VALUE x, VALUE y) {
  (void) this;
  VALUE result = FALSE_VALUE;

  switch (x.type) {
    case NUMBER_TYPE:
      switch (y.type) {
        case NUMBER_TYPE:
          return INT(AS_NUMBER(x) < AS_NUMBER(y));
        case INTEGER_TYPE:
          return INT(AS_NUMBER(x) < AS_INT(y));
      }
      return result;

    case INTEGER_TYPE:
      switch (y.type) {
        case NUMBER_TYPE:
          return INT(AS_INT(x) < AS_NUMBER(y));
        case INTEGER_TYPE:
          return INT(AS_INT(x) < AS_INT(y));
      }
  }

  return result;
}

static VALUE l_le (l_t *this, VALUE x, VALUE y) {
  (void) this;
  VALUE result = FALSE_VALUE;

  switch (x.type) {
    case NUMBER_TYPE:
      switch (y.type) {
        case NUMBER_TYPE:
          return INT(AS_NUMBER(x) <= AS_NUMBER(y));
        case INTEGER_TYPE:
          return INT(AS_NUMBER(x) <= AS_INT(y));
      }
      return result;

    case INTEGER_TYPE:
      switch (y.type) {
        case NUMBER_TYPE:
          return INT(AS_INT(x) <= AS_NUMBER(y));
        case INTEGER_TYPE:
          return INT(AS_INT(x) <= AS_INT(y));
      }
  }

  return result;
}

static VALUE l_gt (l_t *this, VALUE x, VALUE y) {
  (void) this;
  VALUE result = FALSE_VALUE;

  switch (x.type) {
    case NUMBER_TYPE:
      switch (y.type) {
        case NUMBER_TYPE:
          return INT(AS_NUMBER(x) > AS_NUMBER(y));
        case INTEGER_TYPE:
          return INT(AS_NUMBER(x) > AS_INT(y));
      }
      return result;

    case INTEGER_TYPE:
      switch (y.type) {
        case NUMBER_TYPE:
          return INT(AS_INT(x) > AS_NUMBER(y));
        case INTEGER_TYPE:
          return INT(AS_INT(x) > AS_INT(y));
      }
  }

  return result;
}

static VALUE l_ge (l_t *this, VALUE x, VALUE y) {
  (void) this;
  VALUE result = FALSE_VALUE;

  switch (x.type) {
    case NUMBER_TYPE:
      switch (y.type) {
        case NUMBER_TYPE:
          return INT(AS_NUMBER(x) >= AS_NUMBER(y));
        case INTEGER_TYPE:
          return INT(AS_NUMBER(x) >= AS_INT(y));
      }
      return result;

    case INTEGER_TYPE:
      switch (y.type) {
        case NUMBER_TYPE:
          return INT(AS_INT(x) >= AS_NUMBER(y));
        case INTEGER_TYPE:
          return INT(AS_INT(x) >= AS_INT(y));
      }
  }

  return result;
}

static VALUE l_mod (l_t *this, VALUE x, VALUE y) {
  (void) this;
  VALUE result = INT(0);

  switch (x.type) {
    case NUMBER_TYPE: result = INT(0); goto theend;

    case INTEGER_TYPE:
      switch (y.type) {
        case NUMBER_TYPE: result = INT(0); goto theend;
        case INTEGER_TYPE:
          result = INT(AS_INT(x) % AS_INT(y)); goto theend;
      }
  }

theend:
  return result;
}

static VALUE l_add (l_t *this, VALUE x, VALUE y) {
  VALUE result = NULL_VALUE;
  switch (x.type) {
    case NUMBER_TYPE:
      switch (y.type) {
        case NUMBER_TYPE:
          return NUMBER(AS_NUMBER(x) + AS_NUMBER(y));
        case INTEGER_TYPE:
          return NUMBER(AS_NUMBER(x) + AS_INT(y));
      }
      return result;

    case INTEGER_TYPE:
      switch (y.type) {
        case NUMBER_TYPE:
          return NUMBER(AS_INT(x) + AS_NUMBER(y));
        case INTEGER_TYPE:
          return INT(AS_INT(x) + AS_INT(y));
        case STRING_TYPE: {
          integer x_i = AS_INT(x);
          string *nx = string_new (8);
          if (x_i <= '~' + 1) {
            string_append_byte (nx, x_i);
          } else {
            char buf[8];
            int len = utf8_character (x_i, buf, 8);
            string_append_with_len (nx, buf, len);
          }
          x = STRING(nx);
          goto string_type;
        }
      }

      return result;

    case STRING_TYPE:
      string_type:
      switch (y.type) {
        case STRING_TYPE: {
          string *x_str = AS_STRING(x);
          string *y_str = AS_STRING(y);
          if (this->objectState & OBJECT_APPEND) {
            this->objectState &= ~OBJECT_APPEND;
            string_append_with_len (x_str, y_str->bytes, y_str->num_bytes);
            result = STRING(x_str);
            result.refcount = x.refcount;
          } else {
            string *new = string_new_with_len (x_str->bytes, x_str->num_bytes);
            string_append_with_len (new, y_str->bytes, y_str->num_bytes);
            result = STRING(new);

            if (x.sym == NULL && x.refcount != MALLOCED_STRING &&
                0 == (this->objectState & (ARRAY_MEMBER|MAP_MEMBER)) &&
                0 == x.refcount) {
              l_string_release (x);
              this->objectState |= LHS_STRING_RELEASED;
            }
          }

          if (y.sym == NULL && y.refcount != MALLOCED_STRING &&
              0 == (this->objectState & (ARRAY_MEMBER|MAP_MEMBER))) {
            l_string_release (y);
            this->objectState |= RHS_STRING_RELEASED;
          }

          this->objectState &= ~(ARRAY_MEMBER|MAP_MEMBER);

          if (this->exprList)
            result.refcount = STRING_LITERAL;

          goto theend;
        }

        case INTEGER_TYPE: {
          string *new = NULL;
          string *x_str = AS_STRING(x);

          if (this->objectState & OBJECT_APPEND) {
            this->objectState &= ~OBJECT_APPEND;
            new = x_str;
          } else {
            new = string_new_with_len (x_str->bytes, x_str->num_bytes);

            if (x.sym == NULL && x.refcount != MALLOCED_STRING &&
                0 == (this->objectState & (ARRAY_MEMBER|MAP_MEMBER))) {
              string_release (x_str);
              this->objectState |= LHS_STRING_RELEASED;
            }
          }

          this->objectState &= ~(ARRAY_MEMBER|MAP_MEMBER);

          integer y_i = AS_INT(y);
          if (y_i <= '~' + 1) {
            string_append_byte (new, y_i);
          } else {
            char buf[8];
            int len = utf8_character (y_i, buf, 8);
            string_append_with_len (new, buf, len);
          }

          result = STRING(new);
          if (new == x_str)
            result.refcount = x.refcount;
          goto theend;
        }

        case NULL_TYPE:
          result = x;

        goto theend;
      }
  }

theend:
  return result;
}

static VALUE l_mul (l_t *this, VALUE x, VALUE y) {
  (void) this;
  VALUE result = INT(0);

  switch (x.type) {
    case NUMBER_TYPE:
      switch (y.type) {
        case NUMBER_TYPE:
          result = NUMBER(AS_NUMBER(x) * AS_NUMBER(y)); goto theend;
        case INTEGER_TYPE:
          result = NUMBER(AS_NUMBER(x) * AS_INT(y)); goto theend;
      }
      goto theend;

    case INTEGER_TYPE:
      switch (y.type) {
        case NUMBER_TYPE:
          result = NUMBER(AS_INT(x) * AS_NUMBER(y)); goto theend;
        case INTEGER_TYPE:
          result = INT(AS_INT(x) * AS_INT(y)); goto theend;
      }
  }

theend:
  return result;
}

static VALUE l_div (l_t *this, VALUE x, VALUE y) {
  (void) this;
  VALUE result = INT(0);

  switch (x.type) {
    case NUMBER_TYPE:
      switch (y.type) {
        case NUMBER_TYPE:
          result = NUMBER(AS_NUMBER(x) / AS_NUMBER(y)); goto theend;
        case INTEGER_TYPE:
          result = NUMBER(AS_NUMBER(x) / AS_INT(y)); goto theend;
      }
      goto theend;

    case INTEGER_TYPE:
      switch (y.type) {
        case NUMBER_TYPE:
          result = NUMBER(AS_INT(x) / AS_NUMBER(y)); goto theend;
        case INTEGER_TYPE:
          result = INT(AS_INT(x) / AS_INT(y)); goto theend;
      }
  }

theend:
  return result;
}

static VALUE l_sub (l_t *this, VALUE x, VALUE y) {
  (void) this;
  VALUE result = INT(0);

  switch (x.type) {
    case NUMBER_TYPE:
      switch (y.type) {
        case NUMBER_TYPE:
          result = NUMBER(AS_NUMBER(x) - AS_NUMBER(y)); goto theend;
        case INTEGER_TYPE:
          result = NUMBER(AS_NUMBER(x) - AS_INT(y)); goto theend;
      }
      goto theend;

    case INTEGER_TYPE:
      switch (y.type) {
        case NUMBER_TYPE:
          result = NUMBER(AS_INT(x) - AS_NUMBER(y)); goto theend;
        case INTEGER_TYPE:
          result = INT(AS_INT(x) - AS_INT(y)); goto theend;
      }
  }

theend:
  return result;
}

static VALUE l_bset (l_t *this, VALUE x, VALUE y) {
  (void) this;
  int xx = AS_INT(x); xx |= AS_INT(y); x = INT(xx);
  return x;
}

static VALUE l_bnot (l_t *this, VALUE x, VALUE y) {
  (void) this;
  int xx = AS_INT(x); xx &= AS_INT(y); x = INT(xx);
  return x;
}

static VALUE l_shl (l_t *this, VALUE x, VALUE y) {
  (void) this;
  VALUE result = INT(0);

  switch (x.type) {
    case NUMBER_TYPE: result = INT(0); goto theend; // error

    case INTEGER_TYPE:
      switch (y.type) {
        case NUMBER_TYPE: result = INT(0); goto theend;

        case INTEGER_TYPE:
          result = INT(AS_INT(x) << AS_INT(y)); goto theend;
      }
  }

theend:
  return result;
}

static VALUE l_shr (l_t *this, VALUE x, VALUE y) {
  (void) this;
  VALUE result = INT(0);

  switch (x.type) {
    case NUMBER_TYPE: result = INT(0); goto theend; // error

    case INTEGER_TYPE:
      switch (y.type) {
        case NUMBER_TYPE: result = INT(0); goto theend;

        case INTEGER_TYPE:
          result = INT(AS_INT(x) >> AS_INT(y)); goto theend;
      }
  }

theend:
  return result;
}

static VALUE l_bitor (l_t *this, VALUE x, VALUE y) {
  (void) this;
  VALUE result = INT(0);

  switch (x.type) {
    case NUMBER_TYPE: result = INT(0); goto theend; // error

    case INTEGER_TYPE:
      switch (y.type) {
        case NUMBER_TYPE: result = INT(0); goto theend;

        case INTEGER_TYPE:
          result = INT(AS_INT(x) | AS_INT(y)); goto theend;
      }
  }

theend:
  return result;
}

static VALUE l_bitand (l_t *this, VALUE x, VALUE y) {
  (void) this;
  VALUE result = INT(0);

  switch (x.type) {
    case NUMBER_TYPE: result = INT(0); goto theend; // error

    case INTEGER_TYPE:
      switch (y.type) {
        case NUMBER_TYPE: result = INT(0); goto theend;

        case INTEGER_TYPE:
          result = INT(AS_INT(x) & AS_INT(y)); goto theend;
      }
  }

theend:
  return result;
}

static VALUE l_bitxor (l_t *this, VALUE x, VALUE y) {
  (void) this;
  VALUE result = INT(0);

  switch (x.type) {
    case NUMBER_TYPE: result = INT(0); goto theend; // error

    case INTEGER_TYPE:
      switch (y.type) {
        case NUMBER_TYPE: result = INT(0); goto theend;

        case INTEGER_TYPE:
          result = INT(AS_INT(x) ^ AS_INT(y)); goto theend;
      }
  }

theend:
  return result;
}

static VALUE l_logical_and (l_t *this, VALUE x, VALUE y) {
  (void) this;
  VALUE result = FALSE_VALUE;

  switch (x.type) {
    case NUMBER_TYPE:
      switch (y.type) {
        case NUMBER_TYPE:
          result = INT(AS_NUMBER(x) && AS_NUMBER(y)); goto theend;
        case INTEGER_TYPE:
          result = INT(AS_NUMBER(x) && AS_INT(y)); goto theend;
      }
      goto theend;

    case INTEGER_TYPE:
      switch (y.type) {
        case NUMBER_TYPE:
          result = INT(AS_INT(x) && AS_NUMBER(y)); goto theend;
        case INTEGER_TYPE:
          result = INT(AS_INT(x) && AS_INT(y)); goto theend;
      }

  }

theend:
  return result;
}

static VALUE l_logical_or (l_t *this, VALUE x, VALUE y) {
  (void) this;
  VALUE result = FALSE_VALUE;

  switch (x.type) {
    case NUMBER_TYPE:
      switch (y.type) {
        case NUMBER_TYPE:
          result = INT(AS_NUMBER(x) || AS_NUMBER(y)); goto theend;
        case INTEGER_TYPE:
          result = INT(AS_NUMBER(x) || AS_INT(y)); goto theend;
      }
      goto theend;

    case INTEGER_TYPE:
      switch (y.type) {
        case NUMBER_TYPE:
          result = INT(AS_INT(x) || AS_NUMBER(y)); goto theend;
        case INTEGER_TYPE:
          result = INT(AS_INT(x) || AS_INT(y)); goto theend;
      }
  }

theend:
  return result;
}

static VALUE l_eval (l_t *this, VALUE v_str) {
  ifnot (IS_STRING(v_str)) C_THROW(L_ERR_TYPE_MISMATCH, "awaiting a string");
  char *str = AS_STRING_BYTES (v_str);

  sym_t *sym = ns_lookup_symbol (this->std, "__retval");
  sym->value = INT(IS_UNCHANGEABLE);

  int retval = l_eval_string (this, str);

  if (retval < L_NOTOK)
    C_THROW(retval, "eval() error");

  if (AS_INT(sym->value) != IS_UNCHANGEABLE)
    return sym->value;

  return INT(retval);
}

static struct def {
  const char *name;
  int toktype;
  VALUE val;
} l_defs[] = {
  { "var",     TOKEN_VARDEF,   NULL_VALUE },
  { "const",   TOKEN_CONSTDEF, NULL_VALUE },
  { "if",      TOKEN_IF,       PTR(l_parse_if) },
  { "ifnot",   TOKEN_IFNOT,    PTR(l_parse_if) },
  { "else",    TOKEN_ELSE,     NULL_VALUE },
  { "elseif",  TOKEN_ELSEIF,   NULL_VALUE },
  { "orelse",  TOKEN_ORELSE,   NULL_VALUE },
  { "then",    TOKEN_THEN,     NULL_VALUE },
  { "end",     TOKEN_END,      NULL_VALUE },
  { "in",      TOKEN_IN,       NULL_VALUE },
  { "as",      TOKEN_AS,       NULL_VALUE },
  { "times",   TOKEN_TIMES,    NULL_VALUE },
  { "time",    TOKEN_TIMES,    NULL_VALUE },
  { "repeat",  TOKEN_REPEAT,   PTR(l_parse_repeat) },
  { "until",   TOKEN_UNTIL,    NULL_VALUE },
  { "for",     TOKEN_FOR,      PTR(l_parse_for) },
  { "while",   TOKEN_WHILE,    PTR(l_parse_while) },
  { "forever", TOKEN_FOREVER,  PTR(l_parse_forever) },
  { "break",   TOKEN_BREAK,    NULL_VALUE },
  { "continue",TOKEN_CONTINUE, NULL_VALUE },
  { "func",    TOKEN_FUNCDEF,  PTR(l_parse_func_def) },
  { "return",  TOKEN_RETURN,   PTR(l_parse_return) },
  { "import",  TOKEN_IMPORT,   PTR(l_parse_import) },
  { "include", TOKEN_INCLUDE,  PTR(l_parse_include) },
  { "Annotation", TOKEN_ANNOTATION, NULL_VALUE },
  { "evalfile",TOKEN_EVALFILE, NULL_VALUE },
  { "New",     TOKEN_NEW,      NULL_VALUE },
  { "callback",TOKEN_CALLBACK, NULL_VALUE },
  { "Type",    TOKEN_TYPE,     PTR(l_parse_type) },
  { "override",TOKEN_OVERRIDE, NULL_VALUE },
  { "List",    TOKEN_LIST_FUN, NULL_VALUE },
  { "append",  TOKEN_APPEND,   NULL_VALUE },
  { "print",   TOKEN_PRINT,    PTR(l_parse_print) },
  { "format",  TOKEN_FORMAT,   NULL_VALUE },
  { "println", TOKEN_PRINTLN,  PTR(l_parse_println) },
  { "exit",    TOKEN_EXIT,     PTR(l_parse_exit) },
  { "public",  TOKEN_PUBLIC,   PTR(l_parse_visibility) },
  { "private", TOKEN_PRIVATE,  PTR(l_parse_visibility) },
  { "*",       BINOP(1),       PTR(l_mul) },
  { "/",       BINOP(1),       PTR(l_div) },
  { "%",       BINOP(1),       PTR(l_mod) },
  { "+",       BINOP(2),       PTR(l_add) },
  { "-",       BINOP(2),       PTR(l_sub) },
  { "&",       BINOP(3),       PTR(l_bitand) },
  { "|",       BINOP(3),       PTR(l_bitor) },
  { "^",       BINOP(3),       PTR(l_bitxor) },
  { ">>",      BINOP(3),       PTR(l_shr) },
  { "<<",      BINOP(3),       PTR(l_shl) },
  { "==",      BINOP(4),       PTR(l_equals) },
  { "is",      BINOP(4),       PTR(l_equals) },
  { "!=",      BINOP(4),       PTR(l_ne) },
  { "isnot",   BINOP(4),       PTR(l_ne) },
  { "<",       BINOP(4),       PTR(l_lt) },
  { "<=",      BINOP(4),       PTR(l_le) },
  { ">",       BINOP(4),       PTR(l_gt) },
  { ">=",      BINOP(4),       PTR(l_ge) },
  { "&&",      BINOP(5),       PTR(l_logical_and) },
  { "and",     BINOP(5),       PTR(l_logical_and) },
  { "||",      BINOP(5),       PTR(l_logical_or) },
  { "or",      BINOP(5),       PTR(l_logical_or) },
  { "=",       TOKEN_ASSIGN,      NULL_VALUE },
  { "+=",      TOKEN_ASSIGN_APP,  NULL_VALUE },
  { "-=",      TOKEN_ASSIGN_SUB,  NULL_VALUE },
  { "/=",      TOKEN_ASSIGN_DIV,  NULL_VALUE },
  { "%=",      TOKEN_ASSIGN_MOD,  NULL_VALUE },
  { "*=",      TOKEN_ASSIGN_MUL,  NULL_VALUE },
  { "&=",      TOKEN_ASSIGN_AND,  NULL_VALUE },
  { "|=",      TOKEN_ASSIGN_BAR,  NULL_VALUE },
  { "^=",      TOKEN_ASSIGN_XOR,  NULL_VALUE },
  { ":=",      TOKEN_REASSIGN,    NULL_VALUE },
  { "++",      TOKEN_PLUS_PLUS,   NULL_VALUE },
  { "--",      TOKEN_MINUS_MINUS, NULL_VALUE },
  { "NullType",    INTEGER_TYPE,  INT(NULL_TYPE) },
  { "BooleanType", INTEGER_TYPE,  INT(BOOLEAN_TYPE) },
  { "NumberType",  INTEGER_TYPE,  INT(NUMBER_TYPE) },
  { "IntegerType", INTEGER_TYPE,  INT(INTEGER_TYPE) },
  { "CFunctionType",INTEGER_TYPE, INT(CFUNCTION_TYPE) },
  { "FunctionType",INTEGER_TYPE,  INT(UFUNCTION_TYPE) },
  { "StringType",  INTEGER_TYPE,  INT(STRING_TYPE) },
  { "ArrayType",   INTEGER_TYPE,  INT(ARRAY_TYPE) },
  { "MapType",     INTEGER_TYPE,  INT(MAP_TYPE) } ,
  { "ObjectType",  INTEGER_TYPE,  INT(OBJECT_TYPE) },
  { "ListType",    INTEGER_TYPE,  INT(LIST_TYPE) },
  { "FilePtrType", INTEGER_TYPE,  INT(FILEPTR_TYPE) },
  { "FdType",      INTEGER_TYPE,  INT(FD_TYPE) },
  { "ok",          INTEGER_TYPE,  OK_VALUE },
  { "notok",       INTEGER_TYPE,  NOTOK_VALUE },
  { "true",        INTEGER_TYPE,  TRUE_VALUE },
  { "false",       INTEGER_TYPE,  FALSE_VALUE },
  { "null",        NULL_TYPE,     NULL_VALUE },
  { NULL,          NULL_TYPE,     NULL_VALUE }
};

static struct deftype {
  const char *name;
  int toktype;
  VALUE val;
} l_def_datatypes[] = {
  { "map",     MAP_TYPE,     PTR(l_parse_array_def) },
  { "array",   ARRAY_TYPE,   PTR(l_parse_array_def) },
  { "string",  STRING_TYPE,  PTR(l_parse_array_def) },
  { "number",  NUMBER_TYPE,  PTR(l_parse_array_def) },
  { "integer", INTEGER_TYPE, PTR(l_parse_array_def) },
  { "list",    LIST_TYPE,    PTR(l_parse_array_def) },
  { NULL,      NULL_TYPE,    NULL_VALUE }
};

LDefCFun l_funs[] = {
  { "len",              PTR(l_len), 1},
  { "eval",             PTR(l_eval), 1},
  { "fopen",            PTR(l_fopen), 2},
  { "fflush",           PTR(l_fflush), 1},
  { "fclose",           PTR(l_fclose), 1},
  { "fileno",           PTR(l_fileno), 1},
  { "set_errno",        PTR(l_set_errno), 1},
  { "errno_name",       PTR(l_errno_name), 1},
  { "errno_string",     PTR(l_errno_string), 1},
  { "annotate_get",     PTR(l_annotate_get), 1},
  { "typeof",           PTR(l_typeof), 1},
  { "typeAsString",     PTR(l_typeAsString), 1},
  { "typeofArray",      PTR(l_typeofArray), 1},
  { "typeArrayAsString",PTR(l_typeArrayAsString), 1},
  { "qualifier",        PTR(l_qualifier), 2},
  { "qualifiers",       PTR(l_qualifiers), 0},
  { "qualifier_exists", PTR(l_qualifier_exists), 1},
  { "is_defined",       PTR(l_is_defined), 1},
  { "list_pop_at",      PTR(list_pop_at), 2},
  { "list_append",      PTR(list_append), 2},
  { "list_prepend",     PTR(list_prepend), 2},
  { "list_delete_at",   PTR(list_delete_at), 2},
  { "list_insert_at",   PTR(list_insert_at), 3},
  { "list_clear",       PTR(list_clear), 1},
  { NULL,               NULL_VALUE, NULL_TYPE},
};

static int l_def_std (l_t *this, const char *name, int type, VALUE v, int is_const) {
  ifnot (is_const) {
    sym_t *sym = l_define_symbol (this, this->std, name, type, v, 0);
    return (sym == NULL ? L_NOTOK : L_OK);
  }

  if (L_NOTOK == l_define (this, name, type, v))
    if (is_const > 0)  // could be already redefined (happens with S_I* at file/io)
      return L_NOTOK; // no other error could happen i think

  return L_OK;
}

static int l_std_def (l_t *this, l_opts opts) {
  VALUE v = OBJECT(opts.out_fp);
  object *o = l_object_new (NULL, NULL, "FilePtrType", v);
  v = FILEPTR(o);
  v.refcount = UNDELETABLE;
  int err = l_define (this, "stdout", FILEPTR_TYPE, v);
  if (err) return L_NOTOK;

  v = OBJECT(opts.err_fp);
  o = l_object_new (NULL, NULL, "FilePtrType", v);
  v = FILEPTR(o);
  v.refcount = UNDELETABLE;
  err = l_define (this, "stderr", FILEPTR_TYPE, v);
  if (err) return L_NOTOK;

  v = OBJECT(sys_stdin);
  o = l_object_new (NULL, NULL, "FilePtrType", v);
  v = FILEPTR(o);
  v.refcount = UNDELETABLE;
  err = l_define (this, "stdin", FILEPTR_TYPE, v);
  if (err) return L_NOTOK;

  v = OBJECT(0);
  o = l_object_new (NULL, NULL, "FdType", v);
  v = FILEDES(o);
  v.refcount = UNDELETABLE;
  err = l_define (this, "STDIN_FILENO", FD_TYPE, v);
  if (err) return L_NOTOK;

  v = OBJECT(1);
  o = l_object_new (NULL, NULL, "FdType", v);
  v = FILEDES(o);
  v.refcount = UNDELETABLE;
  err = l_define (this, "STDOUT_FILENO", FD_TYPE, v);
  if (err) return L_NOTOK;

  v = OBJECT(2);
  o = l_object_new (NULL, NULL, "FdType", v);
  v = FILEDES(o);
  v.refcount = UNDELETABLE;
  err = l_define (this, "STDERR_FILENO", FD_TYPE, v);
  if (err) return L_NOTOK;

  v = INT(opts.argc);
  err = l_define (this, "__argc", INTEGER_TYPE, v);
  if (err) return L_NOTOK;

  ArrayType *array = ARRAY_NEW(STRING_TYPE, opts.argc);
  string **ar = (string **) AS_ARRAY(array->value);

  for (integer i = 0; i < opts.argc; i++)
    string_replace_with (ar[i], opts.argv[i]);

  v = ARRAY(array);
  err = l_define (this, "__argv", ARRAY_TYPE, v);
  if (err) return L_NOTOK;

  string *file = string_new_with (L_STRING_NS);
  v = STRING(file);
  this->file = l_define_symbol (this, this->std, "__file__", STRING_TYPE, v, 0);
  if (NULL == this->file) return L_NOTOK;

  string *func = string_new_with (this->curScope->funname);
  v = STRING(func);
  this->func = l_define_symbol (this, this->std, "__func__", STRING_TYPE, v, 0);
  if (NULL == this->func) return L_NOTOK;

  int len = 2;
  if (this->l_dir->num_bytes) len++;
  #ifdef ZLIBDIR
    len++;
  #endif
  #ifdef LIBDIR
    len++;
  #endif

  ArrayType *imp_path = ARRAY_NEW(STRING_TYPE, len);
  string **arimp = (string **) AS_ARRAY(imp_path->value);
  int ind = 0;
  if (this->l_dir->num_bytes)
    string_replace_with_fmt (arimp[ind++], "%s/la-modules", this->l_dir->bytes);
  #ifdef ZLIBDIR
    string_replace_with_fmt (arimp[ind++], "%s/la-modules", ZLIBDIR);
  #endif
  #ifdef LIBDIR
    string_replace_with_fmt (arimp[ind++], "%s/la-modules", LIBDIR);
  #endif
  string_replace_with (arimp[ind++], "/lib/la-modules");
  string_replace_with (arimp[ind], "/lib/z/la-modules");

  v = ARRAY(imp_path);
  err = l_define (this, "__importpath", ARRAY_TYPE, v);
  if (err) return L_NOTOK;

  len = 2;
  if (this->l_dir->num_bytes)
    len += 2;
  #ifdef ZLIBDIR
    len++;
  #endif
  #ifdef LIBDIR
    len++;
  #endif

  ArrayType *load_path = ARRAY_NEW(STRING_TYPE, len);
  string **arload = (string **) AS_ARRAY(load_path->value);
  ind = 0;
  if (this->l_dir->num_bytes) {
    string_replace_with (arload[ind++], this->l_dir->bytes);
    string_replace_with_fmt (arload[ind++], "%s/scripts", this->l_dir->bytes);
  }
  #ifdef ZLIBDIR
    string_replace_with_fmt (arload[ind++], "%s/la-lib", ZLIBDIR);
  #endif
  #ifdef LIBDIR
    string_replace_with_fmt (arload[ind++], "%s/la-lib", LIBDIR);
  #endif
  string_replace_with (arload[ind++], "/lib/la-lib");
  string_replace_with (arload[ind], "/lib/z/la-lib");

  v = ARRAY(load_path);
  err = l_define (this, "__loadpath", ARRAY_TYPE, v);
  if (err) return L_NOTOK;

  v = INT(0);
  err = l_define (this, "errno", INTEGER_TYPE, v);
  if (err) return L_NOTOK;

  v = INT(0);
  if (NULL == l_define_symbol (this, this->std, "__retval", INTEGER_TYPE, v, 0))
    return L_NOTOK;

  funT *uf = Fun_new (this, funNew (
    .name = "self", .namelen = 4, .parent = this->function, .nargs = 0));

  v = PTR(uf);
  if (NULL == l_define_symbol (this, this->function, "self", (UFUNC_TYPE | (0 << 8)), v, 0))
    return L_NOTOK;

  return err;
}

/* ABSTRACTION CODE */

static int l_print_bytes (FILE *fp, const char *bytes) {
  if (NULL == bytes) return 0;
  int nbytes = sys_fprintf (fp, "%s", bytes);
  sys_fflush (fp);
  return nbytes;
}

static int l_print_fmt_bytes (FILE *fp, const char *fmt, ...) {
  size_t len = STRING_FORMAT_SIZE (fmt);
  char bytes[len + 1];

  va_list ap; va_start(ap, fmt);
  sys_vsnprintf (bytes, len + 1, fmt, ap);
  va_end(ap);

  return l_print_bytes (fp, bytes);
}

static int l_print_byte (FILE *fp, int c) {
  return l_print_fmt_bytes (fp, "%c", c);
}

static VALUE I_print_bytes (l_t *this, const char *bytes) {
  VALUE result = INT(this->print_bytes (this->out_fp, bytes));
  return result;
}

static VALUE I_print_byte (l_t *this, char bt) {
  VALUE result = INT(this->print_byte (this->out_fp, bt));
  return result;
}

static int l_eval_string (l_t *this, const char *buf) {
  const char *prev_buffer = this->script_buffer;
  this->script_buffer = buf;

  l_string x = StringNew (buf);

  string *file = AS_STRING(this->file->value);
  integer len =  file->num_bytes;
  char prev_file[len + 1];
  str_copy (prev_file, len + 1, file->bytes, len);
  string_replace_with (file, L_STRING_NS);

  int retval = l_parse_string (this, x);

  string_replace_with_len (file, prev_file, len);

  this->script_buffer = prev_buffer;

  if (retval == L_ERR_EXIT) {
    this->didExit = 1;
    return this->exitValue;
  }

  if (retval < L_NOTOK)
    PRINT_ERR_CONSTANT(retval);

  return retval;
}

static int l_eval_expr (l_t *this, const char *buf, VALUE *v) {
  if (*buf != TOKEN_PAREN_OPEN)
    THROW_SYNTAX_ERR("awaiting (");

  this->script_buffer = buf;
  l_string x = StringNew (buf);

  PARSEPTR = x;
  TOKEN = TOKEN_PAREN_OPEN;

  return l_parse_expr (this, v);
}

static int l_eval_file (l_t *this, const char *filename) {
  char fn[PATH_MAX + 1];

  if (NULL == path_real (filename, fn)) {
    ifnot (this->curState & LOADFILE_SILENT)
      this->print_fmt_bytes (this->err_fp, "%s\n", errno_string (sys_errno));
    return L_ERR_LOAD;
  }

  ifnot (file_exists (fn)) {
    ifnot (this->curState & LOADFILE_SILENT)
      this->print_fmt_bytes (this->err_fp, "%s: doesn't exists\n", filename);
    return L_ERR_LOAD;
  }

  int exists = map_key_exists (this->units, fn);
  if (exists && 0 == (this->funcState & EVAL_UNIT_STATE))
    return L_OK;

  struct stat st;
  if (-1 == sys_stat (fn, &st)) {
    this->print_fmt_bytes (this->err_fp, "%s\n", errno_string (sys_errno));
    return L_NOTOK;
  }

  size_t n = st.st_size;

  FILE *fp = sys_fopen (fn, "r");
  if (NULL == fp) {
    this->print_fmt_bytes (this->err_fp, "%s\n", errno_string (sys_errno));
    return L_NOTOK;
  }

  char script[n + 1];
  size_t r = sys_fread (script, 1, n, fp);
  sys_fclose (fp);

  if (r <= 0) {
    this->print_bytes (this->err_fp, "Couldn't read script\n");
    return L_NOTOK;
  }

  if (r > n) {
    this->print_bytes (this->err_fp, "race condition, aborts now\n");
    return L_NOTOK;
  }

  script[r] = '\0';

  if (this->funcState & EVAL_UNIT_STATE) {
    str_copy_fmt (this->curFunName, MAXLEN_SYMBOL + 1, NS_ANON "_%d", this->anon_id++);
    size_t flen = bytelen (this->curFunName);
    funT *uf = Fun_new (this, funNew (
      .name = this->curFunName, .namelen = flen, .parent = this->curScope));
    this->curFunName[0] = '\0';

    string *script_buf;
    if (exists) {
      void *fn_item = map_get_value (this->units, fn);
      string *str = (string *) fn_item;
      string_replace_with_len (str, script, r);
      script_buf = str;
    } else {
      script_buf = string_new_with_len (script, r);
      map_set (this->units, fn, script_buf, l_release_unit);
    }

    uf->body = StringNew (script_buf->bytes);
    uf->nargs = 0;
    this->curFunDef = uf;
    string_replace_with (this->evalFile, fn);
    return L_OK;
  }

  string *script_buf = string_new_with_len (script, r);
  map_set (this->units, fn, script_buf, l_release_unit);

  const char *prev_buffer = this->script_buffer;
  this->script_buffer = script_buf->bytes;

  string *file = AS_STRING(this->file->value);
  integer len =  file->num_bytes;
  char prev_file[len + 1];
  str_copy (prev_file, len + 1, file->bytes, len);
  string_replace_with (file, fn);

  l_string x = StringNew (script_buf->bytes);

  int retval = l_parse_string (this, x);

  string_replace_with_len (file, prev_file, len);

  this->script_buffer = prev_buffer;

  if (retval == L_ERR_EXIT || retval >= L_NOTOK)
    return retval;

  PRINT_ERR_CONSTANT(retval);

  return retval;
}

static int l_define_funs_default_cb (l_t *this) {
  (void) this;
  return L_OK;
}

static l_opts l_default_options (l_t *this, l_opts opts) {
  (void) this;
  if (NULL == opts.print_bytes)
    opts.print_bytes = l_print_bytes;

  if (NULL == opts.print_byte)
    opts.print_byte = l_print_byte;

  if (NULL == opts.print_fmt_bytes)
    opts.print_fmt_bytes = l_print_fmt_bytes;

  if (NULL == opts.syntax_error)
    opts.syntax_error = l_syntax_error;

  if (NULL == opts.syntax_error_fmt)
    opts.syntax_error_fmt = l_syntax_error_fmt;

  if (NULL == opts.err_fp)
    opts.err_fp = sys_stderr;

  if (NULL == opts.define_funs_cb)
    opts.define_funs_cb = l_define_funs_default_cb;

  return opts;
}

static void l_release_stdns (l_t *this) {
  sym_t *sym = ns_lookup_symbol (this->std, "stdout");
  sym->value.refcount = 0;
  sym = ns_lookup_symbol (this->std, "stderr");
  sym->value.refcount = 0;
  sym = ns_lookup_symbol (this->std, "stdin");
  sym->value.refcount = 0;

  sym = ns_lookup_symbol (this->std, "STDIN_FILENO");
  sym->value.refcount = 0;
  sym = ns_lookup_symbol (this->std, "STDOUT_FILENO");
  sym->value.refcount = 0;
  sym = ns_lookup_symbol (this->std, "STDERR_FILENO");
  sym->value.refcount = 0;

  fun_release (&this->std);
}

static void l_release (l_t **thisp) {
  if (NULL == *thisp) return;
  l_t *this = *thisp;

  string_release (this->l_dir);
  string_release (this->message);
  string_release (this->evalFile);
  map_release    (&this->funRefcount);
  map_release    (&this->units);
  map_release    (&this->types);

  fun_release (&this->function);
  fun_release (&this->datatypes);
  fun_release (&this->private_maps);

  l_release_stdns (this);

  Release (this);
  *thisp = NULL;
}

static void l_release_instance (l_t **thisp) {
  l_release (thisp);
}

static l_t *l_new (l_T *__l__) {
  l_t *this = Alloc (sizeof (l_t));
  this->prop = __l__->prop;
  return this;
}

static char *l_name_gen (char *name, int *name_gen, const char *prefix, size_t prelen) {
  size_t num = (*name_gen / 26) + prelen;
  size_t i = 0;
  for (; i < prelen; i++) name[i] = prefix[i];
  for (; i < num; i++) name[i] = 'a' + ((*name_gen)++ % 26);
  name[num] = '\0';
  return name;
}

static void l_remove_instance (l_T *this, l_t *instance) {
  l_t *it = this->prop->head;
  l_t *prev = NULL;

  int idx = 0;
  while (it != instance) {
    prev = it;
    idx++;
    it = it->next;
  }

  if (it == NULL) return;
  if (idx >= this->prop->current_idx)
    this->prop->current_idx--;

  this->prop->num_instances--;

  ifnot (this->prop->num_instances) {
    this->prop->head = NULL;
    goto theend;
  }

  if (1 == this->prop->num_instances) {
    if (it->next == NULL) {
      this->prop->head = prev;
      this->prop->head->next = NULL;
    } else
      this->prop->head = it->next;
    goto theend;
  }

  prev->next = it->next;

theend:
  l_release_instance (&it);
}

static l_t *l_append_instance (l_T *this, l_t *instance) {
  instance->next = NULL;
  this->prop->current_idx = this->prop->num_instances;
  this->prop->num_instances++;

  if (NULL == this->prop->head)
    this->prop->head = instance;
  else {
    l_t *it = this->prop->head;
    while (it) {
      if (it->next == NULL) break;
      it = it->next;
    }

    it->next = instance;
  }

  return instance;
}

static l_t *l_set_current (l_T *this, int idx) {
  if (idx >= this->prop->num_instances)
    return NULL;

  l_t *it = this->prop->head;
  int i = 0;
  while (i++ < idx) it = it->next;

  return it;
}

static void l_set_CFuncError (l_t *this, int err) {
  this->CFuncError = err;
}

static void l_set_curMsg (l_t *this, const char *msg) {
  str_copy (this->curMsg, MAXLEN_MSG + 1, msg, bytelen (msg));
}

static void l_set_function_curMsg (l_t *this, const char *fname, const char *msg) {
  size_t len = str_copy_fmt (this->curMsg, MAXLEN_MSG + 1, "%s(): ", fname);
  str_copy (this->curMsg + len, (MAXLEN_MSG + 1) - len, msg, bytelen (msg));
}

static void l_set_Errno (l_t *this, int err) {
  this->Errno = err;
  sym_t *sym = ns_lookup_symbol (this->std, "errno");
  sym->value = INT (err);
}

static VALUE l_set_errno (l_t *this, VALUE v_err) {
  ifnot (IS_INT(v_err)) C_THROW(L_ERR_TYPE_MISMATCH, "awaiting an integer");

  l_set_Errno (this, AS_INT(v_err));
  return v_err;
}

static void l_set_l_dir (l_t *this, const char *fn) {
  if (NULL == fn) return;
  size_t len = bytelen (fn);
  string_replace_with_len (this->l_dir, fn, len);
}

static void l_set_define_funs_cb (l_t *this, LDefineFuns_cb cb) {
  this->define_funs_cb = cb;
}

static void l_set_user_data (l_t *this, void *user_data) {
  this->user_data = user_data;
}

static void *l_get_user_data (l_t *this) {
  return this->user_data;
}

static char *l_get_message (l_t *this) {
  return this->message->bytes;
}

static int l_get_didExit (l_t *this) {
  return this->didExit;
}

public l_T *l_get_root (l_t *this) {
  return this->root;
}

static char *l_get_eval_str (l_t *this) {
  return (char *) GETSTRPTR(PARSEPTR);
}

static l_t *l_get_current (l_T *this) {
  l_t *it = this->prop->head;
  int i = 0;
  while (i++ < this->prop->current_idx)
    it = it->next;

  return it;
}

static int l_get_current_idx (l_T *this) {
  return this->prop->current_idx;
}

static int l_init (l_T *interp, l_t *this, l_opts opts) {
  int i;
  int err = 0;

  if (NULL == opts.name)
    l_name_gen (this->name, &this->prop->name_gen, "lai:", 2);
  else
    str_copy (this->name, 32, opts.name, 31);

  opts = l_default_options (this, opts);

  this->print_byte = opts.print_byte;
  this->print_fmt_bytes = opts.print_fmt_bytes;
  this->print_bytes = opts.print_bytes;
  this->syntax_error = opts.syntax_error;
  this->syntax_error_fmt = opts.syntax_error_fmt;
  this->err_fp = opts.err_fp;
  this->out_fp = opts.out_fp;
  this->user_data = opts.user_data;
  this->define_funs_cb = opts.define_funs_cb;
  this->didExit = 0;
  this->hasToReturn = 0;
  this->exitValue = L_OK;
  this->curState = 0;
  this->funcState = 0;
  this->objectState = 0;
  this->scopeState = 0;
  this->stackValIdx = -1;
  this->anon_id = 0;
  this->loopCount = 0;
  this->argCount = 0;
  this->qualifierCount = 0;
  this->exprList = 0;

  if (NULL == opts.l_dir) {
    char *ddir = sys_getenv ("DATADIR");
    ifnot (NULL == ddir) {
      this->l_dir = string_new_with (ddir);
      string_trim_end (this->l_dir, '\\');
      string_append_with_len (this->l_dir, "/la", 3);
      if (-1 == make_dir_parents (this->l_dir->bytes, S_IRWXU)) {
        this->print_fmt_bytes (this->err_fp, "%s\n", errno_string (sys_errno));
        return L_NOTOK;
      }
    } else {
#ifdef DATADIR
      this->l_dir = string_new_with (DATADIR);
      string_trim_end (this->l_dir, '\\');
      string_append_with_len (this->l_dir, "/la", 3);
      if (-1 == make_dir_parents (this->l_dir->bytes, S_IRWXU)) {
        this->print_fmt_bytes (this->err_fp, "%s\n", errno_string (sys_errno));
        return L_NOTOK;
      }
#else
      this->l_dir = string_new (32);
#endif
    }
  }
  else
    this->l_dir = string_new_with (opts.l_dir);

  this->message = string_new (32);

  this->std = fun_new (
      funNew (.name = NS_STD, .namelen = NS_STD_LEN, .num_symbols = 256));

  this->datatypes = fun_new (
      funNew (.name = "__types__", .namelen = 9, .num_symbols = 256));

  Fun_new (this,
      funNew (.name = NS_GLOBAL, .namelen = NS_GLOBAL_LEN, .num_symbols = 256));

  this->private_maps = fun_new (
      funNew (.name = NS_PRIVATE, .namelen = NS_PRIVATE_LEN, .num_symbols = 1));

  for (i = 0; l_defs[i].name; i++) {
    err = l_define (this, l_defs[i].name, l_defs[i].toktype, l_defs[i].val);

    if (err != L_OK) {
      l_release (&this);
      return err;
    }
  }

  for (i = 0; l_def_datatypes[i].name; i++) {
    err = l_define_type (this, l_def_datatypes[i].name, l_def_datatypes[i].toktype, l_def_datatypes[i].val);

    if (err != L_OK) {
      l_release (&this);
      return err;
    }
  }

  for (i = 0; l_funs[i].name; i++) {
    err = l_define (this, l_funs[i].name, CFUNC(l_funs[i].nargs), l_funs[i].val);

    if (err != L_OK) {
      l_release (&this);
      return err;
    }
  }

  err = l_std_def (this, opts);
  if (err != L_OK) {
    l_release (&this);
    return err;
  }

  if (L_OK != opts.define_funs_cb (this)) {
    l_release (&this);
    return err;
  }

  this->funRefcount = map_new (256);
  this->units = map_new (32);
  this->types = map_new (32);
  this->evalFile = string_new (128);

  l_append_instance (interp, this);

  return L_OK;
}

static l_t *l_init_instance (l_T *__l__, l_opts opts) {
  #ifdef LIBDIR
   //ZZZZZ sys_setenv ("LD_LIBRARY_PATH", LIBDIR, 0);
  #endif

  l_t *this = l_new (__l__);

  l_init (__l__, this, opts);

  this->root = __l__;

  return this;
}

static int l_loadfile (l_t *this, const char *fn) {
  int err = l_eval_file (this, fn);
  if (err == L_ERR_EXIT)
    return this->exitValue;
  return err;
}

static int l_load_file (l_T *__l__, l_t *this, const char *fn) {
  if (this == NULL)
    this = l_init_instance (__l__, LOpts());

  ifnot (path_is_absolute (fn)) {
    if (file_exists (fn) && file_is_reg (fn))
      return l_loadfile (this, fn);

    size_t fnlen = bytelen (fn);
    char fname[fnlen+5];
    str_copy (fname, fnlen + 1, fn, fnlen);

    char *extname = path_extname (fname);
    size_t extlen = bytelen (extname);

    if (0 == extlen || 0 == str_eq ("." L_EXTENSION, extname)) {
      fname[fnlen] = '.'; fname[fnlen+1] = 'l'; fname[fnlen+2] = 'a';
      fname[fnlen+3] = 'i'; fname[fnlen+4] = '\0';
      if (file_exists (fname))
        return l_loadfile (this, fname);

      fname[fnlen] = '\0';
    }

    ifnot (this->l_dir->num_bytes) {
      l_set_message_fmt (this, 0, "%s: couldn't locate script", fn);
      return L_NOTOK;
    }

    string *ddir = this->l_dir;
    size_t len = ddir->num_bytes + bytelen (fname) + 2 + 7;
    char tmp[len + 5];
    str_copy_fmt (tmp, len + 1, "%s/scripts/%s", ddir->bytes, fname);

    if (0 == file_exists (tmp) || 0 == file_is_reg (tmp)) {
      tmp[len] = '.'; tmp[len+1] = 'l'; tmp[len+2] = 'a';
      tmp[len+3] = 'i'; tmp[len+4] = '\0';
    }

    ifnot (file_exists (tmp)) {
      l_set_message_fmt (this, 0, "%s: couldn't locate script", fn);
      return L_NOTOK;
    }

    return l_loadfile (this, tmp);
  }

  ifnot (file_exists (fn)) {
    l_set_message_fmt (this, 0, "%s: couldn't locate script", fn);
    return L_NOTOK;
  }

  return l_loadfile (this, fn);
}

public l_T *__init_l__ (void) {
  l_T *this =  Alloc (sizeof (l_T));
  this->prop = Alloc (sizeof (l_prop));

  *this = (l_T) {
    .self = (l_self) {
      .new = l_new,
      .init = l_init,
      .def =  l_define,
      .def_std = l_def_std,
      .release = l_release,
      .release_value = l_release_value,
      .release_qualifiers = l_release_qualifiers,
      .eval_file = l_eval_file,
      .eval_expr = l_eval_expr,
      .load_file = l_load_file,
      .print_byte = I_print_byte,
      .print_bytes = I_print_bytes,
      .eval_string =  l_eval_string,
      .init_instance = l_init_instance,
      .remove_instance = l_remove_instance,
      .append_instance = l_append_instance,
      .qualifier_exists = l_C_qualifier_exists,
      .copy_value = _l_copy_value,
      .get = (l_get_self) {
        .root = l_get_root,
        .message = l_get_message,
        .current = l_get_current,
        .didExit = l_get_didExit,
        .eval_str = l_get_eval_str,
        .user_data = l_get_user_data,
        .qualifier = l_get_qualifier,
        .qualifiers = l_get_qualifiers,
        .current_idx = l_get_current_idx
      },
      .set = (l_set_self) {
        .Errno = l_set_Errno,
        .l_dir = l_set_l_dir,
        .curMsg = l_set_curMsg,
        .current = l_set_current,
        .user_data = l_set_user_data,
        .CFuncError = l_set_CFuncError,
        .qualifiers = l_set_qualifiers,
        .define_funs_cb = l_set_define_funs_cb,
        .function_curMsg = l_set_function_curMsg
      },
      .map = (l_map_self) {
        .set_value = l_map_set_value,
        .reset_value = l_map_reset_value,
        .release_value = l_map_release_value
      },
      .object = (l_object_self) {
        .new = l_object_new
      }
    },
    .prop = this->prop
  };

  this->prop->name_gen = ('z' - 'a') + 1;

  this->prop->head = NULL;
  this->prop->num_instances = 0;
  this->prop->current_idx = -1;

  return this;
}

public void __deinit_l__ (l_T **thisp) {
  if (NULL == *thisp) return;
  l_T *this = *thisp;

  l_t *it = this->prop->head;
  while (it) {
    l_t *tmp = it->next;
    l_release_instance (&it);
    it = tmp;
  }

  Release (this->prop);
  Release (this);
  *thisp = NULL;
}
