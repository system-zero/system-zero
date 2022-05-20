/* Derived from the Tinyscript project at:
 * https://github.com/totalspectrum/ (see LICENSE included in this directory)
 *
 * See data/docs/la.md and data/tests/la-semantics.lai
 * for details about syntax and semantics.
 */

 /* an unorthodoxical way */

#define LIBRARY "la"

#define REQUIRE_STDIO
#define REQUIRE_STDARG
#define REQUIRE_DLFCN

#define REQUIRE_DIR_TYPE     DECLARE
#define REQUIRE_PATH_TYPE    DECLARE
#define REQUIRE_FILE_TYPE    DECLARE
#define REQUIRE_STRING_TYPE  DECLARE
#define REQUIRE_CSTRING_TYPE DECLARE
#define REQUIRE_VSTRING_TYPE DECLARE
#define REQUIRE_USTRING_TYPE DECLARE
#define REQUIRE_VMAP_TYPE    DECLARE
#define REQUIRE_IMAP_TYPE    DECLARE
#define REQUIRE_IO_TYPE      DECLARE
#define REQUIRE_ERROR_TYPE   DECLARE
#define REQUIRE_LIST_MACROS
#define REQUIRE_MAP_MACROS
#define REQUIRE_LA_TYPE      DONOT_DECLARE

#include <z/cenv.h>

#define $myprop      this->prop
#define $my(__v__)   $myprop->__v__

#define NS_GLOBAL          "global"
#define NS_GLOBAL_LEN      6
#define NS_STD             "std"
#define NS_STD_LEN         3
#define NS_LOOP_BLOCK      "__block_loop__"
#define NS_LOOP_BLOCK_LEN  14
#define NS_IF_BLOCK        "__block_if__"
#define NS_IF_BLOCK_LEN    12
#define NS_CHAIN_BLOCK      "__chain_block__"
#define NS_CHAIN_BLOCK_LEN  15
#define NS_ANON            "anonymous"
#define LA_EXTENSION       "lai"
#define LA_STRING_NS       "__string__"
#define LA_OPERATORS       "=+-!/*%<>&|^"

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
#define TYPE_NEW_STATE                (1 << 2)
#define EVAL_UNIT_STATE               (1 << 3)
#define RETURN_STATE                  (1 << 4)
#define CHAIN_STATE                   (1 << 5)

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

#define BINOP(x) (((x) << 8) + BINOP_TYPE)
#define CFUNC(x) (((x) << 8) + CFUNC_TYPE)

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
#define TOKEN_DO         'D'
#define TOKEN_ELSEIF     'E'
#define TOKEN_FUNCDEF    'F'
#define TOKEN_LAMBDA     'G'
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
#define TOKEN_LOADFILE   'r'
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
#define TOKEN_ASSIGN_LAST_VAL TOKEN_MINUS_MINUS

#define TOKEN_D_DEFINE    1011
#define TOKEN_D_UNDEF     1012
#define TOKEN_D_ISDEFINED 1013
#define TOKEN_D_GET       1014

#define TOKEN_INCLUDE     89000

typedef struct la_string {
  uint len;
  const char *ptr;
} la_string;

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
  Vmap_t *symbols;
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
  la_t *instance;
  module_so *next;
};

typedef struct modules {
  module_so *head;
} modules;

struct funType {
  char funname[MAXLEN_SYMBOL + 1];

  la_string body;

  int nargs;

  char argName[MAX_BUILTIN_PARAMS][MAXLEN_SYMBOL + 1];

  Vmap_t *symbols;
  Vmap_t *block_symbols;

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
  la_string body;
  int nargs;
  int num_symbols;
  funT *parent;
  funT *root;
} funNewArgs;

#define funNew(...) (funNewArgs) {   \
  .name = NULL,                      \
  .namelen = 0,                      \
  .nargs = 0,                        \
  .num_symbols = MAP_DEFAULT_LENGTH, \
  .parent = NULL,                    \
  .root = NULL,                      \
  __VA_ARGS__}

struct la_prop {
  int name_gen;

  la_t *head;
  int num_instances;
  int current_idx;
};

struct la_t {
  funT *function;
  funT *std;
  funT *datatypes;
  funT *macros;
  funT *curScope;

  fun_stack funstack[1];
  symbol_stack symbolstack[1];
  Imap_t *funRefcount;
  Vmap_t *units;
  Vmap_t *types;
  Vmap_t *qualifiers;

  char name[32];
  const char *script_buffer;

  char symKey[MAXLEN_SYMBOL + 1];
  char curMapKey[MAXLEN_SYMBOL + 1];
  char curFunName[MAXLEN_SYMBOL + 1];
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

  size_t anon_id;

  string
    *la_dir,
    *message,
    *evalFile;

  la_string
    tokenStr,
    parsePtr;

  VALUE
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

  la_prop *prop;

  void *user_data;

  LaPrintByte_cb print_byte;
  LaPrintBytes_cb print_bytes;
  LaPrintFmtBytes_cb print_fmt_bytes;
  LaSyntaxError_cb syntax_error;
  LaDefineFuns_cb define_funs_cb;

  la_t *next;

  la_T *root;
};

typedef struct tokenState {
  int curToken;
  int prevToken;
  int tokenArgs;
  la_string tokenStr;
  la_string parsePtr;
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
  do { if (_cond_) return la_syntax_error_fmt (this, _fmt_, __VA_ARGS__); } while (0)
#define THROW_SYNTAX_ERR_FMT(_fmt_, ...) \
  do { return la_syntax_error_fmt (this, _fmt_, __VA_ARGS__); } while (0)
#define THROW_OUT_OF_BOUNDS(_fmt_, ...) do {                \
  this->print_fmt_bytes (this->err_fp, _fmt_, __VA_ARGS__); \
  this->print_bytes (this->err_fp, "\n");                   \
  return la_err_ptr (this, LA_ERR_OUTOFBOUNDS);             \
} while (0)
#define THROW_OUT_OF_BOUNDS_IF(_cond_, _fmt_, ...) if (_cond_) do { \
  this->print_fmt_bytes (this->err_fp, _fmt_, __VA_ARGS__);         \
  this->print_bytes (this->err_fp, "\n");                           \
  return la_err_ptr (this, LA_ERR_OUTOFBOUNDS);                     \
} while (0)
#define THROW_UNKNOWN_SYMBOL(_symname_) do {                \
  this->print_fmt_bytes (this->err_fp,                      \
    "unknown symbol: %s\n", cur_msg_str (this, _symname_)); \
  return la_err_ptr (this, LA_ERR_UNKNOWN_SYM);             \
} while (0)
#define THROW_INV_ARRAY_TYPE(_tp_) do {                     \
  this->print_fmt_bytes (this->err_fp,                      \
    "invalid array type: %d\n", _tp_);                      \
  return la_err_ptr (this, LA_ERR_INV_ARRAY_TYPE);          \
} while (0)
#define THROW_TOOMANY_FUNCTION_ARGS() do {                  \
  this->print_fmt_bytes (this->err_fp,                      \
    "too many function arguments\n");                       \
  return la_err_ptr (this, LA_ERR_TOOMANY_FUNCTION_ARGS);   \
} while (0)
#define THROW_NUM_FUNCTION_ARGS_MISMATCH(_n_, _p_) do {         \
  this->print_fmt_bytes (this->err_fp,                          \
    "number argument mismatch, awaiting %d got %d\n", _n_, _p_);\
  return la_err_ptr (this, LA_ERR_NUM_FUNCTION_ARGS_MISMATCH);  \
} while (0)
#define THROW_TYPE_MISMATCH(_g_, _a_) do {                  \
  this->print_fmt_bytes (this->err_fp,                      \
    "type mismatch, awaiting %d got %d\n", _a_, _g_);       \
  return la_err_ptr (this, LA_ERR_TYPE_MISMATCH);           \
} while (0)
#define THROW_A_TYPE_MISMATCH(_g_, _m_) do {                \
  this->print_fmt_bytes (this->err_fp,                      \
    "type mismatch, got %d, awaiting %s\n", _g_, _m_);      \
  return la_err_ptr (this, LA_ERR_TYPE_MISMATCH);           \
} while (0)
#define THROW_SYNTAX_ERR_IF_STD_SYM_EXISTS(_n_)             \
do {                                                        \
  THROW_SYNTAX_ERR_FMT_IF(                                  \
    (NULL isnot ns_lookup_symbol (this->std, _n_)),         \
    "%s: can not redefine a standard symbol", _n_);         \
} while (0)
#define THROW_SYNTAX_ERR_IF_SYM_NAME_EXCEEDS_LEN(_n_, _l_)  \
do {                                                        \
  THROW_SYNTAX_ERR_FMT_IF(_l_ > MAXLEN_SYMBOL,              \
   "%s: symbol name exceeded maximum length %d", _n_, _l_); \
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

#define SAVE_TOKENSTATE() la_save_token_state (this)
#define RESTORE_TOKENSTATE(_s_) do {la_restore_token_state (this, _s_);} while (0)

#define NEXT_TOKEN()  do { \
  this->prevToken = TOKEN; \
  if (la_do_next_token (this, 0) < TOKEN_EOF) return LA_ERR_SYNTAX; \
} while (0)

#define NEXT_RAW_TOKEN()  do { \
  this->prevToken = TOKEN;     \
  if (la_do_next_token (this, 1) < TOKEN_EOF) return LA_ERR_SYNTAX; \
} while (0)

#define RESET_TOKEN do {                    \
  SETSTRLEN(TOKENSTR, 0);                   \
  SETSTRPTR(TOKENSTR, GETSTRPTR(PARSEPTR)); \
} while (0)

#define RESET_PARSEPTR do { PARSEPTR.len = 0; } while (0)

#define GET_BYTE() la_get_byte (this)
#define UNGET_BYTE() do {                       \
  SETSTRLEN(PARSEPTR, GETSTRLEN(PARSEPTR) + 1); \
  SETSTRPTR(PARSEPTR, GETSTRPTR(PARSEPTR) - 1); \
  IGNORE_LAST_TOKEN;                            \
} while (0)

#define PEEK_NTH_TOKEN(_n_) la_peek_token (this, _n_)
#define PEEK_NTH_BYTE(_n_) la_peek_nth_byte (this, _n_)
#define PEEK_NTH_BYTE_NOWS_NONL(_n_) la_peek_nth_byte_nows_nonl (this, _n_)
#define PEEK_NTH_BYTE_NOWS_INLINE(_n_) la_peek_nth_byte_nows_inline (this, _n_)
#define NEXT_BYTE_NOWS_NONL() la_next_byte_nows_nonl (this)
#define NEXT_BYTE_NOWS() la_next_byte_nows (this)
#define IS_NEXT_BYTE( _c_) (*GETSTRPTR(PARSEPTR) == _c_)

#define IGNORE_NEXT_BYTE do {                   \
  SETSTRPTR(PARSEPTR, GETSTRPTR(PARSEPTR) + 1); \
  SETSTRLEN(PARSEPTR, GETSTRLEN(PARSEPTR) - 1); \
} while (0)

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

#define C_THROW(__e__, __m__) do {  \
  la_set_CFuncError (this,  __e__); \
  la_set_curMsg (this, __m__);      \
  return NULL_VALUE;                \
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

static int la_do_next_token (la_t *, int);
static int la_parse_iforelse (la_t *, int, VALUE *);
static int la_parse_print (la_t *);
static int la_parse_println (la_t *);
static int la_eval_string (la_t *, const char *);
static int la_parse_cond (la_t *, int);
static int la_parse_stmt (la_t *);
static int la_parse_expr (la_t *, VALUE *);
static int la_parse_primary (la_t *, VALUE *);
static int la_parse_func_def (la_t *);
static int la_eval_file (la_t *, const char *);
static int la_parse_fmt (la_t *, string *, int);
static sym_t *la_lookup_symbol (la_t *, la_string);
static sym_t *la_define_symbol (la_t *, funT *, const char *, int, VALUE, int);
static VALUE la_release_val (la_t *, VALUE);
static VALUE string_release (VALUE);
static VALUE la_copy_map (VALUE);
static VALUE map_release (VALUE);
static VALUE la_copy_value (VALUE v);
static VALUE la_set_errno (la_t *, VALUE);
static VALUE la_equals (la_t *, VALUE, VALUE);
static VALUE la_ne (la_t *, VALUE, VALUE);
static VALUE la_mul  (la_t *, VALUE, VALUE);
static VALUE la_add  (la_t *, VALUE, VALUE);
static VALUE la_sub  (la_t *, VALUE, VALUE);
static VALUE la_div  (la_t *, VALUE, VALUE);
static VALUE la_mod  (la_t *, VALUE, VALUE);
static VALUE la_bset (la_t *, VALUE, VALUE);
static VALUE la_bnot (la_t *, VALUE, VALUE);
static VALUE la_bitxor (la_t *, VALUE, VALUE);
static VALUE array_release (VALUE);
static ArrayType *array_copy (ArrayType *);
static int la_parse_annotation (la_t *, VALUE *);
static int la_array_set_as_array (la_t *, VALUE, integer, integer, integer);
static int la_array_assign (la_t *, VALUE *, VALUE, VALUE, int);
static int la_parse_array_def (la_t *, VALUE *, int);
static int la_parse_array_get (la_t *, VALUE *);
static int la_parse_func_call (la_t *, VALUE *, CFunc, funT *, VALUE);
static int la_parse_map_get (la_t *, VALUE *);
static int la_parse_map_set (la_t *);
static int la_map_set_value (la_t *, Vmap_t *, const char *, VALUE, int);
static int la_parse_loadfile (la_t *);
static int la_parse_chain (la_t *, VALUE *);
static void la_set_CFuncError (la_t *, int);
static void la_set_curMsg (la_t *, const char *);
static void la_set_Errno (la_t *, int);
static int la_peek_nth_byte_nows_inline (la_t *, uint *);

static void la_set_message (la_t *this, int append, const char *msg) {
  if (NULL is msg) return;
  if (append)
    String.append_with (this->message, msg);
  else
    String.replace_with (this->message, msg);
}

static void la_set_message_fmt (la_t *this, int append, const char *fmt, ...) {
  size_t len = VA_ARGS_FMT_SIZE(fmt);
  char bytes[len + 1];
  VA_ARGS_GET_FMT_STR(bytes, len, fmt);
  la_set_message (this, append, bytes);
}

static inline la_string StringNewLen (const char *str, integer len) {
  la_string x;
  SETSTRLEN(x, len);
  SETSTRPTR(x, str);
  return x;
}

static inline la_string StringNew (const char *str) {
  la_string x;
  SETSTRLEN(x, bytelen (str));
  SETSTRPTR(x, str);
  return x;
}

static void la_print_lastring (la_t *this, FILE *fp, la_string s) {
  uint len = GETSTRLEN(s);
  const char *ptr = (const char *) GETSTRPTR(s);
  while (len > 0) {
    this->print_byte (fp, *ptr);
    ptr++;
    --len;
  }
}

static int la_err_ptr (la_t *this, int err) {
  const char *keep = GETSTRPTR(PARSEPTR);
  size_t len = GETSTRLEN(PARSEPTR);

  char *sp = (char *) keep;
  while (sp > this->script_buffer and
      0 is Cstring.byte.in_str (";\n", *(sp - 1)) and
      *(sp - 1) >= ' ')
    sp--;

  size_t n_len = (keep - sp);

  SETSTRPTR(PARSEPTR, sp);
  SETSTRLEN(PARSEPTR, n_len);

  sp = (char *) keep;
  int linenum = 0;
  while (*sp++)
    if (*sp is TOKEN_NL)
      if (++linenum > 9) break;

  n_len += (sp - keep);
  SETSTRLEN(PARSEPTR, n_len);

  la_print_lastring (this, this->err_fp, PARSEPTR);

  SETSTRPTR(PARSEPTR, keep);
  SETSTRLEN(PARSEPTR, len);

  this->print_bytes (this->err_fp, "\n");

  return err;
}

static void la_print_current_line (la_t *this, const char *msg, FILE *fp) {
  const char *sp = GETSTRPTR(PARSEPTR);
  const char *end = sp;
  int max = 80;
  while (max-- and
      sp > this->script_buffer and
      0 is Cstring.byte.in_str (";\n", *(sp - 1)) and
      *(sp - 1) >= ' ')
    sp--;

  while (*end) {
    if (*(end + 1))
      if (Cstring.byte.in_str (";\n", *(end + 1)))
        break;

    end++;
  }

  if (fp is NULL) fp = this->err_fp;

  if (NULL isnot msg) fprintf (fp, "%s", msg);

  for (int i = 0; i < end - sp; i++)
    fprintf (fp, "%c", sp[i]);

  fprintf (fp, "\n");
}

static int la_syntax_error (la_t *this, const char *msg) {
  this->print_fmt_bytes (this->err_fp, "%s\n", msg);
  return la_err_ptr (this, LA_ERR_SYNTAX);
}

static int la_syntax_error_fmt (la_t *this, const char *fmt, ...) {
  size_t len = VA_ARGS_FMT_SIZE (fmt);
  char bytes[len + 1];
  VA_ARGS_GET_FMT_STR(bytes, len, fmt);
  return this->syntax_error (this, bytes);
}

static inline char *sym_key (la_t *this, la_string x) {
  Cstring.cp (this->symKey, MAXLEN_SYMBOL + 1,
      GETSTRPTR(x), GETSTRLEN(x));
  return this->symKey;
}

static inline char *map_key (la_t *this, la_string x) {
  Cstring.cp (this->curMapKey, MAXLEN_SYMBOL + 1,
      GETSTRPTR(x), GETSTRLEN(x));
  return this->curMapKey;
}

static inline char *cur_msg_str (la_t *this, la_string x) {
  Cstring.cp (this->curMsg, MAXLEN_MSG + 1,
      GETSTRPTR(x), GETSTRLEN(x));
  return this->curMsg;
}

static inline int is_space (int c) {
  return (c is ' ') or (c is '\t') or (c is '\r');
}

static inline int is_digit (int c) {
  return (c >= '0' and c <= '9');
}

static inline int is_hexchar (int c) {
  return (c >= '0' and c <= '9') or Cstring.byte.in_str ("abcdefABCDEF", c);
}

static inline int is_lower (int c) {
  return (c >= 'a' and c <= 'z');
}

static inline int is_upper (int c) {
  return (c >= 'A' and c <= 'Z');
}

static inline int is_alpha (int c) {
  return is_lower (c) or is_upper (c);
}

/*
static inline int is_idpunct (int c) {
  return NULL isnot Cstring.byte.in_str (".:_", c);
}
*/

static inline int is_identifier (int c) {
  return is_alpha (c) or c is '_' or is_digit (c);
}

static inline int is_operator (int c) {
  return NULL isnot Cstring.byte.in_str (LA_OPERATORS, c);
}

static inline int is_operator_span (int c) {
  return NULL isnot Cstring.byte.in_str ("=<>&|^+-", c);
}

static tokenState la_save_token_state (la_t *this) {
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

static void la_restore_token_state (la_t *this, tokenState tokenstate) {
  TOKEN     = tokenstate.curToken;
  PREVTOKEN = tokenstate.prevToken;
  TOKENARGS = tokenstate.tokenArgs;
  TOKENVAL  = tokenstate.tokenVal;
  TOKENSYM  = tokenstate.tokenSym;
  TOKENSTR  = tokenstate.tokenStr;
  PARSEPTR  = tokenstate.parsePtr;
}

static int la_peek_token (la_t *this, uint n) {
  tokenState save = SAVE_TOKENSTATE();

  do {
    int err = la_do_next_token (this, 0);
    if (err < TOKEN_EOF) return err;
  } while (n--);

  int retval = TOKEN;
  RESTORE_TOKENSTATE(save);
  return retval;
}

static int la_peek_nth_byte (la_t *this, uint n) {
  if (GETSTRLEN(PARSEPTR) <= n) return TOKEN_EOF;
  return *(GETSTRPTR(PARSEPTR) + n);
}

static int la_get_byte (la_t *this) {
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

static int la_ignore_ws (la_t *this) {
  int c;

  for (;;) {
    c = GET_BYTE();

    if (is_space (c)) {
      RESET_TOKEN;
    } else
      break;
  }

  return c;
}

static int la_peek_nth_byte_nows_nonl (la_t *this, uint *n) {
  int c;
  while (1) {
    c = PEEK_NTH_BYTE(*n);

    if (0 is is_space (c) and c isnot '\n')
      return c;

    *n += 1;
  }

  return TOKEN_EOF;
}

static int la_next_byte_nows_nonl (la_t *this) {
  uint n = 0;
  return PEEK_NTH_BYTE_NOWS_NONL(&n);
}

static int la_next_byte_nows (la_t *this) {
  uint n = 0;
  return PEEK_NTH_BYTE_NOWS_INLINE (&n);
}

static int la_peek_nth_byte_nows_inline (la_t *this, uint *n) {
  int c;
  while (1) {
    c = PEEK_NTH_BYTE(*n);
    ifnot (is_space (c)) return c;
    *n += 1;
  }

  return TOKEN_EOF;
}

static void la_get_span (la_t *this, int (*testfn) (int)) {
  int c;
  do
    c = GET_BYTE();
  while (testfn (c));

  if (c isnot TOKEN_EOF) UNGET_BYTE();
}

static int la_get_opened_block (la_t *this, const char *msg) {
  int bracket = 1;
  int c;
  int prev_c = 0;
  int in_str = 0;
  int in_com = 0;

  while (bracket > 0) {
    c = GET_BYTE();

    if (c is LA_NOTOK or c is TOKEN_EOF) {
      const char *keep = GETSTRPTR(PARSEPTR);
      size_t len = GETSTRLEN(PARSEPTR);

      char *sp = (char *) keep;
      while (sp > this->script_buffer) sp--;
      size_t n_len = (keep - sp);

      size_t n = 0;
      for (n = 0; n < 256 and n < n_len; n++)
        UNGET_BYTE();

      this->print_fmt_bytes (this->err_fp, "unended %s block\n",
          ((Cstring.eq_n (this->curScope->funname, NS_IF_BLOCK, NS_IF_BLOCK_LEN))
            ? "if" :
            (Cstring.eq_n (this->curScope->funname, NS_LOOP_BLOCK, NS_LOOP_BLOCK_LEN))
              ? "loop" :
              this->curScope->funname));

      for (size_t i = 0; i < n; i++)
        this->print_byte (this->err_fp, GET_BYTE());

      SETSTRPTR(PARSEPTR, keep);
      SETSTRLEN(PARSEPTR, len);
      THROW_SYNTAX_ERR(msg);
    }

    if (c is TOKEN_COMMENT) {
      ifnot (in_str) {
        in_com = 1;
        continue;
      }
    }

    if (c is TOKEN_NL) {
      if (in_com) {
        ifnot (in_str) {
          in_com = 0;
          continue;
        }
      }
    }

    if (in_com)
      continue;

    if (c is TOKEN_DQUOTE and prev_c isnot TOKEN_ESCAPE_CHR) {
      if (in_str)
        in_str--;
      else
        in_str++;
      continue;
    }

    prev_c = c;

    if (in_str) continue;

    switch (c) {
      case TOKEN_BLOCK_CLOS: --bracket; break;
      case TOKEN_BLOCK_OPEN: ++bracket; break;
    }
  }

  return LA_OK;
}

static VALUE la_errno_string (la_t *this, VALUE v_err) {
  (void) this;
  int errnum = AS_INT(v_err);
  char buf[256];
  Error.errno_string_s (errnum, buf, 256);
  string *estr = String.new_with (buf);
  return STRING(estr);
}

static VALUE la_errno_name (la_t *this, VALUE v_err) {
  (void) this;
  int errnum = AS_INT(v_err);
  char buf[32];
  Error.errno_name_s (errnum, buf, 32);
  string *estr = String.new_with (buf);
  return STRING(estr);
}

static VALUE la_typeof (la_t *this, VALUE value) {
  (void) this;
  VALUE v;

  if (IS_UFUNC(value.type) or value.type & UFUNCTION_TYPE)
    v = INT(UFUNCTION_TYPE);
  else if (IS_CFUNC(value.type))
    v = INT(CFUNCTION_TYPE);
  else
    v = INT(value.type);
  return v;
}

static VALUE la_typeofArray (la_t *this, VALUE value) {
  (void) this;
  VALUE v = INT(value.type);
  if (value.type isnot ARRAY_TYPE) {
    v = INT(LA_NOTOK);
    return v;
  }

  ArrayType *array = (ArrayType *) AS_ARRAY(value);
  v = INT(array->type);
  return v;
}

static VALUE la_typeAsString (la_t *this, VALUE value) {
  (void) this;
  string *buf = String.new (16);

  switch (value.type) {
    case INTEGER_TYPE: String.append_with_len (buf, "IntegerType", 11); break;
    case FILEPTR_TYPE: String.append_with_len (buf, "FilePtrType", 11); break;
    case NUMBER_TYPE:  String.append_with_len (buf, "NumberType",  10); break;
    case STRING_TYPE:  String.append_with_len (buf, "StringType",  10); break;
    case ARRAY_TYPE:   String.append_with_len (buf, "ArrayType",    9); break;
    case NULL_TYPE:    String.append_with_len (buf, "NullType",     8); break;
    case LIST_TYPE:    String.append_with_len (buf, "ListType",     8); break;
    case MAP_TYPE:     String.append_with_len (buf, "MapType",      7); break;
    case FD_TYPE:      String.append_with_len (buf, "FdType",       6); break;
    case OBJECT_TYPE: {
      object *o = AS_OBJECT(value); String.append_with (buf, o->name);
      break;
    }
    default:
      if (IS_UFUNC(value.type) or value.type & UFUNCTION_TYPE)
        String.append_with_len (buf, "FunctionType", 12);
      else if (IS_CFUNC(value.type))
        String.append_with_len (buf, "CFunctionType", 13);
      else
        String.append_with_len (buf, "UnknownType", 11);
      break;
  }

  VALUE v = STRING (buf);
  return v;
}

static VALUE la_typeArrayAsString (la_t *this, VALUE value) {
  VALUE v;
  if (value.type isnot ARRAY_TYPE)
    v.type = LA_NOTOK;
  else {
    ArrayType *array = (ArrayType *) AS_ARRAY(value);
    v.type = array->type;
  }

  return la_typeAsString (this, v);
}

static void la_release_qualifiers (la_t *this) {
  ifnot (NULL is this->qualifiers) {
    if (this->qualifierCount) {
      this->qualifierCount--;
      return;
    }

    map_release (MAP(this->qualifiers));
  }

  this->qualifiers = NULL;
}

static VALUE la_set_qualifiers (la_t *this, VALUE v_qual) {
  ifnot (IS_MAP(v_qual)) C_THROW(LA_ERR_TYPE_MISMATCH, "awaiting a map as qualifiers");
  la_release_qualifiers (this);
  this->qualifiers = AS_MAP(v_qual);
  return OK_VALUE;
}

static VALUE la_qualifiers (la_t *this) {
  if (NULL is this->qualifiers)
    return NULL_VALUE;

  this->qualifierCount++;

  VALUE val = la_copy_map (MAP(this->qualifiers));
  return val;
}

static Vmap_t *la_get_qualifiers (la_t *this) {
  VALUE v = la_qualifiers (this);
  if (IS_NULL(v)) return NULL;
  this->qualifierCount--;
  return AS_MAP(v);
}

static VALUE la_qualifier_exists (la_t *this, VALUE v_key) {
  if (NULL is this->qualifiers)
    return FALSE_VALUE;

  this->qualifierCount++;

  ifnot (IS_STRING(v_key)) C_THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");

  return INT(Vmap.key_exists (this->qualifiers, AS_STRING_BYTES(v_key)));
}

static int la_C_qualifier_exists (la_t *this, const char *key) {
  return Vmap.key_exists (this->qualifiers, key);
}

static VALUE la_qualifier (la_t *this, VALUE v_key, VALUE v_defval) {
  if (NULL is this->qualifiers)
    return la_copy_value (v_defval);

  this->qualifierCount++;

  ifnot (IS_STRING(v_key)) C_THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");

  VALUE *v = (VALUE *) Vmap.get (this->qualifiers, AS_STRING_BYTES(v_key));

  if (v is NULL)
    return la_copy_value (v_defval);

  return la_copy_value ((*v));
}

static VALUE la_get_qualifier (la_t *this, const char *key, VALUE v_defval) {
  if (NULL is this->qualifiers)
    return v_defval;

  VALUE *v = (VALUE *) Vmap.get (this->qualifiers, key);
  if (NULL is v)
    return v_defval;
  return *v;
}

static object *la_object_new (ObjectRelease o_release, ObjectToString o_tostr, const char *name, VALUE value) {
  object *o = Alloc (sizeof (object));
  o->release = o_release;
  o->toString = o_tostr;
  ifnot (NULL is name)
    Cstring.cp (o->name, MAXLEN_TYPENAME + 1, name, MAXLEN_TYPENAME);
  else
    Cstring.cp (o->name, MAXLEN_TYPENAME + 1, "ObjectType", MAXLEN_TYPENAME);
  o->value = value;
  return o;
}

static VALUE la_fclose (la_t *this, VALUE fp_val) {
  VALUE result = NOTOK_VALUE;

  if (fp_val.type is NULL_TYPE) return result;

  if (fp_val.refcount < 0) return OK_VALUE;

  FILE *fp = AS_FILEPTR(fp_val);

  if (NULL is fp) return result;

  ifnot (NULL is this)
    la_set_Errno (this, 0);

  if (0 isnot fclose (fp)) {
    ifnot (NULL is this)
      la_set_Errno (this, errno);
  } else
    result = OK_VALUE;

  ifnot (NULL is this) {
    ifnot (this->funcState & OBJECT_RELEASE_STATE) {
      object *o = AS_OBJECT(fp_val);
      free (o);
    }
  }

  fp = NULL;
  ifnot (NULL is fp_val.sym)
    fp_val.sym->value = NULL_VALUE;

  fp_val.sym = NULL;
  fp_val = NULL_VALUE;

  return result;
}

static VALUE la_fflush (la_t *this, VALUE fp_val) {
  VALUE result = NOTOK_VALUE;

  if (fp_val.type is NULL_TYPE) return result;

  ifnot (IS_FILEPTR(fp_val)) C_THROW(LA_ERR_TYPE_MISMATCH, "awaiting a file pointer");

  FILE *fp = AS_FILEPTR(fp_val);
  if (NULL is fp) return result;

  la_set_Errno (this, 0);

  int retval = fflush (fp);
  if (retval) {
    la_set_Errno (this, errno);
    return result;
  }

  return OK_VALUE;
}

static VALUE la_fopen (la_t *this, VALUE fn_value, VALUE mod_value) {
  ifnot (IS_STRING(fn_value))  C_THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_STRING(mod_value)) C_THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");

  la_set_Errno (this, 0);

  char *fn = AS_STRING_BYTES(fn_value);
  char *mode = AS_STRING_BYTES(mod_value);
  FILE *fp = fopen (fn, mode);

  if (NULL is fp) {
    la_set_Errno (this, errno);
    return NULL_VALUE;
  }

  VALUE v = FILEPTR(fp);
  object *o = la_object_new (la_fclose, NULL, "FilePtrType", v);
  return FILEPTR(o);
}

static VALUE list_release (la_t *this, VALUE v_list) {
  ifnot (IS_LIST(v_list)) C_THROW(LA_ERR_TYPE_MISMATCH, "awaiting a list");
  listType *list = AS_LIST(v_list);
  listNode *it = list->head;

  while (it) {
    listNode *tmp = it->next;
    la_release_val (this, *(it->value));
    free (it->value);
    free (it);
    it = tmp;
  }

  free (list);
  return OK_VALUE;
}

static VALUE list_set (la_t *this, VALUE v_list, VALUE v_item, int what) {
  ifnot (IS_LIST(v_list)) C_THROW(LA_ERR_TYPE_MISMATCH, "awaiting a list");
  listType *list = AS_LIST(v_list);

  listNode *node = Alloc (sizeof (listNode));

  VALUE v;
  if (v_item.sym isnot NULL)
    v = la_copy_value (v_item);
  else
    v = v_item;

  VALUE *vp = Alloc (sizeof (VALUE));
  *vp = v;
  node->value = vp;
  if (what)
    DListAppend(list, node);
  else
    DListPrepend(list, node);

  return OK_VALUE;
}

static VALUE list_prepend (la_t *this, VALUE v_list, VALUE v_item) {
  return list_set (this, v_list, v_item, 0);
}

static VALUE list_append (la_t *this, VALUE v_list, VALUE v_item) {
  return list_set (this, v_list, v_item, 1);
}

static VALUE list_insert_at (la_t *this, VALUE v_list, VALUE v_idx, VALUE v_v) {
  ifnot (IS_LIST(v_list)) C_THROW(LA_ERR_TYPE_MISMATCH, "awaiting a list");
  ifnot (IS_INT(v_idx)) C_THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer");

  listType *list = AS_LIST(v_list);
  int idx = AS_INT(v_idx);
  if (0 > idx)
    idx += list->num_items;

  if (idx <= -1 or idx > list->num_items) {
    char msg[256]; Cstring.cp_fmt (msg, 256,
      "index %d > than %d length, or less or equal than -1", idx, list->num_items);
    C_THROW(LA_ERR_OUTOFBOUNDS, msg);
  }

  if (idx is 0)
    return list_set (this, v_list, v_v, 0);
  if (idx is list->num_items)
    return list_set (this, v_list, v_v, 1);

  idx--;

  listNode *node = Alloc (sizeof (listNode));
  VALUE v = la_copy_value (v_v);
  VALUE *vp = Alloc (sizeof (VALUE));
  *vp = v;
  node->value = vp;
  DListInsertAt(list, node, idx);
  return OK_VALUE;
}

static int la_parse_list_get (la_t *this, VALUE *vp) {
  VALUE v_list = TOKENVAL;

  NEXT_TOKEN();
  int c = TOKEN;

  if (c isnot TOKEN_INDEX_OPEN) {
    if (this->exprList and this->objectState & MAP_ASSIGNMENT)
      *vp = la_copy_value (v_list);
    else
      *vp = v_list;

    return LA_OK;
  }

  NEXT_TOKEN();

  VALUE ix;
  int err = la_parse_expr (this, &ix);
  THROW_ERR_IF_ERR(err);
  THROW_SYNTAX_ERR_IF(TOKEN isnot TOKEN_INDEX_CLOS,
    "list get: awaiting ]");
  THROW_SYNTAX_ERR_IF(ix.type isnot INTEGER_TYPE,
    "awaiting an integer expression, when getting list index");

  listType *list = AS_LIST(v_list);
  int idx = AS_INT(ix);
  if (0 > idx) idx += list->num_items;
  THROW_OUT_OF_BOUNDS_IF(idx <= -1 or idx >= list->num_items, "list index [%d] is out of bounds [%d]", idx, list->num_items);

  listNode *node = DListGetAt(list, listNode, idx);
  //if (NULL is node) return LA_NOTOK;

  VALUE *val = node->value;

  if (this->exprList and 0 is this->fmtRefcount) {
    *vp = *val;
    if (vp->type is STRING_TYPE)
      vp->refcount++;
  } else
    *vp = la_copy_value (*val);

  NEXT_TOKEN();
  return LA_OK;
}

static int la_parse_list_set (la_t *this) {
  VALUE v_list = TOKENVAL;

  int c = NEXT_BYTE_NOWS_NONL();

  if (c is '=') {
    sym_t *sym = v_list.sym;
    if (sym isnot NULL) {
      sym->type = NULL_TYPE;
      sym->value = NULL_VALUE;
    }

    this->curMsg[0] = '\0';
    VALUE rv = list_release (this, v_list);
    THROW_SYNTAX_ERR_FMT_IF(IS_NULL(rv), "%s\n", this->curMsg);

    return LA_MMT_REASSIGN;
  }

  NEXT_TOKEN();
  THROW_SYNTAX_ERR_IF(TOKEN isnot TOKEN_INDEX_OPEN, "list set, awaiting [");

  NEXT_TOKEN();

  VALUE ix;
  int err = la_parse_expr (this, &ix);
  THROW_ERR_IF_ERR(err);
  THROW_SYNTAX_ERR_IF(ix.type isnot INTEGER_TYPE, "awaiting an integer expression, when getting list index");
  THROW_SYNTAX_ERR_IF(TOKEN isnot TOKEN_INDEX_CLOS, "list set: awaiting ]");

  NEXT_TOKEN();
  THROW_SYNTAX_ERR_IF(TOKEN isnot TOKEN_ASSIGN, "syntax error while setting list, awaiting =");

  NEXT_TOKEN();

  VALUE v;
  err = la_parse_expr (this, &v);
  THROW_ERR_IF_ERR(err);

  listType *list = AS_LIST(v_list);
  int idx = AS_INT(ix);
  if (0 > idx) idx += list->num_items;
  THROW_OUT_OF_BOUNDS_IF(idx <= -1 or idx >= list->num_items, "list index [%d] is out of bounds [%d]", idx, list->num_items);

  listNode *node = DListGetAt(list, listNode, idx);
  la_release_val (this, *node->value);

  switch (v.refcount) {
    case STRING_LITERAL:
      v.refcount = 0;
      // fallthrough

    case 0:
      *node->value = v;
      break;

    default:
      *node->value = la_copy_value (v);
  }

  return LA_OK;
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
    VALUE cpv = la_copy_value (*node->value);
    *vp = cpv;
    new->value = vp;
    DListAppend(l_new, new);
  }

  return v_list;
}

static VALUE list_delete_at (la_t *this, VALUE v_list, VALUE v_idx) {
  ifnot (IS_LIST(v_list)) C_THROW(LA_ERR_TYPE_MISMATCH, "awaiting a list");
  ifnot (IS_INT(v_idx)) C_THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer");

  listType *list = AS_LIST(v_list);

  int idx = AS_INT(v_idx);
  if (0 > idx)
    idx += list->num_items;

  if (idx <= -1 or idx >= list->num_items) {
    char msg[256]; Cstring.cp_fmt (msg, 256,
      "index %d >= than %d length, or less or equal than -1", idx, list->num_items);
    C_THROW(LA_ERR_OUTOFBOUNDS, msg);
  }

  listNode *node = DListPopAt(list, listNode, idx);
  la_release_val (this, *node->value);
  free (node->value);
  free (node);
  return OK_VALUE;
}

static VALUE list_clear (la_t *this, VALUE v_list) {
  ifnot (IS_LIST(v_list)) C_THROW(LA_ERR_TYPE_MISMATCH, "awaiting a list");

  listType *list = AS_LIST(v_list);

  listNode *node = NULL;
  while (1) {
    node = DListPopTail(list, listNode);
    if (NULL == node)
      break;

    la_release_val (this, *node->value);
    free (node->value);
    free (node);
  }

  return OK_VALUE;
}
static VALUE list_pop_at (la_t *this, VALUE v_list, VALUE v_idx) {
  ifnot (IS_LIST(v_list)) C_THROW(LA_ERR_TYPE_MISMATCH, "awaiting a list");
  ifnot (IS_INT(v_idx)) C_THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer");

  listType *list = AS_LIST(v_list);

  int idx = AS_INT(v_idx);
  if (0 > idx)
    idx += list->num_items;

  if (idx <= -1 or idx >= list->num_items) {
    char msg[256]; Cstring.cp_fmt (msg, 256,
      "index %d >= than %d length, or less or equal than -1", idx, list->num_items);
    C_THROW(LA_ERR_OUTOFBOUNDS, msg);
  }

  listNode *node = DListPopAt(list, listNode, idx);

  VALUE v = la_copy_value (*node->value);
  la_release_val (this, *node->value);
  free (node->value);
  free (node);
  return v;
}

static int la_parse_list (la_t *this, VALUE *vp, int is_fun) {
  int err;

  NEXT_TOKEN();
  ifnot (is_fun)
    THROW_SYNTAX_ERR_IF(TOKEN isnot TOKEN_INDEX_OPEN,  "parsing list: awaiting [");
  else
    THROW_SYNTAX_ERR_IF(TOKEN isnot TOKEN_PAREN_OPEN,  "parsing list: awaiting (");

  VALUE v_list = LIST_NEW();
  if (this->exprList)
      v_list.refcount--;

  NEXT_TOKEN();
  if (TOKEN is TOKEN_PAREN_CLOS) {
    THROW_SYNTAX_ERR_IFNOT(is_fun, "parsing list: unexpected token )");
    *vp = v_list;
    NEXT_TOKEN ();
    return LA_OK;
  } else if (TOKEN is TOKEN_INDEX_CLOS) {
    THROW_SYNTAX_ERR_IF(is_fun, "parsing list: unxpected token ]");
    *vp = v_list;
    NEXT_RAW_TOKEN ();
    THROW_SYNTAX_ERR_IF(TOKEN isnot TOKEN_BLOCK_CLOS, "parsing list: awaiting }");
    NEXT_TOKEN ();
    return LA_OK;
  }

  while (1) {
    VALUE v;
    err = la_parse_expr (this, &v);
    THROW_ERR_IF_ERR(err);

    list_append (this, v_list, v);

    if (TOKEN is TOKEN_COMMA) {
      NEXT_TOKEN();
      continue;
    }

    break;
  }

  *vp = v_list;

  if (is_fun) {
    THROW_SYNTAX_ERR_IF(TOKEN isnot TOKEN_PAREN_CLOS,  "parsing list: awaiting )");
  } else {
    THROW_SYNTAX_ERR_IF(TOKEN isnot TOKEN_INDEX_CLOS,  "parsing list: awaiting ]");
    NEXT_RAW_TOKEN();
    THROW_SYNTAX_ERR_IF(TOKEN isnot TOKEN_BLOCK_CLOS, "parsing list: awaiting }");
  }

  NEXT_TOKEN();

  return LA_OK;
}

static int la_parse_append (la_t *this, VALUE *vp) {
  VALUE v;
  NEXT_TOKEN();
  int err = la_parse_expr (this, &v);
  THROW_ERR_IF_ERR(err);

  if (TOKEN isnot TOKEN_IN)
    THROW_SYNTAX_ERR("error parsing append, awaiting in");

  NEXT_TOKEN();

  err = la_parse_expr (this, vp);
  THROW_ERR_IF_ERR(err);

  err = LA_OK;

  switch (vp->type) {
    case ARRAY_TYPE: {
      ArrayType *array = (ArrayType *) AS_ARRAY((*vp));
      if (v.type isnot array->type)
        THROW_SYNTAX_ERR("value type and array type are not the same");

      if (v.type is STRING_TYPE or v.sym isnot NULL) {
        if (v.refcount is STRING_LITERAL)
          v.refcount = 0;
        else {
          VALUE t = la_copy_value (v);
          v = t;
        }
      }

      array = ARRAY_APPEND(array, v);
      *vp = ARRAY(array);
      break;
    }

    case LIST_TYPE:
      list_set (this, *vp, v, 1); // always succeeds
      break;

    case MAP_TYPE: {
      ifnot (Cstring.eq ("as", sym_key (this, TOKENSTR)))
        THROW_SYNTAX_ERR("error while appending to map, awaiting as");

      this->curState |= MALLOCED_STRING_STATE;
      NEXT_TOKEN();
      this->curState &= ~MALLOCED_STRING_STATE;
      THROW_SYNTAX_ERR_IF(TOKEN isnot TOKEN_STRING, "error while appending to map, awaiting a string as a key");
      string *s = AS_STRING(TOKENVAL);
      char *key = s->bytes;
      Vmap_t *map = AS_MAP((*vp));
      err = la_map_set_value (this, map, key, v, 1);
      NEXT_TOKEN();
      break;
    }

    case STRING_TYPE: {
      if (Cstring.eq ("as", sym_key (this, TOKENSTR)))
        TOKEN = TOKEN_AS;

      if (TOKEN isnot TOKEN_AS or
          vp->refcount is STRING_LITERAL)
        this->objectState |= OBJECT_APPEND;

      VALUE rv = la_add (this, *vp, v);
      THROW_SYNTAX_ERR_IF(IS_NULL(rv), "error appending to a string");

      if (TOKEN is TOKEN_AS) {
        NEXT_RAW_TOKEN();
        THROW_SYNTAX_ERR_IF(TOKEN isnot TOKEN_SYMBOL, "awaiting a symbol after an as");
        sym_t * sym = la_lookup_symbol (this, TOKENSTR);
        THROW_SYNTAX_ERR_IF(sym isnot NULL, "can not reassign to a symbol with the append method");
        TOKENSYM = la_define_symbol (this, this->curScope, sym_key (this, TOKENSTR), STRING_TYPE, rv, 0);
        *vp = rv;
        NEXT_TOKEN();
      }
      break;
    }

    default: {
      VALUE rv = la_add (this, *vp, v);
      THROW_SYNTAX_ERR_IF(IS_NULL(rv), "append error");

      if (vp->sym isnot NULL)
        vp->sym->value = rv;
      *vp = rv;
    }
  }

  return err;
}

static int la_parse_format (la_t *, VALUE *);
static int la_parse_format (la_t *this, VALUE *vp) {
  int err;
  string *str = String.new (32);

  int c = la_ignore_ws (this);

  if (c isnot TOKEN_PAREN_OPEN) {
    this->print_bytes (this->err_fp, "format error, awaiting (\n");
    la_err_ptr (this, LA_NOTOK);
    return LA_NOTOK;
  }

  int is_expr = 0;

  c= la_ignore_ws (this);

  if (c isnot TOKEN_DQUOTE) {
    UNGET_BYTE();
    NEXT_TOKEN();
    c = TOKEN;
    VALUE v;
    this->curState |= MALLOCED_STRING_STATE;
    err = la_parse_expr (this, &v);
    this->curState &= ~MALLOCED_STRING_STATE;
    THROW_ERR_IF_ERR(err);

    if (v.type is STRING_TYPE) {
      c = TOKEN;
      la_string savepc = PARSEPTR;
      string *v_s = AS_STRING(v);
      /* very crude algorithm (but for a minor case (evaluation of a string
       * that might include embedded interpolation syntax)) */
      /* Update: mind change. Always include the required characters for the
         evaluation, and remove ambiguities and false interpretation. */

      size_t len = v_s->num_bytes + 4;
      char buf[len + 1];
      buf[0] = '('; buf[1] = '"';
      Cstring.cp (buf + 2, len + 1, v_s->bytes, v_s->num_bytes);
      buf[len - 2] = '"'; buf[len - 1] = ')'; buf[len] = '\0';

      if (v.sym is NULL and
          v.refcount isnot MALLOCED_STRING and
          0 is (this->objectState & (ARRAY_MEMBER|MAP_MEMBER)))
        String.release (v_s);

      PARSEPTR = StringNew (buf);
      err = la_parse_format (this, &v);
      THROW_ERR_IF_ERR(err);

      PARSEPTR = savepc;

      string *vs = AS_STRING(v);

      String.append_with_len (str, vs->bytes, vs->num_bytes);
      String.release (vs);

      is_expr = 1;
      goto theend;
    }
  }

  if (c isnot TOKEN_DQUOTE)
    THROW_SYNTAX_ERR("format error, awaiting \"");

  err = la_parse_fmt (this, str, 0);
  THROW_ERR_IF_ERR(err);

theend:
  *vp = STRING(str);

  ifnot (is_expr) {
    NEXT_TOKEN();
    c = TOKEN;
  }

  if (c isnot TOKEN_PAREN_CLOS)
    THROW_SYNTAX_ERR("format error, awaiting )");

  NEXT_TOKEN();
  c = TOKEN;

  if (this->fmtRefcount)
    this->fmtState |= FMT_LITERAL;
  else
    if (this->exprList)
      vp->refcount = STRING_LITERAL;
//     vp->refcount = -1;
// CHANGE      vp->refcount = STRING_LITERAL;

  return LA_OK;
}

static VALUE la_fileno (la_t *this, VALUE fp_val) {
  (void) this;
  ifnot (IS_FILEPTR(fp_val)) C_THROW(LA_ERR_TYPE_MISMATCH, "awaiting a file pointer");
  FILE *fp = AS_FILEPTR(fp_val);
  la_set_Errno (this, 0);
  int fd = fileno (fp);
  VALUE v = INT(fd);
  if (fd is -1)
    la_set_Errno (this, errno);

  return v;
}

static VALUE la_len (la_t *this, VALUE value) {
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
      Vmap_t *map = AS_MAP(value);
      result = INT(Vmap.num_keys (map));
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

static VALUE string_release (VALUE value) {
  VALUE result = INT(LA_OK);

  if (value.refcount > 0) goto theend;

  if (value.refcount < 0)
    if (value.refcount isnot STRING_LITERAL)
      return result;

  String.release (AS_STRING(value));

  value = NULL_VALUE;

theend:
  value.refcount--;
  return result;
}

static VALUE object_release (la_t *this, VALUE value) {
  VALUE result = INT(LA_OK);

  if (value.refcount < 0)
    return result;

  if (value.refcount > 0) goto theend;

  if (value.type is NULL_TYPE) goto theend;

  object *o = AS_OBJECT(value);

  ifnot (NULL is o->release) {
    ifnot (NULL is this) this->funcState |= OBJECT_RELEASE_STATE;
    o->release (this, value);
    ifnot (NULL is this) this->funcState &= ~OBJECT_RELEASE_STATE;
  }

  free (o);

  value = NULL_VALUE;

theend:
  value.refcount--;
  return result;
}

static VALUE la_release_val (la_t *this, VALUE value) {
  (void) this;
  VALUE result = INT(LA_OK);
  if (value.type < STRING_TYPE) return result;

  void *obj = NULL;

  switch (value.type) {
    case   POINTER_TYPE: obj = AS_VOID_PTR(value); break;
    case     ARRAY_TYPE: return array_release (value);
    case    STRING_TYPE: return string_release (value);
    case       MAP_TYPE: return map_release (value);
    case        FD_TYPE:
    case   FILEPTR_TYPE:
    case      LIST_TYPE:
    case    OBJECT_TYPE: return object_release (this, value);
    default:
      return result;
  }

  ifnot (NULL is obj) {
    free (obj);
    value = NULL_VALUE;
  }

  obj = NULL;
  return result;
}

static void ns_release_malloced_strings (funT *this) {
  malloced_string *item = this->head;
  while (item) {
    malloced_string *tmp = item->next;
    String.release (item->data);
    free (item);
    item = tmp;
  }

  this->head = NULL;
}

static void fun_release (funT **thisp) {
  if (*thisp is NULL) return;
  funT *this = *thisp;
  Vmap.release (this->symbols);
  ns_release_malloced_strings (this);
  if (this->modules isnot NULL) {

  #ifndef STATIC
    module_so *it = this->modules->head;
    while (it) {
      module_so *tmp = it->next;
      free (it->name);
      it->deinit (it->instance);
      dlclose (it->handle);
      free (it);
      it = tmp;
    }
  #endif

    free (this->modules);
  }

  free (this);
  *thisp = NULL;
}

static void la_release_sym (void *sym) {
  if (sym is NULL) return;

  sym_t *this = (sym_t *) sym;

  VALUE v = this->value;

  if (IS_UFUNC(this->type)) {
    ifnot (v.type & UFUNCTION_TYPE) {
      funT *f = AS_FUNC_PTR(v);
      fun_release (&f);
    }
  } else
    la_release_val (NULL, v);

  free (this);
  this = NULL;
}

static void la_release_unit (void *item) {
  string *str = (string *) item;
  String.release (str);
}

static funT *fun_new (funNewArgs options) {
  funT *uf = Alloc (sizeof (funT));
  Cstring.cp (uf->funname, MAXLEN_SYMBOL, options.name, options.namelen);
  uf->body = options.body;
  uf->nargs = options.nargs;
  uf->prev = options.parent;
  uf->root = options.root;
  uf->next = NULL;
  uf->symbols = Vmap.new (options.num_symbols);
  uf->block_symbols = NULL;
  return uf;
}

static funT *Fun_new (la_t *this, funNewArgs options) {
  funT *f = fun_new (options);

  funT *parent = options.parent;

  if (parent is NULL) {
    this->function = this->curScope = f->root = f;
    this->function->modules = Alloc (sizeof (modules));
    return f;
  }

  f->modules = NULL;
  f->prev = parent;
  f->root = this->function;
  return f;
}

static sym_t *la_define_block_symbol (la_t *this, funT *f, const char *key, int typ, VALUE value, int is_const) {
  (void) this;
  ifnot (key) return NULL;

  sym_t *sym = Alloc (sizeof (sym_t));
  sym->type = typ;
  sym->value = value;
  sym->value.sym = sym;
  sym->is_const = is_const;
  sym->scope = f;

  if (NOTOK is Vmap.set (f->block_symbols, key, sym, la_release_sym, is_const)) {
    free (sym);
    return NULL;
  }

  return sym;
}

static sym_t *la_define_symbol (la_t *this, funT *f, const char *key, int typ, VALUE value, int is_const) {
  if (this->curState & BLOCK_STATE)
    return la_define_block_symbol (this, f, key, typ, value, is_const);

  ifnot (key) return NULL;

  sym_t *sym = Alloc (sizeof (sym_t));
  sym->type = typ;
  sym->value = value;
  sym->value.sym = sym;
  sym->is_const = is_const;
  sym->scope = f;

  if (NOTOK is Vmap.set (f->symbols, key, sym, la_release_sym, is_const)) {
    free (sym);
    return NULL;
  }

  return sym;
}

static int la_define (la_t *this, const char *key, int typ, VALUE val) {
  sym_t *sym = la_define_symbol (this, this->std, (char *) key, typ, val, 1);
  return (NULL is sym ? LA_NOTOK : LA_OK);
}

static int la_define_type (la_t *this, const char *key, int typ, VALUE val) {
  sym_t *sym = la_define_symbol (this, this->datatypes, (char *) key, typ, val, 1);
  return (NULL is sym ? LA_NOTOK : LA_OK);
}

static inline sym_t *ns_lookup_symbol (funT *scope, const char *key) {
  return Vmap.get (scope->symbols, key);
}

static sym_t *la_lookup_symbol (la_t *this, la_string name) {
  char *key = sym_key (this, name);

  sym_t *sym = ns_lookup_symbol (this->std, key);
  ifnot (NULL is sym) return sym;

  sym = ns_lookup_symbol (this->datatypes, key);
  ifnot (NULL is sym) return sym;

  funT *f = this->curScope;

  while (NULL isnot f) {
    ifnot (NULL is f->block_symbols) {
      sym = Vmap.get (f->block_symbols, key);
      ifnot (NULL is sym) return sym;
    }

    sym = ns_lookup_symbol (f, key);

    ifnot (NULL is sym) return sym;

    f = f->prev;
  }

  return NULL;
}

static int la_parse_lambda (la_t *this, VALUE *vp, int justFun) {
  int err;

  Cstring.cp_fmt
    (this->curFunName, MAXLEN_SYMBOL + 1, NS_ANON "_%zd", this->anon_id++);

  this->curFunDef = NULL;

  err = la_parse_func_def (this);
  THROW_ERR_IF_ERR(err);

  this->curFunName[0] = '\0';

  sym_t *sym = this->curSym;
  sym->value = NULL_VALUE;
  funT *lambda = this->curFunDef;
  TOKENARGS = lambda->nargs;

  if (TOKEN isnot TOKEN_PAREN_OPEN)
    THROW_SYNTAX_ERR("lambda error, awaiting (");

  UNGET_BYTE();

  if (justFun)
    return LA_OK;

  err = la_parse_func_call (this, vp, NULL, lambda, TOKENVAL);

  fun_release (&lambda);

  NEXT_TOKEN();

  return err;
}

static int la_consume_string (la_t *this, char chr) {
  int pc;
  int c = 0;

  while (1) {
    pc = c;
    c = GET_BYTE();

    if (c is TOKEN_EOF)
      THROW_SYNTAX_ERR_FMT("error while getting literal string, awaiting %s quote, found EOF",
          (chr is '`' ? "back" : "double"));

    if (c is chr) {
      if (pc is TOKEN_ESCAPE_CHR) continue;
      break;
    }
  }

  TOKEN = TOKEN_STRING;
  return TOKEN;
}

static int la_get_string (la_t *this, char chr) {
  string *str = String.new (8);

  size_t ptrlen = GETSTRLEN(PARSEPTR);
  const char *ptr = GETSTRPTR(PARSEPTR);
  const char *save_ptr = ptr;

  int c = 0;
  int pc;

  if (chr is TOKEN_BQUOTE) {
    if (*ptr is '[') {
      ptr++;
      int depth = 1;

      while (1) {
        THROW_SYNTAX_ERR_IF(*ptr is '\0',
          "error while getting literal string, awaiting ]`");

        c = *ptr++;

        if (c is ']') {
          if (*ptr and *ptr is TOKEN_BQUOTE) {
            depth--;
            ptr++;

            ifnot (depth) break;

            String.append_with_len (str, "]`", 2);
            continue;
          }
        }

        if (c is '`') {
          if (*ptr and *ptr is '[') {
            depth++;
            ptr++;
            String.append_with_len (str, "`[", 2);
            continue;
          }
        }

        String.append_byte (str, c);
      }

      goto attributes;
    }

    while (1) {
      pc = c;
      c = *ptr++;

      THROW_SYNTAX_ERR_IF(c is TOKEN_EOF, "error while getting literal string, awaiting back quote");

      if (c is TOKEN_BQUOTE) {
        if (pc is TOKEN_ESCAPE_CHR) {
          String.clear_at (str, -1);
          String.append_byte (str, c);
          continue;
        }

        break;
      }

      String.append_byte (str, c);
    }

    attributes:
    c = *ptr;

    if (c is 'S') {
      ptr++;

      if (*ptr is '0') {
        ptr++;
        goto theend;
      }

      int num_ws = 1024;

      if ('1' <= *ptr and *ptr <= '9') {
        c = *ptr++;

        num_ws = c - '0';

        while ((c = *ptr)) {
          if ('0' <= c and c <= '9') {
            num_ws = 10 * num_ws + (c - '0');
            ptr++;
            continue;
          }
          break;
        }
      }

      size_t len = str->num_bytes;
      char sptr[len + 1];
      Cstring.cp (sptr, len + 1, str->bytes, len);

      String.clear (str);

      char *p = sptr;

      while ((c = *p++)) {
        if (c isnot TOKEN_NL) {
          String.append_byte (str, c);
          continue;
        }

        String.append_byte (str, c);
        int num_stripped = 0;

        while (*p and num_stripped < num_ws) {
          c = *p++;

          if (is_space (c)) {
            num_stripped++;
            continue;
          }

          String.append_byte (str, c);
          if (c is TOKEN_NL) {
            num_stripped = 0;
            continue;
          }

          break;
        }
      }
    }

  } else {
    while (1) {
      pc = c;
      c = *ptr++;

      if (c is TOKEN_EOF)
        THROW_SYNTAX_ERR("error while getting literal string, awaiting double quote");

      if (pc is TOKEN_ESCAPE_CHR) {
        switch (c) {
          case 'a': String.append_byte (str, '\a'); break;
          case 'b': String.append_byte (str, '\b'); break;
          case 'f': String.append_byte (str, '\f'); break;
          case 't': String.append_byte (str, '\t'); break;
          case 'r': String.append_byte (str, '\r'); break;
          case 'n': String.append_byte (str, '\n'); break;
          case 'v': String.append_byte (str, '\v'); break;
          case 'e': String.append_byte (str,  033); break;
          case '"' :String.append_with (str, "\""); break;
          case '\\':String.append_byte (str, '\\'); c = 0; break;
          default:
           THROW_SYNTAX_ERR_FMT("unknow escape sequence \\\\%c", c);
        }
        continue;
      }

      if (c is TOKEN_ESCAPE_CHR) continue;

      if (c is TOKEN_DQUOTE) break;

      String.append_byte (str, c);
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

static inline int parse_number (la_t *this, int c, int *token_type) {
  *token_type = TOKEN_INTEGER;

  int dot_found = 0;
  int plus_found = 0;
  int minus_found = 0;

  int is_octOrbin = c is '0';

  c = GET_BYTE();

  if (is_octOrbin) {
    ifnot (is_digit (c)) {
      if (c is 'b') {
        *token_type = TOKEN_BINARY;
        RESET_TOKEN;
        c = GET_BYTE();
      }
    } else
      *token_type = TOKEN_OCTAL;
  }

  if (c is '-' or c is '+')
    return LA_NOTOK;

  goto parse;

  for (;;) {
    c = GET_BYTE();

    parse:
    if (c is '-' or '+' is c) {
      if (*token_type isnot TOKEN_NUMBER) return LA_NOTOK;
      if (c is '-') {
        if (minus_found++) return LA_NOTOK;
        else if (plus_found++) return LA_NOTOK;
      }

      int cc = PEEK_NTH_BYTE(0);
      ifnot (is_digit (cc)) return LA_NOTOK;
      continue;
    }

    if (c is '.') {
      *token_type = TOKEN_NUMBER;

      if (dot_found++) return LA_NOTOK;
      ifnot (is_digit (PEEK_NTH_BYTE(0))) return LA_NOTOK;
      continue;
    }

    if (c is 'e' or c is 'E') {
      *token_type = TOKEN_NUMBER;

      int cc = PEEK_NTH_BYTE(0);
      if (0 is is_digit (cc) or
          cc isnot '-' or
          cc isnot '.' or
          cc isnot '+') {
        return LA_NOTOK;
      }

      continue;
    }

    ifnot (is_digit (c)) break;

    if (*token_type is TOKEN_OCTAL)
      if (c > '7')
        THROW_SYNTAX_ERR("not an octal number");

    if (*token_type is TOKEN_BINARY)
      if (c > '1')
        THROW_SYNTAX_ERR("not a binary number");
  }

  if (c isnot TOKEN_EOF) UNGET_BYTE();

  return LA_OK;
}

static int la_parse_directive (la_t *this, VALUE *vp, int tok) {
  int err;

  this->curState |= MALLOCED_STRING_STATE;
  NEXT_TOKEN();
  if (TOKEN is TOKEN_PAREN_OPEN)
    NEXT_TOKEN();
  this->curState &= ~MALLOCED_STRING_STATE;

  VALUE v;
  err = la_parse_expr (this, &v);
  if (TOKEN is TOKEN_PAREN_CLOS)
    NEXT_TOKEN();

  THROW_ERR_IF_ERR(err);

  switch (tok) {
    case TOKEN_D_ISDEFINED:
      THROW_SYNTAX_ERR_IF(v.type isnot STRING_TYPE,
        "error while parsing is defined directive: awaiting a string");
      *vp = INT(NULL isnot ns_lookup_symbol (this->macros, AS_STRING_BYTES(v)));
      return OK;

    case TOKEN_D_DEFINE:
      THROW_SYNTAX_ERR_IF(TOKEN isnot TOKEN_AS,
        "error while parsing define directive: awaiting as");

      this->curState |= MALLOCED_STRING_STATE;
      NEXT_TOKEN();
      this->curState &= ~MALLOCED_STRING_STATE;

      VALUE as;
      err = la_parse_expr (this, &as);
      THROW_ERR_IF_ERR(err);
      THROW_SYNTAX_ERR_IF(as.type isnot STRING_TYPE,
        "error while parsing is defined directive: awaiting a string");
      THROW_SYNTAX_ERR_FMT_IF(NULL isnot ns_lookup_symbol (this->macros, AS_STRING_BYTES(as)),
        "error while parsing define directive: %s, is already defined", AS_STRING_BYTES(as));
      THROW_SYNTAX_ERR_FMT_IF(NULL is la_define_symbol (this, this->macros, AS_STRING_BYTES(as), v.type, v, 1),
        "error while parsing define directive: %s, cannot define", AS_STRING_BYTES(as));
      return OK;

    case TOKEN_D_UNDEF: {
      THROW_SYNTAX_ERR_IF(v.type isnot STRING_TYPE,
        "error while parsing undef directive: awaiting a string");

      sym_t *sym = Vmap.pop (this->macros->symbols, AS_STRING_BYTES(v));
      THROW_SYNTAX_ERR_FMT_IF(NULL is sym,
        "error while parsing is undef directive: %s, is not defined", AS_STRING_BYTES(v));
      la_release_sym (sym);
      return OK;
    }

    case TOKEN_D_GET: {
      THROW_SYNTAX_ERR_IF(v.type isnot STRING_TYPE,
        "error while parsing get directive: awaiting a string");

      sym_t *sym = ns_lookup_symbol (this->macros, AS_STRING_BYTES(v));
      if (NULL is sym) {
        *vp = NULL_VALUE;
        return OK;
      }

      *vp = sym->value;

      switch (TOKEN) {
        case TOKEN_INDEX_OPEN:
          switch (vp->type) {
            case STRING_TYPE:
              TOKEN = TOKEN_STRING;
              TOKENVAL = *vp;
              UNGET_BYTE();
              return la_parse_primary (this, vp);

            case ARRAY_TYPE:
              TOKEN = TOKEN_ARRAY;
              TOKENVAL = *vp;
              UNGET_BYTE();
              return la_parse_primary (this, vp);

            default:
              la_release_val (this, *vp);
              THROW_SYNTAX_ERR("unexpected open index");
          }

        case TOKEN_DOT:
          THROW_SYNTAX_ERR_IF(vp->type isnot MAP_TYPE, "awaiting a map");
          TOKEN = TOKEN_MAP;
          TOKENVAL = *vp;
          UNGET_BYTE();
          return la_parse_primary (this, vp);
      }

      *vp = la_copy_value (sym->value);

      return OK;
    }

    default:
      THROW_SYNTAX_ERR("unknown directive");
  }

  return OK;
}

static int la_get_annotated_block (la_t *this, string *s) {
  int c = GET_BYTE();

  THROW_SYNTAX_ERR_IF(c isnot ' ' and c isnot TOKEN_NL, "error while getting annotated block: awaiting a space");

  int prevc;

  while (1) {
    prevc = c;
    c = GET_BYTE();
    THROW_SYNTAX_ERR_IF(c is TOKEN_EOF, "unended annotated block");

    if (c is TOKEN_COMMENT and prevc is TOKEN_NL)
      break;

    if (s)
      String.append_byte (s, c);
  }

  c = GET_BYTE();

  if (c is ' ') {
    int orig_len = GETSTRLEN(PARSEPTR);
    const char *ptr = GETSTRPTR(PARSEPTR);

    if (Cstring.eq_n (ptr, "as ", 3)) {
      const char *p = ptr + 3;
      int idx = 0;
      while (is_identifier (*p)) {
        if (idx++ is MAXLEN_SYMBOL)
          THROW_SYNTAX_ERR("annotated identifier exceeded maximum length");

        p++;
        if (*p is ' ') {
          while (*p isnot TOKEN_NL) p++;
          THROW_SYNTAX_ERR_IF(c is TOKEN_EOF, "unended annotated block");
          break;
        }

        if (*p is TOKEN_NL) break;
      }

      if (s and idx)
        for (int i = 0; i < idx + 3; i++)
          String.append_byte (s, ptr[i]);

      SETSTRPTR(PARSEPTR, p);
      SETSTRLEN(PARSEPTR, orig_len - (p - ptr));
    }
  }

  return c;
}

static int la_do_next_token (la_t *this, int israw) {
  int err;

  TOKENSYM = NULL;
  int token = TOKEN;

  RESET_TOKEN;

  int c = la_ignore_ws (this);

  if (c is TOKEN_COMMENT) {
    c = GET_BYTE();

    switch (c) {
      case TOKEN_AT:
        c = la_get_annotated_block (this, NULL);
        THROW_ERR_IF_ERR(c);
        return la_do_next_token (this, israw);

      case 'i':
      case 'd':
      case 'u':
      case 'g': {
        char *sp = (char *) GETSTRPTR(PARSEPTR) - 1;
        int tok = 0;
        int n = 0;

        if (Cstring.eq_n (sp, "isdefined ", 10)) {
          tok = TOKEN_D_ISDEFINED;
          n = 9;
        } else if (Cstring.eq_n (sp, "define ", 7)) {
          tok = TOKEN_D_DEFINE;
          n = 6;
        } else if (Cstring.eq_n (sp, "undef ", 6)) {
          tok = TOKEN_D_UNDEF;
          n = 5;
        } else if (Cstring.eq_n (sp, "get ", 4)) {
          tok = TOKEN_D_GET;
          n = 3;
        }

        ifnot (n) goto consume_comment;

        for (int i = 0; i < n; i++) GET_BYTE();

        TOKEN = tok;
        return tok;
      }
    }

    consume_comment:
    while (c >= 0 and c isnot TOKEN_NL)
      c = GET_BYTE();

    TOKEN = c;
    return TOKEN;
  }

  if (is_alpha (c) or c is '_') {
    la_get_span (this, is_identifier);

    TOKEN = TOKEN_SYMBOL;

    if (GETSTRLEN(TOKENSTR) > MAXLEN_SYMBOL)
      THROW_SYNTAX_ERR_FMT("%s: exceeds maximum length (%d) of an identifier",
         cur_msg_str (this, TOKENSTR), MAXLEN_SYMBOL);

    if (israw)
      return TOKEN;

    TOKENSYM = la_lookup_symbol (this, TOKENSTR);

    ifnot (TOKENSYM)
      return TOKEN;

    TOKEN = TOKENSYM->type & 0xff;

    if (TOKEN is TOKEN_BINOP) { /* is, isnot, and, or */
      TOKEN = TOKENSYM->type;
      TOKENVAL = TOKENSYM->value;
      return TOKEN;
    }

    TOKENARGS = (TOKENSYM->type >> 8) & 0xff;
    TOKENSYM->value.sym = TOKENSYM;
    TOKENVAL = TOKENSYM->value;

    if (AS_VOID_PTR(TOKENVAL) is la_parse_array_def)
      TOKEN = TOKEN_ARYDEF;

    return TOKEN;
  }

  if (is_digit (c) or (c is '-' and is_digit (PEEK_NTH_BYTE(0)) and token isnot INTEGER_TYPE)) {
    if (c is '0' and NULL isnot Cstring.byte.in_str ("xX", PEEK_NTH_BYTE(0))
        and is_hexchar (PEEK_NTH_BYTE(1))) {
      GET_BYTE();
      RESET_TOKEN;
      la_get_span (this, is_hexchar);
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
    la_get_span (this, is_operator_span);

    TOKENSYM = ns_lookup_symbol (this->std, sym_key (this, TOKENSTR));

    if (TOKENSYM) {
      TOKEN = TOKENSYM->type;
      TOKENVAL = TOKENSYM->value;
    } else
      TOKEN = TOKEN_SYNTAX_ERR;

    return TOKEN;
  }

  if (c is TOKEN_BQUOTE or c is TOKEN_DQUOTE) {
    if (this->curState & CONSUME_STATE) {
      err = la_consume_string (this, c);
      THROW_ERR_IF_ERR(err);
      return TOKEN;
    }

    return la_get_string (this, c);
  }

  if (c is TOKEN_BLOCK_OPEN) {
    if (IS_NEXT_BYTE(TOKEN_INDEX_OPEN)) {
      TOKEN = TOKEN_LIST_NEW;
      return TOKEN;
    }

    RESET_TOKEN;
    err = la_get_opened_block (this, "unended opened block");
    THROW_ERR_IF_ERR(err);

    IGNORE_LAST_TOKEN;
    TOKEN = TOKEN_BLOCK;
    return TOKEN;
  }

  if (c is TOKEN_COLON or c is TOKEN_DOLLAR) {
    TOKEN = c;
    return TOKEN;
  }

  if (c is TOKEN_SQUOTE) {
    c = GET_BYTE();
    if (c is TOKEN_ESCAPE_CHR) {
      if (PEEK_NTH_BYTE(0) is 'x') {
        GET_BYTE();

        THROW_SYNTAX_ERR_IF(PEEK_NTH_BYTE(0) isnot '{',
          "error while parsing char, awaiting {");

        GET_BYTE();
        RESET_TOKEN;

        la_get_span (this, is_hexchar);
        c = GET_BYTE();
        THROW_SYNTAX_ERR_IF(c isnot '}', "error while parsing char, awaiting }");

        IGNORE_LAST_TOKEN;

        c = GET_BYTE();
        THROW_SYNTAX_ERR_IF(c isnot TOKEN_SQUOTE, "error while parsing char, awaiting '");

        IGNORE_LAST_TOKEN;
        TOKEN = TOKEN_HEX_CHAR;
        return TOKEN;
      }

      GET_BYTE();
    }

    int max = 4;

    do {
      c = GET_BYTE();
      if (c is TOKEN_SQUOTE) {
        IGNORE_FIRST_TOKEN;
        IGNORE_LAST_TOKEN;
        TOKEN = TOKEN_CHAR;
        return TOKEN;
      }
    } while (--max isnot 0);

    RESET_TOKEN;
    return LA_ERR_SYNTAX;
  }

  TOKEN = c;
  return TOKEN;
}

static void stack_push (la_t *this, VALUE x) {
  this->stackVal[++this->stackValIdx] = x;
}

static VALUE stack_pop (la_t *this) {
  return this->stackVal[this->stackValIdx--];
}

static void la_fun_stack_push (la_t *this, funT *f) {
  funstack_t *item = Alloc (sizeof (funstack_t));
  item->f = f;
  ListStackPush (this->funstack, item);
}

static funT *la_fun_stack_pop (la_t *this) {
  funstack_t *item = ListStackPop (this->funstack, funstack_t);
  funT *f = item->f;
  free (item);
  return f;
}

static VALUE la_copy_value (VALUE v) {
  VALUE new;

  switch (v.type) {
    case STRING_TYPE:
      new = STRING(String.dup (AS_STRING(v)));
      break;

    case ARRAY_TYPE:
      new = ARRAY(array_copy ((ArrayType *) AS_ARRAY(v)));
      break;

    case MAP_TYPE:
      new = la_copy_map (v);
      break;

    case LIST_TYPE:
      new = list_copy (v);
      break;

    default:
      new = v;
  }

  return new;
}

static void *la_clone_sym_item (void *item, void *obj) {
  (void) obj;
  sym_t *sym = (sym_t *) item;

  sym_t *new = Alloc (sizeof (sym_t));

  new->type = sym->type;
  new->is_const = sym->is_const;

  new->value = la_copy_value (sym->value);
  return new;

#if 0
  if ((new->type & 0xff) is ARRAY_TYPE) {
    VALUE ar = sym->value;
    ArrayType *array = (ArrayType *) AS_ARRAY(ar);
    ArrayType *new_array = array_copy (array);
    new->value = ARRAY(new_array);
    return new;

  } else if ((new->type & 0xff) is STRING_TYPE) {
    string *old_str = AS_STRING(sym->value);
    new->value = STRING_NEW_WITH_LEN(old_str->bytes, old_str->num_bytes);
    return new;
  }

  new->value = sym->value;
  return new;
#endif
}

static Vmap_t *la_symbol_stack_push (la_t *this, Vmap_t *symbols) {
  symbolstack_t *item = Alloc (sizeof (symbolstack_t));
  item->symbols = Vmap.clone (symbols, la_clone_sym_item, NULL);
  ListStackPush (this->symbolstack, item);
  return item->symbols;
}

static Vmap_t *la_symbol_stack_pop (la_t *this) {
  symbolstack_t *item = ListStackPop (this->symbolstack, symbolstack_t);
  Vmap_t *symbols = item->symbols;
  free (item);
  return symbols;
}

static VALUE la_string_to_dec (la_string s) {
  integer r = 0;
  int c;
  const char *ptr = GETSTRPTR(s);
  int len = GETSTRLEN(s);
  int is_sign = *ptr is '-';
  if (is_sign) ptr++;

  while (len-- > 0) {
    c = *ptr++;
    ifnot (is_digit (c)) break;
    r = 10 * r + (c - '0');
  }

  if (is_sign) r = -r;

  VALUE result = INT(r);
  return result;
}

static VALUE la_HexStringToNum (la_string s) {
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

  VALUE result = INT(r);
  return result;
}

static VALUE la_OctalStringToNum (la_string s) {
  integer r = 0;
  int c;
  const char *ptr = GETSTRPTR(s);
  int len = GETSTRLEN(s);
  while (len-- > 0) {
    c = *ptr++;
    r = 8 * r + (c - '0');
  }

  VALUE result = INT(r);
  return result;
}

static VALUE la_BinaryStringToNum (la_string s) {
  integer r = 0;
  int c;
  const char *ptr = GETSTRPTR(s);
  int len = GETSTRLEN(s);

  while (len-- > 0) {
    c = *ptr++;
    r = 2 * r + (c - '0');
  }

  VALUE result = INT(r);
  return result;
}

static int  la_string_get (la_t *this, VALUE *vp) {
  int err;
  int c = PEEK_NTH_BYTE(0);

  if (c is TOKEN_INDEX_OPEN) {
    string *str = AS_STRING(TOKENVAL);

    NEXT_TOKEN();

    VALUE v;
    this->curState |= INDEX_STATE;
    err = la_parse_primary (this, &v);
    this->curState &= ~INDEX_STATE;
    THROW_ERR_IF_ERR(err);

    if (v.type isnot INTEGER_TYPE)
      THROW_SYNTAX_ERR("awaiting an integer expression, when getting string index");

    integer idx = AS_INT(v);

    if (0 > idx) idx += str->num_bytes;

    if (idx < 0 or (size_t) idx >= str->num_bytes)
      THROW_OUT_OF_BOUNDS("index %d >= than %d length, or less than zero", idx, str->num_bytes);

    *vp = INT(str->bytes[idx]);

    if (this->curState & LITERAL_STRING_STATE) {
      this->curState &= ~LITERAL_STRING_STATE;
      String.release (str);
    }

  } else {
    *vp = TOKENVAL;
    NEXT_TOKEN();
  }

  return LA_OK;
}

static int la_string_set_char (la_t *this, VALUE value, int is_const) {
  int err;

  if (is_const)
    THROW_SYNTAX_ERR("can not modify constant string");

  string *str = AS_STRING(value);

  NEXT_TOKEN();

  VALUE v;

  this->curState |= INDEX_STATE;
  err = la_parse_primary (this, &v);
  this->curState &= ~INDEX_STATE;
  THROW_ERR_IF_ERR(err);

  THROW_SYNTAX_ERR_IF(v.type isnot INTEGER_TYPE,
    "awaiting an integer expression, when setting string index");

  integer idx = AS_INT(v);

  if (0 > idx) idx += str->num_bytes;
  if (idx < 0 or (size_t) idx >= str->num_bytes)
    THROW_OUT_OF_BOUNDS("index %d >= than %d length, or less than zero", idx, str->num_bytes);

  THROW_SYNTAX_ERR_IF(TOKEN isnot TOKEN_ASSIGN,
    "syntax error while setting string, awaiting =");

  NEXT_TOKEN();
  err = la_parse_expr (this, &v);
  THROW_ERR_IF_ERR(err);

  THROW_SYNTAX_ERR_IF(v.type isnot INTEGER_TYPE,
    "awaiting an integer expression, when setting string index");

  integer chr = AS_INT(v);

  int num_bytes = 0;
  Ustring.get.code_at (str->bytes, str->num_bytes, idx, &num_bytes);
  char buf[8];
  int len;
  Ustring.character (chr, buf, &len);
  String.replace_numbytes_at_with (str, num_bytes, idx, buf);
  return LA_OK;
}

static VALUE array_release (VALUE value) {
  VALUE result = INT(LA_OK);

  if (value.refcount > 0) goto theend;

  if (value.refcount < 0)
    if (value.refcount isnot ARRAY_LITERAL)
      return result;

  ArrayType *array = (ArrayType *) AS_ARRAY(value);
  if (NULL is array) return result;

  VALUE ary = array->value;

  if (array->type is STRING_TYPE) {
    string **s_ar = (string **) AS_ARRAY(ary);
    if (s_ar is NULL) return result;
    for (size_t i = 0; i < array->len; i++) {
      ifnot (NULL is s_ar[i])
        String.release (s_ar[i]);
    }
    free (s_ar);
    s_ar = NULL;
  } else if (array->type is MAP_TYPE) {
    Vmap_t **m_ar = (Vmap_t **) AS_ARRAY(ary);
    if (m_ar is NULL) return result;
    for (size_t i = 0; i < array->len; i++)
      Vmap.release (m_ar[i]);
    free (m_ar);
    m_ar = NULL;
  } else if (array->type is ARRAY_TYPE) {
    ArrayType **a_ar = (ArrayType **) AS_ARRAY(ary);
    if (a_ar is NULL) return result;
    for (size_t i = 0; i < array->len; i++)
      array_release (ARRAY(a_ar[i]));
    free (a_ar);
    a_ar = NULL;
  } else {
    char *ar = (char *) AS_ARRAY(ary);
    if (ar is NULL) return result;
    free (ar);
  }

  free ((void *) array);
  array = NULL;

  value = NULL_VALUE;

theend:
  value.refcount--;
  return result;
}

static int la_get_anon_array (la_t *this, VALUE *vp) {
  int err;

  THROW_SYNTAX_ERR_IF(NEXT_BYTE_NOWS_NONL() is TOKEN_INDEX_CLOS,
    "inline empty array is not supported");

  int pc;
  int c = 0;
  int instr = 0;
  int inmap = 0;
  int indtokopen = 1;
  int indtokclos = 0;
  int parenopen = 0;
  int inparen = 0;
  int inar = 0;
  int is_array_of_array = 0;
  int num_elem = 1;

  uint n = 0;
  if (PEEK_NTH_BYTE_NOWS_NONL(&n) is TOKEN_INDEX_OPEN)
    is_array_of_array = 1;

  n = 0;

  while (1) {
    pc = c;
    c = PEEK_NTH_BYTE(n++);

    THROW_SYNTAX_ERR_IF(c is TOKEN_EOF, "unended array");

    if (c is TOKEN_INDEX_CLOS and 0 is instr + inparen) {
      if (inmap)
        continue;

      indtokclos++;

      ifnot (indtokopen - indtokclos)
        break;

      if (1 is indtokopen - indtokclos)
        inar = 0;

      continue;
    }

    if (c is TOKEN_INDEX_OPEN and 0 is instr + inparen) {
      if (inmap)
        continue;

      indtokopen++;

      if (indtokopen - indtokclos is 2)
        inar = 1;

     continue;
    }

    if (c is TOKEN_PAREN_OPEN) {
      if (instr)
        continue;

      parenopen++;
      inparen = 1;
      continue;
    }

    if (c is TOKEN_PAREN_CLOS) {
      if (instr)
        continue;

      parenopen--;
      inparen = parenopen;
      continue;
    }

    if (0 is inparen and ((c is TOKEN_DQUOTE and pc isnot TOKEN_ESCAPE_CHR) or c is TOKEN_BQUOTE)) {
      if (instr) instr = 0; else instr = 1;
      continue;
    }

    if (c is TOKEN_MAP_OPEN and 0 is instr + inparen) {
      inmap++;
      continue;
    }

    if (c is TOKEN_MAP_CLOS and 0 is instr + inparen) {
      inmap--;
      continue;
    }

    //if ((c is TOKEN_COMMA or c is TOKEN_NL) and 0 is instr and 0 is inmap and 0 is inar)
    if (c is TOKEN_COMMA and 0 is instr + inmap + inar + inparen)
      num_elem++;
  }

  if (is_array_of_array) {
    VALUE ary = ARRAY(ARRAY_NEW(ARRAY_TYPE, num_elem));
    ArrayType *array = (ArrayType *) AS_ARRAY(ary);
    VALUE ar = array->value;

    err = la_array_set_as_array (this, ar, num_elem, 0, num_elem - 1);
    *vp = ary;
    return err;
  }

  VALUE v;
  this->curState |= MALLOCED_STRING_STATE;
  NEXT_TOKEN();
  err = la_parse_expr (this, &v);
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
      String.replace_with_len (item, val->bytes, val->num_bytes);

      if (v.sym is NULL and
          v.refcount isnot MALLOCED_STRING and
          0 is (this->objectState & (ARRAY_MEMBER|MAP_MEMBER))) {
        la_release_val (this, v);
        this->objectState &= ~(ARRAY_MEMBER|MAP_MEMBER);
      }

      break;
    }

    case MAP_TYPE: {
      Vmap_t **m_ar = (Vmap_t **) AS_ARRAY(ar);
      Vmap_t *item = m_ar[0];
      ifnot (NULL is item)
        Vmap.release (item);
      m_ar[0] = AS_MAP(v);
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

  ifnot (1 is num_elem)
    return la_array_assign (this, &ary, fidx, lidx, 1);

  return err;
}

static ArrayType *array_copy (ArrayType *array) {
  ArrayType *new_array = Alloc (sizeof (ArrayType));

  size_t len = array->len;
  integer type = array->type;

  new_array->len = len;
  new_array->type = type;

  switch (type) {
    case NUMBER_TYPE: {
      number *ary = (number *) AS_ARRAY(array->value);

      number *n_ar = Alloc (len * sizeof (number));
      for (size_t i = 0; i < len; i++)
        n_ar[i] = ary[i];

      new_array->value = ARRAY(n_ar);
      break;
    }

    case STRING_TYPE: {
      string **ary = (string **) AS_ARRAY(array->value);

      string **s_ar = Alloc (len * sizeof (string));
      for (size_t i = 0; i < len; i++) {
        string *item = ary[i];
        s_ar[i] = String.new_with_len (item->bytes, item->num_bytes);
      }
      new_array->value = ARRAY(s_ar);
      break;
    }

    case MAP_TYPE: {
      Vmap_t **ary = (Vmap_t **) AS_ARRAY(array->value);

      Vmap_t **m_ar = Alloc (len * Vmap.size_of ());
      for (size_t i = 0; i < len; i++) {
        Vmap_t *item = ary[i];
        m_ar[i] = AS_MAP(la_copy_map (MAP(item)));
      }
      new_array->value = ARRAY(m_ar);
      break;
    }

    default: {
      integer *ary = (integer *) AS_ARRAY(array->value);

      integer *i_ar = Alloc (len * sizeof (integer));
      for (size_t i = 0; i < len; i++)
        i_ar[i] = ary[i];

      new_array->value = ARRAY(i_ar);
      break;
    }
  }

  return new_array;
}

static int la_array_set_as_array (la_t *this, VALUE ar, integer len, integer idx, integer last_idx) {
  int err;
  VALUE val;

  ArrayType **a_ar = (ArrayType **) AS_ARRAY(ar);

  do {
    if (idx < 0 or idx >= len or idx > last_idx)
      THROW_OUT_OF_BOUNDS("index %d >= than %d length, less than zero or %d > than %d",
        idx, len, idx, last_idx);

    NEXT_TOKEN();

    err = la_parse_expr (this, &val);
    THROW_ERR_IF_ERR(err);

    if (val.type isnot ARRAY_TYPE and val.type isnot NULL_TYPE)
      THROW_SYNTAX_ERR("error while setting Array of an array, awaiting an array or null");

    if (a_ar[idx] isnot NULL)
      array_release (val);

    if (ar.type isnot ARRAY_TYPE)
      a_ar[idx] = NULL;
    else
      a_ar[idx] = (ArrayType *) AS_ARRAY(val);

    idx++;
  } while (TOKEN is TOKEN_COMMA);

  if (idx - 1 isnot last_idx)
    THROW_OUT_OF_BOUNDS("index %d - 1 isnot %d", idx - 1, last_idx);

  return LA_OK;
}

static int la_array_set_as_map (la_t *this, VALUE ar, integer len, integer idx, integer last_idx) {
  int err;
  VALUE val;

  Vmap_t **m_ar = (Vmap_t **) AS_ARRAY(ar);

  do {
    if (idx < 0 or idx >= len or idx > last_idx)
      THROW_OUT_OF_BOUNDS("index %d >= than %d length, less than zero or %d > than %d",
         idx, len, idx, last_idx);

    NEXT_TOKEN();

    err = la_parse_expr (this, &val);
    THROW_ERR_IF_ERR(err);

    if (val.type isnot MAP_TYPE and val.type isnot NULL_TYPE)
      THROW_SYNTAX_ERR("error while setting Map array, awaiting a map or null");

    m_ar[idx] = (val.type is MAP_TYPE) ? AS_MAP(val) : NULL;
    idx++;
  } while (TOKEN is TOKEN_COMMA);

  if (idx - 1 isnot last_idx)
    THROW_OUT_OF_BOUNDS("index %d - 1 isnot %d", idx - 1, last_idx);

  if (TOKEN is TOKEN_NL and PEEK_NTH_TOKEN(0) is TOKEN_INDEX_CLOS)
    NEXT_TOKEN();

  return LA_OK;
}

static int la_array_set_as_string (la_t *this, VALUE ar, integer len, integer idx, integer last_idx) {
  int err;
  VALUE val;

  string **s_ar = (string **) AS_ARRAY(ar);

  do {
    if (idx < 0 or idx >= len or idx > last_idx)
      THROW_OUT_OF_BOUNDS("index %d >= than %d length, less than zero or %d > than %d",
        idx, len, idx, last_idx);

    this->curState |= MALLOCED_STRING_STATE;
    NEXT_TOKEN();

    err = la_parse_expr (this, &val);
    this->curState &= ~MALLOCED_STRING_STATE;
    THROW_ERR_IF_ERR(err);

    switch (val.type) {
      case STRING_TYPE: {
        ifnot (NULL is s_ar[idx]) {
          string *item = s_ar[idx];
          string *s_val = AS_STRING(val);
          String.replace_with_len (item, s_val->bytes, s_val->num_bytes);
        } else {
          string *s_val = AS_STRING(val);
          s_ar[idx] = String.new_with_len (s_val->bytes, s_val->num_bytes);
        }

        if (val.sym is NULL and
            val.refcount isnot MALLOCED_STRING and
            0 is (this->objectState & (ARRAY_MEMBER|MAP_MEMBER))) {
          la_release_val (this, val);
          this->objectState &= ~(ARRAY_MEMBER|MAP_MEMBER);
        }

        break;
      }

      case NULL_TYPE:
        ifnot (NULL is s_ar[idx]) {
          string *item = s_ar[idx];
          String.release (item);
        }

        s_ar[idx] = NULL;
        break;

      default:
        THROW_SYNTAX_ERR("error while setting string array, awaiting a string or null");
    }

    idx++;
  } while (TOKEN is TOKEN_COMMA);
  // } while (TOKEN is TOKEN_COMMA or (TOKEN is TOKEN_NL and PEEK_NTH_TOKEN(0) isnot TOKEN_INDEX_CLOS));

  if (idx - 1 isnot last_idx)
    THROW_OUT_OF_BOUNDS("index %d - 1 isnot %d", idx - 1, last_idx);

  return LA_OK;
}

static int la_array_set_as_number (la_t *this, VALUE ar, integer len, integer idx, integer last_idx) {
  int err;
  VALUE val;

  number *n_ar = (number *) AS_ARRAY(ar);

  do {
    if (idx < 0 or idx >= len or idx > last_idx)
      THROW_OUT_OF_BOUNDS("index %d >= than %d length, less than zero or %d > than %d",
        idx, len, idx, last_idx);

    NEXT_TOKEN();

    err = la_parse_expr (this, &val);
    THROW_ERR_IF_ERR(err);

    n_ar[idx] = AS_NUMBER(val);

    idx++;
  } while (TOKEN is TOKEN_COMMA);

  if (idx - 1 isnot last_idx)
    THROW_OUT_OF_BOUNDS("index %d - 1 isnot %d", idx - 1, last_idx);

  return LA_OK;
}

static int la_array_set_as_int (la_t *this, VALUE ar, integer len, integer idx, integer last_idx) {
  int err;
  VALUE val;

  integer *s_ar = (integer *) AS_ARRAY(ar);

  do {
    if (idx < 0 or idx >= len or idx > last_idx)
      THROW_OUT_OF_BOUNDS("index %d >= than %d length, less than zero or %d > than %d",
        idx, len, idx, last_idx);

    NEXT_TOKEN();

    err = la_parse_expr (this, &val);
    THROW_ERR_IF_ERR(err);

    s_ar[idx] = AS_INT(val);

    idx++;
  } while (TOKEN is TOKEN_COMMA);

  if (idx - 1 isnot last_idx)
    THROW_OUT_OF_BOUNDS("index %d - 1 isnot %d", idx - 1, last_idx);

  return LA_OK;
}

/* Initial Array Interface by MickeyDelp <mickey at delptronics dot com> */
static int la_array_assign (la_t *this, VALUE *ar, VALUE ix, VALUE last_ix, int is_single) {
  int err;

  ArrayType *array = (ArrayType *) AS_ARRAY((*ar));
  integer len = array->len;

  integer idx = AS_INT(ix);
  if (0 > idx)
    idx += len;

  integer last_idx = AS_INT(last_ix);
  if (0 > last_idx)
    last_idx += len;

  if (last_idx < 0 or last_idx >= len)
    THROW_OUT_OF_BOUNDS("index %d >= %d len, or less than zero", last_idx, len);

  VALUE ary = array->value;

  ifnot (is_single) {
    NEXT_TOKEN();
    if (TOKEN isnot TOKEN_INDEX_OPEN)
      THROW_SYNTAX_ERR("array assignment: awaiting [");
  }

  switch (array->type) {
    case INTEGER_TYPE:
      err = la_array_set_as_int (this, ary, len, idx, last_idx);
      break;

    case STRING_TYPE:
      err = la_array_set_as_string (this, ary, len, idx, last_idx);
      break;

    case MAP_TYPE:
      err = la_array_set_as_map (this, ary, len, idx, last_idx);
      break;

    case ARRAY_TYPE:
      err = la_array_set_as_array (this, ary, len, idx, last_idx);
      break;

    default:
      err = la_array_set_as_number (this, ary, len, idx, last_idx);
  }

  THROW_ERR_IF_ERR(err);

  ifnot (is_single) {
    if (TOKEN is TOKEN_NL)
      NEXT_TOKEN();

    THROW_SYNTAX_ERR_IF(TOKEN isnot TOKEN_INDEX_CLOS,
      "array assignment: awaiting ]");

    NEXT_TOKEN();
  }

  return LA_OK;
}

static int la_parse_array_def (la_t *this, VALUE *vp, int flags) {
  int err;
  int type = TOKENSYM->type;

  NEXT_TOKEN();

  THROW_SYNTAX_ERR_IF(TOKEN isnot TOKEN_INDEX_OPEN,
    "array assignment: awaiting [");

  int c = TOKEN;

  NEXT_TOKEN();

  VALUE len;
  err = la_parse_expr (this, &len);
  THROW_ERR_IF_ERR(err);
  THROW_SYNTAX_ERR_IF(TOKEN isnot TOKEN_INDEX_CLOS,
    "array assignment: awaiting ]");
  THROW_SYNTAX_ERR_IF(len.type isnot INTEGER_TYPE,
    "awaiting an integer expression, when getting array length");

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

  if (TOKEN isnot TOKEN_SYMBOL and
      TOKEN isnot TOKEN_ARRAY)
    THROW_SYNTAX_ERR("syntax error, awaiting an identifier for the array declaration");

  la_string name = TOKENSTR;

  integer nlen = AS_INT(len);

  ar = ARRAY(ARRAY_NEW(type, nlen));

  funT *scope = (this->scopeState is PUBLIC_SCOPE ? this->function : this->curScope);
  TOKENSYM = la_define_symbol (this, scope, sym_key (this, name), ARRAY_TYPE,
      ar, 0);
  this->scopeState = 0;

  NEXT_TOKEN();
  c = TOKEN;

assign:
  if (c is TOKEN_ASSIGN) {
    VALUE at_idx = INT(0);
    VALUE last_idx = INT(-1);
    return la_array_assign (this, &ar, at_idx, last_idx, 0);
  }

  return LA_OK;
}

static int la_parse_array_set (la_t *this) {
  int c = NEXT_BYTE_NOWS_NONL();

  if (c is '=') {
    VALUE v = TOKENVAL;
    sym_t *sym = v.sym;
    if (sym isnot NULL) {
      sym->type = NULL_TYPE;
      sym->value = NULL_VALUE;
    }

    array_release (v);

    return LA_MMT_REASSIGN;
  }

  VALUE ix = INT(0);

  VALUE ary = TOKENVAL;

  int err;
  NEXT_TOKEN();
  c = TOKEN;

  VALUE last_ix = INT(-1);

  int is_index = c is TOKEN_INDEX_OPEN;

  if (is_index) {
    uint n = 0;
    c = PEEK_NTH_BYTE_NOWS_NONL(&n);
    if (c is TOKEN_STAR) {
      for (uint i = 0; i <= n; i++)
        IGNORE_NEXT_BYTE;

      NEXT_TOKEN();

      if (TOKEN isnot TOKEN_INDEX_CLOS)
        THROW_SYNTAX_ERR("array set: awaiting ]");

      NEXT_TOKEN();

      if (TOKEN < TOKEN_ASSIGN)
        THROW_SYNTAX_ERR("syntax error while setting array, awaiting =");

      VALUE ar_val;
      NEXT_TOKEN();
      err = la_parse_expr (this, &ar_val);
      THROW_ERR_IF_ERR(err);

      ArrayType *array = (ArrayType *) AS_ARRAY(ary);
      if (array->type isnot ar_val.type)
        THROW_TYPE_MISMATCH(array->type, ar_val.type);

      switch (array->type) {
        case STRING_TYPE: {
          string **s_ar = (string **) AS_ARRAY(array->value);
          string *s_val = AS_STRING(ar_val);
          for (size_t i = 0; i < array->len; i++) {
            string *item = s_ar[i];
            String.replace_with_len (item, s_val->bytes, s_val->num_bytes);
          }

          String.release (s_val);
          return LA_OK;
        }

        case MAP_TYPE: {
          Vmap_t **m_ar = (Vmap_t **) AS_ARRAY(array->value);
          for (size_t i = 0; i < array->len; i++) {
            ifnot (NULL is m_ar[i])
              Vmap.release (m_ar[i]);
            m_ar[i] = AS_MAP(la_copy_value (ar_val));
          }

          if (NULL is ar_val.sym)
            la_release_val (this, ar_val);

          return LA_OK;
        }

        case INTEGER_TYPE: {
          int i_val = AS_INT(ar_val);
          integer *i_ar = (integer *) AS_ARRAY(array->value);
          for (size_t i = 0; i < array->len; i++)
            i_ar[i] = i_val;
          return LA_OK;
        }

        case NUMBER_TYPE: {
          number n_val = AS_NUMBER(ar_val);
          number *n_ar = (number *) AS_ARRAY(array->value);
          for (size_t i = 0; i < array->len; i++)
            n_ar[i] = n_val;
          return LA_OK;
        }

        default:
          THROW_INV_ARRAY_TYPE(array->type);
      }
    }

    this->curState |= INDEX_STATE;
    err = la_parse_primary (this, &ix);
    this->curState &= ~INDEX_STATE;
    THROW_ERR_IF_ERR(err);

    if (TOKEN is TOKEN_COLON) {
      NEXT_TOKEN();
      c = TOKEN;
      if (c and c isnot TOKEN_INDEX_CLOS) {
        err = la_parse_expr (this, &last_ix);
        THROW_ERR_IF_ERR(err);
      }

      NEXT_TOKEN();
      is_index = 0;
    }
  }

  c = TOKEN;

  if (c is TOKEN_INDEX_OPEN) {
    ArrayType *array = (ArrayType *) AS_ARRAY(ary);
    VALUE ar = array->value;

    switch (array->type) {
      case STRING_TYPE: {
        string **s_ar = (string **) AS_ARRAY(ar);
        VALUE v = STRING(s_ar[AS_INT(ix)]);
        UNGET_BYTE();
        err = la_string_set_char (this, v, 0);
        return err;
      }

      default:
        THROW_SYNTAX_ERR("Arrays of arrays haven't been implemented");
    }
  }

  if (c is TOKEN_DOT) {
    ArrayType *array = (ArrayType *) AS_ARRAY(ary);
    VALUE ar = array->value;

    switch (array->type) {
      case MAP_TYPE: {
        Vmap_t **m_ar = (Vmap_t **) AS_ARRAY(ar);
        VALUE v = MAP(m_ar[AS_INT(ix)]);
        TOKENVAL = v;
        UNGET_BYTE();
        err = la_parse_map_set (this);
        return err;
      }

      default:
        THROW_SYNTAX_ERR("not a map");
    }
  }

  if (c is TOKEN_MAP) {
    err = la_parse_map_get (this, &ix);
    THROW_ERR_IF_ERR(err);

    if (TOKEN is TOKEN_INDEX_CLOS) {
      NEXT_TOKEN();
      is_index = 1;
    }
  }

  int token = TOKEN;
  int is_un = 0;

  if (token > TOKEN_ASSIGN) {
    if (token is TOKEN_PLUS_PLUS or token is TOKEN_MINUS_MINUS) {
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
            (token is TOKEN_PLUS_PLUS ? "++" : "--"));
      }

      ArrayType *array = (ArrayType *) AS_ARRAY(ary);
      int idx = AS_INT(ix);

      switch (array->type) {
        case INTEGER_TYPE: {
          integer *i_ar = (integer *) AS_ARRAY(array->value);
          i_ar[idx] += (token is TOKEN_PLUS_PLUS ? 1 : -1);
          NEXT_TOKEN();
          return LA_OK;
        }

        case NUMBER_TYPE: {
          number *n_ar = (number *) AS_ARRAY(array->value);
          n_ar[idx] += (token is TOKEN_PLUS_PLUS ? 1 : -1);
          NEXT_TOKEN();
          return LA_OK;
        }

        default:
          THROW_SYNTAX_ERR("awaiting an integer or a number");
      }
    }

    NEXT_TOKEN();

    if (TOKEN is TOKEN_UNARY) {
      is_un = 1;
      NEXT_TOKEN();
    }

    VALUE v;
    err = la_parse_expr (this, &v);
    THROW_ERR_IF_ERR(err);

    if (is_un) {
      if (v.type is INTEGER_TYPE)
        AS_INT(v) = ~AS_INT(v);
      else
        THROW_SYNTAX_ERR("error while setting an unary object, awaiting an integer");
    }

    ArrayType *array = (ArrayType *) AS_ARRAY(ary);
    int idx = AS_INT(ix);
    VALUE val;
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
        THROW_A_TYPE_MISMATCH(array->type, "is not allowd for a binary operation");
    }

    VALUE result;
    switch (token) {
      case TOKEN_ASSIGN_APP:
        this->objectState |= OBJECT_APPEND;
        result = la_add (this, val, v);
        this->objectState &= ~OBJECT_APPEND;
        break;

      case TOKEN_ASSIGN_SUB:
        result = la_sub (this, val, v); break;
      case TOKEN_ASSIGN_DIV:
        result = la_div (this, val, v); break;
      case TOKEN_ASSIGN_MUL:
        result = la_mul (this, val, v); break;
      case TOKEN_ASSIGN_MOD:
        result = la_mod (this, val, v); break;
      case TOKEN_ASSIGN_BAR:
        result = la_bset (this, val, v); break;
      case TOKEN_ASSIGN_AND:
        result = la_bnot (this, val, v); break;
      case TOKEN_ASSIGN_XOR:
        result = la_bitxor (this, val, v); break;
    }

    if (result.type is NULL_TYPE)
       THROW_SYNTAX_ERR("unxpected operation");

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

    return LA_OK;
  }

  if (TOKEN < TOKEN_ASSIGN)
    THROW_SYNTAX_ERR("syntax error while setting array, awaiting =");

  ifnot (is_index)
    return la_array_assign (this, &ary, ix, last_ix, is_index);

  last_ix = INT(AS_INT(ix));
  return la_array_assign (this, &ary, ix, last_ix, is_index);
}

static int la_array_from_array (la_t *this, ArrayType *src_ar, VALUE v_iar, VALUE *vp) {
  (void) this;
  ArrayType *ary = (ArrayType *) AS_ARRAY(v_iar);

  if (ary->type isnot INTEGER_TYPE)
    THROW_SYNTAX_ERR("awaiting an integer type array");

  ArrayType *array = ARRAY_NEW(src_ar->type, (integer) ary->len);

  integer *x_ar = (integer *) AS_ARRAY(ary->value);

  switch (src_ar->type) {
    case STRING_TYPE: {
      string **s_ar = (string **) AS_ARRAY(array->value);
      string **s_ar_src = (string **) AS_ARRAY(src_ar->value);
      for (size_t i = 0; i < ary->len; i++) {
        size_t idx = x_ar[i];
        if (idx >= src_ar->len)
          THROW_OUT_OF_BOUNDS("index %d >= than %d length", idx, src_ar->len);

        String.replace_with_len (s_ar[i], s_ar_src[idx]->bytes, s_ar_src[idx]->num_bytes);
      }
      break;
    }

    case INTEGER_TYPE: {
      integer *i_ar = (integer *) AS_ARRAY(array->value);
      integer *i_ar_src = (integer *) AS_ARRAY(src_ar->value);
      for (size_t i = 0; i < ary->len; i++) {
        size_t idx = x_ar[i];
        if (idx >= src_ar->len)
          THROW_OUT_OF_BOUNDS("index %d >= than %d length", idx, src_ar->len);

        i_ar[i] = i_ar_src[idx];
      }
      break;
    }

    case NUMBER_TYPE: {
      double *d_ar = (double *) AS_ARRAY(array->value);
      double *d_ar_src = (double *) AS_ARRAY(src_ar->value);
      for (size_t i = 0; i < ary->len; i++) {
        size_t idx = x_ar[i];
        if (idx >= src_ar->len)
          THROW_OUT_OF_BOUNDS("index %d >= than %d length", idx, src_ar->len);

        d_ar[i] = d_ar_src[idx];
      }
      break;
    }
  }

  *vp = ARRAY(array);

  if (v_iar.sym is NULL) array_release (v_iar);

  return LA_OK;
}

static int la_parse_array_get (la_t *this, VALUE *vp) {
  VALUE ar = TOKENVAL;
  ArrayType *array = (ArrayType *) AS_ARRAY(ar);

  NEXT_TOKEN();
  int c = TOKEN;

  if (c isnot TOKEN_INDEX_OPEN) {
    *vp = ar;
    return LA_OK;
  }

  int err;
  if (NEXT_BYTE_NOWS_NONL() is TOKEN_INDEX_OPEN) {
     NEXT_TOKEN();
     VALUE v;
     err = la_get_anon_array (this, &v);
     THROW_ERR_IF_ERR(err);

     NEXT_TOKEN();

     if (TOKEN isnot TOKEN_INDEX_CLOS)
       THROW_SYNTAX_ERR("array get, awaiting ]");

     err = la_array_from_array (this, array, v, vp);

     if (TOKEN isnot TOKEN_INDEX_CLOS)
       THROW_SYNTAX_ERR("array get, awaiting ]");

     NEXT_TOKEN();

     return err;
  }

  VALUE ix;
  this->curState |= INDEX_STATE;
  err = la_parse_primary (this, &ix);
  this->curState &= ~INDEX_STATE;
  THROW_ERR_IF_ERR(err);

  if (ix.type is ARRAY_TYPE)
    return la_array_from_array (this, array, ix, vp);

  if (ix.type isnot INTEGER_TYPE)
    THROW_SYNTAX_ERR("array get, awaiting an integer expression");

  integer len = array->len;
  integer idx = AS_INT(ix);

  if (0 > idx)
    idx += len;

  if (idx <= -1 or idx >= len)
    THROW_OUT_OF_BOUNDS("index %d >= than %d length, or less or equal than -1",
        idx, len);

  switch (array->type) {
    case INTEGER_TYPE: {
      integer *ary = (integer *) AS_ARRAY(array->value);
      *vp = INT(ary[idx]);
      break;
    }

    case NUMBER_TYPE: {
      number *ary = (number *) AS_ARRAY(array->value);
      *vp = NUMBER(ary[idx]);
      break;
    }

    case STRING_TYPE: {
      string **ary = (string **) AS_ARRAY(array->value);
      if (ary[idx] isnot NULL) {
        *vp = STRING(ary[idx]);
        vp->sym = ar.sym;
        this->objectState |= ARRAY_MEMBER;
      } else
        *vp = NULL_VALUE;
      break;
    }

    case MAP_TYPE: {
      Vmap_t **ary = (Vmap_t **) AS_ARRAY(array->value);
      if (ary[idx] isnot NULL) {
        *vp = MAP(ary[idx]);
      }
      else
        *vp = NULL_VALUE;
      break;
    }

    case ARRAY_TYPE: {
      ArrayType **ary = (ArrayType **) AS_ARRAY(array->value);
      if (ary[idx] isnot NULL)
        *vp = ARRAY(ary[idx]);
      else
        *vp = NULL_VALUE;
      break;
    }

    default:
      THROW_INV_ARRAY_TYPE(array->type);

  }

  c = TOKEN;

  if (c is TOKEN_DOT) {
    VALUE v = *vp;
    if (v.type isnot MAP_TYPE)
      THROW_SYNTAX_ERR("not a map");

    TOKENVAL = v;
    UNGET_BYTE();
    return la_parse_map_get (this, vp);
  }

  if (c is TOKEN_PLUS_PLUS or c is TOKEN_MINUS_MINUS) {
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
          (c is TOKEN_PLUS_PLUS ? "++" : "--"));
    }

    switch (vp->type) {
      case INTEGER_TYPE: {
        integer *ary = (integer *) AS_ARRAY(array->value);
        ary[idx] += (c is TOKEN_PLUS_PLUS ? 1 : -1);
        NEXT_TOKEN();
        return LA_OK;
      }

      case NUMBER_TYPE: {
        number *ary = (number *) AS_ARRAY(array->value);
        ary[idx] += (c is TOKEN_PLUS_PLUS ? 1 : -1);
        NEXT_TOKEN();
        return LA_OK;
     }
      default:
        THROW_SYNTAX_ERR("awaiting an integer or a number");
    }
  }

  if (c is TOKEN_INDEX_OPEN) {
    VALUE v = *vp;
    switch (v.type) {
      case STRING_TYPE:
        TOKENVAL = v;
        this->objectState |= ARRAY_MEMBER;
        UNGET_BYTE();
        err = la_string_get (this, vp);
        if (TOKEN is TOKEN_INDEX_OPEN)
          THROW_SYNTAX_ERR("unsupported indexing");

        return err;

      case ARRAY_TYPE:
        TOKENVAL = v;
        UNGET_BYTE();
        return la_parse_array_get (this, vp);

      default:
        THROW_SYNTAX_ERR("Arrays of arrays haven't been implemented");
    }
  }

  return LA_OK;
}

static int la_array_eq (VALUE x, VALUE y) {
  ArrayType *xa = (ArrayType *) AS_ARRAY(x);
  ArrayType *ya = (ArrayType *) AS_ARRAY(y);

  if (xa is ya) return 1;

  if (xa->len isnot ya->len) return 0;

  switch (xa->type) {
    case INTEGER_TYPE:
      switch (ya->type) {
        case INTEGER_TYPE: {
          integer *x_ar = (integer *) AS_ARRAY(xa->value);
          integer *y_ar = (integer *) AS_ARRAY(ya->value);
          for (size_t i = 0; i < xa->len; i++)
            if (x_ar[i] isnot y_ar[i]) return 0;

          return 1;
        }
      }
      return 0;

    case NUMBER_TYPE:
      switch (ya->type) {
        case NUMBER_TYPE: {
          number *x_ar = (number *) AS_ARRAY(xa->value);
          number *y_ar = (number *) AS_ARRAY(ya->value);
          for (size_t i = 0; i < xa->len; i++)
            if (x_ar[i] isnot y_ar[i]) return 0;

          return 1;
        }
      }
      return 0;

    case STRING_TYPE:
      switch (ya->type) {
        case STRING_TYPE: {
          string **x_ar = (string **) AS_ARRAY(xa->value);
          string **y_ar = (string **) AS_ARRAY(ya->value);
          for (size_t i = 0; i < xa->len; i++)
            ifnot (Cstring.eq (x_ar[i]->bytes, y_ar[i]->bytes)) return 0;

          return 1;
        }
      }
      return 0;

    default:
      return 0;
  }
  return 0;
}

static void la_release_map_val (void *v) {
  VALUE *val = (VALUE *) v;
  free (val->sym);
  la_release_val (NULL, *val);
  free (val);
}

static void la_map_release_value (la_t *this, VALUE *v) {
  (void) this;
  la_release_map_val (v);
}

static int la_map_set_value (la_t *this, Vmap_t *map, const char *key, VALUE v, int scope) { 
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
        val->asString = AS_STRING(la_copy_value (v));
        val->refcount = 0;
        this->objectState &= ~(ARRAY_MEMBER|MAP_MEMBER);
      } else {
        if (v.sym is NULL) {
          val->asString  = v.asString;
          if (val->refcount isnot MALLOCED_STRING)
              val->refcount = 0;
          this->curState &= ~LITERAL_STRING_STATE;
        } else {
          val->asString = AS_STRING(la_copy_value (v));
          val->refcount = 0;
        }
      }
      break;

    case NUMBER_TYPE: val->asNumber  = v.asNumber;  break;
    case NULL_TYPE  : val->asNull    = v.asNull;    break;
    case MAP_TYPE:
      if (v.sym isnot NULL) {
        val->asInteger = (pointer) AS_MAP(la_copy_map (v));
        break;
      }
      // fall through

    default:          val->asInteger = v.asInteger; break;
  }

  if (NOTOK is Vmap.set (map, key, val, la_release_map_val, 0)) {
    this->print_bytes (this->err_fp, "Map.set() internal error\n");
    return LA_NOTOK;
  }

  return LA_OK;
}

static int la_map_reset_value (la_t *this, Vmap_t *map, const char *key, VALUE v) {
  VALUE *val = Vmap.pop (map, key);
  int scope = PUBLIC_SCOPE;

  if (val isnot NULL) {
    scope = val->sym->scope isnot NULL;
    la_release_map_val (val);
  }

  return la_map_set_value (this, map, key, v, scope);
}

static int map_set_append_rout (la_t *this, Vmap_t *map, char *key, int token) {
  int err;
  VALUE *val = Vmap.get (map, key);

  if (NULL is val)
    THROW_SYNTAX_ERR_FMT("%s key doesn't exists", key);

  if (val->type > STRING_TYPE or IS_UFUNC(val->type))
    THROW_A_TYPE_MISMATCH(val->type, "illegal map type");

  if (token is TOKEN_PLUS_PLUS or token is TOKEN_MINUS_MINUS) {
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
          (token is TOKEN_PLUS_PLUS ? "++" : "--"));
    }

    switch (val->type) {
      case INTEGER_TYPE:
        AS_INT((*val)) = AS_INT((*val)) + (token is TOKEN_PLUS_PLUS ? 1 : -1);
        NEXT_TOKEN();
        return LA_OK;

      case NUMBER_TYPE:
        AS_NUMBER((*val)) = AS_NUMBER((*val)) + (token is TOKEN_PLUS_PLUS ? 1 : -1);
        NEXT_TOKEN();
        return LA_OK;

      default:
        THROW_SYNTAX_ERR("awaiting an integer or a number");
    }
  }

  VALUE v;
  NEXT_TOKEN();
  int is_un = TOKEN is TOKEN_UNARY;
  if (is_un)
    NEXT_TOKEN();

  err = la_parse_expr (this, &v);
  THROW_ERR_IF_ERR(err);

  if (is_un) {
    if (v.type is INTEGER_TYPE)
      AS_INT(v) = ~AS_INT(v);
    else
      THROW_SYNTAX_ERR("error while setting an unary object, awaiting an integer");
  }

  VALUE result;
  switch (token) {
    case TOKEN_ASSIGN_APP:
      this->objectState |= OBJECT_APPEND;
      result = la_add (this, *val, v);
      this->objectState &= ~OBJECT_APPEND;
      break;

    case TOKEN_ASSIGN_SUB:
      result = la_sub (this, *val, v); break;
    case TOKEN_ASSIGN_DIV:
      result = la_div (this, *val, v); break;
    case TOKEN_ASSIGN_MUL:
      result = la_mul (this, *val, v); break;
    case TOKEN_ASSIGN_MOD:
      result = la_mod (this, *val, v); break;
    case TOKEN_ASSIGN_BAR:
      result = la_bset (this, *val, v); break;
    case TOKEN_ASSIGN_AND:
      result = la_bnot (this, *val, v); break;
    case TOKEN_ASSIGN_XOR:
      result = la_bitxor (this, *val, v); break;
  }

  if (result.type is NULL_TYPE)
     THROW_SYNTAX_ERR("unxpected operation");

  sym_t *sym = val->sym;
  *val = result;
  val->sym = sym;

  return LA_OK;
}

static int map_set_rout (la_t *this, Vmap_t *map, char *key, int scope) {
  int err;
  VALUE v;

  NEXT_TOKEN();

  while (TOKEN is TOKEN_NL)
    NEXT_TOKEN();

  if (TOKEN is TOKEN_FUNCDEF) {
    Cstring.cp_fmt (this->curFunName, MAXLEN_SYMBOL + 1, NS_ANON "_%zd", this->anon_id++);

    this->scopeState = PUBLIC_SCOPE; // this is for map.f = func ... cases

    err = la_parse_func_def (this);
    this->curFunName[0] = '\0';
    THROW_ERR_IF_ERR(err);

    v = PTR(this->curFunDef);
    v.type = UFUNCTION_TYPE;
    goto assign;
  }

  if (NULL isnot TOKENSYM and
      IS_INT(TOKENSYM->value) and
      AS_VOID_PTR(TOKENSYM->value) is la_parse_array_def) {
  /* moved from do_next_token (),
   * first commit after 8ed84cdc92c074e4dba4c1d38e72cae6b8236178
   * if (IS_INT(symbol->value) and AS_VOID_PTR(symbol->value) is la_parse_array_def) {
   *   r = TOKEN_ARYDEF;
   *   goto theend;
   * }
   */

    err = la_parse_array_def (this, &v, ARRAY_IS_MAP_MEMBER);
    THROW_ERR_IF_ERR(err);

    goto assign;
  }

  if (TOKEN is TOKEN_ARRAY) {
    sym_t *sym = TOKENSYM;
    v =  ARRAY(array_copy ((ArrayType *) AS_ARRAY(sym->value)));
    NEXT_TOKEN();

    goto assign;
  }

  this->objectState |= MAP_ASSIGNMENT;
  err = la_parse_expr (this, &v);
  this->objectState &= ~MAP_ASSIGNMENT;
  THROW_ERR_IF_ERR(err);

  assign:

  err = la_map_set_value (this, map, key, v, scope);
  THROW_ERR_IF_ERR(err);

  if ((v.type is ARRAY_TYPE or v.type is LIST_TYPE) and TOKEN is TOKEN_INDEX_CLOS)
    NEXT_TOKEN();

  return err;
}

static VALUE map_release (VALUE value) {
  VALUE result = INT(LA_OK);

  if (value.sym isnot NULL) {
    if (value.refcount > 0) goto theend;

    if (value.refcount < 0)
      if (value.refcount isnot MAP_LITERAL)
        return result;
  }

  Vmap.release ((Vmap_t *) AS_PTR(value));

theend:
  value.refcount--;
  return result;
}

static void *la_copy_map_cb (void *value, void *mapval) {
  Vmap_t *map = (Vmap_t *) mapval;
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
    case ARRAY_TYPE : val->asInteger = AS_PTR(la_copy_value (*v));    break;
    case STRING_TYPE: val->asString  = AS_STRING(la_copy_value (*v)); break;
    case NUMBER_TYPE: val->asNumber  = v->asNumber;                   break;
    case NULL_TYPE  : val->asNull    = v->asNull;                     break;
    default         : val->asInteger = v->asInteger;                  break;
  }

  return val;
}

static VALUE la_copy_map (VALUE mapval) {
  Vmap_t *map = AS_MAP(mapval);
  Vmap_t *new = Vmap.clone (map, la_copy_map_cb, map);
  VALUE nval = MAP(new);
  return nval;
}

static int la_get_map_key (la_t *this, char ident[MAXLEN_SYMBOL + 1]) {
  int idx = 0;
  int num_comma = 0;
  int c;

  get_byte:
  c = GET_BYTE();

  while (is_space (c)) c = GET_BYTE();

  if (c is TOKEN_NL) goto get_byte;

  if (c is TOKEN_COMMENT) {
    do c = GET_BYTE(); while (c >= 0 and c isnot TOKEN_NL);
    goto get_byte;
  }

  if (c is TOKEN_COMMA) {
    if (num_comma++)
      THROW_SYNTAX_ERR("error while getting map field, found two consecutive commas");
    goto get_byte;
  }

  if (is_alpha (c) or c is '_' or is_digit (c)) {

    identifier:
    THROW_SYNTAX_ERR_IF(idx is MAXLEN_SYMBOL, "map key identifier exceeded maximum length");

    ident[idx++] = c;

    c = GET_BYTE();
    if (c is TOKEN_EOF)
      goto theend;

    if (0 is is_alpha (c) and c isnot '_' and 0 is is_digit (c)) {
      UNGET_BYTE();
      goto theend;
    }

    goto identifier;
  }

  if (c is TOKEN_DOLLAR) {
    VALUE v_key;

    this->curState |= (MALLOCED_STRING_STATE|CLOSURE_STATE);
    NEXT_TOKEN();
    int err = la_parse_expr (this, &v_key);
    this->curState &= ~(MALLOCED_STRING_STATE|CLOSURE_STATE);
    THROW_ERR_IF_ERR(err);

    THROW_SYNTAX_ERR_IF(TOKEN isnot TOKEN_PAREN_CLOS, "awaiting ')'");

    if (v_key.type isnot STRING_TYPE)
      THROW_SYNTAX_ERR("map get, awaiting a string as a key");

    string *s_key = AS_STRING(v_key);
    if (s_key->num_bytes >= MAXLEN_SYMBOL)
      THROW_SYNTAX_ERR("identifier exceeded maximum length");

    Cstring.cp (ident, MAXLEN_SYMBOL + 1, s_key->bytes, s_key->num_bytes);
    return LA_OK;
  }

  if (c isnot TOKEN_DQUOTE)
    return c;

  get_char:
  c = GET_BYTE();
  THROW_SYNTAX_ERR_IF(c is TOKEN_EOF,
    "error while getting string as a map key, found end of input");
  THROW_SYNTAX_ERR_IF(c is TOKEN_NL,
    "error while getting string as a map key, found a new line");

  if (c is TOKEN_DQUOTE) goto theend;

  THROW_SYNTAX_ERR_IF(idx is MAXLEN_SYMBOL, "map key identifier exceeded maximum length");

  ident[idx++] = c;
  goto get_char;

theend:
  ident[idx] = '\0';
  if ('p' is *ident) {
    if (idx >= 6) {
      if (Cstring.eq (ident, "private"))
        return TOKEN_PRIVATE;
      if (Cstring.eq (ident, "public"))
        return TOKEN_PUBLIC;
   }
  }

  return LA_OK;
}

static int la_parse_map_members (la_t *this, VALUE map) {
  int err;
  this->curState |= MAP_STATE;
  this->scopeState = PUBLIC_SCOPE;
  int scope = this->scopeState;

  for (;;) {

    char key[MAXLEN_SYMBOL + 1];
    TOKEN = la_get_map_key (this, key);

    if (TOKEN is TOKEN_EOF) break;

    THROW_ERR_IF_ERR(TOKEN);

    switch (TOKEN) {
      case LA_OK:
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

    NEXT_TOKEN();

    if (TOKEN isnot TOKEN_COLON) {
      ifnot (this->funcState & TYPE_NEW_STATE) {
        THROW_SYNTAX_ERR("error while setting map field, awaiting :");
      } else {
        la_map_set_value (this, AS_MAP(map), key, NULL_VALUE, scope);
        UNGET_BYTE();
        continue;
      }
    }

    err = map_set_rout (this, AS_MAP(map), key, this->scopeState is PUBLIC_SCOPE);
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
  return LA_OK;
}

static int la_parse_map (la_t *this, VALUE *vp) {
  la_string saved_ptr = PARSEPTR;
  PARSEPTR = TOKENSTR;

  Vmap_t *map = Vmap.new (32);
  int err = la_parse_map_members (this, MAP(map));
  THROW_ERR_IF_ERR(err);

  PARSEPTR = saved_ptr;

  VALUE v = MAP(map);
  if (this->exprList) v.refcount--;
  *vp = v;

  NEXT_TOKEN();
  return LA_OK;
}

static int la_parse_map_get (la_t *this, VALUE *vp) {
  int err;

  int is_this = (GETSTRLEN(TOKENSTR) is 4 and
      Cstring.eq_n (GETSTRPTR(TOKENSTR), "this", 4));

  VALUE m = TOKENVAL;

  NEXT_TOKEN();

  if (TOKEN isnot TOKEN_DOT) {
    if (this->objectState & MAP_ASSIGNMENT)
      *vp = la_copy_map (m);
    else
      *vp = m;

    return LA_OK;
  }

  err = 0;
  TOKENVAL = m;
  VALUE map_par = TOKENVAL;

  int submap = 0;

  redo: {}

  VALUE save_map = TOKENVAL;

  char key[MAXLEN_SYMBOL + 1];
  TOKEN = la_get_map_key (this, key);
  THROW_ERR_IF_ERR(TOKEN);

  TOKENVAL = save_map;

  VALUE mapv = TOKENVAL;
  Vmap_t *map = AS_MAP(mapv);

  VALUE *v = Vmap.get (map, key);

  if (v is NULL)
    THROW_SYNTAX_ERR_FMT("%s, not a key", key);

  if (v->sym->scope is NULL) {
    ifnot (is_this)
      THROW_SYNTAX_ERR_FMT("%s, symbol has private scope", key);
  }

  *vp = *v;

  NEXT_TOKEN();

  if (TOKEN is TOKEN_PLUS_PLUS or TOKEN is TOKEN_MINUS_MINUS) {
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
          (TOKEN is TOKEN_PLUS_PLUS ? "++" : "--"));
    }

    switch (v->type) {
      case INTEGER_TYPE:
        AS_INT((*v)) = AS_INT((*v)) + (TOKEN is TOKEN_PLUS_PLUS ? 1 : -1);
        NEXT_TOKEN();
        return LA_OK;

      case NUMBER_TYPE:
        AS_NUMBER((*v)) = AS_NUMBER((*v)) + (TOKEN is TOKEN_PLUS_PLUS ? 1 : -1);
        NEXT_TOKEN();
        return LA_OK;

      default:
        THROW_SYNTAX_ERR("awaiting an integer or a number");
    }
  }

  if (TOKEN is TOKEN_DOT or TOKEN is TOKEN_COLON) {
    if (v->type isnot MAP_TYPE) {
      if (TOKEN is TOKEN_COLON)
        goto theend;
      else
        THROW_SYNTAX_ERR_FMT("%s, not a map", key);
    }

    if (TOKEN is TOKEN_COLON) {
      const char *ptr = GETSTRPTR(PARSEPTR);
      while (is_space (*ptr)) ptr++;
      char k[MAXLEN_SYMBOL + 1];
      int idx = 0;
      while (is_identifier (*ptr)) {
        if (idx is MAXLEN_SYMBOL)
          THROW_SYNTAX_ERR("identifier exceeded maximum length");
        k[idx++] = *ptr;
        ptr++;
      }

      k[idx] = '\0';

      Vmap_t *vmap = AS_MAP((*v));
      ifnot (Vmap.key_exists (vmap, k))
        goto theend;
    }

    TOKENVAL = *v;

    if (TOKEN is TOKEN_COLON)
      submap = 1;
    else
      submap = 0;
    goto redo;
  }

  if (TOKEN is TOKEN_INDEX_OPEN) {
    THROW_SYNTAX_ERR_IF(v->type isnot ARRAY_TYPE and v->type isnot LIST_TYPE,
      "map get: not an array nor a list");

    TOKENVAL = *v;
    UNGET_BYTE();

    if (v->type is ARRAY_TYPE)
      return la_parse_array_get (this, vp);

    return la_parse_list_get (this, vp);
  }

  if (TOKEN is TOKEN_PAREN_OPEN) {
    if (NULL is v)
      THROW_SYNTAX_ERR_FMT("%s, method doesn't exists", key);

    if (v->sym->scope is NULL and 0 is is_this)
      THROW_SYNTAX_ERR_FMT("%s, symbol has private scope", key);

    int type;
    if (v->type & UFUNCTION_TYPE)
      type = UFUNCTION_TYPE;
    else if ((v->type & 0x77) is TOKEN_BUILTIN)
      type = CFUNCTION_TYPE;
    else
      THROW_SYNTAX_ERR_FMT("%s, not a method", key);

    UNGET_BYTE();

    if (type is UFUNCTION_TYPE) {
      funT *uf = AS_FUNC_PTR((*v));

      VALUE th;

      if (submap)
        th = v->sym->value;
      else
        th = map_par.sym->value;

      la_define_symbol (this, uf, "this", MAP_TYPE, th, 0);
      this->funcState |= MAP_METHOD_STATE;
      err = la_parse_func_call (this, vp, NULL, uf, *v);

      NEXT_TOKEN();
    } else {
      CFunc op = (CFunc) AS_PTR((*v));
      TOKENARGS = ((*v).type >> 8) & 0xff;
      err = la_parse_func_call (this, vp, op, NULL, *v);
    }

    return err;
  }

  if (v->type is MAP_TYPE) {
    if (0 is is_this or this->objectState & MAP_ASSIGNMENT) {
      *vp = la_copy_map (*v);
    } else {
      if (v->sym isnot NULL and is_this and this->funcState & RETURN_STATE
           and 0 is this->exprList) {
        vp->refcount++;
        this->objectState |= MAP_MEMBER;
      }
    }
  }

theend:
  if (vp->type is STRING_TYPE or vp->type is ARRAY_TYPE)
    this->objectState |= MAP_MEMBER;

  return LA_OK;
}

static int la_parse_map_set (la_t *this) {
  int is_this = (GETSTRLEN(TOKENSTR) is 4 and
      Cstring.eq_n (GETSTRPTR(TOKENSTR), "this", 4));

  VALUE map_par = TOKENVAL;
  Vmap_t *map = AS_MAP(map_par);
  int override = this->objectState & FUNC_OVERRIDE;
  this->objectState &= ~FUNC_OVERRIDE;

  int c = NEXT_BYTE_NOWS_NONL();

  if (c is '=') {
    VALUE v = TOKENVAL;
    sym_t *sym = v.sym;
    if (sym isnot NULL) {
      sym->type = NULL_TYPE;
      sym->value = NULL_VALUE;
    }

    Vmap.release (map);

    return LA_MMT_REASSIGN;
  }

  NEXT_TOKEN();
  c = TOKEN;

  if (c isnot TOKEN_DOT)
    THROW_SYNTAX_ERR("awaiting .");

  int submap = 0;
  int err;

  redo: {}
  char key[MAXLEN_SYMBOL + 1];
  TOKEN = la_get_map_key (this, key);
  THROW_ERR_IF_ERR(TOKEN);

  NEXT_TOKEN();
  c = TOKEN;
  VALUE *v;

  if (c is TOKEN_PAREN_OPEN) {
    v = Vmap.get (map, key);
    if (NULL is v)
      THROW_SYNTAX_ERR_FMT("error while seting map: %s method doesn't exists", key);

    if (v->sym->scope is NULL and 0 is is_this)
      THROW_SYNTAX_ERR_FMT("%s, symbol has private scope", key);

    int type;
    if (v->type & UFUNCTION_TYPE)
      type = UFUNCTION_TYPE;
    else if (IS_CFUNC(v->type))
      type = CFUNCTION_TYPE;
    else
      THROW_SYNTAX_ERR_FMT("%s, not a method", key);

    UNGET_BYTE();

    if (type is UFUNCTION_TYPE) {
      funT *uf = AS_FUNC_PTR((*v));

      VALUE th;

      if (submap)
        th = v->sym->value;
      else
        th = map_par.sym->value;

      la_define_symbol (this, uf, "this", MAP_TYPE, th, 0);
      this->funcState |= MAP_METHOD_STATE;
      VALUE vp;
      err = la_parse_func_call (this, &vp, NULL, uf, *v);
      NEXT_TOKEN();
    } else {
      CFunc op = (CFunc) AS_PTR((*v));
      TOKENARGS = ((*v).type >> 8) & 0xff;
      VALUE vp;
      err = la_parse_func_call (this, &vp, op, NULL, *v);
    }

    return err;
  }

  if (c < TOKEN_ASSIGN) {
    ifnot (Vmap.key_exists (map, key))
      THROW_SYNTAX_ERR_FMT("%s: map key couldn't been found", key);

    if (c is TOKEN_DOT or c is TOKEN_COLON) {
      v = Vmap.get (map, key);
      if (v->type isnot MAP_TYPE)
        THROW_SYNTAX_ERR_FMT("%s, not a map", key);

      map = AS_MAP((*v));

      is_this = 0;
      if (c is TOKEN_COLON)
        submap = 1;
      else
        submap = 0;

      goto redo;
    }

    if (c is TOKEN_INDEX_OPEN) {
      v = Vmap.get (map, key);
        THROW_SYNTAX_ERR_IF(v->type isnot ARRAY_TYPE and v->type isnot LIST_TYPE,
          "map set: not an array nor a list");

      TOKENVAL = *v;
      UNGET_BYTE();

      if (v->type is ARRAY_TYPE)
        return la_parse_array_set (this);

      return la_parse_list_set (this);
    }
  }

  v = Vmap.get (map, key);
  if (v isnot NULL) {
    if (v->sym->scope is NULL and 0 is is_this)
      THROW_SYNTAX_ERR_FMT("%s, symbol has private scope", key);

    if (v->type & UFUNCTION_TYPE or
       (v->type & 0x77) is TOKEN_BUILTIN) {
      if (c is TOKEN_ASSIGN and 0 is override)
        THROW_SYNTAX_ERR("you can not override a method");
    }
  }

  int scope = PUBLIC_SCOPE;
  if (c <= TOKEN_ASSIGN) {
    if (v isnot NULL) {
      scope = v->sym->scope isnot NULL;
      la_release_map_val (Vmap.pop (map, key));
    }
  } else
    return map_set_append_rout (this, map, key, c);

  return map_set_rout (this, map, key, scope);
}


static int la_parse_new (la_t *this, VALUE *vp) {
  int err;

  NEXT_RAW_TOKEN();
  int c = TOKEN;

  THROW_SYNTAX_ERR_IF(c isnot TOKEN_SYMBOL, "awaiting a type name");

  char type[MAXLEN_SYMBOL + 1];
  Cstring.cp (type, MAXLEN_SYMBOL + 1, GETSTRPTR(TOKENSTR),
      GETSTRLEN(TOKENSTR));

  ifnot (Vmap.key_exists (this->types, type))
    THROW_SYNTAX_ERR_FMT("%s, not such type", type);

  string *block = (string *) Vmap.get (this->types, type);

  TOKENSTR = StringNew (block->bytes);

  int state = this->funcState;
  this->funcState |= TYPE_NEW_STATE;
  err = la_parse_map (this, vp);
  ifnot (state & TYPE_NEW_STATE) this->funcState &= ~TYPE_NEW_STATE;
  THROW_ERR_IF_ERR(err);

  THROW_SYNTAX_ERR_IF(vp->type isnot MAP_TYPE, "not a type");

  Vmap_t *map = AS_MAP((*vp));

  THROW_SYNTAX_ERR_IF(TOKEN isnot TOKEN_PAREN_OPEN, "awaiting (");

  VALUE *val = Vmap.get (map, "init");

  THROW_SYNTAX_ERR_IF(NULL is val, "init method doesn't exists");

  ifnot (val->type & UFUNCTION_TYPE)
   THROW_SYNTAX_ERR("init, not a function method");

  UNGET_BYTE();

  funT *uf = AS_FUNC_PTR((*val));
  VALUE th = val->sym->value;
  la_define_symbol (this, uf, "this", MAP_TYPE, th, 0);
  this->funcState |= MAP_METHOD_STATE;
  VALUE v;
  err = la_parse_func_call (this, &v, NULL, uf, *val);
  THROW_ERR_IF_ERR(err);
  NEXT_TOKEN();
  return err;
}

static int la_parse_type (la_t *this) {
  NEXT_RAW_TOKEN();

  THROW_SYNTAX_ERR_IF(TOKEN isnot TOKEN_SYMBOL, "awaiting a type name");

  char type[MAXLEN_SYMBOL + 1];
  Cstring.cp (type, MAXLEN_SYMBOL + 1, GETSTRPTR(TOKENSTR),
      GETSTRLEN(TOKENSTR));

  THROW_SYNTAX_ERR_IF('Z' < type[0] or type[0] < 'A',
    "type names begin with a capital");

  NEXT_TOKEN();

  THROW_SYNTAX_ERR_IF(TOKEN isnot TOKEN_BLOCK, "awaiting block");

  string *block = String.new_with_len (GETSTRPTR(TOKENSTR),
      GETSTRLEN(TOKENSTR));

  Vmap.set (this->types, type, block, la_release_unit, 0);

  NEXT_TOKEN();

  return LA_OK;
}

static char EXPR_NAMES[20][128];

static int la_parse_expr_list (la_t *this) {
  int err;
  la_string saved_ptr;
  int count = this->argCount;
  this->argCount = 0;
  VALUE v;

  int c = TOKEN;
  if (c is TOKEN_SEMICOLON) {
    if (NEXT_BYTE_NOWS() is TOKEN_NL) NEXT_TOKEN();
    goto parse_qualifiers;
  }

  this->exprList++;
  do {
    this->curState |= MALLOCED_STRING_STATE;
    this->objectState &= ~ANNON_ARRAY;
    this->exprList++;

    la_string tokenstr = TOKENSTR;
    int len = GETSTRLEN(tokenstr);
    if (len < 128) {
      char *key = sym_key (this, TOKENSTR);
      Cstring.cp (EXPR_NAMES[this->argCount], 128, key, len);
    }

    err = la_parse_expr (this, &v);

    this->exprList--;
    THROW_ERR_IF_ERR(err);

    if (v.refcount is STRING_LITERAL)
      // if (v.sym isnot NULL and 0 is (this->funcState & MAP_METHOD_STATE)) {
      if (v.sym isnot NULL) {
        VALUE t = la_copy_value (v);
        v = t;
        v.refcount--;
      }

    if (v.refcount is ARRAY_LITERAL)
      ifnot (this->objectState & ANNON_ARRAY)
        v.refcount--;

    if (this->curState & FUNC_CALL_RESULT_IS_MMT)
      if (v.sym is NULL)
        if (v.refcount > -1)
          v.refcount--;

    stack_push (this, v);

    count++;

    c = TOKEN;

    if (c is TOKEN_COMMA) {
      this->objectState &= ~ANNON_ARRAY;
      this->curState |= MALLOCED_STRING_STATE;
      NEXT_TOKEN();
    }
  } while (c is TOKEN_COMMA);

  this->exprList--;

  this->objectState &= ~(ARRAY_MEMBER|MAP_MEMBER|ANNON_ARRAY);

  if (c isnot TOKEN_PAREN_CLOS) {
    ifnot (c is TOKEN_SEMICOLON) {
      ifnot (NEXT_BYTE_NOWS_NONL() is TOKEN_SEMICOLON)
        goto theend;

      NEXT_TOKEN();
      if (TOKEN is TOKEN_NL)
        NEXT_TOKEN();

    } else
      if (NEXT_BYTE_NOWS() is TOKEN_NL)
        NEXT_TOKEN();

parse_qualifiers:
    saved_ptr = PARSEPTR;

    NEXT_TOKEN();

    THROW_SYNTAX_ERR_IF(TOKEN is TOKEN_NL, "error while parsing qualifiers of an expression list, found two consecutive new lines");

    if (TOKEN is TOKEN_BLOCK) {
      err = la_parse_map (this, &v);
      THROW_ERR_IF_ERR(err);

      la_set_qualifiers (this, v);

    } else if (TOKEN is TOKEN_MAP) {
      this->exprList++;
      err = la_parse_map_get (this, &v);
      this->exprList--;
      THROW_ERR_IF_ERR(err);

      VALUE val = la_copy_map (v);
      v = val;

      la_set_qualifiers (this, v);

    } else {
      if (PEEK_NTH_TOKEN(0) is TOKEN_COLON) {
        Vmap_t *map = Vmap.new (32);
        PARSEPTR = saved_ptr;
        RESET_TOKEN;

        this->exprList++;
        err = la_parse_map_members (this, MAP(map));
        this->exprList--;
        THROW_ERR_IF_ERR(err);

        THROW_SYNTAX_ERR_IF(TOKEN isnot TOKEN_PAREN_CLOS,
          "error while getting qualifiers, awaiting )");

        la_set_qualifiers (this, MAP(map));

      } else {
        this->exprList++;
        err = la_parse_primary (this, &v);
        this->exprList--;
        THROW_ERR_IF_ERR(err);

        if (v.type is MAP_TYPE)
          la_set_qualifiers (this, v);
        else if (v.type isnot NULL_TYPE)
          THROW_SYNTAX_ERR("awaiting a map as qualifiers");
      }
    }
  }

theend:
  return count;
}

static int la_parse_char (la_t *this, VALUE *vp, la_string token) {
  VALUE v = INT(0);
  const char *ptr = GETSTRPTR(token);

  if (ptr[0] is TOKEN_SQUOTE) THROW_SYNTAX_ERR("error while getting a char token ");

  if (ptr[0] is TOKEN_ESCAPE_CHR) {
    if (ptr[1] is 'n')  { v = INT('\n'); goto theend; }
    if (ptr[1] is 't')  { v = INT('\t'); goto theend; }
    if (ptr[1] is 'r')  { v = INT('\r'); goto theend; }
    if (ptr[1] is '\\') { v = INT('\\'); goto theend; }
    if (ptr[1] is TOKEN_SQUOTE) { v = INT(TOKEN_SQUOTE); goto theend; }
    THROW_SYNTAX_ERR("unknown escape sequence");
  }

  if (ptr[0] >= ' ' and ptr[0] <= '~') {
    if (ptr[1] is TOKEN_SQUOTE) {
      v = INT (ptr[0]);
      goto theend;
    } else {
      THROW_SYNTAX_ERR("error while taking character literal");
    }
  }

  int len = 0;
  utf8 c = Ustring.get.code_at ((char *) ptr, 4, 0, &len);

  if (TOKEN_SQUOTE isnot ptr[len])
    THROW_SYNTAX_ERR("error while taking character literal");

  v = INT(c);

theend:
  *vp = v;
  return LA_OK;
}

static int la_parse_string (la_t *this, la_string str) {
  int c,  r;
  la_string savepc = PARSEPTR;

  PARSEPTR = str;

  for (;;) {
    NEXT_TOKEN();

    while (TOKEN is TOKEN_NL or TOKEN is TOKEN_SEMICOLON)
      NEXT_TOKEN();

    if (TOKEN < 0) break;

    do_token:
    if (TOKEN is UFUNC_TYPE) {
      c = NEXT_BYTE_NOWS_NONL();
      if (c isnot TOKEN_PAREN_OPEN)
        TOKEN = TOKEN_VAR;
    }

    r = la_parse_stmt (this);

    if (r isnot LA_OK) return r;

    if (TOKEN is TOKEN_NL or TOKEN is TOKEN_SEMICOLON or
        TOKEN < 0)
      continue;
    //else
    goto do_token; // this is experimental (allow multiply statements in a single line)
    //return this->syntax_error (this, STR_FMT("%s(), unknown token |%c| |%d|", __func__, c, c));
  }

  PARSEPTR = savepc;

  return LA_OK;
}

static void la_fun_refcount_incr (int *count) {
  (*count)++;
}

static void la_fun_refcount_decr (int *count) {
  (*count)--;
}

static void la_fun_release_symbols (funT *uf, int clear, int is_method) {
  if (NULL is uf->symbols) return;

  if (is_method) {
    sym_t *sym = Vmap.pop (uf->symbols, "this");
    free (sym);
  }

  if (clear) {
    Vmap.clear (uf->symbols);
    return;
  }

  Vmap.release (uf->symbols);
}

static void la_fun_release_symbols_only (funT *uf, int is_method) {
  if (NULL is uf->symbols) return;

  if (is_method) {
    sym_t *sym = Vmap.pop (uf->symbols, "this");
    free (sym);
  }

  string **keys = Vmap.keys (uf->symbols);
  int num_keys  = Vmap.num_keys (uf->symbols);
  for (int i = 0; i < num_keys; i++) {
    sym_t *sym = Vmap.pop (uf->symbols, keys[i]->bytes);
    if (sym->type is STRING_TYPE) { // or sym->type is ARRAY_TYPE) {
      VALUE v = sym->value;
      if (0 is v.refcount or v.refcount is MALLOCED_STRING)
        String.release (AS_STRING(v));
    }
    free (sym);
  }

  for (int i = 0; i < num_keys; i++)
    String.release (keys[i]);
  free (keys);
}

static int la_parse_func_call (la_t *this, VALUE *vp, CFunc op, funT *uf, VALUE value) {
  NEXT_TOKEN();
  int c = TOKEN;

  if (c isnot TOKEN_PAREN_OPEN) {
    UNGET_BYTE();
    VALUE v;
    if (uf isnot NULL) {
      v = PTR(uf);
      v.type |= UFUNCTION_TYPE;
    } else {
      NEXT_TOKEN();
      v = PTR(op);
      v.type = value.sym->type;
    }
    v.sym = value.sym;
    *vp = v;
    return LA_OK;
  }

  int expectargs = (uf ? uf->nargs : TOKENARGS);

  int paramCount = 0;

  int is_method = this->funcState & MAP_METHOD_STATE;
  this->funcState &= ~MAP_METHOD_STATE;

  this->curState |= MALLOCED_STRING_STATE;

  NEXT_TOKEN();
  c = TOKEN;

  if (c isnot TOKEN_PAREN_CLOS) {
    paramCount = la_parse_expr_list (this);

    c = TOKEN;
    if (paramCount < 0)
      return paramCount;
  } else {
    paramCount = this->argCount;
    this->argCount = 0;
  }

  this->curState &= ~(MALLOCED_STRING_STATE);

  do {
    if (c isnot TOKEN_PAREN_CLOS) {
      if (c is TOKEN_NL) {
        if (PEEK_NTH_TOKEN(0) is TOKEN_PAREN_CLOS) {
          NEXT_TOKEN();
          break;
        }
      }

      THROW_SYNTAX_ERR("error while parsing expression list: expected closed parentheses");
    }
  } while (0);
  if (expectargs isnot paramCount)
    THROW_NUM_FUNCTION_ARGS_MISMATCH(expectargs, paramCount);

  while (paramCount > 0) {
    --paramCount;
    this->funArgs[paramCount] = stack_pop (this);
  }

  int err = LA_OK;

  if (uf) {
    int refcount = Imap.set_by_callback (this->funRefcount, uf->funname, la_fun_refcount_incr);
    if (refcount > 1) {
      if (Cstring.eq (this->curScope->funname, uf->funname))
        la_symbol_stack_push (this, this->curScope->symbols);
      else
        la_symbol_stack_push (this, uf->symbols);

      for (int i = 0; i < expectargs; i++)
        this->funArgs[i] = la_copy_value (this->funArgs[i]);

      la_fun_release_symbols_only (uf, is_method);
    }

    sym_t *uf_argsymbols[expectargs];

    for (int i = 0; i < expectargs; i++) {
      VALUE v = this->funArgs[i];
      if (v.type & UFUNCTION_TYPE) {
        funT *f = AS_FUNC_PTR(v);
        uf_argsymbols[i] = la_define_symbol (this, uf, uf->argName[i], (UFUNC_TYPE | (f->nargs << 8)), v, 0);
      } else {
        v.refcount += (v.refcount > -2 ? (refcount < 2) : 0);
        uf_argsymbols[i] = la_define_symbol (this, uf, uf->argName[i], v.type, v, 0);
      }
    }

    int hastoreturn = HASTORETURN;
    HASTORETURN = 0;

    uf->result = NULL_VALUE;

    la_fun_stack_push (this, this->curScope);

    this->curScope = uf;

    string *func = AS_STRING(this->func->value);
    String.replace_with (func, uf->funname);

    int varisnotallowed = this->curState & VAR_IS_NOT_ALLOWED;
    this->curState &= ~VAR_IS_NOT_ALLOWED;

    int loopcount = this->loopCount;
    this->loopCount = 0;

    la_string savepc = PARSEPTR;
again:

    err = la_parse_string (this, uf->body);

    if (err is LA_ERR_TCALLREC) {
      NEXT_TOKEN();
      THROW_SYNTAX_ERR_IF(TOKEN isnot TOKEN_PAREN_OPEN,
        "error while parsing function call, awaiting (");

      this->curState |= MALLOCED_STRING_STATE;
      NEXT_TOKEN();
      paramCount = la_parse_expr_list (this);
      this->curState &= ~MALLOCED_STRING_STATE;

      THROW_SYNTAX_ERR_IF(TOKEN isnot TOKEN_PAREN_CLOS,
        "error while parsing function call, awaiting )");

      int peek_tok = PEEK_NTH_TOKEN(0);

      if ((peek_tok & 0xff) is TOKEN_BINOP)
        THROW_SYNTAX_ERR ("awaiting a tail call");

      if (paramCount < 0)
        return paramCount;

      if (expectargs isnot paramCount)
        THROW_NUM_FUNCTION_ARGS_MISMATCH(expectargs, paramCount);

      while (paramCount > 0) {
        this->funArgs[--paramCount] = stack_pop (this);
      }

      for (int i = 0; i < expectargs; i++) {
        VALUE v = this->funArgs[i];
        VALUE val = la_copy_value (v);
        val.refcount = 0;
        this->funArgs[i] = val;
      }

      la_fun_release_symbols (uf, 1, is_method);

      for (int i = 0; i < expectargs; i++) {
        VALUE v = this->funArgs[i];
        if (v.type & UFUNCTION_TYPE) {
          funT *f = AS_FUNC_PTR(v);
          uf_argsymbols[i] = la_define_symbol (this, uf, uf->argName[i], (UFUNC_TYPE | (f->nargs << 8)), v, 0);
        } else {
          uf_argsymbols[i] = la_define_symbol (this, uf, uf->argName[i], v.type, v, 0);
        }
      }

      goto again;
    }

    PARSEPTR = savepc;

    this->loopCount = loopcount;
    if (this->loopCount)
      this->curState |= LOOP_STATE;

    if (varisnotallowed) this->curState |= VAR_IS_NOT_ALLOWED;

    this->curScope = la_fun_stack_pop (this);

    String.replace_with (func, this->curScope->funname);

    HASTORETURN = hastoreturn;

    la_release_qualifiers (this);

    this->curState &= ~FUNC_CALL_RESULT_IS_MMT;
    if (uf->result.type >= UFUNCTION_TYPE) {
      sym_t *sym = uf->result.sym;
      this->curState |= FUNC_CALL_RESULT_IS_MMT;

      ifnot (NULL is sym) {
        if (uf->result.type is STRING_TYPE and
            uf->result.sym->type is MAP_TYPE) {
          VALUE v = STRING(String.dup(AS_STRING(uf->result)));
          uf->result = v;
        }

        sym->value = NULL_VALUE;
        uf->result.sym = NULL;

      } else {
        ifnot (this->exprList)
          if (uf->result.refcount is MALLOCED_STRING) {
            VALUE v = STRING(String.dup(AS_STRING(uf->result)));
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

        if (sym isnot NULL and uf_sym->scope isnot sym->scope) {
          sym->value = uf_val;
          uf_sym->value = NULL_VALUE;
        } else {
          if (v.type is STRING_TYPE and v.refcount <= MALLOCED_STRING)
            uf_sym->value = NULL_VALUE;
          else if  (v.type is MAP_TYPE and v.sym isnot NULL)
            uf_sym->value = NULL_VALUE;
        }
      }
    }

    refcount = Imap.set_by_callback (this->funRefcount, uf->funname, la_fun_refcount_decr);

    ifnot (refcount)
      la_fun_release_symbols (uf, 1, is_method);
    else {
      la_fun_release_symbols (uf, 0, is_method);

      if (Cstring.eq (this->curScope->funname, uf->funname))
        this->curScope->symbols = la_symbol_stack_pop (this);
      else
        uf->symbols = la_symbol_stack_pop (this);
    }

    *vp = uf->result;

    return err;
  }

  this->CFuncError = LA_OK;
  this->curMsg[0] = '\0';

  *vp = op (this, this->funArgs[0], this->funArgs[1], this->funArgs[2],
                  this->funArgs[3], this->funArgs[4], this->funArgs[5],
                  this->funArgs[6], this->funArgs[7], this->funArgs[8]);

  for (int i = 0; i < expectargs; i++) {
    VALUE v = this->funArgs[i];
    if (v.type is STRING_TYPE)
      if (v.refcount <= MALLOCED_STRING)
        continue;

    if (v.sym is NULL)
      la_release_val (this, v);
  }

  la_release_qualifiers (this);

  this->curState &= ~FUNC_CALL_RESULT_IS_MMT;
  if (vp->type >= UFUNCTION_TYPE)
    this->curState |= FUNC_CALL_RESULT_IS_MMT;

  err = this->CFuncError;

  if (err isnot LA_OK)
    if (*this->curMsg)
      this-> print_fmt_bytes (this->err_fp, "C function error: %s\n", this->curMsg);

  NEXT_TOKEN();

  return err;
}

static int la_parse_if_in_chain (la_t *, VALUE *);
static int la_parse_chain (la_t *this, VALUE *vp) {
  int err, c;

  sym_t *sym = NULL;
  char method[MAXLEN_SYMBOL * 2];
  this->funcState |= CHAIN_STATE;

  do {
    NEXT_RAW_TOKEN();
    while (TOKEN is TOKEN_NL) NEXT_RAW_TOKEN();
    c = TOKEN;

    VALUE save_v;

    this->objectState &= ~(ARRAY_MEMBER|MAP_MEMBER);

    int fun_should_be_freed = 0;

    if (c isnot TOKEN_SYMBOL)
      THROW_SYNTAX_ERR("awaiting a method");

    char *key = map_key (this, TOKENSTR);

    switch (vp->type) {
      case STRING_TYPE:
        Cstring.cp_fmt (method, MAXLEN_SYMBOL * 2, "string_%s", key);
        break;

      case INTEGER_TYPE:
        Cstring.cp_fmt (method, MAXLEN_SYMBOL * 2, "integer_%s", key);
        break;

      case NUMBER_TYPE:
        Cstring.cp_fmt (method, MAXLEN_SYMBOL * 2, "number_%s", key);
        break;

      case ARRAY_TYPE:
        Cstring.cp_fmt (method, MAXLEN_SYMBOL * 2, "array_%s", key);
        break;

      case MAP_TYPE:
        Cstring.cp_fmt (method, MAXLEN_SYMBOL * 2, "map_%s", key);
        break;

      case LIST_TYPE:
        Cstring.cp_fmt (method, MAXLEN_SYMBOL * 2, "list_%s", key);
        break;

      case NULL_TYPE:
      default:
          method[0] = '\0';
        break;

      // default: allow
      //   THROW_SYNTAX_ERR("unsupported datatype");
    }

    sym = ns_lookup_symbol (this->std, method);

    if (sym is NULL)
      sym = ns_lookup_symbol (this->std, key);

    if (sym is NULL or sym->value.type is 0) {
      sym = la_lookup_symbol (this, TOKENSTR);

      if (sym is NULL)
        THROW_SYNTAX_ERR_FMT("%s|%s: unknown function", method, key);

      switch (sym->type) {
        case TOKEN_LAMBDA:
          err = la_parse_lambda (this, vp, 1);
          THROW_ERR_IF_ERR(err);

          sym = this->curSym;
          sym->value = PTR(this->curFunDef);
          fun_should_be_freed = 1;
          break;

        case TOKEN_FORMAT: {
          if (vp->type isnot STRING_TYPE)
            THROW_SYNTAX_ERR("awaiting a string type");

          string *s = AS_STRING ((*vp));
          size_t len = s->num_bytes + 4;
          char buf[len + 1];
          int idx = 0;
          buf[idx++] = '(';
          char *ptr = s->bytes;
          if (*ptr isnot '\"') {
            char *evptr = Cstring.byte.in_str (ptr, '$');
            if (NULL isnot evptr and (evptr is ptr or
                *(evptr - 1) isnot '\\')) {
              buf[idx++] = '"';
              Cstring.cp (buf + idx, len, ptr, s->num_bytes);
              idx += s->num_bytes;
              buf[idx++] = '"';
            } else {
              Cstring.cp (buf + idx, len, ptr, s->num_bytes);
              idx += s->num_bytes;
            }
          } else {
            Cstring.cp (buf + idx, len, ptr, s->num_bytes);
            idx += s->num_bytes;
          }
          buf[idx++] = ')';
          buf[idx] = '\0';

          la_string savepc = PARSEPTR;
          PARSEPTR = StringNew (buf);

          save_v = *vp;
          if (save_v.refcount is STRING_LITERAL)
            save_v.refcount = 0;

          err = la_parse_format (this, vp);
          PARSEPTR = savepc;

          NEXT_TOKEN();
          c = TOKEN;

          if (c isnot TOKEN_PAREN_OPEN)
            THROW_SYNTAX_ERR("awaiting (");

          NEXT_TOKEN();
          c = TOKEN;
          if (c isnot TOKEN_PAREN_CLOS)
            THROW_SYNTAX_ERR("awaiting )");
          NEXT_TOKEN();
          goto release_value;
        }

        default:
          if (IS_UFUNC(sym->type) or IS_CFUNC(sym->type))
            break;

          THROW_SYNTAX_ERR_FMT("%s|%s: unknown function", method, key);
      }
    }

    if (sym->type is TOKEN_IF or sym->type is TOKEN_IFNOT) {
      save_v = *vp;
      if (save_v.refcount is STRING_LITERAL)
        save_v.refcount = 0;

      TOKEN = sym->type;

      err = la_parse_if_in_chain (this, vp);

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

    if (type is UFUNCTION_TYPE) {
      funT *uf = AS_FUNC_PTR(val);
      err = la_parse_func_call (this, vp, NULL, uf, *vp);

      if (save_v.refcount is ARRAY_LITERAL)
        save_v.refcount--;

      if (fun_should_be_freed) {
        sym->value = NULL_VALUE;
        fun_release (&uf);
        fun_should_be_freed = 0;
      }

      NEXT_TOKEN();

    } else {
      CFunc op = (CFunc) AS_PTR(sym->value);
      TOKENARGS = (sym->type >> 8) & 0xff;
      err = la_parse_func_call (this, vp, op, NULL, val);

      if (save_v.type is MAP_TYPE or save_v.refcount is ARRAY_LITERAL)
        save_v.refcount--;
    }

    release_value:

    if (save_v.sym is NULL) {
      if (save_v.refcount > -1) {
        la_release_val (this, save_v);
      } else if (save_v.refcount is ARRAY_LITERAL) {
        if (vp->refcount is ARRAY_LITERAL + 1) // care as ARRAY_LITERAL < 0
          vp->refcount--; // though it is the opposite, it matches with the code above
        else              // and with the how the if_in_chain() handles it
          la_release_val (this, save_v);
      }
    }

  } while (TOKEN is TOKEN_COLON);

  this->funcState &= ~CHAIN_STATE;
  return err;
}

static int la_parse_prefix (la_t *this, VALUE *vp, int op_token) {
  int err;

  NEXT_TOKEN();

  switch (TOKEN) {
    case INTEGER_TYPE:
    case NUMBER_TYPE: {
      err = la_parse_primary (this, vp);
      THROW_ERR_IF_ERR(err);
      THROW_SYNTAX_ERR_IF(vp->type isnot INTEGER_TYPE and vp->type isnot NUMBER_TYPE,
         "awaiting an integer or a number");

      sym_t *sym = NULL;

      sym = vp->sym;
      THROW_SYNTAX_ERR_IF(sym is NULL, "awaiting a symbol");

      switch (vp->type) {
        case INTEGER_TYPE:
          sym->value = INT(AS_INT((*vp)) + (op_token is TOKEN_PLUS_PLUS ? 1 : -1));
          break;

        case NUMBER_TYPE:
          sym->value = NUMBER(AS_NUMBER((*vp)) + (op_token is TOKEN_PLUS_PLUS ? 1 : -1));
      }

      *vp = sym->value;
      return LA_OK;
    }

    case TOKEN_ARRAY:
    // fall through
    case TOKEN_MAP: {
      int token = TOKEN;
      VALUE tokenval = TOKENVAL;

      const char *ptr = GETSTRPTR(PARSEPTR);
      err = la_parse_primary (this, vp);
      THROW_ERR_IF_ERR(err);
      THROW_SYNTAX_ERR_IF(vp->type isnot INTEGER_TYPE and vp->type isnot NUMBER_TYPE,
        "awaiting an integer or a number");

      const char *cptr = GETSTRPTR(PARSEPTR);
      integer len = cptr - ptr;
      char buf[len + 128];
      integer idx = 0;
      for (integer i = 0; i < len; i++) {
        if (ptr[i] is '-') {
          if (ptr[i+1] is '-') { // there is room: if last then it is a syntax error
            THROW_SYNTAX_ERR_IF(i + 2 is len or
              (i + 3 is len and (ptr[i+2] is '\n' or ptr[i+2] is ';')),
              "prefixed and postfixed operations on the same object, is not allowed");
              /* the above should be catched before and should count and for
               * the opposite operators
               */

            if (i and (ptr[i - 1] isnot ' ' and ptr[i - 1] isnot TOKEN_INDEX_OPEN and ptr[i - 1] isnot TOKEN_PAREN_OPEN)) {
              buf[idx++] = ' '; buf[idx++] = '+';
              buf[idx++] = ' '; buf[idx++] = '1';
            }

            i++;
            continue;
          }
        }

        if (ptr[i] is '+') {
          if (ptr[i+1] is '+') {
            THROW_SYNTAX_ERR_IF(i + 2 is len or
              (i + 3 is len and (ptr[i+2] is '\n' or ptr[i+2] is ';')),
              "prefixed and postfixed operations on the same object, is not allowed");

            if (i and (ptr[i - 1] isnot ' ' and ptr[i - 1] isnot TOKEN_INDEX_OPEN and ptr[i - 1] isnot TOKEN_PAREN_OPEN)) {
              buf[idx++] = ' '; buf[idx++] = '-';
              buf[idx++] = ' '; buf[idx++] = '1';
            }

            i++;
            continue;
          }
        }

        buf[idx++] = ptr[i];
      }

      if (buf[idx - 1] is '\n' or buf[idx - 1] is ';') {
        idx--;
      } else {
        int toklen = GETSTRLEN(TOKENSTR);
        if (buf[idx - toklen - 1] is ' ')
          idx -= (toklen + 1);
        else
          THROW_SYNTAX_ERR("unexpected token on a prefix operation");
      }

      op_token = (op_token is TOKEN_PLUS_PLUS ? '+' : '-');

      buf[idx++] = op_token; buf[idx++] = op_token;
      buf[idx] = '\0';

      int curtok = TOKEN;
      VALUE curtokenval = TOKENVAL;
      la_string savepc = PARSEPTR;

      PARSEPTR = StringNew(buf);
      TOKEN = token;
      TOKENVAL = tokenval;

      err = la_parse_primary (this, vp);
      THROW_ERR_IF_ERR(err);

      PARSEPTR = savepc;
      TOKEN = curtok;
      TOKENVAL = curtokenval;

      VALUE v;
      switch (vp->type) {
        case INTEGER_TYPE:
          v = INT(AS_INT((*vp)) + (op_token is '+' ? 1 : -1));
          *vp = v;
          return LA_OK;

        case NUMBER_TYPE:
          v = NUMBER(AS_NUMBER((*vp)) + (op_token is '-' ? 1 : -1));
          *vp = v;
          return LA_OK;
      }
    }
    break;

    default:
      THROW_SYNTAX_ERR("awaiting a variable, a map or an array");
  }

  return LA_NOTOK;
}

static int la_parse_primary (la_t *this, VALUE *vp) {
  int err = LA_OK;
  int c = TOKEN;

  switch (c) {
    case TOKEN_INDEX_OPEN:
      ifnot (this->curState & INDEX_STATE) {
        err = la_get_anon_array (this, vp);
        if (err is LA_OK) {

          if (TOKEN isnot TOKEN_INDEX_CLOS) {
            if (TOKEN isnot TOKEN_NL)
              THROW_SYNTAX_ERR("array expression, awaiting ]");
            else
              GET_BYTE();
          }

          c = NEXT_BYTE_NOWS_NONL();

          if (c is TOKEN_INDEX_OPEN) {
            VALUE saved_val = *vp;
            TOKEN = TOKEN_ARRAY;
            TOKENVAL = *vp;

            err = la_parse_primary (this, vp);
            THROW_ERR_IF_ERR(err);

            if (vp->type is STRING_TYPE) {
              string *str = AS_STRING((*vp));
              *vp = STRING_NEW_WITH_LEN(str->bytes, str->num_bytes);
            }

            la_release_val (this, saved_val);
            return err;
          }

          NEXT_TOKEN();
          c = TOKEN;

          if (c is TOKEN_COLON and 0 is (this->curState & INDEX_STATE)) {
            vp->refcount = ARRAY_LITERAL;
            TOKENVAL = *vp;
            return la_parse_chain (this, vp);
          }

          return LA_OK;
        }

        return err;
      }
      /* fall through */

    case TOKEN_PAREN_OPEN: {
      int close_token = (c is TOKEN_PAREN_OPEN
         ? TOKEN_PAREN_CLOS : TOKEN_INDEX_CLOS);

      NEXT_TOKEN();

      err = la_parse_expr (this, vp);
      THROW_ERR_IF_ERR(err);

      if (TOKEN is close_token and 0 is (this->curState & CLOSURE_STATE)) {
        NEXT_TOKEN();

        if (TOKEN is TOKEN_COLON and 0 is (this->curState & INDEX_STATE)) {
          TOKENVAL = *vp;
          return la_parse_chain (this, vp);
        }

        return LA_OK;
      }

      return err;
    }

    case TOKEN_IFNOT:
    case TOKEN_IF:

      err = la_parse_cond (this, c is TOKEN_IFNOT);
      THROW_ERR_IF_ERR(err);

      if (TOKEN isnot TOKEN_THEN) {
        if (TOKEN is TOKEN_NL or TOKEN is TOKEN_SEMICOLON) {
          if (PEEK_NTH_TOKEN(0) isnot TOKEN_THEN) {
            if (this->funcState & RETURN_STATE) {
              HASTORETURN = AS_INT(CONDVAL);
              *vp = NULL_VALUE;
              return LA_OK;
            }
            else
              THROW_SYNTAX_ERR("if expression: awaiting then");
          } else
            NEXT_TOKEN();
        } else
          THROW_SYNTAX_ERR("if expression: awaiting then");
      }

      return la_parse_iforelse (this, AS_INT(CONDVAL), vp);

    case TOKEN_INTEGER: {
      VALUE val = la_string_to_dec (TOKENSTR);
      *vp = val;

      NEXT_TOKEN();
      c = TOKEN;

      if (c is TOKEN_COLON and 0 is (this->curState & INDEX_STATE)) {
        TOKENVAL = *vp;
        return la_parse_chain (this, vp);
      }

      return LA_OK;
    }

    case TOKEN_NUMBER: {
      char *endptr; char str[32];
      Cstring.cp (str, 32, GETSTRPTR(TOKENSTR), GETSTRLEN(TOKENSTR));
      double val = strtod (str, &endptr);
      *vp = NUMBER(val);

      NEXT_TOKEN();
      c = TOKEN;

      if (c is TOKEN_COLON) {
        TOKENVAL = *vp;
        return la_parse_chain (this, vp);
      }
      return LA_OK;
    }

    case TOKEN_OCTAL:
      *vp = la_OctalStringToNum (TOKENSTR);

      NEXT_TOKEN();
      c = TOKEN;

      if (c is TOKEN_COLON) {
        TOKENVAL = *vp;
        return la_parse_chain (this, vp);
      }

      return LA_OK;

    case TOKEN_BINARY:
      *vp = la_BinaryStringToNum (TOKENSTR);

      NEXT_TOKEN();
      c = TOKEN;

      if (c is TOKEN_COLON and 0 is (this->curState & INDEX_STATE)) {
        TOKENVAL = *vp;
        return la_parse_chain (this, vp);
      }
      return LA_OK;

    case TOKEN_HEX_CHAR:
    case TOKEN_HEX_NUMBER:
      *vp = la_HexStringToNum (TOKENSTR);

      NEXT_TOKEN();
      c = TOKEN;

      if (c is TOKEN_COLON and 0 is (this->curState & INDEX_STATE)) {
        TOKENVAL = *vp;
        return la_parse_chain (this, vp);
      }
      return LA_OK;

    case TOKEN_CHAR:
      err = la_parse_char (this, vp, TOKENSTR);
      THROW_ERR_IF_ERR(err);

      NEXT_TOKEN();

      if (TOKEN is TOKEN_COLON and 0 is (this->curState & INDEX_STATE)) {
        TOKENVAL = *vp;
        return la_parse_chain (this, vp);
      }

      return err;

    case TOKEN_PRINTLN:
    case TOKEN_PRINT:
      if (c is TOKEN_PRINT)
        err = la_parse_print (this);
      else
        err = la_parse_println (this);
      THROW_ERR_IF_ERR (err);

      *vp = INT(this->byteCount);
      return LA_OK;

    case TOKEN_FORMAT:
      err = la_parse_format (this, vp);

      if (TOKEN is TOKEN_COLON) {
        vp->refcount = STRING_LITERAL;
        TOKENVAL = *vp;
        return la_parse_chain (this, vp);
      }
      return err;

    case TOKEN_LIST:
      err = la_parse_list_get (this, vp);
      THROW_ERR_IF_ERR(err);

      switch (TOKEN) {
        case TOKEN_COLON:
          if (this->curState & INDEX_STATE) return LA_OK;
          return la_parse_chain (this, vp);

        case TOKEN_INDEX_OPEN:
          switch (vp->type) {
            case ARRAY_TYPE:
              UNGET_BYTE();
              TOKEN = TOKEN_ARRAY;
              TOKENVAL = *vp;
              return la_parse_primary (this, vp);

            case STRING_TYPE:
              UNGET_BYTE();
              TOKEN = TOKEN_STRING;
              TOKENVAL = *vp;
              return la_parse_primary (this, vp);

            case LIST_TYPE:
              UNGET_BYTE();
              TOKEN = TOKEN_LIST;
              TOKENVAL = *vp;
              return la_parse_primary (this, vp);

            default:
              THROW_SYNTAX_ERR("awaiting an array or a string or a list");
          }

        case TOKEN_DOT:
          switch (vp->type) {
            case MAP_TYPE:
              UNGET_BYTE();
              TOKEN = TOKEN_MAP;
              TOKENVAL = *vp;
              return la_parse_primary (this, vp);

            default:
              THROW_SYNTAX_ERR("awaiting a map");
          }

        default:
          return LA_OK;
      }

    case  TOKEN_OBJECT:
    case TOKEN_FILEPTR:
    case      TOKEN_FD:
    case       0 ... 4:
    case     TOKEN_VAR:
      *vp = TOKENVAL;

      NEXT_TOKEN();

      if (TOKEN is TOKEN_PLUS_PLUS or TOKEN is TOKEN_MINUS_MINUS) {
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
              (TOKEN is TOKEN_PLUS_PLUS ? "++" : "--"));
        }

        VALUE v = *vp;
        sym_t *sym = vp->sym;

        switch (vp->type) {
          case INTEGER_TYPE:
            sym->value = INT(AS_INT((*vp)) + (TOKEN is TOKEN_PLUS_PLUS ? 1 : -1));
            NEXT_TOKEN();
            *vp = v;
            break;

          case NUMBER_TYPE:
            sym->value = NUMBER(AS_NUMBER((*vp)) + (TOKEN is TOKEN_PLUS_PLUS ? 1 : -1));
            NEXT_TOKEN();
            *vp = v;
            break;

          default:
            THROW_SYNTAX_ERR("awaiting an integer or a number");
        }
      }

      if (TOKEN isnot TOKEN_COLON or (this->curState & INDEX_STATE))
        return LA_OK;

      return la_parse_chain (this, vp);

    case TOKEN_ARRAY:
      err = la_parse_array_get (this, vp);
      THROW_ERR_IF_ERR(err);

      if (TOKEN isnot TOKEN_DOT) {
        if (TOKEN is TOKEN_COLON and 0 is (this->curState & INDEX_STATE)) {
          TOKENVAL = *vp;
          return la_parse_chain (this, vp);
        }

        return err;
      }

      if (vp->type isnot MAP_TYPE)
        THROW_SYNTAX_ERR("not a map");

      UNGET_BYTE();
      TOKENVAL = *vp;
      /* fall through */

    case TOKEN_MAP: {
      err = la_parse_map_get (this, vp);
      THROW_ERR_IF_ERR(err);

      int type;
      if (vp->type & UFUNCTION_TYPE)
        type = UFUNCTION_TYPE;
      else if (IS_CFUNC(vp->type))
        type = CFUNCTION_TYPE;
      else {
        if (TOKEN is TOKEN_COLON) {
          TOKENVAL = *vp;
          return la_parse_chain (this, vp);
        }

        break;
      }

      ifnot (TOKEN is TOKEN_PAREN_OPEN)
        break;

      UNGET_BYTE();

      if (type is UFUNCTION_TYPE) {
        funT *uf = AS_FUNC_PTR((*vp));
        VALUE th = vp->sym->value;
        la_define_symbol (this, uf, "this", MAP_TYPE, th, 0);
        this->funcState |= MAP_METHOD_STATE;
        err = la_parse_func_call (this, vp, NULL, uf, *vp);
        NEXT_TOKEN();
        return err;
      } else {
        CFunc op = (CFunc) AS_PTR((*vp));
        TOKENARGS = ((*vp).type >> 8) & 0xff;
        return la_parse_func_call (this, vp, op, NULL, *vp);
      }
    }

    case TOKEN_PLUS_PLUS:
    case TOKEN_MINUS_MINUS:
      return la_parse_prefix (this, vp, c);

    case TOKEN_APPEND:
      return la_parse_append (this, vp);

    case TOKEN_ANNOTATION:
      return la_parse_annotation (this, vp);

    case TOKEN_D_ISDEFINED:
    case TOKEN_D_GET:
      return la_parse_directive (this, vp, c);

    case TOKEN_EVALFILE: {
      this->funcState |= EVAL_UNIT_STATE;
      err = la_parse_loadfile (this);
      this->funcState &= ~EVAL_UNIT_STATE;
      THROW_ERR_IF_ERR(err);

      VALUE curFile = this->file->value;
      this->file->value = STRING(this->evalFile);

      funT *uf = this->curFunDef;
      VALUE ufv = PTR(uf);
      la_define_symbol (this, this->function, uf->funname, (UFUNC_TYPE | (uf->nargs << 8)), ufv, 0);
      la_string savepc = PARSEPTR;
      PARSEPTR = StringNew ("()");
      err = la_parse_func_call (this, vp, NULL, uf, ufv);
      this->file->value = curFile;
      PARSEPTR = savepc;
      return err;
    }

    case TOKEN_USRFUNC: {
      sym_t *symbol = TOKENSYM;
      ifnot (symbol)
        THROW_SYNTAX_ERR("user defined function, not declared");

      funT *uf = AS_FUNC_PTR(symbol->value);

      err = la_parse_func_call (this, vp, NULL, uf, symbol->value);
      THROW_ERR_IF_ERR(err);

      NEXT_TOKEN();
      c = TOKEN;

      if (c is TOKEN_COLON) {
        TOKENVAL = *vp;
        return la_parse_chain (this, vp);
      }

      return err;
    }

    case TOKEN_BUILTIN: {
      CFunc op = (CFunc) AS_PTR(TOKENVAL);
      err = la_parse_func_call (this, vp, op, NULL, TOKENSYM->value);
      THROW_ERR_IF_ERR(err);

      if (TOKEN is TOKEN_COLON) {
        TOKENVAL = *vp;
        return la_parse_chain (this, vp);
      }

      return err;
    }

    case TOKEN_LAMBDA:
      return la_parse_lambda (this, vp, 0);

    case TOKEN_STRING:
      err = la_string_get (this, vp);
      THROW_ERR_IF_ERR(err);

      if (TOKEN is TOKEN_COLON) {
        if (vp->sym is NULL and
            vp->refcount isnot MALLOCED_STRING and
            0 is (this->objectState & (ARRAY_MEMBER|MAP_MEMBER)))
          vp->refcount = STRING_LITERAL;

        this->objectState &= ~(ARRAY_MEMBER|MAP_MEMBER);
        TOKENVAL = *vp;
        return la_parse_chain (this, vp);
      }

      return err;

    case TOKEN_NEW:
      err = la_parse_new (this, vp);
      THROW_ERR_IF_ERR(err);

      while (TOKEN is TOKEN_DOT) {
        UNGET_BYTE();

        TOKENVAL = *vp;
        VALUE mapval = TOKENVAL;

        TOKEN = TOKEN_MAP;
        err = la_parse_primary (this, vp);
        THROW_ERR_IF_ERR(err);

        if (mapval.sym is NULL) {
          VALUE v = *vp;

          if (v.type isnot MAP_TYPE) {
            *vp = la_copy_value (v);
            this->objectState &= ~MAP_MEMBER;
          }

          la_release_val (this, mapval);
        }
      }

      return err;

    case TOKEN_BLOCK:
      err = la_parse_map (this, vp);
      THROW_ERR_IF_ERR(err);

      if (TOKEN is TOKEN_COLON) {
        vp->refcount = MAP_LITERAL;
        TOKENVAL = *vp;
        return la_parse_chain (this, vp);
      }

      while (TOKEN is TOKEN_DOT) {
        UNGET_BYTE();

        TOKENVAL = *vp;
        VALUE mapval = TOKENVAL;

        TOKEN = TOKEN_MAP;
        err = la_parse_primary (this, vp);
        THROW_ERR_IF_ERR(err);

        if (mapval.sym is NULL) {
          VALUE v = *vp;

          if (v.type isnot MAP_TYPE) {
            *vp = la_copy_value (v);
            this->objectState &= ~MAP_MEMBER;
          }

          la_release_val (this, mapval);
        }
      }

      return err;

    case TOKEN_LIST_NEW:
    case TOKEN_LIST_FUN:
      err = la_parse_list (this, vp, c is TOKEN_LIST_FUN);
      THROW_ERR_IF_ERR(err);

      if (TOKEN is TOKEN_COLON) {
        TOKENVAL = *vp;
        return la_parse_chain (this, vp);
      }

      return err;

    case TOKEN_ARYDEF:
      err = la_parse_array_def (this, vp, ARRAY_IS_REASSIGNMENT);
      THROW_ERR_IF_ERR(err);

      if (TOKEN is TOKEN_COLON) {
        TOKENVAL = *vp;
        vp->refcount = ARRAY_LITERAL;
        return la_parse_chain (this, vp);
      }

      return err;

    default:
      /* if ((c & 0xff) is TOKEN_BINOP) {
        OpFunc op = (OpFunc) AS_PTR(TOKENVAL);
        NEXT_TOKEN();
        VALUE v;
        err = la_parse_expr (this, &v);
        THROW_ERR_IF_ERR(err);

        *vp = op (this, INT(0), v);
        return err;
      }
      */

      if (c is TOKEN_NL) {
        NEXT_TOKEN();
        return la_parse_primary (this, vp);
      }

      if (c isnot TOKEN_EOF)
        return this->syntax_error (this, STR_FMT(
            "%s(), syntax error, unknown token |%c| |%d|", __func__, c, c));
  }

  return LA_OK;
}

static int la_parse_boolean_stmt (la_t *this, int tok) {
  int err;

  size_t len = GETSTRLEN(PARSEPTR);
  const char *Ptr = GETSTRPTR(PARSEPTR);
  const char *ptr = Ptr;

  while (*ptr and is_space (*ptr)) ptr++;

  if (len - (ptr - Ptr) < 5) return tok;

  if ('i' is *ptr and 'f' is *(++ptr)) {
    ptr++;
    int isspace = 0;
    int isif = -1;

    if ('(' is *ptr or (isspace = is_space (*ptr))) {
      isif = 1;
    } else if (len - (ptr - Ptr) > 5 and (
      'n' is *ptr and 'o' is *(++ptr) and 't' is *(++ptr))) {
      ptr++;
      if ('(' is *ptr or (isspace = is_space (*ptr)))
        isif = 0;
    }

    if (-1 is isif) return tok;

    if (isspace) {
      ptr++;
      while (*ptr and (is_space (*ptr) or *ptr is TOKEN_NL)) ptr++;
    }

    len -= ptr - Ptr;
    PARSEPTR = StringNew (ptr);
    SETSTRLEN(PARSEPTR, len);

    err = la_parse_cond (this, isif is 0);
    THROW_ERR_IF_ERR(err);

    if (AS_INT(CONDVAL))
      return tok;
    return LA_OK;
  }

  return tok;
}

static int la_handle_break (la_t *this) {
  ifnot (this->curState & LOOP_STATE)
    THROW_SYNTAX_ERR("break is not in a loop");

  int c;
  uint nth = 0;
  int level = 0;
  int num = PEEK_NTH_BYTE_NOWS_NONL(&nth);
  if ('1' <= num and num <= '9') {
    nth++;
    c = PEEK_NTH_BYTE_NOWS_INLINE(&nth);

    if (c isnot TOKEN_SEMICOLON and c isnot TOKEN_NL and
        c isnot TOKEN_BLOCK_CLOS and c isnot TOKEN_EOF and
        c isnot 'i' and c isnot 'e')
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

  int err = la_parse_boolean_stmt (this, LA_ERR_BREAK);
  THROW_ERR_IF_ERR(err);
  if (err is LA_ERR_BREAK) this->breakCount = level;
  return err;
}

static int la_parse_stmt (la_t *this) {
  int err = LA_OK;
  int c;
  int prev_token = 0;
  la_string name;
  funT *scope = this->curScope;
  VALUE val;

  do_token:
  c = TOKEN;

  switch (c) {
    case TOKEN_BREAK:
      return la_handle_break (this);

    case TOKEN_CONTINUE:
      ifnot (this->curState & LOOP_STATE)
        THROW_SYNTAX_ERR("continue is not in a loop");

      return la_parse_boolean_stmt (this, LA_ERR_CONTINUE);

    case TOKEN_VARDEF:
    case TOKEN_CONSTDEF: {
      if (this->curState & VAR_IS_NOT_ALLOWED)
        THROW_SYNTAX_ERR("var declaration is not allowed");

      prev_token = c;
      int is_const = c is TOKEN_CONSTDEF;

      NEXT_RAW_TOKEN();
      c = TOKEN;

      if (c is TOKEN_NL) {
        NEXT_TOKEN();
        c = TOKEN;
      }

      if (c isnot TOKEN_SYMBOL)
        THROW_SYNTAX_ERR("var definition: expected a symbol");

      name = TOKENSTR;

      ifnot (GETSTRLEN(name))
        THROW_SYNTAX_ERR("unknown symbol");

      char *key = sym_key (this, name);

      sym_t *sym = ns_lookup_symbol (this->std, key);
      ifnot (NULL is sym)
        THROW_SYNTAX_ERR("can not redefine a standard symbol");

      sym = ns_lookup_symbol (this->datatypes, key);

      ifnot (NULL is sym) {
        TOKENSYM = sym;
        VALUE v;
        int (*func) (la_t *, VALUE *, int) = AS_VOID_PTR(sym->value);
        err = (*func) (this, &v, 0);
        THROW_ERR_IF_ERR(err);

        if (TOKEN is TOKEN_COMMA) {
          uint n = 0;
          c = PEEK_NTH_BYTE_NOWS_INLINE(&n);
          if (c is TOKEN_NL) {
            for (uint i = 0; i <= n; i++)
              IGNORE_NEXT_BYTE;
          }

          TOKEN = prev_token;
          goto do_token;
        }

        return err;
      }

      scope = (this->scopeState is PUBLIC_SCOPE ? this->function : this->curScope);

      sym = ns_lookup_symbol (scope, key);

      ifnot (NULL is sym)
        THROW_SYNTAX_ERR("can not redeclare a symbol in this scope");

      VALUE v = NULL_VALUE;

      TOKENSYM = la_define_symbol (this, scope, key, v.type, v, is_const);

      if (NULL is TOKENSYM)
        THROW_SYNTAX_ERR("unknown error on declaration");

      int token = PEEK_NTH_TOKEN(0);
      if (token isnot TOKEN_ASSIGN) {
        if (token is TOKEN_COMMA) {
          NEXT_TOKEN();
          TOKEN = prev_token;
          goto do_token;
        }

        if (token is TOKEN_NL) {
          NEXT_TOKEN();
          return LA_OK;
        }
      }

      this->scopeState = 0;
      TOKEN = TOKEN_VAR;
      goto do_token;
    }

    case TOKEN_STRING:
      if (PEEK_NTH_BYTE(0) is TOKEN_INDEX_OPEN) {
        sym_t *symbol = TOKENSYM;
        return la_string_set_char (this, symbol->value, symbol->is_const);
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

      if (token < TOKEN_ASSIGN and
          token > TOKEN_ASSIGN_LAST_VAL)
        THROW_SYNTAX_ERR("expected assignment operator");

      if (token is TOKEN_PLUS_PLUS or token is TOKEN_MINUS_MINUS) {
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
              (token is TOKEN_PLUS_PLUS ? "++" : "--"));
        }

        switch (val.type) {
          case INTEGER_TYPE:
            symbol->value = INT(AS_INT(val) + (token is TOKEN_PLUS_PLUS ? 1 : -1));
            NEXT_TOKEN();
            return LA_OK;

          case NUMBER_TYPE:
            symbol->value = NUMBER(AS_NUMBER(val) + (token is TOKEN_PLUS_PLUS ? 1 : -1));
            NEXT_TOKEN();
            return LA_OK;

          default:
            THROW_SYNTAX_ERR_IF(val.type isnot INTEGER_TYPE or val.type isnot NUMBER_TYPE,
              "awaiting an integer or a number");
        }
      }

      if (symbol->is_const)
        if (symbol->value.type isnot NULL_TYPE)
          THROW_SYNTAX_ERR("can not reassign to a constant");

      NEXT_TOKEN();

      int is_un = 0;

      switch (TOKEN) {
        case TOKEN_FUNCDEF: {
          char *key = sym_key (this, name);
          la_release_sym (Vmap.pop (scope->symbols, key));
          Cstring.cp (this->curFunName, MAXLEN_SYMBOL + 1, key, GETSTRLEN(name));
          err = la_parse_func_def (this);
          this->curFunName[0] = '\0';
          return err;
        }

        case TOKEN_UNARY:
          is_un = 1;
          NEXT_TOKEN();
          break;
      }

      val.refcount = IS_UNCHANGEABLE;
      err = la_parse_expr (this, &val);
      THROW_ERR_IF_ERR(err);

      switch (val.refcount) {
        case IS_UNCHANGEABLE:
          return LA_OK;
        case STRING_LITERAL:
          val.refcount = 0;
      }

      if (this->objectState & (ARRAY_MEMBER|MAP_MEMBER)) {
        switch (val.type) {
          case ARRAY_TYPE:
          case STRING_TYPE: {
            VALUE v = val;
            val = la_copy_value (v);
          }
        }
        this->objectState &= ~(ARRAY_MEMBER|MAP_MEMBER);
      }

      if (is_un) {
        if (val.type is INTEGER_TYPE)
          AS_INT(val) = ~AS_INT(val);
        else
          THROW_SYNTAX_ERR("error while setting an unary object, awaiting an integer");
      }

      if (val.type & UFUNCTION_TYPE) {
        funT *f = AS_FUNC_PTR(val);
        symbol->type = (UFUNC_TYPE | (f->nargs << 8));
        Cstring.cp (f->funname, MAXLEN_SYMBOL, GETSTRPTR(name),
            GETSTRLEN(name));
        f->prev = scope->prev;
      } else
        symbol->type = val.type;

      if (token is TOKEN_ASSIGN) {
        switch (symbol->value.type) {
          case STRING_TYPE:
            //if (Cstring.eq (symbol->scope->funname, scope->funname))
              la_release_val (this, symbol->value);
            break;

          default:
            if (symbol->type & UFUNCTION_TYPE) {
              funT *f = AS_FUNC_PTR(symbol->value);
              fun_release (&f);
              break;
            }

            la_release_val (this, symbol->value);
        }

        if ((val.type is STRING_TYPE or val.type is ARRAY_TYPE or val.type is LIST_TYPE) and
             val.sym isnot NULL) {
          VALUE v = val;
          val = la_copy_value (v);
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
          result = la_add (this, symbol->value, val);
          this->objectState &= ~OBJECT_APPEND;
          break;

        case TOKEN_ASSIGN_SUB: result = la_sub  (this, symbol->value, val); break;
        case TOKEN_ASSIGN_DIV: result = la_div  (this, symbol->value, val); break;
        case TOKEN_ASSIGN_MUL: result = la_mul  (this, symbol->value, val); break;
        case TOKEN_ASSIGN_MOD: result = la_mod  (this, symbol->value, val); break;
        case TOKEN_ASSIGN_BAR: result = la_bset (this, symbol->value, val); break;
        case TOKEN_ASSIGN_AND: result = la_bnot (this, symbol->value, val); break;
        case TOKEN_ASSIGN_XOR: result = la_bitxor (this, symbol->value, val); break;
        default: THROW_SYNTAX_ERR("unknown operator");
      }

      if (result.type is NULL_TYPE)
        THROW_SYNTAX_ERR("unexpected operation");

      assign_and_return:
      symbol->value = result;

      this->curState &= ~LITERAL_STRING_STATE;

      if (prev_token isnot TOKEN_VARDEF)
        return LA_OK;

      if (TOKEN is TOKEN_COMMA) {
        uint n = 0;
        c = PEEK_NTH_BYTE_NOWS_INLINE(&n);
        if (c is TOKEN_NL) {
          for (uint i = 0; i <= n; i++)
            IGNORE_NEXT_BYTE;
        }

        TOKEN = TOKEN_VARDEF;
        goto do_token;
      }

      return LA_OK;
    }

    case TOKEN_LIST:
      err = la_parse_list_set (this);
      THROW_ERR_IF_ERR(err);

      if (err is LA_MMT_REASSIGN) {
        TOKEN = TOKEN_VAR;
        goto do_token;
      }

      return err;

    case TOKEN_ARRAY:
      err = la_parse_array_set (this);
      THROW_ERR_IF_ERR(err);

      if (err is LA_MMT_REASSIGN) {
        TOKEN = TOKEN_VAR;
        goto do_token;
      }

      return err;

    case TOKEN_MAP:
      err = la_parse_map_set (this);
      THROW_ERR_IF_ERR(err);

      if (err is LA_MMT_REASSIGN) {
        TOKEN = TOKEN_VAR;
        goto do_token;
      }

      return err;

    case TOKEN_OVERRIDE:
      NEXT_TOKEN();
      c = TOKEN;

      if (c isnot TOKEN_MAP)
        THROW_SYNTAX_ERR("override works only for maps");
      this->objectState |= FUNC_OVERRIDE;
      TOKEN = TOKEN_MAP;
      goto do_token;

    case TOKEN_BUILTIN:
    case UFUNC_TYPE:
      err = la_parse_primary (this, &val);
      THROW_ERR_IF_ERR(err);

      if (val.sym is NULL) // plain function call without reason
        la_release_val (this, val);

      return err;

    case TOKEN_APPEND:
      err = la_parse_append (this, &val);
      THROW_ERR_IF_ERR(err);
      return err;

    case TOKEN_MINUS_MINUS:
    case TOKEN_PLUS_PLUS: {
      VALUE v;
      return la_parse_prefix (this, &v, c);
    }

    case TOKEN_D_DEFINE:
    case TOKEN_D_UNDEF: {
      VALUE v;
      return la_parse_directive (this, &v, c);
    }

    case TOKEN_COMMA:
      NEXT_TOKEN();
      return la_parse_stmt (this);

    default:
      if (TOKENSYM and AS_INT(TOKENVAL)) {
        int (*func) (la_t *) = AS_VOID_PTR(TOKENVAL);
        err = (*func) (this);
        if (err is TOKEN_PUBLIC)
          goto do_token;
        return err;
      }

      return this->syntax_error (this, STR_FMT("%s(), unknown token |%c| |%d|", __func__, c, c));
  }

  return err;
}

static int la_consume_binop (la_t *this) {
  int is_index_open = 0;
  int is_paren_open = 0;
  int is_if = 0;
  int is_binop = 0;

  NEXT_TOKEN();

next:
  is_binop = (TOKEN & 0xff) is TOKEN_BINOP;
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
      return LA_NOTOK;
    default:
      goto next;
  }

  return LA_OK;
}

static int la_parse_expr_level (la_t *this, int max_level, VALUE *vp) {
  int err;
  int c = TOKEN;

  ifnot ((c & 0xff) is TOKEN_BINOP)
    return LA_OK;

  err = LA_OK;
  VALUE lhs = *vp;
  VALUE rhs;

  string *x = NULL;
  if (lhs.type is STRING_TYPE and lhs.sym is NULL) {
    if (this->curState & LITERAL_STRING_STATE) {
      if (lhs.refcount isnot MALLOCED_STRING)
        x = AS_STRING(lhs);
    } else if (lhs.refcount is STRING_LITERAL) {
      x = AS_STRING(lhs);
    } else {
      if (0 is (this->objectState & (ARRAY_MEMBER|MAP_MEMBER)) and
          lhs.refcount isnot MALLOCED_STRING and
          0 is lhs.refcount) {
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

    this->curState |= MALLOCED_STRING_STATE;
    NEXT_TOKEN();
    c = TOKEN;

    err = la_parse_primary (this, &rhs);
    THROW_ERR_IF_ERR(err);

    c = TOKEN;

    while ((c & 0xff) is TOKEN_BINOP) {
      int nextlevel = (c >> 8) & 0xff;
      if (level <= nextlevel) break;

      this->objectState &= ~(LHS_STRING_RELEASED|RHS_STRING_RELEASED);
      err = la_parse_expr_level (this, nextlevel, &rhs);
      THROW_ERR_IF_ERR(err);

      c = TOKEN;
    }

    this->CFuncError = LA_OK;
    this->curMsg[0] = '\0';
    VALUE sv_lhs = lhs;

    if (num_iter++) this->objectState |= OBJECT_APPEND;
    this->objectState &= ~(LHS_STRING_RELEASED|RHS_STRING_RELEASED);

    const char *ptr = GETSTRPTR(TOKENSTR);
    lhs = op (this, lhs, rhs);

    if (op is la_equals or op is la_ne) {
      ifnot (AS_INT(lhs)) {
        if (Cstring.eq_n (ptr, "and", 3) or Cstring.eq_n (ptr, "&&", 2)) {
          err = la_consume_binop (this);
          THROW_SYNTAX_ERR_IF(err is LA_NOTOK, "awaiting the end of a binary operation");
          lhs = FALSE_VALUE;
          goto theend;
        }
      } else {
        if (Cstring.eq_n (ptr, "or", 2) or Cstring.eq_n (ptr, "||", 2)) {
          err = la_consume_binop (this);
          THROW_SYNTAX_ERR_IF(err is LA_NOTOK, "awaiting the end of a binary operation");
          lhs = TRUE_VALUE;
          goto theend;
        }
      }
    }

    ifnot (num_iter - 1)
      lhs_released = this->objectState & LHS_STRING_RELEASED;

    if (this->CFuncError isnot LA_OK) {
      if (this->curMsg[0] isnot '\0')
        this-> print_fmt_bytes (this->err_fp, "binary operation: %s\n", this->curMsg);
      return this->CFuncError;
    }

    if (sv_lhs.type isnot STRING_TYPE) {
      if (sv_lhs.sym is NULL)
        la_release_val (this, sv_lhs);
    }

    if (rhs.sym is NULL) {
      if (rhs.type isnot STRING_TYPE) {
        la_release_val (this, rhs);
      } else {
        ifnot (this->objectState & RHS_STRING_RELEASED) {
          if (rhs.refcount is STRING_LITERAL) {
            string_release (rhs);
          } else if (rhs.refcount isnot MALLOCED_STRING and
              0 is (this->objectState & (ARRAY_MEMBER|MAP_MEMBER)))
            string_release (rhs);
         }
       }
    }

    this->curState &= ~LITERAL_STRING_STATE;
  } while ((c & 0xff) is TOKEN_BINOP);

theend:
  this->curState &= ~(MALLOCED_STRING_STATE|LITERAL_STRING_STATE);
  this->objectState &= ~(ARRAY_MEMBER|MAP_MEMBER|OBJECT_APPEND|LHS_STRING_RELEASED|RHS_STRING_RELEASED);

  *vp = lhs;

  ifnot (lhs_released)
    String.release (x);

  return err;
}

static int la_parse_expr (la_t *this, VALUE *vp) {
  int err = la_parse_primary (this, vp);
  THROW_ERR_IF_ERR(err);

  err = la_parse_expr_level (this, MAX_EXPR_LEVEL, vp);
  return err;
}

static int la_parse_block (la_t *this, const char *descr) {
  if (TOKEN is TOKEN_BLOCK)
    return LA_OK;

  if (TOKEN is TOKEN_EOF)
    THROW_SYNTAX_ERR(STR_FMT("unended %s block", descr));

  if (TOKEN is TOKEN_NL) NEXT_TOKEN();

  THROW_SYNTAX_ERR_IF(TOKEN is TOKEN_NL,
    STR_FMT("%s: expecting a block, found two consecutive new lines\n", descr));

  int parens = 0;
  int in_str = 0;
  int c = 0;
  int prev;

  while (1) {
    prev = c;
    c = GET_BYTE();

    switch (c) {
      case TOKEN_PAREN_OPEN:
        ifnot (in_str) parens++;
         continue;

      case TOKEN_PAREN_CLOS:
        ifnot (in_str) parens--;
         continue;

      case '"':
        if (prev isnot TOKEN_ESCAPE_CHR) {
          if (in_str)
            in_str--;
          else
            in_str++;
        }

        continue;

      case TOKEN_SEMICOLON:
        if (parens or in_str)
          continue;
        break;

      case TOKEN_NL:
        UNGET_BYTE();

      case TOKEN_EOF:
        break;

      default:
        continue;
    }

    break;
  }

  return LA_OK;
}

static int la_consume_ifelse (la_t *this) {
  int err;
  int c;

  while (1) {
    NEXT_TOKEN();
    c = TOKEN;

    if (c is TOKEN_EOF)
      THROW_SYNTAX_ERR("unended conditional block");

    while ((c = GET_BYTE())) {
      if (c is TOKEN_EOF)
        THROW_SYNTAX_ERR("unended conditional block");

      if (c is TOKEN_BLOCK_OPEN) {
        err = la_get_opened_block (this, "unended conditional block");
        THROW_ERR_IF_ERR(err);

        NEXT_TOKEN();
        c = TOKEN;

        if (c isnot TOKEN_ELSE)
          return LA_OK;
      }
    }
  }

  return LA_OK;
}

static int la_consume_iforelse (la_t *this, int break_at_orelse) {
  int levels = 1;
  this->curState |= CONSUME_STATE;

  int paren_open = 0;
  int index_open = 0;

  while (1) {
    NEXT_TOKEN();

    check:

    switch (TOKEN) {
      case TOKEN_COLON:
        ifnot (this->curState & INDEX_STATE)
          break;

      // fall through
      case TOKEN_SEMICOLON:
      case TOKEN_NL:
        if (paren_open) break;

      // fall through
      case TOKEN_EOF:
        if (this->funcState & CHAIN_STATE)
          break;
        goto theend;

      case TOKEN_END:
        NEXT_TOKEN();
        levels--;
        goto theend;

      case TOKEN_ORELSE:
        if (break_at_orelse)
          if (levels is 1) goto theend;
        break;

      case TOKEN_THEN:
        levels++;
        NEXT_TOKEN();
        while (TOKEN is TOKEN_NL) NEXT_TOKEN();
        goto check;

      case TOKEN_PAREN_OPEN:
        paren_open++;
        break;

      case TOKEN_PAREN_CLOS:
        ifnot (paren_open) goto theend;
        paren_open--;
        break;

      case TOKEN_COMMA:
        ifnot (paren_open) goto theend;
        break;

      case TOKEN_INDEX_OPEN:
        index_open++;
        break;

      case TOKEN_INDEX_CLOS:
        ifnot (index_open) goto theend;
        index_open--;
        break;
    }
  }

theend:
  this->curState &= ~CONSUME_STATE;
  return levels;
}

static int la_parse_iforelse (la_t *this, int cond, VALUE *vp) {
  int err;
  this->conditionState = 0;
  tokenState save;
  int isorelse = TOKEN is TOKEN_ORELSE;

  this->curState |= MALLOCED_STRING_STATE;
  NEXT_TOKEN();
  while (TOKEN is TOKEN_NL) NEXT_TOKEN();
  this->curState &= ~MALLOCED_STRING_STATE;

  THROW_SYNTAX_ERR_IF(TOKEN is TOKEN_EOF, "unended if/orelse");

  ifnot (cond) {
    if (TOKEN is TOKEN_ORELSE)
      return la_parse_iforelse (this, 1, vp);

    err = la_consume_iforelse (this, 1);
    THROW_ERR_IF_ERR(err);

    if (TOKEN is TOKEN_ORELSE)
      return la_parse_iforelse (this, 1, vp);

    save = SAVE_TOKENSTATE();

    this->curState |= CONSUME_STATE;

    NEXT_TOKEN();

    if (TOKEN is TOKEN_ORELSE) {
      this->curState &= ~CONSUME_STATE;
      if (err > 1)
        goto consume;
      else
        return la_parse_iforelse (this, 1, vp);
    }

    this->curState &= ~CONSUME_STATE;

    RESTORE_TOKENSTATE(save);
    return LA_OK;
  }

  ifnot (NULL is vp) {
    err = la_parse_expr (this, vp);

  } else {
    this->curState |= VAR_IS_NOT_ALLOWED;
    err = la_parse_stmt (this);
    if (err is LA_ERR_BREAK or err is LA_ERR_CONTINUE)
      if (TOKEN is TOKEN_BREAK or TOKEN is TOKEN_CONTINUE)
        NEXT_TOKEN();

    this->curState &= ~VAR_IS_NOT_ALLOWED;
  }

  this->conditionState = 1;
  THROW_ERR_IF_ERR(err);

  if (isorelse or TOKEN is TOKEN_END) {
    if (TOKEN is TOKEN_END) NEXT_TOKEN();
    goto theend;
  }

  if (TOKEN isnot TOKEN_NL and
      TOKEN isnot TOKEN_SEMICOLON and
      TOKEN isnot TOKEN_EOF) {
    if (TOKEN is TOKEN_ORELSE) {
      goto consume;

    } else if (
        TOKEN is TOKEN_PAREN_CLOS or
        TOKEN is TOKEN_INDEX_CLOS or
        TOKEN is TOKEN_COMMA or
       (TOKEN is TOKEN_COLON and this->curState is INDEX_STATE)) {
      goto theend;

    } else if (TOKEN is TOKEN_END) {
      NEXT_TOKEN();
      goto theend;
    } else {
      THROW_SYNTAX_ERR("awaiting a new line or ;");
    }

  } else {
    int p0 = PEEK_NTH_TOKEN(0);
    if (p0 is TOKEN_ORELSE) {
      NEXT_TOKEN();
      goto consume;
    }

    if (p0 is TOKEN_NL)
      if (PEEK_NTH_TOKEN(1) is TOKEN_ORELSE)
        goto consume;
  }

  save = SAVE_TOKENSTATE();

  consume_orelse:
  this->curState |= CONSUME_STATE;
  int nc = PEEK_NTH_TOKEN(0);
  if (nc is TOKEN_PAREN_CLOS or nc is TOKEN_INDEX_CLOS) {
    RESTORE_TOKENSTATE(save);
    goto theend;
  }

  NEXT_TOKEN();

  if (TOKEN is TOKEN_ORELSE) {

    consume:
    this->curState |= CONSUME_STATE;

    NEXT_TOKEN();

    while (TOKEN is TOKEN_NL) NEXT_TOKEN();

    int lerr = la_consume_iforelse (this, 0);
    THROW_ERR_IF_ERR(lerr);

    save = SAVE_TOKENSTATE();

    goto consume_orelse;
  } else {
    RESTORE_TOKENSTATE(save);
  }

theend:
  this->curState &= ~CONSUME_STATE;
  return err;
}

static int la_parse_cond (la_t *this, int nottrue) {
  int err;

  NEXT_TOKEN();
  THROW_SYNTAX_ERR_IF(TOKEN is TOKEN_EOF, "unended conditional expression");

  this->curState |= MALLOCED_STRING_STATE;
  err = la_parse_expr (this, &CONDVAL);
  this->curState &= ~MALLOCED_STRING_STATE;
  THROW_ERR_IF_ERR(err);

  if (IS_INT(CONDVAL))
    CONDVAL = INT((AS_INT(CONDVAL) ? 0 is nottrue : nottrue));
  else if (IS_NULL(CONDVAL))
    CONDVAL = INT((nottrue ? 0 : 1));
  else if (IS_NUMBER(CONDVAL))
    CONDVAL = INT((AS_NUMBER(CONDVAL) is 0.0 ? nottrue: 0 is nottrue));

  return LA_OK;
}

static int la_parse_if (la_t *this) {
  int err = la_parse_cond (this, TOKEN is TOKEN_IFNOT);
  THROW_ERR_IF_ERR(err);

  int is_true = AS_INT(CONDVAL);

  if (TOKEN is TOKEN_THEN)
    return la_parse_iforelse (this, is_true, NULL);

  err = la_parse_block (this, "if");
  THROW_ERR_IF_ERR(err);

  la_string ifpart = TOKENSTR;
  la_string elsepart;

  NEXT_TOKEN();

  int haveelse = 0;
  int haveelif = 0;

  la_string savepc;

  if (TOKEN is TOKEN_ELSE) {
    savepc = PARSEPTR;

    NEXT_TOKEN();

    if (TOKEN isnot TOKEN_IF and TOKEN isnot TOKEN_IFNOT) {
      if (TOKEN isnot TOKEN_BLOCK)
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
    err = la_parse_string (this, ifpart);
    if (haveelif) {
      PARSEPTR = savepc;
      int e = la_consume_ifelse (this);
      if (e isnot LA_OK) err = e;
    }

  } else if (haveelse) {
    err = la_parse_string (this, elsepart);

  } else if (haveelif) {
    TOKEN = haveelif;
    err = la_parse_if (this);
  }

  if (HASTORETURN) RESET_PARSEPTR;

  this->curScope = save_scope;
  fun_release (&fun);
  return err;
}

static int la_parse_if_in_chain (la_t *this, VALUE *vp) {
  int err;
  int token = TOKEN;

  la_string savepc = PARSEPTR;

  NEXT_RAW_TOKEN();

  if (TOKEN isnot TOKEN_SYMBOL)
    THROW_SYNTAX_ERR("expected a symbol");

  char *key = sym_key (this, TOKENSTR);
  sym_t *sym = ns_lookup_symbol (this->std, key);
  ifnot (NULL is sym)
    THROW_SYNTAX_ERR("can not redefine a standard symbol");

  funT *fun = Fun_new (this, funNew (
    .name = NS_CHAIN_BLOCK, .namelen = NS_CHAIN_BLOCK_LEN, .parent = this->curScope
  ));

  funT *save_scope = this->curScope;
  this->curScope = fun;

  VALUE stackval = la_copy_value ((*vp));
  if (stackval.type is STRING_TYPE)
    stackval.refcount = STRING_LITERAL;

  sym = la_define_symbol (this, fun, key, stackval.type, stackval, 0);

  PARSEPTR = savepc;

  err = la_parse_cond (this, token is TOKEN_IFNOT);
  THROW_ERR_IF_ERR(err);

  err = la_parse_iforelse (this, AS_INT(CONDVAL), vp);
  THROW_ERR_IF_ERR(err);

  if (this->conditionState is 0 or
      AS_VOID_PTR((*vp)) is AS_VOID_PTR(stackval)) {
    sym->value.refcount = -1;
    stackval.sym = NULL;
    *vp = stackval;
    if (vp->refcount is ARRAY_LITERAL)
      vp->refcount++;

  } else {
    if (vp->refcount is MALLOCED_STRING) {
      VALUE v = STRING(String.dup(AS_STRING((*vp))));
      *vp = v;
    }
  }

  this->curScope = save_scope;

  fun_release (&fun);

  return err;
}

static int la_parse_while (la_t *this) {
  int err;
  this->loopCount++;

  la_string exprpptr = PARSEPTR;

  NEXT_TOKEN();
  VALUE v;
  err = la_parse_expr (this, &v);
  THROW_ERR_IF_ERR(err);

  funT *save_scope = this->curScope;

  funT *fun = Fun_new (this, funNew (
    .name = NS_LOOP_BLOCK, .namelen = NS_LOOP_BLOCK_LEN, .parent = this->curScope
  ));

  fun->block_symbols = Vmap.new (8);

  this->curScope = fun;

  err = la_parse_block (this, "while");
  THROW_ERR_IF_ERR(err);

  int is_single = TOKEN isnot TOKEN_BLOCK;

  const char *tmp_ptr = GETSTRPTR(TOKENSTR);

  integer bodylen = GETSTRLEN(TOKENSTR);
  char body[bodylen + 1];
  for (integer i = 0; i < bodylen; i++)
    body[i] = tmp_ptr[i];
  body[bodylen] = '\0';

  la_string body_str = StringNewLen (body, bodylen);

  la_string savepc = PARSEPTR;

  int count = 0;

  for (;;) {
    if (count++) {
      PARSEPTR = exprpptr;
      NEXT_TOKEN();
      err = la_parse_expr (this, &v);
      THROW_ERR_IF_ERR(err);
    }

    ifnot (AS_INT(v)) goto theend;

    this->curState |= LOOP_STATE;
    if (is_single) {
      PARSEPTR = body_str;
      NEXT_TOKEN();
      err = la_parse_stmt (this);
      if (TOKEN > TOKEN_EOF and TOKEN isnot TOKEN_SEMICOLON) {
        la_print_current_line (this,
          "[WARNING]: extra tokens detected after the end of a while statement\n",
          NULL);
      }

    } else {
      err = la_parse_string (this, body_str);
    }

    this->curState &= ~LOOP_STATE;
    THROW_ERR_IF_ERR(err);

    if (err is LA_ERR_BREAK) goto theend;

    if (HASTORETURN) {
      this->curState &= ~LOOP_STATE;
      RESET_PARSEPTR;
      Vmap.release (fun->block_symbols);
      fun_release (&fun);
      return LA_OK;
    }

    if (err is LA_ERR_CONTINUE) {
      la_fun_release_symbols (fun, 1, 0);
      continue;
    }

    la_fun_release_symbols (fun, 1, 0);

    THROW_ERR_IF_ERR(err);
  }

theend:
  this->curScope = save_scope;
  Vmap.release (fun->block_symbols);
  fun_release (&fun);

  this->loopCount--;
  ifnot (this->loopCount)
    this->curState &= ~LOOP_STATE;
  else
    this->curState |= LOOP_STATE;

  PARSEPTR = savepc;

  TOKEN = TOKEN_SEMICOLON;

  if (err is LA_ERR_BREAK and this->breakCount) {
    this->breakCount--;
    return LA_ERR_BREAK;
  }

  return LA_OK;
}

static int la_parse_do (la_t *this) {
  int err;
  this->loopCount++;

  NEXT_TOKEN();
  err = la_parse_block (this, "do/while");
  THROW_ERR_IF_ERR(err);

  integer bodylen = GETSTRLEN(TOKENSTR);
  const char *tmp_ptr = GETSTRPTR(TOKENSTR);

  int is_single = TOKEN isnot TOKEN_BLOCK;

  if (is_single) {
    int idx = 0;
    const char *ptr = tmp_ptr;
    while (idx++ < bodylen - 4) {
      if (*ptr is 'w' and Cstring.eq_n (ptr, "while", 5)) {
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

  la_string body_str = StringNewLen (body, bodylen);

  NEXT_TOKEN();
  int c = TOKEN;

  if (c isnot TOKEN_WHILE)
    THROW_SYNTAX_ERR("error while parsing do/while loop, awaiting while");

  const char *orig_ptr = GETSTRPTR(PARSEPTR);
  tmp_ptr = orig_ptr;

  integer orig_len = GETSTRLEN(PARSEPTR);

  int is_paren_open = PEEK_NTH_TOKEN(0) is TOKEN_PAREN_OPEN;
  int has_a_nl = 0;
  if (is_paren_open) {
    while (*tmp_ptr isnot TOKEN_PAREN_OPEN) tmp_ptr++;
    tmp_ptr++;
  } else {
    while (*tmp_ptr is ' ') tmp_ptr++;
    if (*tmp_ptr is TOKEN_NL) {
      tmp_ptr++;
      has_a_nl = 1;
    }
  }

  const char *ptr = tmp_ptr;

  int parenopen = is_paren_open;
  int in_str = 0;
  int prevc = *ptr;

  THROW_SYNTAX_ERR_IF(has_a_nl and *ptr is TOKEN_NL, "error while parsing do/while condition: found two consecutive new lines");

  while (*ptr) {
    if (*ptr is TOKEN_NL) {
      if (is_paren_open or in_str)
        goto next;

      break;
    }

    if (*ptr is TOKEN_SEMICOLON) {
      if (is_paren_open or in_str)
        goto next;

      break;
    }

    if (*ptr is TOKEN_PAREN_OPEN and 1 is in_str + is_paren_open) {
      parenopen++;
      goto next;
    }

    if (*ptr is TOKEN_PAREN_CLOS and 1 is in_str + is_paren_open) {
      parenopen--;
      ifnot (parenopen)
        break;

      goto next;
    }

    if ((*ptr is TOKEN_DQUOTE or *ptr is TOKEN_BQUOTE) and prevc isnot TOKEN_ESCAPE_CHR) {
      if (in_str)
        in_str--;
      else
        in_str++;
    }

    next: prevc = *ptr; ptr++;
  }

  ifnot (*ptr)
    THROW_SYNTAX_ERR("unended do/while loop");

  TOKEN = TOKEN_PAREN_OPEN;

  integer cond_len = ptr - tmp_ptr;

  char cond[cond_len + 3];
  cond[0] = TOKEN_PAREN_OPEN;
  for (integer i = 0; i < cond_len; i++)
    cond[i + 1] = tmp_ptr[i];
  cond[cond_len + 1] = TOKEN_PAREN_CLOS;
  cond[cond_len + 2] = '\0';

  la_string cond_str = StringNewLen (cond, cond_len + 2);

  funT *save_scope = this->curScope;

  funT *fun = Fun_new (this, funNew (
    .name = NS_LOOP_BLOCK, .namelen = NS_LOOP_BLOCK_LEN, .parent = this->curScope
  ));

  fun->block_symbols = Vmap.new (8);

  this->curScope = fun;

  SETSTRPTR(PARSEPTR, ptr + 1);
  SETSTRLEN(PARSEPTR, orig_len - ((ptr + 1) - orig_ptr));

  la_string savepc = PARSEPTR;

  VALUE v;
  for (;;) {
    this->curState |= LOOP_STATE;
    if (is_single) {
      PARSEPTR = body_str;
      NEXT_TOKEN();
      err = la_parse_stmt (this);
      if (TOKEN > TOKEN_EOF and TOKEN isnot TOKEN_SEMICOLON) {
        la_print_current_line (this,
          "[WARNING]: extra tokens detected after the end of a do/while statement\n",
          NULL);
      }

    } else {
      err = la_parse_string (this, body_str);
    }

    this->curState &= ~LOOP_STATE;
    THROW_ERR_IF_ERR(err);

    if (err is LA_ERR_BREAK) goto theend;

    if (HASTORETURN) {
      this->curState &= ~LOOP_STATE;
      RESET_PARSEPTR;
      Vmap.release (fun->block_symbols);
      fun_release (&fun);
      return LA_OK;
    }

    if (err is LA_ERR_CONTINUE) {
      la_fun_release_symbols (fun, 1, 0);
      continue;
    }

    THROW_ERR_IF_ERR(err);

    PARSEPTR = cond_str;

    TOKEN = TOKEN_PAREN_OPEN;
    this->curState |= BLOCK_STATE;
    err = la_parse_expr (this, &v);
    this->curState &= ~BLOCK_STATE;
    THROW_ERR_IF_ERR(err);

    ifnot (AS_INT(v)) goto theend;
    la_fun_release_symbols (fun, 1, 0);
  }

theend:
  this->curScope = save_scope;
  Vmap.release (fun->block_symbols);
  fun_release (&fun);

  this->loopCount--;
  ifnot (this->loopCount)
    this->curState &= ~LOOP_STATE;
  else
    this->curState |= LOOP_STATE;

  PARSEPTR = savepc;

  TOKEN = TOKEN_SEMICOLON;

  if (err is LA_ERR_BREAK and this->breakCount) {
    this->breakCount--;
    return LA_ERR_BREAK;
  }

  return LA_OK;
}

static int la_parse_init_foreach (la_t *this, char *aident, char *bident, char *cident) {
  int num = 0;
  int len = GETSTRLEN(TOKENSTR);
  SETSTRLEN(PARSEPTR, GETSTRLEN(PARSEPTR) + len);
  SETSTRPTR(PARSEPTR, GETSTRPTR(PARSEPTR) - len);

  NEXT_RAW_TOKEN();
  THROW_SYNTAX_ERR_IF(TOKEN isnot TOKEN_SYMBOL, "parsing for each: awaiting identifier");

  num++;
  len = GETSTRLEN(TOKENSTR);
  Cstring.cp (aident, len + 1, GETSTRPTR(TOKENSTR), len);

  NEXT_RAW_TOKEN();
  if (TOKEN is TOKEN_COMMA)
    NEXT_RAW_TOKEN();
  else {
    THROW_SYNTAX_ERR_IF(GETSTRLEN(TOKENSTR) isnot 2 or (TOKENSTRBYTES[0] isnot 'i' and TOKENSTRBYTES[1] isnot 'n'),
      "parsing for each: awaiting in");
    return num;
  }

  THROW_SYNTAX_ERR_IF(TOKEN isnot TOKEN_SYMBOL, "parsing for each: awaiting identifier");

  num++;
  len = GETSTRLEN(TOKENSTR);
  Cstring.cp (bident, len + 1, GETSTRPTR(TOKENSTR), len);

  NEXT_RAW_TOKEN();

  if (TOKEN is TOKEN_COMMA)
    NEXT_RAW_TOKEN();
  else {
    THROW_SYNTAX_ERR_IF(GETSTRLEN(TOKENSTR) isnot 2 or (TOKENSTRBYTES[0] isnot 'i' and TOKENSTRBYTES[1] isnot 'n'),
      "parsing for each: awaiting in");
    return num;
  }

  THROW_SYNTAX_ERR_IF(TOKEN isnot TOKEN_SYMBOL, "parsing for each: awaiting identifier");

  num++;
  len = GETSTRLEN(TOKENSTR);
  Cstring.cp (cident, len + 1, GETSTRPTR(TOKENSTR), len);

  NEXT_RAW_TOKEN();
  THROW_SYNTAX_ERR_IF(GETSTRLEN(TOKENSTR) isnot 2 or (TOKENSTRBYTES[0] isnot 'i' and TOKENSTRBYTES[1] isnot 'n'),
    "parsing for each: awaiting in");

  return num;
}

static int la_parse_foreach (la_t *this) {
  int err = LA_OK;

  char aident[MAXLEN_SYMBOL + 1];
  char bident[MAXLEN_SYMBOL + 1];
  char cident[MAXLEN_SYMBOL + 1];

  int num = la_parse_init_foreach (this, aident, bident, cident);
  THROW_ERR_IF_ERR(num);

  VALUE v;
  NEXT_TOKEN();

  la_string sym_str = TOKENSTR;
  int symlen = GETSTRLEN (TOKENSTR);

  err = la_parse_primary (this, &v);
  THROW_ERR_IF_ERR(err);

  int type = v.type;

  switch (type) {
    case MAP_TYPE:
    case ARRAY_TYPE:
    case STRING_TYPE:
    case LIST_TYPE:
      break;

    default:
      THROW_A_TYPE_MISMATCH(type, "illegal foreach type");
  }

  err = la_parse_block (this, "for each");
  THROW_ERR_IF_ERR(err);

  int len = GETSTRLEN(TOKENSTR);
  char body[len+1];
  Cstring.cp (body, len + 1, GETSTRPTR(TOKENSTR), len);

  this->loopCount++;

  funT *fun = Fun_new (this, funNew (
    .name = NS_LOOP_BLOCK, .namelen = NS_LOOP_BLOCK_LEN, .parent = this->curScope
  ));

  fun->block_symbols = Vmap.new (8);

  la_string savepc = PARSEPTR;
  funT *save_scope = this->curScope;
  this->curScope = fun;

  if (type is MAP_TYPE) {
    THROW_SYNTAX_ERR_IF(num isnot 2, "error parsing for each for maps: awaiting two identifiers");

    Vmap_t *map = AS_MAP(v);

    int is_this = Cstring.eq_n (GETSTRPTR(sym_str), "this", 4) and symlen is 4;

    char *key = aident;
    char *val = bident;

    v = STRING_NEW_WITH_LEN("", 0);
    sym_t *key_sym = la_define_block_symbol (this, fun, key, STRING_TYPE, v, 0);
    v = INT(0);
    sym_t *val_sym = la_define_block_symbol (this, fun, val, INTEGER_TYPE, v, 0);

    num = Vmap.num_keys (map);
    string **keys = Vmap.keys (map);

    la_string body_str = StringNew (body);

    for (int i = 0; i < num; i++) {
      VALUE *value = (VALUE *) Vmap.get (map, keys[i]->bytes);
      if (value->sym->scope is NULL and 0 is is_this)
        continue;

      string *v_k = AS_STRING(key_sym->value);
      String.replace_with_len (v_k, keys[i]->bytes, keys[i]->num_bytes);
      val_sym->value = *value;
      val_sym->type = value->type;

      this->curState |= LOOP_STATE;
      err = la_parse_string (this, body_str);
      this->curState &= ~LOOP_STATE;

      val_sym->value = NULL_VALUE;

      if (err is LA_ERR_BREAK) {
        for (int j = 0; j < num; j++)
          String.release (keys[j]);
        free (keys);
        goto theend;
      }

      if (HASTORETURN) {
        this->curState &= ~LOOP_STATE;
        RESET_PARSEPTR;
        Vmap.release (fun->block_symbols);
        fun_release (&fun);
        for (int j = 0; j < num; j++)
          String.release (keys[j]);
        free (keys);
        return LA_OK;
      }

      if (err is LA_ERR_CONTINUE) {
        la_fun_release_symbols (fun, 1, 0);
        continue;
      }

      THROW_ERR_IF_ERR(err);

      la_fun_release_symbols (fun, 1, 0);
    }

    for (int j = 0; j < num; j++)
      String.release (keys[j]);
    free (keys);

    goto theend;
  }

  if (type is ARRAY_TYPE) {
    THROW_SYNTAX_ERR_IF(num isnot 2 and num isnot 1, "error parsing for each for arrays: awaiting one or two identifiers");

    ArrayType *array = (ArrayType *) AS_ARRAY(v);

    char *index = NULL;
    char *elem = NULL;

    if (num is 2) {
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
    Vmap_t **m_ar = NULL;
    ArrayType **a_ar = NULL;

    v = INT(-1);
    sym_t *index_sym = la_define_block_symbol (this, fun, index, INTEGER_TYPE, v, 0);

    sym_t *elem_sym = NULL;

    switch (array->type) {
      case INTEGER_TYPE:
        i_ar = (integer *) AS_ARRAY(array->value);
        v = INT(0);
        elem_sym = la_define_block_symbol (this, fun, elem, INTEGER_TYPE, v, 0);
        break;

      case NUMBER_TYPE:
        n_ar = (number *) AS_ARRAY(array->value);
        v = NUMBER(0);
        elem_sym = la_define_block_symbol (this, fun, elem, NUMBER_TYPE, v, 0);
        break;

      case STRING_TYPE:
        s_ar = (string **) AS_ARRAY(array->value);
        v = NULL_VALUE;
        elem_sym = la_define_block_symbol (this, fun, elem, STRING_TYPE, v, 0);
        break;

      case MAP_TYPE:
        m_ar = (Vmap_t **) AS_ARRAY(array->value);
        v = NULL_VALUE;
        elem_sym = la_define_block_symbol (this, fun, elem, MAP_TYPE, v, 0);
        break;

      case ARRAY_TYPE:
        a_ar = (ArrayType **) AS_ARRAY(array->value);
        v = NULL_VALUE;
        elem_sym = la_define_block_symbol (this, fun, elem, ARRAY_TYPE, v, 0);
        break;
    }

    num = array->len;

    la_string body_str = StringNew (body);

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
          if (NULL is s_ar[v_idx])
            elem_sym->value = NULL_VALUE;
          else
            elem_sym->value = STRING(s_ar[v_idx]);
          elem_sym->value.refcount++;
          break;

        case MAP_TYPE:
          if (NULL is m_ar[v_idx])
            elem_sym->value = NULL_VALUE;
          else
            elem_sym->value = MAP(m_ar[v_idx]);
          elem_sym->value.refcount++;
          break;

        case ARRAY_TYPE:
          if (NULL is a_ar[v_idx])
            elem_sym->value = NULL_VALUE;
          else
            elem_sym->value = ARRAY(a_ar[v_idx]);
          elem_sym->value.refcount++;
          break;
      }

      this->curState |= LOOP_STATE;
      err = la_parse_string (this, body_str);
      this->curState &= ~LOOP_STATE;
      THROW_ERR_IF_ERR(err);

      elem_sym->value = NULL_VALUE;

      if (err is LA_ERR_BREAK) goto theend;

      if (HASTORETURN) {
        this->curState &= ~LOOP_STATE;
        RESET_PARSEPTR;
        Vmap.release (fun->block_symbols);
        fun_release (&fun);
        return LA_OK;
      }

      if (err is LA_ERR_CONTINUE) {
        la_fun_release_symbols (fun, 1, 0);
        continue;
      }

      THROW_ERR_IF_ERR(err);

      la_fun_release_symbols (fun, 1, 0);
    }

    goto theend;
  }

  if (v.type is LIST_TYPE) {
    THROW_SYNTAX_ERR_IF(num isnot 2 and num isnot 1, "error parsing for each for lists: awaiting one or two identifiers");

    listType *list = AS_LIST(v);

    char *index = NULL;
    char *elem = NULL;

    if (num is 2) {
      index = aident;
      elem =  bident;
    } else {
      bident[0] = '_'; bident[1] = '\0';
      index = bident;
      elem = aident;
    }

    VALUE vidx = INT(-1);
    sym_t *index_sym = la_define_block_symbol (this, fun, index, INTEGER_TYPE, vidx, 0);

    VALUE elem_value = NULL_VALUE;

    sym_t *elem_sym = la_define_block_symbol (this, fun, elem, NULL_TYPE, elem_value, 0);
    num = list->num_items;

    la_string body_str = StringNew (body);

    for (int i = 0; i < num; i++) {
      integer v_idx = AS_INT(index_sym->value);
      v_idx++;
      if (v_idx >= num) break;
      index_sym->value = INT(v_idx);

      listNode *node = DListGetAt(list, listNode, v_idx);

      elem_value = *node->value;
      elem_sym->value = elem_value;
      elem_value.refcount++;
      this->curState |= LOOP_STATE;
      err = la_parse_string (this, body_str);
      this->curState &= ~LOOP_STATE;
      THROW_ERR_IF_ERR(err);
      elem_value.refcount--;
      elem_sym->value = NULL_VALUE;

      if (err is LA_ERR_BREAK) goto theend;

      if (HASTORETURN) {
        this->curState &= ~LOOP_STATE;
        RESET_PARSEPTR;
        Vmap.release (fun->block_symbols);
        fun_release (&fun);
        return LA_OK;
      }

      if (err is LA_ERR_CONTINUE) {
        la_fun_release_symbols (fun, 1, 0);
        continue;
      }

      THROW_ERR_IF_ERR(err);

      la_fun_release_symbols (fun, 1, 0);
    }

    goto theend;
  }

  THROW_SYNTAX_ERR_IF(num isnot 1 and num isnot 3, "error parsing for each for strings: awaiting one or three identifiers");

  string *str = AS_STRING(v);

  char *ci = aident;
  char *s  = NULL;
  char *w  = NULL;

  if (num is 3) {
    s  = bident;
    w  = cident;

    v = INT(0);
    sym_t *c_sym = la_define_block_symbol (this, fun, ci, INTEGER_TYPE, v, 0);
    v = STRING_NEW_WITH_LEN("", 0);
    sym_t *s_sym = la_define_block_symbol (this, fun, s, STRING_TYPE, v, 0);
    v = INT(0);
    sym_t *w_sym = la_define_block_symbol (this, fun, w, INTEGER_TYPE, v, 0);

    Ustring_t *U = Ustring.new ();
    ustring_t *u = Ustring.encode (U, str->bytes, str->num_bytes, 0, 8, 0);
    num = U->num_items;

    la_string body_str = StringNew (body);

    for (int i = 0; i < num; i++) {
      c_sym->value = INT(u->code);
      string *v_s = AS_STRING(s_sym->value);
      String.replace_with (v_s, u->buf);
      w_sym->value = INT(u->width);
      u = u->next;

      this->curState |= LOOP_STATE;
      err = la_parse_string (this, body_str);
      this->curState &= ~LOOP_STATE;
      THROW_ERR_IF_ERR(err);

      if (err is LA_ERR_BREAK) {
        Ustring.release (U);
        goto theend;
      }

      if (HASTORETURN) {
        this->curState &= ~LOOP_STATE;
        RESET_PARSEPTR;
        Vmap.release (fun->block_symbols);
        fun_release (&fun);
        Ustring.release (U);
        return LA_OK;
      }

      if (err is LA_ERR_CONTINUE) {
        la_fun_release_symbols (fun, 1, 0);
        continue;
      }

      THROW_ERR_IF_ERR(err);

      la_fun_release_symbols (fun, 1, 0);
    }

    Ustring.release (U);
    goto theend;
  }

  v = INT(0);
  sym_t *c_sym = la_define_block_symbol (this, fun, ci, INTEGER_TYPE, v, 0);
  num = str->num_bytes;

  la_string body_str = StringNew (body);

  for (int i = 0; i < num; i++) {
    c_sym->value = INT(str->bytes[i]);

    this->curState |= LOOP_STATE;
    err = la_parse_string (this, body_str);
    this->curState &= ~LOOP_STATE;
    THROW_ERR_IF_ERR(err);

    if (err is LA_ERR_BREAK) goto theend;

    if (HASTORETURN) {
      this->curState &= ~LOOP_STATE;
      RESET_PARSEPTR;
      Vmap.release (fun->block_symbols);
      fun_release (&fun);
      return LA_OK;
    }

    if (err is LA_ERR_CONTINUE) {
      la_fun_release_symbols (fun, 1, 0);
      continue;
    }

    THROW_ERR_IF_ERR(err);

    la_fun_release_symbols (fun, 1, 0);
  }

theend:
  this->curScope = save_scope;
  Vmap.release (fun->block_symbols);
  fun_release (&fun);

  this->loopCount--;
  ifnot (this->loopCount)
    this->curState &= ~LOOP_STATE;
  else
    this->curState |= LOOP_STATE;

  PARSEPTR = savepc;

  TOKEN = TOKEN_SEMICOLON;

  if (err is LA_ERR_BREAK and this->breakCount) {
    this->breakCount--;
    return LA_ERR_BREAK;
  }

  return LA_OK;
}

static char *find_end_for_stmt (const char *str) {
  int in_str = 0;
  int op_paren = 0;
  int cl_paren = 0;

  char *ptr = (char *) str;
  char prev_c = *ptr;

  while (cl_paren <= op_paren) {
    if (*ptr is TOKEN_EOF)
      return NULL;

    if (*ptr is TOKEN_PAREN_CLOS) {
      ifnot (in_str) cl_paren++;
      goto next;
    }

    if (*ptr is TOKEN_PAREN_OPEN) {
      ifnot (in_str) op_paren++;
      goto next;
    }

    if ((*ptr is TOKEN_DQUOTE or *ptr is TOKEN_BQUOTE) and prev_c isnot TOKEN_ESCAPE_CHR) {
      if (in_str)
        in_str--;
      else
        in_str++;
      goto next;
    }

   next: prev_c = *ptr; ptr++;
  }

  return ptr;
}

static int la_parse_loop_for_n_times (la_t *this, int n) {
  int err;
  this->loopCount++;

  funT *save_scope = this->curScope;

  funT *fun = Fun_new (this, funNew (
    .name = NS_LOOP_BLOCK, .namelen = NS_LOOP_BLOCK_LEN, .parent = this->curScope
  ));

  fun->block_symbols = Vmap.new (8);

  err = la_parse_block (this, "loop for nth times");
  THROW_ERR_IF_ERR(err);

  int is_single = TOKEN isnot TOKEN_BLOCK;

  const char *tmp_ptr = GETSTRPTR(TOKENSTR);

  integer bodylen = GETSTRLEN(TOKENSTR);
  char body[bodylen + 1];
  for (integer i = 0; i < bodylen; i++)
    body[i] = tmp_ptr[i];
  body[bodylen] = '\0';

  la_string body_str = StringNew (body);

  la_string savepc = PARSEPTR;

  this->curScope = fun;

  for (integer i = 0; i < n; i++) {
    this->curState |= LOOP_STATE;
    if (is_single) {
      PARSEPTR = body_str;
      NEXT_TOKEN();
      err = la_parse_stmt (this);
      if (TOKEN > TOKEN_EOF and TOKEN isnot TOKEN_SEMICOLON) {
        la_print_current_line (this,
          "[WARNING]: extra tokens detected after the end of a loop statement\n",
          NULL);
      }

    } else {
      err = la_parse_string (this, body_str);
    }

    this->curState &= ~LOOP_STATE;
    THROW_ERR_IF_ERR(err);

    if (err is LA_ERR_BREAK) goto theend;

    if (HASTORETURN) {
      this->curState &= ~LOOP_STATE;
      RESET_PARSEPTR;
      Vmap.release (fun->block_symbols);
      fun_release (&fun);
      return LA_OK;
    }

    if (err is LA_ERR_CONTINUE) {
      la_fun_release_symbols (fun, 1, 0);
      continue;
    }

    la_fun_release_symbols (fun, 1, 0);

    THROW_ERR_IF_ERR(err);
  }

theend:
  this->curScope = save_scope;
  Vmap.release (fun->block_symbols);
  fun_release (&fun);

  this->loopCount--;
  ifnot (this->loopCount)
    this->curState &= ~LOOP_STATE;
  else
    this->curState |= LOOP_STATE;

  PARSEPTR = savepc;

  TOKEN = TOKEN_SEMICOLON;

  if (err is LA_ERR_BREAK and this->breakCount) {
    this->breakCount--;
    return LA_ERR_BREAK;
  }

  return LA_OK;
}

static int la_parse_for (la_t *this) {
  int err;
  int c;
  VALUE v;

  NEXT_TOKEN();

  switch (TOKEN) {
    case TOKEN_PAREN_OPEN:
      goto cfor;

    case TOKEN_INTEGER:
      err = la_parse_primary (this, &v);
      THROW_ERR_IF_ERR(err);

      NEXT_TOKEN();

      return la_parse_loop_for_n_times (this, AS_INT(v));
  }


  c = PEEK_NTH_TOKEN(0);

  if (c is TOKEN_TIMES) {
    v = TOKENVAL;
    NEXT_RAW_TOKEN();
    NEXT_TOKEN();
    return la_parse_loop_for_n_times (this, AS_INT(v));
  }

  if (c isnot TOKEN_COMMA and c isnot TOKEN_IN)
    THROW_SYNTAX_ERR("error while parsing for loop, awaiting ( or a comma or in or times[s]");

  return la_parse_foreach (this);

cfor:
  c = TOKEN;

  this->loopCount++;

  if (c isnot TOKEN_PAREN_OPEN)
    THROW_SYNTAX_ERR("error while parsing for loop, awaiting (");

  funT *save_scope = this->curScope;

  funT *fun = Fun_new (this, funNew (
    .name = NS_LOOP_BLOCK, .namelen = NS_LOOP_BLOCK_LEN, .parent = this->curScope
  ));
  fun->block_symbols = Vmap.new (8);

  this->curScope = fun;

  NEXT_TOKEN();

  this->curState |= BLOCK_STATE;
  do {
    err = la_parse_stmt (this);
    THROW_ERR_IF_ERR(err);
  } while (TOKEN is TOKEN_COMMA);
  this->curState &= ~BLOCK_STATE;

  const char *tmp_ptr = GETSTRPTR(PARSEPTR);
  char *ptr = Cstring.byte.in_str (tmp_ptr, TOKEN_SEMICOLON);
  if (NULL is ptr)
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

  la_string cond_str = StringNewLen (cond, cond_len + 2);

  advanced_len++;

  tmp_ptr = ptr + 1;
  ptr = find_end_for_stmt (tmp_ptr);
  if (NULL is ptr)
    THROW_SYNTAX_ERR("error while parsing for loop, awaiting )");

  integer stmt_len = ptr - tmp_ptr;
  advanced_len += stmt_len;

  char stmt[stmt_len + 2];
  for (integer i = 0; i < stmt_len; i++)
    stmt[i] = tmp_ptr[i];
  stmt[stmt_len] = TOKEN_SEMICOLON;
  stmt[stmt_len + 1] = '\0';

  la_string stmt_str = StringNewLen (stmt, stmt_len + 1);

  advanced_len++;

  SETSTRPTR(PARSEPTR, ptr + 1);
  SETSTRLEN(PARSEPTR, orig_len - advanced_len);

  if (*ptr is TOKEN_NL) UNGET_BYTE();

  NEXT_TOKEN();

  err = la_parse_block (this, "for");
  THROW_ERR_IF_ERR(err);

  int is_single = TOKEN isnot TOKEN_BLOCK;

  tmp_ptr = GETSTRPTR(TOKENSTR);

  integer bodylen = GETSTRLEN(TOKENSTR);
  char body[bodylen + 1];
  for (integer i = 0; i < bodylen; i++)
    body[i] = tmp_ptr[i];
  body[bodylen] = '\0';

  la_string body_str = StringNewLen (body, bodylen);

  la_string savepc = PARSEPTR;

  for (;;) {
    PARSEPTR = cond_str;

    TOKEN = TOKEN_PAREN_OPEN;
    this->curState |= BLOCK_STATE;
    err = la_parse_expr (this, &v);
    this->curState &= ~BLOCK_STATE;
    THROW_ERR_IF_ERR(err);

    ifnot (AS_INT(v)) goto theend;

    this->curState |= LOOP_STATE;
    if (is_single) {
      PARSEPTR = body_str;
      NEXT_TOKEN();
      err = la_parse_stmt (this);
      if (TOKEN > TOKEN_EOF and TOKEN isnot TOKEN_SEMICOLON) {
        la_print_current_line (this,
          "[WARNING]: extra tokens detected after the end of a for statement\n",
          NULL);
      }

    } else {
      err = la_parse_string (this, body_str);
    }

    this->curState &= ~LOOP_STATE;
    THROW_ERR_IF_ERR(err);

    if (err is LA_ERR_BREAK) goto theend;

    if (HASTORETURN) {
      this->curState &= ~LOOP_STATE;
      TOKEN = TOKEN_SEMICOLON;
      RESET_PARSEPTR;
      Vmap.release (fun->block_symbols);
      fun_release (&fun);
      return LA_OK;
    }

    if (err is LA_ERR_CONTINUE)
      goto ctl_stmt;

    THROW_ERR_IF_ERR(err);

    ctl_stmt:
      PARSEPTR = stmt_str;
      this->curState |= BLOCK_STATE;
      do {
        NEXT_TOKEN();
        err = la_parse_stmt (this);
        THROW_ERR_IF_ERR(err);
      } while (TOKEN is TOKEN_COMMA);
      this->curState &= ~BLOCK_STATE;

      la_fun_release_symbols (fun, 1, 0);
  }

theend:
  this->curScope = save_scope;
  Vmap.release (fun->block_symbols);
  fun_release (&fun);

  this->loopCount--;
  ifnot (this->loopCount)
    this->curState &= ~LOOP_STATE;
  else
    this->curState |= LOOP_STATE;

  PARSEPTR = savepc;

  TOKEN = TOKEN_SEMICOLON;

  if (err is LA_ERR_BREAK and this->breakCount) {
    this->breakCount--;
    return LA_ERR_BREAK;
  }

  return LA_OK;
}

static int la_parse_forever (la_t *this) {
  int err;
  this->loopCount++;

  funT *save_scope = this->curScope;

  funT *fun = Fun_new (this, funNew (
    .name = NS_LOOP_BLOCK, .namelen = NS_LOOP_BLOCK_LEN, .parent = this->curScope
  ));

  fun->block_symbols = Vmap.new (8);

  this->curScope = fun;

  NEXT_TOKEN();

  err = la_parse_block (this, "forever");
  THROW_ERR_IF_ERR(err);

  int is_single = TOKEN isnot TOKEN_BLOCK;

  const char *tmp_ptr = GETSTRPTR(TOKENSTR);

  integer bodylen = GETSTRLEN(TOKENSTR);
  char body[bodylen + 1];
  for (integer i = 0; i < bodylen; i++)
    body[i] = tmp_ptr[i];
  body[bodylen] = '\0';

  la_string body_str = StringNewLen (body, bodylen);

  la_string savepc = PARSEPTR;

  for (;;) {
    this->curState |= LOOP_STATE;
    if (is_single) {
      PARSEPTR = body_str;
      NEXT_TOKEN();
      err = la_parse_stmt (this);
      if (TOKEN > TOKEN_EOF and TOKEN isnot TOKEN_SEMICOLON) {
        la_print_current_line (this,
          "[WARNING]: extra tokens detected after the end of a forever statement\n",
          NULL);
      }

    } else {
      err = la_parse_string (this, body_str);
    }

    this->curState &= ~LOOP_STATE;
    THROW_ERR_IF_ERR(err);

    if (err is LA_ERR_BREAK) goto theend;

    if (HASTORETURN) {
      this->curState &= ~LOOP_STATE;
      RESET_PARSEPTR;
      Vmap.release (fun->block_symbols);
      fun_release (&fun);
      return LA_OK;
    }

    if (err is LA_ERR_CONTINUE) {
      la_fun_release_symbols (fun, 1, 0);
      continue;
    }

    la_fun_release_symbols (fun, 1, 0);

    THROW_ERR_IF_ERR(err);
  }

theend:
  this->curScope = save_scope;
  Vmap.release (fun->block_symbols);
  fun_release (&fun);

  this->loopCount--;
  ifnot (this->loopCount)
    this->curState &= ~LOOP_STATE;
  else
    this->curState |= LOOP_STATE;

  PARSEPTR = savepc;
  TOKEN = TOKEN_SEMICOLON;

  if (err is LA_ERR_BREAK and this->breakCount) {
    this->breakCount--;
    return LA_ERR_BREAK;
  }

  return LA_OK;
}

static int la_parse_visibility (la_t *this) {
  this->scopeState = 0;
  int token = Cstring.eq_n (GETSTRPTR(TOKENSTR), "public", 6)
      ? TOKEN_PUBLIC : TOKEN_PRIVATE;
  int scope = (token is TOKEN_PUBLIC ? PUBLIC_SCOPE : PRIVATE_SCOPE);

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

      if (c is TOKEN_EOF)
        THROW_SYNTAX_ERR("unended statement");
      THROW_SYNTAX_ERR("unsupported visibility/scope attribute");
  }

  return token;
}

static int la_parse_arg_list (la_t *this, funT *uf) {
  int c;
  int nargs = 0;

  NEXT_RAW_TOKEN();
  c = TOKEN;

  for (;;) {
    if (c is TOKEN_PAREN_CLOS) break;

    THROW_SYNTAX_ERR_IF(c isnot TOKEN_SYMBOL, "argument list definition, awaiting a symbol name");

    la_string name = TOKENSTR;

    if (nargs >= MAX_BUILTIN_PARAMS)
      THROW_TOOMANY_FUNCTION_ARGS();

    const char *ptr = GETSTRPTR(name);
    size_t len = GETSTRLEN(name);
    Cstring.cp (uf->argName[nargs], MAXLEN_SYMBOL, ptr, len);

    THROW_SYNTAX_ERR_IF_SYM_NAME_EXCEEDS_LEN(uf->argName[nargs], len);

    nargs++;

    NEXT_TOKEN();
    c = TOKEN;

    if (c is TOKEN_COMMA) {
      NEXT_RAW_TOKEN();
      if (TOKEN is TOKEN_NL)
        NEXT_RAW_TOKEN();

      THROW_SYNTAX_ERR_IF(TOKEN is TOKEN_NL or TOKEN is TOKEN_COMMA,
          "awaiting a symbol, while parsing argument list");

      c = TOKEN;
    }
  }

  uf->nargs = nargs;
  return nargs;
}

static int la_parse_func_def (la_t *this) {
  la_string name;
  int nargs = 0;
  size_t len = 0;

  char *fn = NULL;

  ifnot (this->curFunName[0]) {
    NEXT_RAW_TOKEN();

    THROW_SYNTAX_ERR_IF(TOKEN isnot TOKEN_SYMBOL,
      "function definition, not a symbol");

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

  funT *uf = Fun_new (this, funNew (
    .name = fn, .namelen = len, .parent = this->curScope
  ));

  NEXT_TOKEN();

  if (TOKEN is TOKEN_PAREN_OPEN) {
    la_fun_stack_push (this, this->curScope);

    this->curScope = uf;

    nargs = la_parse_arg_list (this, uf);

    this->curScope = la_fun_stack_pop (this);

    if (nargs < 0) return nargs;

    NEXT_TOKEN();
  }

  int err = la_parse_block (this, STR_FMT("`%s' function", uf->funname));
  THROW_ERR_IF_ERR(err);

  uf->body = TOKENSTR;

  VALUE v = PTR(uf);

  funT *scope = (this->scopeState is PUBLIC_SCOPE ? this->function : this->curScope);
  this->curSym = la_define_symbol (this, scope, uf->funname, (UFUNC_TYPE | (nargs << 8)), v, 0);
  this->scopeState = 0;

  this->curFunDef = uf;

  NEXT_TOKEN();
  return LA_OK;
}

static int la_parse_fmt (la_t *this, string *str, int break_at_eof) {
  int err = LA_NOTOK;
  VALUE value;
  char directive = 0;

  this->fmtRefcount++;

  int pc = 0;
  int c;

  for (;;) {
    get_byte:
    c = GET_BYTE();

    if (c is TOKEN_EOF and break_at_eof)
      break;

    if (c is TOKEN_DQUOTE) {
      if (pc isnot TOKEN_ESCAPE_CHR)
        break;

      String.append_byte (str, '\\');
      String.append_byte (str, TOKEN_DQUOTE);
      pc = TOKEN_DQUOTE;
      c = GET_BYTE();
    }

    if (c is TOKEN_ESCAPE_CHR) {
      if (pc is TOKEN_ESCAPE_CHR) {
        pc = str->bytes[str->num_bytes - 1];
        String.append_byte (str, TOKEN_ESCAPE_CHR);
      } else {
        int cc = PEEK_NTH_BYTE(0);
        switch (cc) {
          case 'a' : String.append_byte (str, '\a'); break;
          case 'b' : String.append_byte (str, '\b'); break;
          case 'f' : String.append_byte (str, '\f'); break;
          case 't' : String.append_byte (str, '\t'); break;
          case 'r' : String.append_byte (str, '\r'); break;
          case 'n' : String.append_byte (str, '\n'); break;
          case 'v' : String.append_byte (str, '\v'); break;
          case 'e' : String.append_byte (str,  033); break;
          case '$' :
            if (PEEK_NTH_BYTE(1) is TOKEN_BLOCK_OPEN) {
              String.append_byte (str, '$');
              break;
            }
            // fall through

          default:
            String.append_byte (str, TOKEN_ESCAPE_CHR);
            String.append_byte (str, cc);
        }

        GET_BYTE(); pc = 0;
        goto get_byte;
      }

      c = GET_BYTE();
    }

    if (c is '$' and PEEK_NTH_BYTE(0) is TOKEN_BLOCK_OPEN) {
      c = GET_BYTE();

      pc = c;
      NEXT_TOKEN();
      c = TOKEN;

      if (*(GETSTRPTR(TOKENSTR)) is '%' and
            GETSTRLEN(TOKENSTR) is 1) {
        c = GET_BYTE();
        if (c isnot 's' and c isnot 'p' and c isnot 'd' and
            c isnot 'o' and c isnot 'x' and c isnot 'f') {
          this->print_fmt_bytes (this->err_fp, "string fmt error, unsupported directive [%c]\n", c);
          la_err_ptr (this, LA_NOTOK);
          err = LA_ERR_SYNTAX;
          goto theend;
        } else
          directive = c;

        NEXT_TOKEN();
        c = TOKEN;

        if (c isnot TOKEN_COMMA) {
          this->print_fmt_bytes (this->err_fp, "string fmt error, awaiting a comma\n");
          la_err_ptr (this, LA_NOTOK);
          err = LA_ERR_SYNTAX;
          goto theend;
        }

        pc = TOKEN_COMMA;
        NEXT_TOKEN();
        c = TOKEN;
      }

      this->exprList++;
      err = la_parse_expr (this, &value);
      this->exprList--;
      if (err isnot LA_OK) goto theend;

      if (TOKEN isnot TOKEN_BLOCK_CLOS) {
        this->print_fmt_bytes (this->err_fp, "string fmt error, awaiting '}'\n");
        la_err_ptr (this, LA_NOTOK);
        err = LA_ERR_SYNTAX;
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
              String.append_with_fmt (str, "%s", AS_STRING_BYTES(value));
              if ((this->fmtState & FMT_LITERAL) or (value.sym is NULL and
                  value.refcount isnot MALLOCED_STRING and
                  0 is (this->objectState & (MAP_MEMBER|ARRAY_MEMBER)))) {
                la_release_val (this, value);
              }

              this->fmtState &= ~FMT_LITERAL;
              this->objectState &= ~(ARRAY_MEMBER|MAP_MEMBER);
              break;

            case NULL_TYPE:
              String.append_with_len (str, "(null)", 6);
              break;

            case ARRAY_TYPE: {
              VALUE v;
              TOKENVAL = value;
              err = la_parse_array_get (this, &v);
              if (err isnot LA_OK) {
                this->print_bytes (this->err_fp, "string fmt error, awaiting an array\n");
                err = LA_ERR_SYNTAX;
                goto theend;
              }

              switch (v.type) {
                case STRING_TYPE:
                  String.append_with_fmt (str, "%s", AS_STRING_BYTES(v));
                  break;

                default:
                  this->print_bytes (this->err_fp, "string fmt error, awaiting a string\n");
                  err = LA_ERR_SYNTAX;
                  goto theend;

              }
              break;
            }

            default:
              this->print_bytes (this->err_fp, "string fmt error, awaiting a string\n");
              err = LA_ERR_SYNTAX;
              goto theend;
          }

          break;

        case 'p':
          String.append_with_fmt (str, "%p", AS_PTR(value));
          if (value.type is STRING_TYPE) {
            if ((this->fmtState & FMT_LITERAL) or (value.sym is NULL and
                value.refcount isnot MALLOCED_STRING and
               0 is (this->objectState & (ARRAY_MEMBER|MAP_MEMBER)))) {
                la_release_val (this, value);
            }
            this->fmtState &= ~FMT_LITERAL;
            this->objectState &= ~(ARRAY_MEMBER|MAP_MEMBER);
          }
          break;

        case 'o':
          switch (value.type) {
            case INTEGER_TYPE:
              String.append_with_fmt (str, "0%o", AS_INT(value));
              break;

            default:
              this->print_bytes (this->err_fp, "string fmt error, awaiting an integer\n");
              err = LA_ERR_SYNTAX;
              goto theend;
          }

          break;

        case 'x':
          switch (value.type) {
            case INTEGER_TYPE:
              String.append_with_fmt (str, "0x%x", AS_INT(value));
              break;

            default:
              this->print_bytes (this->err_fp, "string fmt error, awaiting an integer\n");
              err = LA_ERR_SYNTAX;
              goto theend;
           }
           break;

        case 'f':
          switch (value.type) {
            case ARRAY_TYPE: {
              VALUE v;
              TOKENVAL = value;
              err = la_parse_array_get (this, &v);
              if (err isnot LA_OK) {
                this->print_bytes (this->err_fp, "string fmt error, awaiting an array\n");
                err = LA_ERR_SYNTAX;
                goto theend;
              }

              switch (v.type) {
                case NUMBER_TYPE:
                  String.append_with_fmt (str, "%.15f", AS_NUMBER(v));
                  break;

                default:
                  this->print_bytes (this->err_fp, "string fmt error, awaiting a number\n");
                  err = LA_ERR_SYNTAX;
                  goto theend;
              }
              break;
            }

          case NUMBER_TYPE:
            String.append_with_fmt (str, "%.15f", AS_NUMBER(value));
            break;

            default:
              this->print_bytes (this->err_fp, "string fmt error, awaiting a number\n");
              err = LA_ERR_SYNTAX;
              goto theend;
          }

          break;

        case 'd':
        default:
          switch (value.type) {
            case ARRAY_TYPE: {
              VALUE v;
              TOKENVAL = value;
              err = la_parse_array_get (this, &v);
              if (err isnot LA_OK) {
                this->print_bytes (this->err_fp, "string fmt error, awaiting array\n");
                err = LA_ERR_SYNTAX;
                goto theend;
              }

              switch (v.type) {
                case INTEGER_TYPE:
                  String.append_with_fmt (str, "%d", AS_INT(v));
                  break;

                default:
                  this->print_bytes (this->err_fp, "string fmt error, awaiting an integer\n");
                  err = LA_ERR_SYNTAX;
                  goto theend;

              }
              break;
            }

            case STRING_TYPE: {
              VALUE v;
              TOKENVAL = value;
              err = la_string_get (this, &v);
              if (err isnot LA_OK) {
                this->print_bytes (this->err_fp, "string fmt error, awaiting a string\n");
                err = LA_ERR_SYNTAX;
                goto theend;
              }

              switch (v.type) {
                case INTEGER_TYPE:
                  String.append_with_fmt (str, "%d", AS_INT(v));
                  break;

                default:
                  this->print_bytes (this->err_fp, "string fmt error, awaiting an integer\n");
                  err = LA_ERR_SYNTAX;
                  goto theend;
              }
              break;
            }

          case INTEGER_TYPE:
            String.append_with_fmt (str, "%d", AS_INT(value));
            break;

          default:
            this->print_bytes (this->err_fp, "string fmt error, awaiting an integer\n");
            err = LA_ERR_SYNTAX;
            goto theend;
        }
      }

      directive = 0;
      continue;
    }

    String.append_byte (str, c);
  }

  err = LA_OK;

theend:
  this->fmtRefcount--;
  return err;
}

static int la_parse_print (la_t *this) {
  int err = LA_NOTOK;

  this->byteCount = 0;

  string *str = String.new (32);

  int c = la_ignore_ws (this);

  if (c isnot TOKEN_PAREN_OPEN) {
    this->print_bytes (this->err_fp, "print string error, awaiting (\n");
    la_err_ptr (this, LA_NOTOK);
    goto theend;
  }

  c = la_ignore_ws (this);

  FILE *fp = this->out_fp;
  int is_add_operation = 0;
  int num_iterations = 0;

  VALUE v;

  if (c isnot TOKEN_DQUOTE) {
    UNGET_BYTE();

    NEXT_TOKEN();
    c = TOKEN;

    this->curState |= MALLOCED_STRING_STATE;
    err = la_parse_expr (this, &v);
    this->curState &= ~MALLOCED_STRING_STATE;
    THROW_ERR_IF_ERR(err);

    check_expr:
    if (err isnot LA_OK) goto theend;
    num_iterations++;

    c = TOKEN;

    switch (v.type) {
      case STRING_TYPE: {
          string *vs = AS_STRING(v);

          ifnot (is_add_operation)
            String.append_with_len (str, vs->bytes, vs->num_bytes);
          else
            String.replace_with_len (str, vs->bytes, vs->num_bytes);

          ifnot (this->objectState & RHS_STRING_RELEASED)
            if (v.sym is NULL and
                v.refcount isnot MALLOCED_STRING and
                0 is (this->objectState & (ARRAY_MEMBER|MAP_MEMBER)))
              la_release_val (this, v);

          this->objectState &= ~(ARRAY_MEMBER|MAP_MEMBER);
        }

        goto print_str;

      case INTEGER_TYPE:
        String.append_with_fmt (str, "%d", AS_INT(v));
        goto print_str;

      case NUMBER_TYPE:
        String.append_with_fmt (str, "%f", AS_NUMBER(v));
        goto print_str;

      case NULL_TYPE:
        String.append_with_len (str, "(null)", 6);
        goto print_str;

      case FILEPTR_TYPE:
        if (1 is num_iterations)
          break;

      // fall through
      default:
        String.append_with_fmt (str, "%p", AS_PTR(v));
        goto print_str;
    }

    fp = AS_FILEPTR(v);
    IGNORE_NEXT_BYTE;
    c = la_ignore_ws (this);

    if (c isnot TOKEN_DQUOTE) {
      UNGET_BYTE();

      NEXT_TOKEN();
      c = TOKEN;

      err = la_parse_expr (this, &v);
      THROW_ERR_IF_ERR(err);
      goto check_expr;
    }
  }

  if (c isnot TOKEN_DQUOTE) {
    this->print_bytes (this->err_fp, "print string error, awaiting double quote\n");
    la_err_ptr (this, LA_NOTOK);
    goto theend;
  }

  err = la_parse_fmt (this, str, 0);
  THROW_ERR_IF_ERR(err);

  NEXT_TOKEN();
  c = TOKEN;

print_str:
  this->print_fp = fp;

  if (c isnot TOKEN_PAREN_CLOS) {
    if ((c & 0xff) is TOKEN_BINOP) {
      if (AS_VOID_PTR(TOKENVAL) is la_add) {
        is_add_operation = 1;
        TOKEN = c;
        v = STRING(str);
        if (v.refcount > -1)
          v.refcount++;
        this->curState |= MALLOCED_STRING_STATE;
        err = la_parse_expr_level (this, MAX_EXPR_LEVEL, &v);
        THROW_ERR_IF_ERR(err);

        if (v.refcount > 0)
          v.refcount--;
        this->curState &= ~MALLOCED_STRING_STATE;
        goto check_expr;
      }
    }

    this->print_fmt_bytes (this->err_fp, "string fmt error, awaiting ) found %c\n", c);
    la_err_ptr (this, LA_NOTOK);
    err = LA_ERR_SYNTAX;
    goto theend;
  }

  int num_bts = this->print_bytes (fp, str->bytes);
  if (num_bts < 0) {
    this->print_bytes (this->err_fp, "error while printing string\n");
    this->print_fmt_bytes (this->err_fp, "%s\n", str->bytes);
    this->print_fmt_bytes (this->err_fp, "%s|n", Error.errno_string (errno));
    goto theend;
  }

  NEXT_TOKEN();

  this->byteCount = num_bts;
  err = LA_OK;

theend:
  String.release (str);
  return err;
}

static int la_parse_println (la_t *this) {
  int err = la_parse_print (this);
  if (err > LA_NOTOK) {
    fprintf (this->print_fp, "\n");
    this->byteCount++;
  }

  return err;
}

static int la_parse_exit (la_t *this) {
  NEXT_TOKEN();

  VALUE v;
  la_parse_expr (this, &v);
  this->exitValue = AS_INT(v);

  RESET_PARSEPTR;
  HASTORETURN = 1;

  return LA_ERR_EXIT;
}

static int la_parse_return (la_t *this) {
  int err = LA_OK, type;
  funT *scope = this->curScope;

  NEXT_TOKEN();

  switch (TOKEN) {
    case TOKEN_SEMICOLON:
    case       TOKEN_EOF:
    case        TOKEN_NL:
      HASTORETURN = 1;
      RESET_PARSEPTR;
      return LA_OK;
  }

  while (scope and Cstring.eq_n (scope->funname, "__block_", 8))
    scope = scope->prev;

  if (NULL is scope)
    THROW_SYNTAX_ERR("error while parsing return, unknown scope");

  this->curState |= MALLOCED_STRING_STATE;
  int token = PEEK_NTH_TOKEN(0);
  this->curState &= ~MALLOCED_STRING_STATE;

  if (token is TOKEN_IF or token is TOKEN_IFNOT ) {
    tokenState save = SAVE_TOKENSTATE();

    NEXT_TOKEN();

    err = la_parse_cond (this, token is TOKEN_IFNOT);
    THROW_ERR_IF_ERR(err);

    ifnot (AS_INT(CONDVAL)) return LA_OK;

    tokenState save_again = SAVE_TOKENSTATE();

    RESTORE_TOKENSTATE(save);
    SETSTRLEN(PARSEPTR, GETSTRLEN(TOKENSTR));

    err = la_parse_primary (this, &scope->result);
    THROW_ERR_IF_ERR(err);

    RESTORE_TOKENSTATE(save_again);

    HASTORETURN = 1;
    goto check_value;
  }

  if (TOKEN is TOKEN_USRFUNC)
    if (Cstring.eq ("self", sym_key (this, TOKENSTR)))
      return LA_ERR_TCALLREC;

  HASTORETURN = 1;

  this->funcState |= RETURN_STATE;
  err = la_parse_expr (this, &scope->result);
  this->funcState &= ~RETURN_STATE;
  THROW_ERR_IF_ERR(err);

  ifnot (HASTORETURN) return LA_OK;

  check_value:
  type = scope->result.type;

  switch (type) {
    case ARRAY_TYPE:
    case STRING_TYPE:
      if (this->objectState & MAP_MEMBER) {
        scope->result.refcount++;
        if (scope->result.sym isnot NULL) {
          ifnot (scope->result.sym->type)
            scope->result.sym->type = MAP_TYPE;
        }

        this->objectState &= ~MAP_MEMBER;
        break;
      }
  }

  if (TOKEN isnot TOKEN_SEMICOLON and
      TOKEN isnot TOKEN_NL and
      TOKEN isnot TOKEN_EOF)
    if (PREVTOKEN isnot TOKEN_EOF or
        PREVTOKEN isnot TOKEN_NL or
        PREVTOKEN isnot TOKEN_SEMICOLON)
      THROW_SYNTAX_ERR("awaiting a semicolon or a new line after return");

  RESET_PARSEPTR;
  return err;
}

static int la_import_file (la_t *this, const char *module, const char *err_msg) {
  int err = LA_NOTOK;

  char *mname = Path.basename_sans_extname ((char *) module);

  module_so *it = this->function->modules->head;
  while (it) {
    if (Cstring.eq (it->name, mname)) {
      free (mname);
      return LA_OK;
    }

    it = it->next;
  }

  void *handle = dlopen (module, RTLD_NOW|RTLD_GLOBAL);

  if (handle is NULL) {
    err_msg = dlerror ();
    ifnot (this->curState & LOADFILE_SILENT)
      this->print_fmt_bytes (this->err_fp, "import error, %s\n", err_msg);

    free (mname);
    return LA_ERR_IMPORT;
  }

  size_t len = bytelen (mname) - 7;
  char tmp[len + 1];
  Cstring.substr (tmp, len, mname, len + 7, 0);

  string *init_fun = String.new_with_fmt ("__init_%s_module__", tmp);
  string *deinit_fun = String.new_with_fmt ("__deinit_%s_module__", tmp);

  dlerror ();
  ModuleInit init_sym = (ModuleInit) dlsym (handle, init_fun->bytes);
  err_msg = dlerror ();
  if (err_msg isnot NULL) {
    this->print_fmt_bytes (this->err_fp, "import error while getting %s symbol symbol address, %s\n", init_fun->bytes, err_msg);
    err = LA_ERR_DYNLINK;
    goto theend;
  }

  dlerror ();
  ModuleDeinit deinit_sym = (ModuleDeinit) dlsym (handle, deinit_fun->bytes);
  err_msg = dlerror ();
  if (err_msg isnot NULL) {
    this->print_fmt_bytes (this->err_fp, "import error while getting %s symbol symbol address, %s\n", deinit_fun->bytes, err_msg);
    err = LA_ERR_DYNLINK;
    goto theend;
  }

  int retval = init_sym (this);
  if (retval isnot LA_OK) {
    dlclose (handle);
    err = LA_ERR_DYNLINK;
    goto theend;
  }

  module_so *modl = Alloc (sizeof (module_so));
  modl->name = mname;
  modl->handle = handle;
  modl->init = init_sym;
  modl->deinit = deinit_sym;
  modl->instance = this;
  ListStackPush(this->function->modules, modl);
  err = LA_OK;

theend:
  ifnot (err is LA_OK)
    free (mname);

  String.release (init_fun);
  String.release (deinit_fun);
  return err;
}

static int la_parse_import (la_t *this) {
  int err;
  const char *err_msg = "";

  NEXT_TOKEN();
  int c = TOKEN;

  ifnot (c is TOKEN_PAREN_OPEN)
    THROW_SYNTAX_ERR("error while parsing import(), awaiting (");

  VALUE v;
  err = la_parse_expr (this, &v);
  THROW_SYNTAX_ERR_IF_ERR(err, "error while parsing import()");

  THROW_SYNTAX_ERR_IF (v.type isnot STRING_TYPE,
    "error while parsing import(), awaiting a string");

  string *fname = AS_STRING(v);
  string *fn = NULL;

  if (this->curState & LITERAL_STRING_STATE)
    this->curState &= ~LITERAL_STRING_STATE;
  else
    fname = String.dup (fname);

  String.append_with (fname, "-module.so");

#ifdef STATIC
  char *mname = Path.basename_sans_extname (fname->bytes);
  size_t len = bytelen (mname) - 7;
  char tmp[len+1];
  Cstring.substr (tmp, len, mname, len + 7, 0);
  free (mname);
  if ('a' <= *tmp and *tmp <= 'z') {
    utf8 chr = Ustring.to_upper (*tmp);
    *tmp = chr;
  }

  sym_t *sym = ns_lookup_symbol (this->function, tmp);
  if (NULL is sym)
    THROW_SYNTAX_ERR_FMT("%s module hasn't been initialized", tmp);

  return LA_OK;
#endif

  string *ns = NULL;
  funT *load_ns = this->curScope;
  funT *prev_ns = load_ns;

  if (TOKEN is TOKEN_COMMA) {
    NEXT_TOKEN();
    err = la_parse_expr (this, &v);
    THROW_ERR_IF_ERR(err);

    ifnot (v.type is STRING_TYPE)
      THROW_SYNTAX_ERR("error while parsing import() ns, awaiting a string");

    ns = AS_STRING(v);

    if (this->curState & LITERAL_STRING_STATE)
      this->curState &= ~LITERAL_STRING_STATE;
    else
      ns = String.dup (ns);

    ifnot (ns->num_bytes)
      goto theload;

    la_string x = StringNew (ns->bytes);
    sym_t *symbol = la_lookup_symbol (this, x);
    if (symbol isnot NULL) {
      v = symbol->value;
      if (v.type & UFUNCTION_TYPE) {
        funT *f = AS_FUNC_PTR(v);
        load_ns = f;
        goto theload;
      }

      this->print_fmt_bytes (this->err_fp, "import(): %s is not a namespace\n", ns->bytes);
      err = LA_ERR_SYNTAX;
      goto theend;
    }

    this->print_bytes (this->err_fp, "import(), functionality hasn't been implemented\n");
    err = LA_ERR_SYNTAX;
    goto theend;
  }

theload:
  this->curScope = load_ns;

  ifnot (Path.is_absolute (fname->bytes)) {
    ifnot (Cstring.eq (LA_STRING_NS, AS_STRING_BYTES(this->file->value))) {
      fn = String.dup (fname);
      String.prepend_byte (fn, DIR_SEP);
      char *dname = Path.dirname (AS_STRING_BYTES(this->file->value));
      String.prepend_with (fn, dname);
      free (dname);
      this->curState |= LOADFILE_SILENT;
      err = la_import_file (this, fn->bytes, err_msg);
      this->curState &= ~LOADFILE_SILENT;
      if (err is LA_OK or err is LA_ERR_DYNLINK)
        goto theend;
    }

    this->curState |= LOADFILE_SILENT;
    err = la_import_file (this, fname->bytes, err_msg);
    this->curState &= ~LOADFILE_SILENT;

    if (err is LA_OK or err is LA_ERR_DYNLINK)
      goto theend;

    sym_t *symbol = la_lookup_symbol (this, StringNew ("__importpath"));
    ArrayType *p_ar = (ArrayType *) AS_ARRAY(symbol->value);
    string **s_ar = (string **) AS_ARRAY(p_ar->value);
    for (size_t i = 0; i < p_ar->len; i++) {
      string *p = s_ar[i];
      ifnot (p->num_bytes) continue;

      String.release (fn);
      fn = String.dup (fname);
      String.prepend_with_fmt (fn, "%s/", p->bytes);
      this->curState |= LOADFILE_SILENT;
      err = la_import_file (this, fn->bytes, err_msg);
      this->curState &= ~LOADFILE_SILENT;
      if (err is LA_OK or err is LA_ERR_DYNLINK)
        goto theend;
    }
    goto theend;
  }

  err = la_import_file (this, fname->bytes, err_msg);

theend:
  if (err isnot LA_OK) {
    if (err isnot LA_ERR_DYNLINK)
      err = LA_ERR_IMPORT;

    this->print_fmt_bytes (this->err_fp, "[%s] import error: %s\n", fname->bytes, err_msg);
  }

  this->curScope = prev_ns;
  String.release (fname);
  String.release (fn);
  String.release (ns);

  return err;
}

static int la_search_for_annotation (la_t *this, string *evalbuf, VALUE *vp) {
  ifnot (evalbuf->num_bytes) {
    *vp = NULL_VALUE;
    return 0;
  }

  int c;

again:
  while ((c = GET_BYTE()))
    if (c is TOKEN_COMMENT or c <= 0)
      break;

  if (c isnot TOKEN_COMMENT) {
    *vp = NULL_VALUE;
    return 0;
  }

  c = GET_BYTE();

  if (c isnot TOKEN_AT) {
    if (c > 0)
      goto again;
    else {
      *vp = NULL_VALUE;
      return c;
    }
  }

  return 1;
}

static int la_parse_annotation (la_t *this, VALUE *vp) {
  int c = GET_BYTE();
  THROW_SYNTAX_ERR_IF(c isnot ' ', "parsing annotation: awaiting a space");

  c = GET_BYTE();

  switch (c) {
    case 'g':
      c = GET_BYTE();
      switch (c) {
        case 'e':
          c = GET_BYTE();
          THROW_SYNTAX_ERR_IF(c isnot 't', "parsing annotation: awaiting get");
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
  int err = la_parse_expr (this, &v);
  THROW_SYNTAX_ERR_IF_ERR(err, "error while parsing annotation, awaiting an expression");

  THROW_SYNTAX_ERR_IF (v.type isnot STRING_TYPE,
    "error while parsing annotation, awaiting a string");

  string *evalbuf = AS_STRING(v);

  la_string saved_ptr = PARSEPTR;
  PARSEPTR = StringNewLen (evalbuf->bytes, evalbuf->num_bytes);

  err = la_search_for_annotation (this, evalbuf, vp);
  THROW_ERR_IF_ERR(err);

  ifnot (err) {
    *vp = NULL_VALUE;
    return LA_OK;
  }

  string *s = String.new (32);

  c = la_get_annotated_block (this, s);
  THROW_ERR_IF_ERR(c);

  *vp = STRING(s);

  PARSEPTR = saved_ptr;

  return c;
}

static VALUE la_annotate_get (la_t *this, VALUE v_buf) {
  ifnot (IS_STRING(v_buf))  C_THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");

  string *buf = AS_STRING(v_buf);

  la_string saved_ptr = PARSEPTR;
  PARSEPTR = StringNewLen (buf->bytes, buf->num_bytes);

  VALUE v;

  int err = la_search_for_annotation (this, buf, &v);

  if (err < 0)
    C_THROW(err, "error while getting annotation");

  ifnot (err) {
    PARSEPTR = saved_ptr;
    return STRING_NEW ("");
  }

  string *s = String.new (32);

  err = la_get_annotated_block (this, s);

  PARSEPTR = saved_ptr;

  if (err < 0) {
    String.release (s);
    C_THROW(err, "error while getting annotation");
  }

  return STRING(s);
}

static int la_parse_loadfile (la_t *this) {
  NEXT_TOKEN();
  int c = TOKEN;
  int err;

  ifnot (c is TOKEN_PAREN_OPEN)
    THROW_SYNTAX_ERR("error while parsing loadfile(), awaiting (");

  VALUE v;
  err = la_parse_expr (this, &v);
  THROW_SYNTAX_ERR_IF_ERR(err, "error while parsing loadfile()");

  THROW_SYNTAX_ERR_IF (v.type isnot STRING_TYPE,
    "error while parsing loadfile(), awaiting a string");

  string *fname = AS_STRING(v);
  string *fn = NULL;

  if (this->curState & LITERAL_STRING_STATE)
    this->curState &= ~LITERAL_STRING_STATE;
  else {
    string *tmp = fname;
    fname = String.dup (tmp);
    if (v.sym is NULL and v.refcount isnot MALLOCED_STRING and
        0 is (this->objectState & (ARRAY_MEMBER|MAP_MEMBER)))
      String.release (tmp);
  }

  if (File.exists (fname->bytes) and File.is_reg (fname->bytes))
    goto loadfile;

  char *extname = Path.extname (fname->bytes);

  size_t exlen = bytelen (extname);
  if (exlen) {
    if (exlen isnot fname->num_bytes) {
      char *p = fname->bytes + fname->num_bytes - 1;
      while (*p isnot '.') {
        p--;
        String.clear_at (fname, fname->num_bytes - 1);
      }
    } else  // .file
      String.append_byte (fname, '.');
  } else
    String.append_byte (fname, '.');

  String.append_with (fname, LA_EXTENSION);

loadfile: {}

  string *ns = NULL;
  funT *load_ns = this->curScope;
  funT *prev_ns = load_ns;

  while (load_ns and Cstring.eq_n (load_ns->funname, "__block_", 8))
    load_ns = load_ns->prev;

  if (TOKEN is TOKEN_COMMA) {
    NEXT_TOKEN();
    err = la_parse_expr (this, &v);
    THROW_ERR_IF_ERR(err);

    ifnot (v.type is STRING_TYPE)
      THROW_SYNTAX_ERR("error while parsing loadfile() ns, awaiting a string");

    ns = AS_STRING(v);

    if (this->curState & LITERAL_STRING_STATE)
      this->curState &= ~LITERAL_STRING_STATE;
    else
      ns = String.dup (ns);

    ifnot (ns->num_bytes)
      goto theload;

    la_string x = StringNew (ns->bytes);
    sym_t *symbol = la_lookup_symbol (this, x);
    if (symbol isnot NULL) {
      v = symbol->value;
      if (v.type & UFUNCTION_TYPE) {
        funT *f = AS_FUNC_PTR(v);
        load_ns = f;
        goto theload;
      }

      this->print_fmt_bytes (this->err_fp, "loadfile(): %s is not a namespace\n", ns->bytes);
      err = LA_ERR_SYNTAX;
      goto theend;
    }

    this->print_bytes (this->err_fp, "loadfile(), functionality hasn't been implemented\n");
    err = LA_ERR_SYNTAX;
    goto theend;
  }

theload:
  this->curScope = load_ns;

  ifnot (Path.is_absolute (fname->bytes)) {
    ifnot (Cstring.eq (LA_STRING_NS, AS_STRING_BYTES(this->file->value))) {
      fn = String.dup (fname);
      String.prepend_byte (fn, DIR_SEP);
      char *dname = Path.dirname (AS_STRING_BYTES(this->file->value));
      String.prepend_with (fn, dname);
      free (dname);
      this->curState |= LOADFILE_SILENT;
      err = la_eval_file (this, fn->bytes);
      this->curState &= ~LOADFILE_SILENT;
      if (err isnot LA_ERR_LOAD)
        goto theend;
    }

    this->curState |= LOADFILE_SILENT;
    err = la_eval_file (this, fname->bytes);
    this->curState &= ~LOADFILE_SILENT;
    if (err isnot LA_ERR_LOAD)
      goto theend;

    if (this->la_dir->num_bytes) {
      String.release (fn);
      fn = String.dup (fname);
      String.prepend_with_fmt (fn, "%s/", this->la_dir->bytes);
      this->curState |= LOADFILE_SILENT;
      err = la_eval_file (this, fn->bytes);
      this->curState &= ~LOADFILE_SILENT;
      if (err isnot LA_ERR_LOAD) goto theend;

      String.release (fn);
      fn = String.dup (fname);
      String.prepend_with_fmt (fn, "%s/scripts/", this->la_dir->bytes);
      this->curState |= LOADFILE_SILENT;
      err = la_eval_file (this, fn->bytes);
      this->curState &= ~LOADFILE_SILENT;
      if (err isnot LA_ERR_LOAD) goto theend;
    }

    sym_t *symbol = la_lookup_symbol (this, StringNew ("__loadpath"));
    ArrayType *p_ar = (ArrayType *) AS_ARRAY(symbol->value);
    string **s_ar = (string **) AS_ARRAY(p_ar->value);
    for (size_t i = 0; i < p_ar->len; i++) {
      string *p = s_ar[i];
      ifnot (p->num_bytes) continue;

      String.release (fn);
      fn = String.dup (fname);
      String.prepend_with_fmt (fn, "%s/", p->bytes);
      this->curState |= LOADFILE_SILENT;
      err = la_eval_file (this, fn->bytes);
      this->curState &= ~LOADFILE_SILENT;
      if (err isnot LA_ERR_LOAD)
        goto theend;
    }

    goto theend;
  }

  err = la_eval_file (this, fname->bytes);

theend:
  this->curScope = prev_ns;
  String.release (fname);
  String.release (fn);
  String.release (ns);
  return err;
}

static VALUE la_equals (la_t *this, VALUE x, VALUE y) {
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
          this->CFuncError = LA_ERR_TYPE_MISMATCH;
          Cstring.cp_fmt (this->curMsg, MAXLEN_MSG + 1,
              "NumberType == %s is not possible",
              AS_STRING_BYTES(la_typeAsString (this, y)));
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
          this->CFuncError = LA_ERR_TYPE_MISMATCH;
          Cstring.cp_fmt (this->curMsg, MAXLEN_MSG + 1,
              "IntegerType == %s is not possible",
              AS_STRING_BYTES(la_typeAsString (this, y)));
          return result;
      }
      return result;

    case STRING_TYPE:
      switch (y.type) {
        case STRING_TYPE:
          return INT(Cstring.eq (AS_STRING_BYTES(x), AS_STRING_BYTES(y)));

        case NULL_TYPE: return result;
        default:
          this->CFuncError = LA_ERR_TYPE_MISMATCH;
          Cstring.cp_fmt (this->curMsg, MAXLEN_MSG + 1,
              "StringType == %s is not possible",
              AS_STRING_BYTES(la_typeAsString (this, y)));
          return result;
      }
      return result;

    case ARRAY_TYPE:
      switch (y.type) {
        case ARRAY_TYPE:
          return INT(la_array_eq (x, y));
        case NULL_TYPE: return result;
        default:
          this->CFuncError = LA_ERR_TYPE_MISMATCH;
          Cstring.cp_fmt (this->curMsg, MAXLEN_MSG + 1,
              "ArrayType == %s is not possible",
              AS_STRING_BYTES(la_typeAsString (this, y)));
          return result;
      }
      return result;

    case MAP_TYPE:
      switch (y.type) {
        case NULL_TYPE: return result;
        case MAP_TYPE: {
          Vmap_t *xmap = AS_MAP(x);
          Vmap_t *ymap = AS_MAP(y);
          return INT (xmap == ymap);
        }

        default:
          this->CFuncError = LA_ERR_TYPE_MISMATCH;
          Cstring.cp_fmt (this->curMsg, MAXLEN_MSG + 1,
              "MapType == %s is not possible",
              AS_STRING_BYTES(la_typeAsString (this, y)));
          return result;
      }

    case LIST_TYPE:
      switch (y.type) {
        case NULL_TYPE: return FALSE_VALUE;
        default:
          Cstring.cp_fmt (this->curMsg, MAXLEN_MSG + 1,
              "ListType == %s is not possible",
              AS_STRING_BYTES(la_typeAsString (this, y)));
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
          this->CFuncError = LA_ERR_TYPE_MISMATCH;
          Cstring.cp_fmt (this->curMsg, MAXLEN_MSG + 1,
              "%s == %s is not possible",
              AS_STRING_BYTES(la_typeAsString (this, x)),
              AS_STRING_BYTES(la_typeAsString (this, y)));
          return result;
      }

    default:
      if (x.type > OBJECT_TYPE) {
        switch (y.type) {
          case NULL_TYPE: return result;
          default:
            this->CFuncError = LA_ERR_TYPE_MISMATCH;
            Cstring.cp_fmt (this->curMsg, MAXLEN_MSG + 1,
                "%s == %s is not possible",
                AS_STRING_BYTES(la_typeAsString (this, x)),
                AS_STRING_BYTES(la_typeAsString (this, y)));
            return result;
        }
      }

      this->CFuncError = LA_ERR_TYPE_MISMATCH;
      Cstring.cp_fmt (this->curMsg, MAXLEN_MSG + 1,
         "%s == %s is not possible",
         AS_STRING_BYTES(la_typeAsString (this, x)),
         AS_STRING_BYTES(la_typeAsString (this, y)));
      return result;
  }

  return result;
}

static VALUE la_ne (la_t *this, VALUE x, VALUE y) {
  VALUE result = la_equals (this, x, y);
  return INT(0 == AS_INT(result));
}

static VALUE la_lt (la_t *this, VALUE x, VALUE y) {
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

static VALUE la_le (la_t *this, VALUE x, VALUE y) {
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

static VALUE la_gt (la_t *this, VALUE x, VALUE y) {
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

static VALUE la_ge (la_t *this, VALUE x, VALUE y) {
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

static VALUE la_mod (la_t *this, VALUE x, VALUE y) {
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

static VALUE la_add (la_t *this, VALUE x, VALUE y) {
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
          string *nx = String.new (8);
          if (x_i <= '~' + 1) {
            String.append_byte (nx, x_i);
          } else {
            char buf[8];
            int len;
            Ustring.character (x_i, buf, &len);
            String.append_with_len (nx, buf, len);
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
            String.append_with_len (x_str, y_str->bytes, y_str->num_bytes);
            result = STRING(x_str);
            result.refcount = x.refcount;
          } else {
            string *new = String.new_with_len (x_str->bytes, x_str->num_bytes);
            String.append_with_len (new, y_str->bytes, y_str->num_bytes);
            result = STRING(new);

            if (x.sym is NULL and x.refcount isnot MALLOCED_STRING and
                0 is (this->objectState & (ARRAY_MEMBER|MAP_MEMBER)) and
                0 is x.refcount) {
              string_release (x);
              this->objectState |= LHS_STRING_RELEASED;
            }
          }

          if (y.sym is NULL and y.refcount isnot MALLOCED_STRING and
              0 is (this->objectState & (ARRAY_MEMBER|MAP_MEMBER))) {
            string_release (y);
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
            new = String.new_with_len (x_str->bytes, x_str->num_bytes);

            if (x.sym is NULL and x.refcount isnot MALLOCED_STRING and
                0 is (this->objectState & (ARRAY_MEMBER|MAP_MEMBER))) {
              String.release (x_str);
              this->objectState |= LHS_STRING_RELEASED;
            }
          }

          this->objectState &= ~(ARRAY_MEMBER|MAP_MEMBER);

          integer y_i = AS_INT(y);
          if (y_i <= '~' + 1) {
            String.append_byte (new, y_i);
          } else {
            char buf[8];
            int len;
            Ustring.character (y_i, buf, &len);
            String.append_with_len (new, buf, len);
          }

          result = STRING(new);
          if (new is x_str)
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

static VALUE la_mul (la_t *this, VALUE x, VALUE y) {
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

static VALUE la_div (la_t *this, VALUE x, VALUE y) {
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

static VALUE la_sub (la_t *this, VALUE x, VALUE y) {
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

static VALUE la_bset (la_t *this, VALUE x, VALUE y) {
  (void) this;
  int xx = AS_INT(x); xx |= AS_INT(y); x = INT(xx);
  return x;
}

static VALUE la_bnot (la_t *this, VALUE x, VALUE y) {
  (void) this;
  int xx = AS_INT(x); xx &= AS_INT(y); x = INT(xx);
  return x;
}

static VALUE la_shl (la_t *this, VALUE x, VALUE y) {
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

static VALUE la_shr (la_t *this, VALUE x, VALUE y) {
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

static VALUE la_bitor (la_t *this, VALUE x, VALUE y) {
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

static VALUE la_bitand (la_t *this, VALUE x, VALUE y) {
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

static VALUE la_bitxor (la_t *this, VALUE x, VALUE y) {
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

static VALUE la_logical_and (la_t *this, VALUE x, VALUE y) {
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

static VALUE la_logical_or (la_t *this, VALUE x, VALUE y) {
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

static VALUE la_eval (la_t *this, VALUE v_str) {
  ifnot (IS_STRING(v_str)) C_THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  char *str = AS_STRING_BYTES (v_str);

  sym_t *sym = ns_lookup_symbol (this->std, "__retval");
  sym->value = INT(IS_UNCHANGEABLE);

  int retval = la_eval_string (this, str);

  if (retval < LA_NOTOK)
    C_THROW(retval, "eval() error");

  if (AS_INT(sym->value) isnot IS_UNCHANGEABLE)
    return sym->value;

  return INT(retval);
}

static struct def {
  const char *name;
  int toktype;
  VALUE val;
} la_defs[] = {
  { "var",     TOKEN_VARDEF,   NULL_VALUE },
  { "const",   TOKEN_CONSTDEF, NULL_VALUE },
  { "if",      TOKEN_IF,       PTR(la_parse_if) },
  { "ifnot",   TOKEN_IFNOT,    PTR(la_parse_if) },
  { "else",    TOKEN_ELSE,     NULL_VALUE },
  { "elseif",  TOKEN_ELSEIF,   NULL_VALUE },
  { "orelse",  TOKEN_ORELSE,   NULL_VALUE },
  { "then",    TOKEN_THEN,     NULL_VALUE },
  { "end",     TOKEN_END,      NULL_VALUE },
  { "in",      TOKEN_IN,       NULL_VALUE },
  { "as",      TOKEN_AS,       NULL_VALUE },
  { "times",   TOKEN_TIMES,    NULL_VALUE },
  { "time",    TOKEN_TIMES,    NULL_VALUE },
  { "do",      TOKEN_DO,       PTR(la_parse_do) },
  { "for",     TOKEN_FOR,      PTR(la_parse_for) },
  { "while",   TOKEN_WHILE,    PTR(la_parse_while) },
  { "forever", TOKEN_FOREVER,  PTR(la_parse_forever) },
  { "break",   TOKEN_BREAK,    NULL_VALUE },
  { "continue",TOKEN_CONTINUE, NULL_VALUE },
  { "func",    TOKEN_FUNCDEF,  PTR(la_parse_func_def) },
  { "lambda",  TOKEN_LAMBDA,   NULL_VALUE },
  { "return",  TOKEN_RETURN,   PTR(la_parse_return) },
  { "import",  TOKEN_IMPORT,   PTR(la_parse_import) },
  { "loadfile",TOKEN_LOADFILE, PTR(la_parse_loadfile) },
  { "include", TOKEN_INCLUDE,  PTR(la_parse_loadfile) },
  { "Annotation", TOKEN_ANNOTATION, NULL_VALUE },
  { "evalfile",TOKEN_EVALFILE, NULL_VALUE },
  { "New",     TOKEN_NEW,      NULL_VALUE },
  { "Type",    TOKEN_TYPE,     PTR(la_parse_type) },
  { "override",TOKEN_OVERRIDE, NULL_VALUE },
  { "list",    TOKEN_LIST_FUN, NULL_VALUE },
  { "append",  TOKEN_APPEND,   NULL_VALUE },
  { "print",   TOKEN_PRINT,    PTR(la_parse_print) },
  { "format",  TOKEN_FORMAT,   NULL_VALUE },
  { "println", TOKEN_PRINTLN,  PTR(la_parse_println) },
  { "exit",    TOKEN_EXIT,     PTR(la_parse_exit) },
  { "public",  TOKEN_PUBLIC,   PTR(la_parse_visibility) },
  { "private", TOKEN_PRIVATE,  PTR(la_parse_visibility) },
  { "*",       BINOP(1),       PTR(la_mul) },
  { "/",       BINOP(1),       PTR(la_div) },
  { "%",       BINOP(1),       PTR(la_mod) },
  { "+",       BINOP(2),       PTR(la_add) },
  { "-",       BINOP(2),       PTR(la_sub) },
  { "&",       BINOP(3),       PTR(la_bitand) },
  { "|",       BINOP(3),       PTR(la_bitor) },
  { "^",       BINOP(3),       PTR(la_bitxor) },
  { ">>",      BINOP(3),       PTR(la_shr) },
  { "<<",      BINOP(3),       PTR(la_shl) },
  { "==",      BINOP(4),       PTR(la_equals) },
  { "is",      BINOP(4),       PTR(la_equals) },
  { "!=",      BINOP(4),       PTR(la_ne) },
  { "isnot",   BINOP(4),       PTR(la_ne) },
  { "<",       BINOP(4),       PTR(la_lt) },
  { "<=",      BINOP(4),       PTR(la_le) },
  { ">",       BINOP(4),       PTR(la_gt) },
  { ">=",      BINOP(4),       PTR(la_ge) },
  { "&&",      BINOP(5),       PTR(la_logical_and) },
  { "and",     BINOP(5),       PTR(la_logical_and) },
  { "||",      BINOP(5),       PTR(la_logical_or) },
  { "or",      BINOP(5),       PTR(la_logical_or) },
  { "=",       TOKEN_ASSIGN,      NULL_VALUE },
  { "+=",      TOKEN_ASSIGN_APP,  NULL_VALUE },
  { "-=",      TOKEN_ASSIGN_SUB,  NULL_VALUE },
  { "/=",      TOKEN_ASSIGN_DIV,  NULL_VALUE },
  { "*=",      TOKEN_ASSIGN_MUL,  NULL_VALUE },
  { "&=",      TOKEN_ASSIGN_AND,  NULL_VALUE },
  { "|=",      TOKEN_ASSIGN_BAR,  NULL_VALUE },
  { "^=",      TOKEN_ASSIGN_XOR,  NULL_VALUE },
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
} la_def_datatypes[] = {
  { "map",     MAP_TYPE,     PTR(la_parse_array_def) },
  { "array",   ARRAY_TYPE,   PTR(la_parse_array_def) },
  { "string",  STRING_TYPE,  PTR(la_parse_array_def) },
  { "number",  NUMBER_TYPE,  PTR(la_parse_array_def) },
  { "integer", INTEGER_TYPE, PTR(la_parse_array_def) },
  { NULL,      NULL_TYPE,    NULL_VALUE }
};

LaDefCFun la_funs[] = {
  { "len",              PTR(la_len), 1},
  { "eval",             PTR(la_eval), 1},
  { "fopen",            PTR(la_fopen), 2},
  { "fflush",           PTR(la_fflush), 1},
  { "fclose",           PTR(la_fclose), 1},
  { "fileno",           PTR(la_fileno), 1},
  { "set_errno",        PTR(la_set_errno), 1},
  { "errno_name",       PTR(la_errno_name), 1},
  { "errno_string",     PTR(la_errno_string), 1},
  { "annotate_get",     PTR(la_annotate_get), 1},
  { "typeof",           PTR(la_typeof), 1},
  { "typeAsString",     PTR(la_typeAsString), 1},
  { "typeofArray",      PTR(la_typeofArray), 1},
  { "typeArrayAsString",PTR(la_typeArrayAsString), 1},
  { "qualifier",        PTR(la_qualifier), 2},
  { "qualifiers",       PTR(la_qualifiers), 0},
  { "qualifier_exists", PTR(la_qualifier_exists), 1},
  { "list_pop_at",      PTR(list_pop_at), 2},
  { "list_append",      PTR(list_append), 2},
  { "list_prepend",     PTR(list_prepend), 2},
  { "list_delete_at",   PTR(list_delete_at), 2},
  { "list_insert_at",   PTR(list_insert_at), 3},
  { "list_clear",       PTR(list_clear), 1},
  { NULL,               NULL_VALUE, NULL_TYPE},
};

static int la_def_std (la_t *this, const char *name, int type, VALUE v, int is_const) {
  ifnot (is_const) {
    sym_t *sym = la_define_symbol (this, this->std, name, type, v, 0);
    return (sym is NULL ? LA_NOTOK : LA_OK);
  }

  if (LA_NOTOK is la_define (this, name, type, v))
    if (is_const > 0)  // could be already redefined (happens with S_I* at file/io)
      return LA_NOTOK; // no other error could happen i think

  return LA_OK;
}

static int la_std_def (la_t *this, la_opts opts) {
  VALUE v = OBJECT(opts.out_fp);
  object *o = la_object_new (NULL, NULL, "FilePtrType", v);
  v = FILEPTR(o);
  v.refcount = UNDELETABLE;
  int err = la_define (this, "stdout", FILEPTR_TYPE, v);
  if (err) return LA_NOTOK;

  v = OBJECT(opts.err_fp);
  o = la_object_new (NULL, NULL, "FilePtrType", v);
  v = FILEPTR(o);
  v.refcount = UNDELETABLE;
  err = la_define (this, "stderr", FILEPTR_TYPE, v);
  if (err) return LA_NOTOK;

  v = OBJECT(stdin);
  o = la_object_new (NULL, NULL, "FilePtrType", v);
  v = FILEPTR(o);
  v.refcount = UNDELETABLE;
  err = la_define (this, "stdin", FILEPTR_TYPE, v);
  if (err) return LA_NOTOK;

  v = OBJECT(0);
  o = la_object_new (NULL, NULL, "FdType", v);
  v = FILEDES(o);
  v.refcount = UNDELETABLE;
  err = la_define (this, "STDIN_FILENO", FD_TYPE, v);
  if (err) return LA_NOTOK;

  v = OBJECT(1);
  o = la_object_new (NULL, NULL, "FdType", v);
  v = FILEDES(o);
  v.refcount = UNDELETABLE;
  err = la_define (this, "STDOUT_FILENO", FD_TYPE, v);
  if (err) return LA_NOTOK;

  v = OBJECT(2);
  o = la_object_new (NULL, NULL, "FdType", v);
  v = FILEDES(o);
  v.refcount = UNDELETABLE;
  err = la_define (this, "STDERR_FILENO", FD_TYPE, v);
  if (err) return LA_NOTOK;

  v = INT(opts.argc);
  err = la_define (this, "__argc", INTEGER_TYPE, v);
  if (err) return LA_NOTOK;

  ArrayType *array = ARRAY_NEW(STRING_TYPE, opts.argc);
  string **ar = (string **) AS_ARRAY(array->value);

  for (integer i = 0; i < opts.argc; i++)
    String.replace_with (ar[i], opts.argv[i]);

  v = ARRAY(array);
  err = la_define (this, "__argv", ARRAY_TYPE, v);
  if (err) return LA_NOTOK;

  string *file = String.new_with (LA_STRING_NS);
  v = STRING(file);
  this->file = la_define_symbol (this, this->std, "__file__", STRING_TYPE, v, 0);
  if (NULL is this->file) return LA_NOTOK;

  string *func = String.new_with (this->curScope->funname);
  v = STRING(func);
  this->func = la_define_symbol (this, this->std, "__func__", STRING_TYPE, v, 0);
  if (NULL is this->func) return LA_NOTOK;

  int len = 2;
  if (this->la_dir->num_bytes) len++;
  #ifdef ZLIBDIR
    len++;
  #endif
  #ifdef LIBDIR
    len++;
  #endif

  ArrayType *imp_path = ARRAY_NEW(STRING_TYPE, len);
  string **arimp = (string **) AS_ARRAY(imp_path->value);
  int ind = 0;
  if (this->la_dir->num_bytes)
    String.replace_with_fmt (arimp[ind++], "%s/la-modules", this->la_dir->bytes);
  #ifdef ZLIBDIR
    String.replace_with_fmt (arimp[ind++], "%s/la-modules", ZLIBDIR);
  #endif
  #ifdef LIBDIR
    String.replace_with_fmt (arimp[ind++], "%s/la-modules", LIBDIR);
  #endif
  String.replace_with (arimp[ind++], "/lib/la-modules");
  String.replace_with (arimp[ind], "/lib/z/la-modules");

  v = ARRAY(imp_path);
  err = la_define (this, "__importpath", ARRAY_TYPE, v);
  if (err) return LA_NOTOK;

  len = 2;
  if (this->la_dir->num_bytes)
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
  if (this->la_dir->num_bytes) {
    String.replace_with (arload[ind++], this->la_dir->bytes);
    String.replace_with_fmt (arload[ind++], "%s/scripts", this->la_dir->bytes);
  }
  #ifdef ZLIBDIR
    String.replace_with_fmt (arload[ind++], "%s/la-lib", ZLIBDIR);
  #endif
  #ifdef LIBDIR
    String.replace_with_fmt (arload[ind++], "%s/la-lib", LIBDIR);
  #endif
  String.replace_with (arload[ind++], "/lib/la-lib");
  String.replace_with (arload[ind], "/lib/z/la-lib");

  v = ARRAY(load_path);
  err = la_define (this, "__loadpath", ARRAY_TYPE, v);
  if (err) return LA_NOTOK;

  v = INT(0);
  err = la_define (this, "errno", INTEGER_TYPE, v);
  if (err) return LA_NOTOK;

  v = INT(0);
  if (NULL is la_define_symbol (this, this->std, "__retval", INTEGER_TYPE, v, 0))
    return LA_NOTOK;

  funT *uf = Fun_new (this, funNew (
    .name = "self", .namelen = 4, .parent = this->function, .nargs = 0));

  v = PTR(uf);
  if (NULL is la_define_symbol (this, this->function, "self", (UFUNC_TYPE | (0 << 8)), v, 0))
    return LA_NOTOK;

  return err;
}

/* ABSTRACTION CODE */

static int la_print_bytes (FILE *fp, const char *bytes) {
  if (NULL is bytes) return 0;
  int nbytes = fprintf (fp, "%s", bytes);
  fflush (fp);
  return nbytes;
}

static int la_print_fmt_bytes (FILE *fp, const char *fmt, ...) {
  size_t len = VA_ARGS_FMT_SIZE (fmt);
  char bytes[len + 1];
  VA_ARGS_GET_FMT_STR(bytes, len, fmt);
  return la_print_bytes (fp, bytes);
}

static int la_print_byte (FILE *fp, int c) {
  return la_print_fmt_bytes (fp, "%c", c);
}

static VALUE I_print_bytes (la_t *this, const char *bytes) {
  VALUE result = INT(this->print_bytes (this->out_fp, bytes));
  return result;
}

static VALUE I_print_byte (la_t *this, char byte) {
  VALUE result = INT(this->print_byte (this->out_fp, byte));
  return result;
}

static int la_eval_string (la_t *this, const char *buf) {
  const char *prev_buffer = this->script_buffer;
  this->script_buffer = buf;

  la_string x = StringNew (buf);

  string *file = AS_STRING(this->file->value);
  integer len =  file->num_bytes;
  char prev_file[len + 1];
  Cstring.cp (prev_file, len + 1, file->bytes, len);
  String.replace_with (file, LA_STRING_NS);

  int retval = la_parse_string (this, x);

  String.replace_with_len (file, prev_file, len);

  this->script_buffer = prev_buffer;

  if (retval is LA_ERR_EXIT) {
    this->didExit = 1;
    return this->exitValue;
  }

  if (retval < LA_NOTOK)
    PRINT_ERR_CONSTANT(retval);

  return retval;
}

static int la_eval_expr (la_t *this, const char *buf, VALUE *v) {
  if (*buf isnot TOKEN_PAREN_OPEN)
    THROW_SYNTAX_ERR("awaiting (");

  this->script_buffer = buf;
  la_string x = StringNew (buf);

  PARSEPTR = x;
  TOKEN = TOKEN_PAREN_OPEN;

  return la_parse_expr (this, v);
}

static int la_eval_file (la_t *this, const char *filename) {
  char fn[PATH_MAX + 1];
  if (NULL is Path.real (filename, fn)) {
    ifnot (this->curState & LOADFILE_SILENT)
      this->print_fmt_bytes (this->err_fp, "%s\n", Error.errno_string (errno));
    return LA_ERR_LOAD;
  }

  ifnot (File.exists (fn)) {
    ifnot (this->curState & LOADFILE_SILENT)
      this->print_fmt_bytes (this->err_fp, "%s: doesn't exists\n", filename);
    return LA_ERR_LOAD;
  }

  int exists = Vmap.key_exists (this->units, fn);
  if (exists and 0 is (this->funcState & EVAL_UNIT_STATE))
    return LA_OK;

  FILE *fp = fopen (fn, "r");
  if (NULL is fp) {
    this->print_fmt_bytes (this->err_fp, "%s\n", Error.errno_string (errno));
    return LA_NOTOK;
  }

  if (-1 is fseek (fp, 0, SEEK_END)) {
    fclose (fp);
    this->print_fmt_bytes (this->err_fp, "%s\n", Error.errno_string (errno));
    return LA_NOTOK;
  }

  long n = ftell (fp);

  if (-1 is n) {
    fclose (fp);
    this->print_fmt_bytes (this->err_fp, "%s\n", Error.errno_string (errno));
    return LA_NOTOK;
  }

  if (-1 is fseek (fp, 0, SEEK_SET)) {
    fclose (fp);
    this->print_fmt_bytes (this->err_fp, "%s\n", Error.errno_string (errno));
    return LA_NOTOK;
  }

  char script[n + 1];
  int r = fread (script, 1, n, fp);
  fclose (fp);

  if (r <= 0) {
    this->print_bytes (this->err_fp, "Couldn't read script\n");
    return LA_NOTOK;
  }

  if (r > n) {
    this->print_bytes (this->err_fp, "race condition, aborts now\n");
    return LA_NOTOK;
  }

  script[r] = '\0';

  if (this->funcState & EVAL_UNIT_STATE) {
    Cstring.cp_fmt (this->curFunName, MAXLEN_SYMBOL + 1, NS_ANON "_%zd", this->anon_id++);
    size_t flen = bytelen (this->curFunName);
    funT *uf = Fun_new (this, funNew (
      .name = this->curFunName, .namelen = flen, .parent = this->curScope));

    string *script_buf;
    if (exists) {
      void *fn_item = Vmap.get (this->units, fn);
      string *str = (string *) fn_item;
      String.replace_with_len (str, script, r);
      script_buf = str;
    } else {
      script_buf = String.new_with_len (script, r);
      Vmap.set (this->units, fn, script_buf, la_release_unit, 0);
    }

    uf->body = StringNew (script_buf->bytes);
    uf->nargs = 0;
    this->curFunDef = uf;
    String.replace_with (this->evalFile, fn);
    return LA_OK;
  }

  string *script_buf = String.new_with_len (script, r);
  Vmap.set (this->units, fn, script_buf, la_release_unit, 0);

  const char *prev_buffer = this->script_buffer;
  this->script_buffer = script_buf->bytes;

  string *file = AS_STRING(this->file->value);
  integer len =  file->num_bytes;
  char prev_file[len + 1];
  Cstring.cp (prev_file, len + 1, file->bytes, len);
  String.replace_with (file, fn);

  la_string x = StringNew (script_buf->bytes);

  int retval = la_parse_string (this, x);

  String.replace_with_len (file, prev_file, len);

  this->script_buffer = prev_buffer;

  if (retval is LA_ERR_EXIT or retval >= LA_NOTOK)
    return retval;

  PRINT_ERR_CONSTANT(retval);

  return retval;
}

static int la_define_funs_default_cb (la_t *this) {
  (void) this;
  return LA_OK;
}

static la_opts la_default_options (la_t *this, la_opts opts) {
  (void) this;
  if (NULL is opts.print_bytes)
    opts.print_bytes = la_print_bytes;

  if (NULL is opts.print_byte)
    opts.print_byte = la_print_byte;

  if (NULL is opts.print_fmt_bytes)
    opts.print_fmt_bytes = la_print_fmt_bytes;

  if (NULL is opts.syntax_error)
    opts.syntax_error = la_syntax_error;

  if (NULL is opts.err_fp)
    opts.err_fp = stderr;

  if (NULL is opts.define_funs_cb)
    opts.define_funs_cb = la_define_funs_default_cb;

  return opts;
}

static void la_release_stdns (la_t *this) {
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

static void la_release (la_t **thisp) {
  if (NULL is *thisp) return;
  la_t *this = *thisp;

  String.release (this->la_dir);
  String.release (this->message);
  String.release (this->evalFile);
  Imap.release   (this->funRefcount);
  Vmap.release   (this->units);
  Vmap.release   (this->types);

  fun_release (&this->function);
  fun_release (&this->datatypes);
  fun_release (&this->macros);

  la_release_stdns (this);
  la_release_qualifiers (this);

  free (this);
  *thisp = NULL;
}

static void la_release_instance (la_t **thisp) {
  la_release (thisp);
}

static la_t *la_new (la_T *__la__) {
  la_t *this = Alloc (sizeof (la_t));
  this->prop = __la__->prop;
  return this;
}

static char *la_name_gen (char *name, int *name_gen, const char *prefix, size_t prelen) {
  size_t num = (*name_gen / 26) + prelen;
  size_t i = 0;
  for (; i < prelen; i++) name[i] = prefix[i];
  for (; i < num; i++) name[i] = 'a' + ((*name_gen)++ % 26);
  name[num] = '\0';
  return name;
}

static void la_remove_instance (la_T *this, la_t *instance) {
  la_t *it = $my(head);
  la_t *prev = NULL;

  int idx = 0;
  while (it isnot instance) {
    prev = it;
    idx++;
    it = it->next;
  }

  if (it is NULL) return;
  if (idx >= $my(current_idx)) $my(current_idx)--;
  $my(num_instances)--;

  ifnot ($my(num_instances)) {
    $my(head) = NULL;
    goto theend;
  }

  if (1 is $my(num_instances)) {
    if (it->next is NULL) {
      $my(head) = prev;
      $my(head)->next = NULL;
    } else
      $my(head) = it->next;
    goto theend;
  }

  prev->next = it->next;

theend:
  la_release_instance (&it);
}

static la_t *la_append_instance (la_T *this, la_t *instance) {
  instance->next = NULL;
  $my(current_idx) = $my(num_instances);
  $my(num_instances)++;

  if (NULL is $my(head))
    $my(head) = instance;
  else {
    la_t *it = $my(head);
    while (it) {
      if (it->next is NULL) break;
      it = it->next;
    }

    it->next = instance;
  }

  return instance;
}

static la_t *la_set_current (la_T *this, int idx) {
  if (idx >= $my(num_instances)) return NULL;
  la_t *it = $my(head);
  int i = 0;
  while (i++ < idx) it = it->next;

  return it;
}

static void la_set_CFuncError (la_t *this, int err) {
  this->CFuncError = err;
}

static void la_set_curMsg (la_t *this, const char *msg) {
  Cstring.cp (this->curMsg, MAXLEN_MSG + 1, msg, bytelen (msg));
}

static void la_set_Errno (la_t *this, int err) {
  this->Errno = err;
  sym_t *sym = ns_lookup_symbol (this->std, "errno");
  sym->value = INT (err);
}

static VALUE la_set_errno (la_t *this, VALUE v_err) {
  ifnot (IS_INT(v_err)) C_THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer");

  la_set_Errno (this, AS_INT(v_err));
  return v_err;
}

static void la_set_la_dir (la_t *this, const char *fn) {
  if (NULL is fn) return;
  size_t len = bytelen (fn);
  String.replace_with_len (this->la_dir, fn, len);
}

static void la_set_define_funs_cb (la_t *this, LaDefineFuns_cb cb) {
  this->define_funs_cb = cb;
}

static void la_set_user_data (la_t *this, void *user_data) {
  this->user_data = user_data;
}

static void *la_get_user_data (la_t *this) {
  return this->user_data;
}

static char *la_get_message (la_t *this) {
  return this->message->bytes;
}

static int la_get_didExit (la_t *this) {
  return this->didExit;
}

public la_T *la_get_root (la_t *this) {
  return this->root;
}

static char *la_get_eval_str (la_t *this) {
  return (char *) GETSTRPTR(PARSEPTR);
}

static la_t *la_get_current (la_T *this) {
  la_t *it = $my(head);
  int i = 0;
  while (i++ < $my(current_idx)) it = it->next;

  return it;
}

static int la_get_current_idx (la_T *this) {
  return $my(current_idx);
}

static int la_init (la_T *interp, la_t *this, la_opts opts) {
  int i;
  int err = 0;

  if (NULL is opts.name)
    la_name_gen (this->name, &$my(name_gen), "lai:", 2);
  else
    Cstring.cp (this->name, 32, opts.name, 31);

  opts = la_default_options (this, opts);

  this->print_byte = opts.print_byte;
  this->print_fmt_bytes = opts.print_fmt_bytes;
  this->print_bytes = opts.print_bytes;
  this->syntax_error = opts.syntax_error;
  this->err_fp = opts.err_fp;
  this->out_fp = opts.out_fp;
  this->user_data = opts.user_data;
  this->define_funs_cb = opts.define_funs_cb;

  this->didExit = 0;
  this->hasToReturn = 0;
  this->exitValue = LA_OK;
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

  if (NULL is opts.la_dir) {
    char *ddir = getenv ("DATADIR");
    ifnot (NULL is ddir) {
      this->la_dir = String.new_with (ddir);
      String.trim_end (this->la_dir, '\\');
      String.append_with_len (this->la_dir, "/la", 3);
      if (NOTOK is Dir.make_parents (this->la_dir->bytes, S_IRWXU, DirOpts())) {
        this->print_fmt_bytes (this->err_fp, "%s\n", Error.errno_string (errno));
        return LA_NOTOK;
      }
    } else {
#ifdef DATADIR
      this->la_dir = String.new_with (DATADIR);
      String.trim_end (this->la_dir, '\\');
      String.append_with_len (this->la_dir, "/la", 3);
      if (NOTOK is Dir.make_parents (this->la_dir->bytes, S_IRWXU, DirOpts())) {
        this->print_fmt_bytes (this->err_fp, "%s\n", Error.errno_string (errno));
        return LA_NOTOK;
      }
#else
      this->la_dir = String.new (32);
#endif
    }
  }
  else
    this->la_dir = String.new_with (opts.la_dir);

  this->message = String.new (32);

  this->std = fun_new (
      funNew (.name = NS_STD, .namelen = NS_STD_LEN, .num_symbols = 256));

  this->datatypes = fun_new (
      funNew (.name = "__types__", .namelen = 9, .num_symbols = 256));

  this->macros = fun_new (
      funNew (.name = "__macros__", .namelen = 10, .num_symbols = 256));

  Fun_new (this,
      funNew (.name = NS_GLOBAL, .namelen = NS_GLOBAL_LEN, .num_symbols = 256));

  for (i = 0; la_defs[i].name; i++) {
    err = la_define (this, la_defs[i].name, la_defs[i].toktype, la_defs[i].val);

    if (err isnot LA_OK) {
      la_release (&this);
      return err;
    }
  }

  for (i = 0; la_def_datatypes[i].name; i++) {
    err = la_define_type (this, la_def_datatypes[i].name, la_def_datatypes[i].toktype, la_def_datatypes[i].val);

    if (err isnot LA_OK) {
      la_release (&this);
      return err;
    }
  }

  for (i = 0; la_funs[i].name; i++) {
    err = la_define (this, la_funs[i].name, CFUNC(la_funs[i].nargs), la_funs[i].val);

    if (err isnot LA_OK) {
      la_release (&this);
      return err;
    }
  }

  err = la_std_def (this, opts);
  if (err isnot LA_OK) {
    la_release (&this);
    return err;
  }

  if (LA_OK isnot opts.define_funs_cb (this)) {
    la_release (&this);
    return err;
  }

  this->funRefcount = Imap.new (256);
  this->units = Vmap.new (32);
  this->types = Vmap.new (32);
  this->qualifiers = NULL;
  this->evalFile = String.new (128);

  la_append_instance (interp, this);

  return LA_OK;
}

static la_t *la_init_instance (la_T *__la__, la_opts opts) {
  la_t *this = la_new (__la__);

  la_init (__la__, this, opts);

  this->root = __la__;

  return this;
}

static int la_loadfile (la_t *this, const char *fn) {
  int err = la_eval_file (this, fn);
  if (err is LA_ERR_EXIT)
    return this->exitValue;
  return err;
}

static int la_load_file (la_T *__la__, la_t *this, const char *fn) {
  if (this is NULL)
    this = la_init_instance (__la__, LaOpts());

  ifnot (Path.is_absolute (fn)) {
    if (File.exists (fn) and File.is_reg (fn))
      return la_loadfile (this, fn);

    size_t fnlen = bytelen (fn);
    char fname[fnlen+5];
    Cstring.cp (fname, fnlen + 1, fn, fnlen);

    char *extname = Path.extname (fname);
    size_t extlen = bytelen (extname);

    if (0 is extlen or 0 is Cstring.eq ("." LA_EXTENSION, extname)) {
      fname[fnlen] = '.'; fname[fnlen+1] = 'l'; fname[fnlen+2] = 'a';
      fname[fnlen+3] = 'i'; fname[fnlen+4] = '\0';
      if (File.exists (fname))
        return la_loadfile (this, fname);

      fname[fnlen] = '\0';
    }

    ifnot (this->la_dir->num_bytes) {
      la_set_message_fmt (this, 0, "%s: couldn't locate script", fn);
      return LA_NOTOK;
    }

    string *ddir = this->la_dir;
    size_t len = ddir->num_bytes + bytelen (fname) + 2 + 7;
    char tmp[len + 5];
    Cstring.cp_fmt (tmp, len + 1, "%s/scripts/%s", ddir->bytes, fname);

    if (0 is File.exists (tmp) or 0 is File.is_reg (tmp)) {
      tmp[len] = '.'; tmp[len+1] = 'l'; tmp[len+2] = 'a';
      tmp[len+3] = 'i'; tmp[len+4] = '\0';
    }

    ifnot (File.exists (tmp)) {
      la_set_message_fmt (this, 0, "%s: couldn't locate script", fn);
      return LA_NOTOK;
    }

    return la_loadfile (this, tmp);
  }

  ifnot (File.exists (fn)) {
    la_set_message_fmt (this, 0, "%s: couldn't locate script", fn);
    return LA_NOTOK;
  }

  return la_loadfile (this, fn);
}

public la_T *__init_la__ (void) {
  __INIT__ (io);
  __INIT__ (dir);
  __INIT__ (path);
  __INIT__ (file);
  __INIT__ (vmap);
  __INIT__ (imap);
  __INIT__ (error);
  __INIT__ (string);
  __INIT__ (cstring);
  __INIT__ (vstring);
  __INIT__ (ustring);

  la_T *this =  Alloc (sizeof (la_T));
  $myprop = Alloc (sizeof (la_prop));

  *this = (la_T) {
    .self = (la_self) {
      .new = la_new,
      .init = la_init,
      .def =  la_define,
      .def_std = la_def_std,
      .release = la_release,
      .eval_file = la_eval_file,
      .eval_expr = la_eval_expr,
      .load_file = la_load_file,
      .print_byte = I_print_byte,
      .print_bytes = I_print_bytes,
      .eval_string =  la_eval_string,
      .init_instance = la_init_instance,
      .remove_instance = la_remove_instance,
      .append_instance = la_append_instance,
      .qualifier_exists = la_C_qualifier_exists,
      .get = (la_get_self) {
        .root = la_get_root,
        .message = la_get_message,
        .current = la_get_current,
        .didExit = la_get_didExit,
        .eval_str = la_get_eval_str,
        .user_data = la_get_user_data,
        .qualifier = la_get_qualifier,
        .qualifiers = la_get_qualifiers,
        .current_idx = la_get_current_idx
      },
      .set = (la_set_self) {
        .Errno = la_set_Errno,
        .la_dir = la_set_la_dir,
        .curMsg = la_set_curMsg,
        .current = la_set_current,
        .user_data = la_set_user_data,
        .CFuncError = la_set_CFuncError,
        .qualifiers = la_set_qualifiers,
        .define_funs_cb = la_set_define_funs_cb
      },
      .map = (la_map_self) {
        .set_value = la_map_set_value,
        .reset_value = la_map_reset_value,
        .release_value = la_map_release_value
      },
      .object = (la_object_self) {
        .new = la_object_new
      }
    },
    .prop = $myprop,
  };

  $my(name_gen) = ('z' - 'a') + 1;

  $my(head) = NULL;
  $my(num_instances) = 0;
  $my(current_idx) = -1;

  return this;
}

public void __deinit_la__ (la_T **thisp) {
  if (NULL is *thisp) return;
  la_T *this = *thisp;

  la_t *it = $my(head);
  while (it) {
    la_t *tmp = it->next;
    la_release_instance (&it);
    it = tmp;
  }

  free ($myprop);
  free (this);
  *thisp = NULL;
}
