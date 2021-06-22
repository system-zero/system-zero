/* Derived from the Tinyscript project at:
 * https://github.com/totalspectrum/ (see LICENSE included in this directory)
 *
 * See data/docs/la.md and data/tests/la-semantics.i
 * for details about syntax and semantics.
 */

 /* this code is based on expectations and|or on non unpredictable cases,
  * and established in our brain interfaces; and there is nothing (not even
  * close) like C and|or a C modern library that supports all the standards,
  * and of course nothing like a generous and blessed by time and wisdom,
  * UNIX like environment, that respects and offers a POSIX interface, even if
  * in specific cases offers a saner interface than the standard one (and which
  * of course if it proves usefull and sane, can be then blessed by POSIX (as
  * this is the way it works and brings expectativity (if such a word))) */

 /* also it doesn't care much of how it performs, rather than to realize and
  * implement the needed syntax and set the semantics, perhaps with an (sometimes)
  * unorthodoxical way (compared with established and common by now techniques
  * by most (if all) modern programming languages (no, its not that thing)),
  * and while it borrowed the machinery of TinyScript (a great tiny code) (
  * and still uses these small bit details abouts bits and bytes from them
  * (which it makes a difference to get them exact right (like shift right or
  * shift left (i do not have a single hope to work with them always perferct
  * in practise (i always forget those really true realizations, but always
  * forget the exact realization which is important to have to be confident
  * that what you want is what you write (perhaps because i really never had
  * any proper (normal) education, (probably because and of my own absense (
  * okey..., following an instict of an unconscious human child, that really
  * had no control on dreams) to participate in a system that you didn't really
  * believe that it was the best we could (not even close), to provide with
  * fresh energy the continuation of a gained knowledge, with contributions
  * to stabilization, refiniment and evolution, of humans that love to do what
  * they do, of that knowledge))))))), and which it wasn't meant to support a
  * complicated environment other that for what it was made for (basic syntax
  * enough to develop logic to work with integers (not even strings or arrays)) */
 /* it was really because i wanted to implement that thing, to extend this system
  * with scripting cababilities, that will first emulate and the C style way of
  * this project (so to satisfy expectations and instant familiarity), but with
  * a proud way (enough datatypes, automatic memory managment, common consepts,
  * ridiculous easy to understant syntax (to at least realize fast the intentions
  * of the reading code, and ideally to write within a few explanations)), and
  * so there isn't a question if it could be implemented better (some thousands
  * of times better!!! (a proper lexer, parser, compiler)), but what it does?
  * It seems that it does what it does want to do, or it is close, albeit it
  * might does it with an unorthodoxical way */

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

/* IGNORE FOR NOW (it doesn't fit really here, though it is a core thing) */
#define REQUIRE_TERMIOS
#define REQUIRE_TERM_TYPE    DECLARE
#define REQUIRE_TERM_MACROS
#define REQUIRE_KEYS_MACROS

#include <z/cenv.h>

#define $myprop      this->prop
#define $my(__v__)   $myprop->__v__

#define NS_GLOBAL          "global"
#define NS_GLOBAL_LEN      6
#define NS_STD             "std"
#define NS_STD_LEN         3
#define NS_BLOCK           "__block__"
#define NS_BLOCK_LEN       9
#define NS_ANON            "anonymous"
#define LA_EXTENSION       "lai"
#define LA_STRING_NS       "__string__"

#define STRING_LITERAL_ARG_STATE      (1 << 0)
#define LOOP_STATE                    (1 << 1)
#define BREAK_STATE                   (1 << 2)
#define CONTINUE_STATE                (1 << 3)
#define LITERAL_STRING_STATE          (1 << 4)
#define FUNC_CALL_RESULT_IS_MMT       (1 << 5)
#define LOADFILE_SILENT               (1 << 6)
#define FORCE_LOADFILE                (1 << 7)
#define INDEX_STATE                   (1 << 8)
#define MAP_STATE                     (1 << 9)

#define EXPR_LIST_STATE               (1 << 0)
#define MAP_METHOD_STATE              (1 << 1)

#define FMT_LITERAL                   (1 << 0)

#define OBJECT_APPEND                 (1 << 0)
#define IDENT_LEAD_CHAR_CAN_BE_DIGIT  (1 << 1)
#define MMT_OBJECT                    (1 << 2)
#define ASSIGNMENT_STATE              (1 << 3)
#define OBJECT_MMT_REASSIGN           (1 << 4)
#define ARRAY_MEMBER                  (1 << 5)

#define PRIVATE_SCOPE                 (1 << 0)
#define PUBLIC_SCOPE                  (1 << 1)

#define BINOP(x) (((x) << 8) + BINOP_TYPE)
#define CFUNC(x) (((x) << 8) + CFUNC_TYPE)

#define CFUNC_TYPE          'B'
#define UFUNC_TYPE          'f'
#define BINOP_TYPE          'o'

#define LA_TOKEN_SYMBOL     'A'
#define LA_TOKEN_BUILTIN    'B'
#define LA_TOKEN_CHAR       'C'
#define LA_TOKEN_DO         'D'
#define LA_TOKEN_ELSEIF     'E'
#define LA_TOKEN_FUNCDEF    'F'
#define LA_TOKEN_IFNOT      'I'
#define LA_TOKEN_LOOP       'L'
#define LA_TOKEN_MAP        'M'
#define LA_TOKEN_CONTINUE   'N'
#define LA_TOKEN_OPERATOR   'O'
#define LA_TOKEN_PRINT      'P'
#define LA_TOKEN_RETURN     'R'
#define LA_TOKEN_STRING     'S'
#define LA_TOKEN_VARDEF     'V'
#define LA_TOKEN_EXIT       'X'
#define LA_TOKEN_ARYDEF     'Y'
#define LA_TOKEN_SYNTAX_ERR 'Z'
#define LA_TOKEN_BREAK      'b'
#define LA_TOKEN_CONSTDEF   'c'
#define LA_TOKEN_DOUBLE     'd'
#define LA_TOKEN_ELSE       'e'
#define LA_TOKEN_USRFUNC    'f'
#define LA_TOKEN_PUBLIC     'g'
#define LA_TOKEN_PRIVATE    'h'
#define LA_TOKEN_IF         'i'
#define LA_TOKEN_FOR        'l'
#define LA_TOKEN_FOREVER    'm'
#define LA_TOKEN_NUMBER     'n'
#define LA_TOKEN_BINOP      'o'
#define LA_TOKEN_PRINTLN    'p'
#define LA_TOKEN_LOADFILE   'r'
#define LA_TOKEN_IMPORT     's'
#define LA_TOKEN_VAR        'v'
#define LA_TOKEN_WHILE      'w'
#define LA_TOKEN_HEX_NUMBER 'x'
#define LA_TOKEN_ARRAY      'y'
#define LA_TOKEN_BLOCK      'z'
#define LA_TOKEN_INDEX_OPEN '['
#define LA_TOKEN_INDEX_CLOS ']'
#define LA_TOKEN_PAREN_OPEN '('
#define LA_TOKEN_PAREN_CLOS ')'
#define LA_TOKEN_BLOCK_OPEN '{'
#define LA_TOKEN_MAP_OPEN   LA_TOKEN_BLOCK_OPEN
#define LA_TOKEN_BLOCK_CLOS '}'
#define LA_TOKEN_MAP_CLOS   LA_TOKEN_BLOCK_CLOS
#define LA_TOKEN_SEMICOLON  ';'
#define LA_TOKEN_COLON      ':'
#define LA_TOKEN_DOT        '.'
#define LA_TOKEN_COMMA      ','
#define LA_TOKEN_COMMENT    '#'
#define LA_TOKEN_UNARY      '~'
#define LA_TOKEN_DQUOTE     '"'
#define LA_TOKEN_SQUOTE     '\''
#define LA_TOKEN_STAR       '*'
#define LA_TOKEN_NL         '\n'
#define LA_TOKEN_SLASH      '\\'
#define LA_TOKEN_ESCAPE_CHR LA_TOKEN_SLASH
//#define LA_TOKEN_NULL       '0'
#define LA_TOKEN_ASSIGN      1000
#define LA_TOKEN_ASSIGN_APP  1001
#define LA_TOKEN_ASSIGN_SUB  1002
#define LA_TOKEN_ASSIGN_DIV  1003
#define LA_TOKEN_ASSIGN_MUL  1004
#define LA_TOKEN_ASSIGN_MOD  1005
#define LA_TOKEN_ASSIGN_BAR  1006
#define LA_TOKEN_ASSIGN_AND  1007
#define LA_TOKEN_ASSIGN_LAST_VAL LA_TOKEN_ASSIGN_AND
#define LA_TOKEN_EOF        -1

typedef struct la_string {
  uint len_;
  const char *ptr_;
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
  void *handle;
  ModuleInit init;
  ModuleDeinit deinit;
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
  funT *curScope;

  fun_stack funstack[1];
  symbol_stack symbolstack[1];
  Imap_t *funRefcount;
  Vmap_t *units;

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
    lineNum,
    curToken,
    curState,
    funcState,
    objectState,
    scopeState,
    fmtState,
    fmtRefcount,
    exitValue,
    tokenArgs,
    didReturn;

  size_t anon_id;

  string
    *la_dir,
    *message;

  la_string
    curStrToken,
    parsePtr;

  VALUE
    tokenValue,
    funArgs[MAX_BUILTIN_PARAMS];

  integer stackValIdx;
  VALUE stackVal[MAX_BUILTIN_PARAMS];

  sym_t
    *tokenSymbol;

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

#define MAX_EXPR_LEVEL 5

static int la_parse_stmt (la_t *);
static int la_parse_expr (la_t *, VALUE *);
static int la_parse_primary (la_t *, VALUE *);
static int la_parse_func_def (la_t *);
static int la_next_token (la_t *);
static int la_eval_file (la_t *, const char *);
static int la_parse_fmt (la_t *, string *, int);
static VALUE la_mul  (la_t *, VALUE, VALUE);
static VALUE la_add  (la_t *, VALUE, VALUE);
static VALUE la_sub  (la_t *, VALUE, VALUE);
static VALUE la_div  (la_t *, VALUE, VALUE);
static VALUE la_mod  (la_t *, VALUE, VALUE);
static VALUE la_bset (la_t *, VALUE, VALUE);
static VALUE la_bnot (la_t *, VALUE, VALUE);
static VALUE array_release (VALUE);
static ArrayType *array_copy (ArrayType *);
static int la_array_set_as_array (la_t *, VALUE, integer, integer, integer);
static int la_array_assign (la_t *, VALUE *, VALUE, VALUE, int);
static int la_parse_func_call (la_t *, VALUE *, CFunc, funT *, VALUE);
static VALUE la_copy_map (VALUE);
static int la_parse_map_get (la_t *, VALUE *);
static int la_parse_map_set (la_t *);

static void la_set_message (la_t *this, int append, char *msg) {
  if (NULL is msg) return;
  if (append)
    String.append_with (this->message, msg);
  else
    String.replace_with (this->message, msg);
}

static void la_set_message_fmt (la_t *this, int append, char *fmt, ...) {
  size_t len = VA_ARGS_FMT_SIZE(fmt);
  char bytes[len + 1];
  VA_ARGS_GET_FMT_STR(bytes, len, fmt);
  la_set_message (this, append, bytes);
}

static inline uint la_StringGetLen (la_string s) {
  return (uint)s.len_;
}

static inline const char *la_StringGetPtr (la_string s) {
  return (const char *)s.ptr_;
}

static inline void la_StringSetLen (la_string *s, uint len) {
  s->len_ = len;
}

static inline void la_StringSetPtr (la_string *s, const char *ptr) {
  s->ptr_ = ptr;
}

static inline la_string la_StringNewLen (const char *str, integer len) {
  la_string x;
  la_StringSetLen (&x, len);
  la_StringSetPtr (&x, str);
  return x;
}

static inline la_string la_StringNew (const char *str) {
  la_string x;
  la_StringSetLen (&x, bytelen (str));
  la_StringSetPtr (&x, str);
  return x;
}

static void la_print_lastring (la_t *this, FILE *fp, la_string s) {
  uint len = la_StringGetLen (s);
  const char *ptr = (const char *) la_StringGetPtr (s);
  while (len > 0) {
    this->print_byte (fp, *ptr);
    ptr++;
    --len;
  }
}

static int la_err_ptr (la_t *this, int err) {
  const char *keep = la_StringGetPtr (this->parsePtr);
  size_t len = la_StringGetLen (this->parsePtr);

  char *sp = (char *) keep;
  while (sp > this->script_buffer and 0 is Cstring.byte.in_str (";\n", *(sp - 1)))
    sp--;

  size_t n_len = (keep - sp);

  la_StringSetPtr (&this->parsePtr, sp);
  la_StringSetLen (&this->parsePtr, n_len);

  sp = (char *) keep;
  int linenum = 0;
  while (*sp++)
    if (*sp is LA_TOKEN_NL)
      if (++linenum > 9) break;

  n_len += (sp - keep);
  la_StringSetLen (&this->parsePtr, n_len);

  la_print_lastring (this, this->err_fp, this->parsePtr);

  la_StringSetPtr (&this->parsePtr, keep);
  la_StringSetLen (&this->parsePtr, len);

  this->print_bytes (this->err_fp, "\n");

  return err;
}

static int la_syntax_error (la_t *this, const char *msg) {
  this->print_fmt_bytes (this->err_fp, "\nSYNTAX ERROR: %s\n", msg);
  return la_err_ptr (this, LA_ERR_SYNTAX);
}

static int la_syntax_error_fmt (la_t *this, const char *fmt, ...) {
  size_t len = VA_ARGS_FMT_SIZE (fmt);
  char bytes[len + 1];
  VA_ARGS_GET_FMT_STR(bytes, len, fmt);
  return this->syntax_error (this, bytes);
}

static int la_arg_mismatch (la_t *this) {
  this->print_fmt_bytes (this->err_fp, "\nargument mismatch:");
  return la_err_ptr (this, LA_ERR_BADARGS);
}

static int la_too_many_args (la_t *this) {
  this->print_fmt_bytes (this->err_fp, "\ntoo many arguments:");
  return la_err_ptr (this, LA_ERR_TOOMANYARGS);
}

static int la_unknown_symbol (la_t *this) {
  this->print_fmt_bytes (this->err_fp, "\nunknown symbol:");
  return la_err_ptr (this, LA_ERR_UNKNOWN_SYM);
}

static int la_unknown_type (la_t *this) {
  this->print_fmt_bytes (this->err_fp, "\nunknown type:");
  return la_err_ptr (this, LA_ERR_UNKNOWN_TYPE);
}

static int la_out_of_bounds (la_t *this) {
  this->print_fmt_bytes (this->err_fp, "\nout of bounds:");
  return la_err_ptr (this, LA_ERR_OUTOFBOUNDS);
}

static int la_type_mismatch (la_t *this) {
  this->print_fmt_bytes (this->err_fp, "\ntype mismatch:");
  return la_err_ptr (this, LA_ERR_UNKNOWN_TYPE);
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
  return NULL isnot Cstring.byte.in_str ("=+-!/*%<>&|^", c);
}

static inline int is_operator_span (int c) {
  return NULL isnot Cstring.byte.in_str ("=<>&|^", c);
}

static void la_reset_token (la_t *this) {
  la_StringSetLen (&this->curStrToken, 0);
  la_StringSetPtr (&this->curStrToken, la_StringGetPtr (this->parsePtr));
}

static void la_ignore_last_token (la_t *this) {
  la_StringSetLen (&this->curStrToken, la_StringGetLen (this->curStrToken) - 1);
}

static void la_ignore_first_token (la_t *this) {
  la_StringSetPtr (&this->curStrToken, la_StringGetPtr (this->curStrToken) + 1);
  la_StringSetLen (&this->curStrToken, la_StringGetLen (this->curStrToken) - 1);
}

static void la_ignore_next_char (la_t *this) {
  la_StringSetPtr (&this->parsePtr, la_StringGetPtr (this->parsePtr) + 1);
  la_StringSetLen (&this->parsePtr, la_StringGetLen (this->parsePtr) - 1);
}

static int la_peek_char (la_t *this, uint n) {
  if (la_StringGetLen (this->parsePtr) <= n) return LA_TOKEN_EOF;
  return *(la_StringGetPtr (this->parsePtr) + n);
}

static int la_get_char (la_t *this) {
  uint len = la_StringGetLen (this->parsePtr);

  ifnot (len) return LA_TOKEN_EOF;

  const char *ptr = la_StringGetPtr (this->parsePtr);
  int c = *ptr++;

  --len;

  la_StringSetPtr (&this->parsePtr, ptr);
  la_StringSetLen (&this->parsePtr, len);
  la_StringSetLen (&this->curStrToken, la_StringGetLen (this->curStrToken) + 1);
  return c;
}

static void la_unget_char (la_t *this) {
  la_StringSetLen (&this->parsePtr, la_StringGetLen (this->parsePtr) + 1);
  la_StringSetPtr (&this->parsePtr, la_StringGetPtr (this->parsePtr) - 1);
  la_ignore_last_token (this);
}

static int la_ignore_ws (la_t *this) {
  int c;

  for (;;) {
    c = la_get_char (this);

    if (is_space (c))
      la_reset_token (this);
    else
      break;
  }

  return c;
}

static int la_peek_char_nows (la_t *this, uint *n) {
  int c;
  while (1) {
    c = la_peek_char (this, *n);

    if (0 is is_space (c) and c isnot '\n')
      return c;

    *n += 1;
  }

  return LA_TOKEN_EOF;
}

static int la_next_char (la_t *this) {
  uint n = 0;
  return la_peek_char_nows (this, &n);
}

static void la_get_span (la_t *this, int (*testfn) (int)) {
  int c;
  do
    c = la_get_char (this);
  while (testfn (c));

  if (c isnot LA_TOKEN_EOF) la_unget_char (this);
}

static int la_get_opened_block (la_t *this, char *msg) {
  int bracket = 1;
  int c;
  int prev_c = 0;
  int in_str = 0;

  while (bracket > 0) {
    c = la_get_char (this);

    if (c is LA_NOTOK or c is LA_TOKEN_EOF)
      return this->syntax_error (this, msg);

    if (c is LA_TOKEN_DQUOTE and prev_c isnot LA_TOKEN_ESCAPE_CHR) {
      if (in_str)
        in_str--;
      else
        in_str++;
      continue;
    }

    prev_c = c;

    if (in_str) continue;

    switch (c) {
      case LA_TOKEN_BLOCK_CLOS: --bracket; break;
      case LA_TOKEN_BLOCK_OPEN: ++bracket; break;
    }
  }

  return LA_OK;
}

static inline int parse_number (la_t *this, int c, int *token_type) {
  *token_type = LA_TOKEN_NUMBER;

  int dot_found = 0;
  int plus_found = 0;
  int minus_found = 0;

  c = la_get_char (this);

  if (c is '-' or c is '+')
    return LA_NOTOK;

  goto parse;

  for (;;) {
    c = la_get_char (this);

    parse:
    if (c is '-' or '+' is c) {
      if (*token_type isnot LA_TOKEN_DOUBLE) return LA_NOTOK;
      if (c is '-') {
        if (minus_found++) return LA_NOTOK;
        else if (plus_found++) return LA_NOTOK;
      }

      int cc = la_peek_char (this, 0);
      ifnot (is_digit (cc)) return LA_NOTOK;
      continue;
    }

    if (c is '.') {
      *token_type = LA_TOKEN_DOUBLE;

      if (dot_found++) return LA_NOTOK;
      ifnot (is_digit (la_peek_char (this, 0))) return LA_NOTOK;
      continue;
    }

    if (c is 'e' or c is 'E') {
      *token_type = LA_TOKEN_DOUBLE;

      int cc = la_peek_char (this, 0);
      if (0 is is_digit (cc) or
          cc isnot '-' or
          cc isnot '.' or
          cc isnot '+') {
        return LA_NOTOK;
      }

      continue;
    }

    ifnot (is_digit (c)) break;
  }

  if (c isnot LA_TOKEN_EOF) la_unget_char (this);

  return LA_OK;
}

static int ns_is_malloced_string (funT *this, string *str) {
  malloced_string *item = this->head;
  while (item isnot NULL) {
    if (str is item->data)
      return 1;

    item = item->next;
  }
  return 0;
}

static void ns_release_malloced_strings (funT *this) {
  malloced_string *item = this->head;
  while (item isnot NULL) {
    malloced_string *tmp = item->next;
    String.release (item->data);
    free (item);
    item = tmp;
  }

  this->head = NULL;
}

static malloced_string *new_malloced_string (size_t len) {
  malloced_string *mbuf = Alloc (sizeof (malloced_string));
  mbuf->data = String.new (len + 1);
  return mbuf;
}

static VALUE la_getcwd (la_t *this) {
  (void) this;
  char *dir = Dir.current ();
  string *cwd = String.new_with (dir);
  free (dir);
  VALUE v = STRING(cwd);
  return v;
}

static VALUE la_typeof (la_t *this, VALUE value) {
  (void) this;
  VALUE v;
  if ((value.type & 0xff) is UFUNC_TYPE)
    v = INT(FUNCPTR_TYPE);
  else if ((value.type & 0x77) is LA_TOKEN_BUILTIN)
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
  malloced_string *mbuf = new_malloced_string (16);
  string *buf = mbuf->data;

  switch (value.type) {
    case INTEGER_TYPE: String.append_with_len (buf, "IntegerType", 11); break;
    case NUMBER_TYPE:  String.append_with_len (buf, "NumberType",  10); break;
    case STRING_TYPE:  String.append_with_len (buf, "StringType",  10); break;
    case OBJECT_TYPE:  String.append_with_len (buf, "ObjectType",  10); break;
    case ARRAY_TYPE:   String.append_with_len (buf, "ArrayType",    9); break;
    case NULL_TYPE:    String.append_with_len (buf, "NullType",     8); break;
    case MAP_TYPE:     String.append_with_len (buf, "MapType",      7); break;

    default:
      if (value.type & FUNCPTR_TYPE)
        String.append_with_len (buf, "FunctionType", 12);
      else if ((value.type & 0xff) is LA_TOKEN_BUILTIN)
        String.append_with_len (buf, "CFunctionType", 13);
      else
        String.append_with_len (buf, "UnknownType", 11);
      break;
  }

  ListStackPush (this->curScope, mbuf);
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

static object *la_object_new (ObjectRelease o_release, ObjectToString o_tostr, VALUE value) {
  object *o = Alloc (sizeof (object));
  o->release = o_release;
  o->toString = o_tostr;
  o->value = value;
  return o;
}

static VALUE la_fclose (la_t *this, VALUE fp_val) {
  VALUE result = INT(LA_NOTOK);

  if (fp_val.type is NULL_TYPE) return result;

  FILE *fp = AS_FILEPTR(fp_val);
  if (NULL is fp) {
    this->Errno = errno;
    return result;
  }

  fclose (fp);
  fp = NULL;

  result = INT(LA_OK);
  return result;
}

static VALUE la_fflush (la_t *this, VALUE fp_val) {
  VALUE result = INT(LA_NOTOK);

  if (fp_val.type is NULL_TYPE) return result;

  FILE *fp = AS_FILEPTR(fp_val);
  if (NULL is fp) return result;

  int retval = fflush (fp);
  if (retval) {
    this->Errno = errno;
    return result;
  }

  result = INT(LA_OK);
  return result;
}

static VALUE la_fopen (la_t *this, VALUE fn_value, VALUE mod_value) {
  char *fn = AS_STRING_BYTES(fn_value);
  char *mode = AS_STRING_BYTES(mod_value);
  FILE *fp = fopen (fn, mode);
  if (NULL is fp) {
    this->Errno = errno;
    VALUE v = NULL_VALUE;
    return v;
  }

  VALUE v = OBJECT(fp);
  object *o = la_object_new (la_fclose, NULL, v);
  v = OBJECT(o);
  return v;
}

static VALUE la_format (la_t *this, VALUE v_fmt) {
  int err = LA_NOTOK;
  VALUE v;

  string *fmt = AS_STRING(v_fmt);
  string *str = String.new (8);

  la_string saved_ptr = this->parsePtr;

  this->parsePtr = la_StringNew (fmt->bytes);

  err = la_parse_fmt (this, str, 1);

 this->parsePtr = saved_ptr;

  if (err isnot LA_OK) {
    this->CFuncError = err;
    String.release (str);
    v = NULL_VALUE;
    return v;
  }

  v = STRING(str);

  if (this->fmtRefcount)
    this->fmtState |= FMT_LITERAL;
  else
    if (this->funcState & EXPR_LIST_STATE)
      v.refcount--;

  return v;
}

static VALUE la_getkey (la_t *this, VALUE fd) {
  (void) this;
  utf8 k = IO.input.getkey (AS_INT(fd));
  VALUE v = INT(k);
  return v;
}

static VALUE la_term_release (la_t *this, VALUE term_val) {
  (void) this;
  object *o = AS_OBJECT(term_val);
  term_t *term = (term_t *) AS_PTR(o->value);
  Term.release (&term);
  VALUE v = INT(LA_OK);
  return v;
}

static VALUE la_term_new (la_t *this) {
  (void) this;
  term_t *term = Term.new ();
  VALUE v = OBJECT(term);
  object *o = la_object_new (la_term_release, NULL, v);
  v = OBJECT(o);
  return v;
}

static VALUE la_term_raw_mode (la_t *this, VALUE term_val) {
  (void) this;
  object *o = AS_OBJECT(term_val);
  term_t *term = (term_t *) AS_PTR(o->value);
  int retval = Term.raw_mode (term);
  VALUE v = INT(retval);
  return v;
}

static VALUE la_term_sane_mode (la_t *this, VALUE term_val) {
  (void) this;
  object *o = AS_OBJECT(term_val);
  term_t *term = (term_t *) AS_PTR(o->value);
  int retval = Term.sane_mode (term);
  VALUE v = INT(retval);
  return v;
}

static VALUE la_fileno (la_t *this, VALUE fp_val) {
  (void) this;
  FILE *fp = AS_FILEPTR(fp_val);
  int fd = fileno (fp);
  VALUE v = INT(fd);
  if (fd is -1)
    this->Errno = errno;

  return v;
}

static void *la_malloc (la_t *this, VALUE size) {
  (void) this;
  return Alloc (AS_MEMSIZE(size));
}

static void *la_realloc (la_t *this, VALUE obj, VALUE size) {
  (void) this;
  return Realloc (AS_VOID_PTR(obj), AS_MEMSIZE(size));
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
  }

  return result;
}

static VALUE string_release (VALUE value) {
  VALUE result = INT(LA_OK);

  if (value.refcount < 0) return result;

  if (value.refcount) goto theend;

  String.release (AS_STRING(value));

  value = NULL_VALUE;

theend:
  value.refcount--;
  return result;
}

static VALUE object_release (la_t *this, VALUE value) {
  VALUE result = INT(LA_OK);

  if (value.refcount < 0) return result;

  if (value.refcount) goto theend;

  object *o = AS_OBJECT(value);

  ifnot (NULL is o->release)
    o->release (this, value);

  free (o);

  value = NULL_VALUE;

theend:
  value.refcount--;
  return result;
}

static VALUE la_free (la_t *this, VALUE value) {
  (void) this;
  VALUE result = INT(LA_OK);
  if (value.type < STRING_TYPE) return result;

  void *obj = NULL;
  switch (value.type) {
    case POINTER_TYPE: obj = AS_VOID_PTR(value); break;
    case   ARRAY_TYPE: return array_release (value);
    case  STRING_TYPE: return string_release (value);
    case  OBJECT_TYPE: return object_release (this, value);
    case     MAP_TYPE: Vmap.release ((Vmap_t *) AS_PTR(value));
    default: return result;
  }

  ifnot (NULL is obj) {
    free (obj);
    value = NULL_VALUE;
  }

  obj = NULL;
  return result;
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

  if ((this->type & 0xff) is UFUNC_TYPE) {
    ifnot (v.type & FUNCPTR_TYPE) {
      funT *f = AS_FUNC_PTR(v);
      fun_release (&f);
    }
  } else
    la_free (NULL, v);

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

static inline char *sym_key (la_t *this, la_string x) {
  Cstring.cp (this->symKey, MAXLEN_SYMBOL + 1,
      la_StringGetPtr (x), la_StringGetLen (x));
  return this->symKey;
}

static inline char *map_key (la_t *this, la_string x) {
  Cstring.cp (this->curMapKey, MAXLEN_SYMBOL + 1,
      la_StringGetPtr (x), la_StringGetLen (x));
  return this->curMapKey;
}

static inline char *cur_msg_str (la_t *this, la_string x) {
  Cstring.cp (this->curMsg, MAXLEN_MSG + 1,
      la_StringGetPtr (x), la_StringGetLen (x));
  return this->curMsg;
}

static sym_t *la_define_symbol (la_t *this, funT *f, char *key, int typ, VALUE value, int is_const) {
  (void) this;
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

static inline sym_t *ns_lookup_symbol (funT *scope, char *key) {
  return Vmap.get (scope->symbols, key);
}

static sym_t *la_lookup_symbol (la_t *this, la_string name) {
  char *key = sym_key (this, name);
  sym_t *sym = ns_lookup_symbol (this->std, key);
  ifnot (NULL is sym) return sym;

  funT *f = this->curScope;
  while (NULL isnot f) {
    sym = ns_lookup_symbol (f, key);

    ifnot (NULL is sym) return sym;

    f = f->prev;
  }

  return NULL;
}

static int la_lambda (la_t *this) {
  Cstring.cp_fmt
    (this->curFunName, MAXLEN_SYMBOL + 1, NS_ANON "_%zd", this->anon_id++);

  la_ignore_ws (this);
  this->curFunDef = NULL;

  int err = la_parse_func_def (this);
  if (err isnot LA_OK)
    return err;

  this->tokenSymbol = this->curSym;
  funT *lambda = this->curFunDef;
  this->tokenArgs = lambda->nargs;
  this->tokenValue = this->tokenSymbol->value;

  this->curFunName[0] = '\0';

  if (this->curToken isnot LA_TOKEN_PAREN_CLOS)
    return this->syntax_error (this, "awaiting )");

  return (this->tokenSymbol->type & 0xff);
}

static int la_do_next_token (la_t *this, int israw) {
  int r = LA_NOTOK;

  sym_t *symbol = NULL;
  this->tokenSymbol = NULL;

  la_reset_token (this);

  int c = la_ignore_ws (this);

  char token = c;

  if (c is '#')
    token = LA_TOKEN_COMMENT;
  else if (is_alpha (c) or c is '_' or (
      (this->objectState & IDENT_LEAD_CHAR_CAN_BE_DIGIT) and is_digit (c)))
    token = LA_TOKEN_SYMBOL;
  else if (is_digit (c) or (c is '-' and is_digit (la_peek_char (this, 0))))
    token = LA_TOKEN_NUMBER;
  else if (is_operator (c))
    token = LA_TOKEN_OPERATOR;
  else if (c is LA_TOKEN_COLON) {
    if (la_peek_char (this, 0) is ' ') {
      this->curToken = LA_TOKEN_COLON;
      return c;
    }
  }

  switch (token) {
    case LA_TOKEN_COMMENT:
      do
        c = la_get_char (this);
      while (c >= 0 and c isnot LA_TOKEN_NL);
      this->lineNum++;

      r = c;
      break;

    case LA_TOKEN_NUMBER:
      if (c is '0' and NULL isnot Cstring.byte.in_str ("xX", la_peek_char (this, 0))
          and is_hexchar (la_peek_char(this, 1))) {
        la_get_char (this);
        la_ignore_first_token (this);
        la_ignore_first_token (this);
        la_get_span (this, is_hexchar);
        r = LA_TOKEN_HEX_NUMBER;
      } else
        if (LA_NOTOK is parse_number (this, c, &r))
          return this->syntax_error (this, "error while tokenizing a number");

      break;

    case LA_TOKEN_SQUOTE: {
        c = la_get_char (this); // get first
        if (c is LA_TOKEN_ESCAPE_CHR) la_get_char (this);

        int max = 4;
        r = LA_TOKEN_SYNTAX_ERR;

        /* multibyte support */
        do {
          c = la_get_char (this);
          if (c is LA_TOKEN_SQUOTE) {
            la_ignore_first_token (this);
            la_ignore_last_token (this);
            r = LA_TOKEN_CHAR;
            break;
          }
        } while (--max isnot 0);

      break;
    }

    case LA_TOKEN_SYMBOL:
      la_get_span (this, is_identifier);
      r = LA_TOKEN_SYMBOL;

      uint toklen = la_StringGetLen (this->curStrToken);
      if (toklen > MAXLEN_SYMBOL)
        return la_syntax_error_fmt (this, "%s: exceeds maximum length (%d) of an identifier",
            cur_msg_str (this, this->curStrToken), MAXLEN_SYMBOL);

      char ident[MAXLEN_SYMBOL + 1];
      Cstring.cp (ident, MAXLEN_SYMBOL + 1, la_StringGetPtr (this->curStrToken), toklen);

      if (*ident isnot 'i' and *ident isnot 'o' and *ident isnot 'a')
        goto raw_block;

      switch (toklen) {
        case 2:
          if (Cstring.eq (ident, "is")) {
            this->tokenSymbol = ns_lookup_symbol (this->std, "is");
            break;
          } else if (Cstring.eq (ident, "or")) {
            this->tokenSymbol = ns_lookup_symbol (this->std, "or");
            break;
          }

          goto raw_block;

        case 3:
          if (Cstring.eq (ident, "and")) {
            this->tokenSymbol = ns_lookup_symbol (this->std, "and");
            break;
          }

          goto raw_block;

        case 5:
          if (Cstring.eq (ident, "isnot")) {
            this->tokenSymbol = ns_lookup_symbol (this->std, "isnot");
            break;
          }

        default:
          goto raw_block;
      }

      r = this->tokenSymbol->type;
      this->tokenValue = this->tokenSymbol->value;
      goto theend;

      raw_block:
      ifnot (israw) {
        if (Cstring.eq ("lambda", ident)) {
          r = la_lambda (this);
          if (r < LA_OK)
            return this->syntax_error (this, "lambda error");

          goto theend;
        }

        this->tokenSymbol = symbol = la_lookup_symbol (this, this->curStrToken);

        if (symbol) {
          r = symbol->type & 0xff;

          this->tokenArgs = (symbol->type >> 8) & 0xff;
          symbol->value.sym = symbol;

          if (r is ARRAY_TYPE) {
            r = LA_TOKEN_ARRAY;
            symbol->value.type = ARRAY_TYPE;
          } else if (r is MAP_TYPE) {
            r = LA_TOKEN_MAP;
            symbol->value.type = MAP_TYPE;
          } else
            if (r < '@')
              r = LA_TOKEN_VAR;

          this->tokenValue = symbol->value;
        }
      }

      break;

    case LA_TOKEN_OPERATOR: {
      la_get_span (this, is_operator_span);

      char *key = sym_key (this, this->curStrToken);
      this->tokenSymbol = ns_lookup_symbol (this->std, key);

      if (this->tokenSymbol) {
        r = this->tokenSymbol->type;
        this->tokenValue = this->tokenSymbol->value;
      } else
        r = LA_TOKEN_SYNTAX_ERR;

      break;
    }

    case LA_TOKEN_BLOCK_OPEN: {
      la_reset_token (this);
      int err = la_get_opened_block (this, "unended block");
      if (err isnot LA_OK)
        return err;

      la_ignore_last_token (this);
      r = LA_TOKEN_BLOCK;

      break;
    }

    case LA_TOKEN_DQUOTE: {
      size_t len = 0;
      int pc = 0;
      int cc = 0;

      while (pc = cc, (cc = la_peek_char (this, len)) isnot LA_TOKEN_EOF) {
        if (LA_TOKEN_DQUOTE is cc and pc isnot LA_TOKEN_ESCAPE_CHR) break;
        if (LA_TOKEN_NL     is cc and pc isnot LA_TOKEN_ESCAPE_CHR)
          return this->syntax_error (this, "error while getting literal string, awaiting double quote, found a newline character");

        len++;
      }

      if (cc is LA_TOKEN_EOF)
        return this->syntax_error (this, "unended string, a '\"' is missing");

      ifnot (this->curState & STRING_LITERAL_ARG_STATE) {
        string *str = String.new (len + 1);
        pc = 0;
        for (size_t i = 0; i < len; i++) {
          c = la_get_char (this);
          if (c is LA_TOKEN_DQUOTE and pc is LA_TOKEN_ESCAPE_CHR)
            String.clear_at (str, -1);

          String.append_byte (str, c);
          pc = c;
        }

        this->tokenValue = STRING(str);
        this->curState |= LITERAL_STRING_STATE;

      } else {
        malloced_string *mbuf = new_malloced_string (len + 1);
        pc = 0;
        for (size_t i = 0; i < len; i++) {
          c = la_get_char (this);
          if (c is LA_TOKEN_DQUOTE and pc is LA_TOKEN_ESCAPE_CHR)
            String.clear_at (mbuf->data, -1);
          String.append_byte (mbuf->data, c);
          pc = c;
        }

        ListStackPush (this->curScope, mbuf);
        this->tokenValue = STRING (mbuf->data);
      }

      c = la_get_char (this);
      la_reset_token (this);

      r = LA_TOKEN_STRING;

      break;
    }

    default:
      r = c;
  }

theend:
  this->curToken = r;
  return r;
}

static int la_next_token (la_t *this) {
  return la_do_next_token (this, 0);
}

static int la_next_raw_token (la_t *this) {
  return la_do_next_token (this, 1);
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
}

static void la_symbol_stack_push (la_t *this, Vmap_t *symbols) {
  symbolstack_t *item = Alloc (sizeof (symbolstack_t));
  item->symbols = Vmap.clone (symbols, la_clone_sym_item, NULL);
  ListStackPush (this->symbolstack, item);
}

static Vmap_t *la_symbol_stack_pop (la_t *this) {
  symbolstack_t *item = ListStackPop (this->symbolstack, symbolstack_t);
  Vmap_t *symbols = item->symbols;
  free (item);
  return symbols;
}

static VALUE la_string_to_num (la_string s) {
  integer r = 0;
  int c;
  const char *ptr = la_StringGetPtr (s);
  int len = la_StringGetLen (s);
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
  const char *ptr = la_StringGetPtr (s);
  int len = la_StringGetLen (s);
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

static int  la_string_get (la_t *this, VALUE *vp) {
  int c = la_peek_char (this, 0);

  if (c is LA_TOKEN_INDEX_OPEN) {
    string *str = AS_STRING(this->tokenValue);

    la_next_token (this);

    VALUE v;
    this->curState |= INDEX_STATE;
    int err = la_parse_primary (this, &v);
    this->curState &= ~INDEX_STATE;
    if (err isnot LA_OK) return err;

    if (v.type isnot INTEGER_TYPE)
      return this->syntax_error (this, "awaiting an integer expression, when getting string index");

    integer idx = AS_INT(v);

    if (0 > idx) idx += str->num_bytes;

    if (idx < 0 or (size_t) idx >= str->num_bytes)
      return la_out_of_bounds (this);

    *vp = INT(str->bytes[idx]);

    if (this->curState & LITERAL_STRING_STATE) {
      this->curState &= ~LITERAL_STRING_STATE;
      String.release (str);
    }

  } else {
    *vp = this->tokenValue;
    la_next_token (this);
  }

  return LA_OK;
}

static int la_string_set_char (la_t *this, VALUE value, int is_const) {
  if (is_const)
    return this->syntax_error (this, "can not modify constant string");

  string *str = AS_STRING(value);

  la_next_token (this);

  VALUE v;

  this->curState |= INDEX_STATE;
  int err = la_parse_primary (this, &v);
  this->curState &= ~INDEX_STATE;
  if (err isnot LA_OK) return err;

  if (v.type isnot INTEGER_TYPE)
    return this->syntax_error (this, "awaiting an integer expression, when setting string index");

  integer idx = AS_INT(v);

  if (0 > idx) idx += str->num_bytes;
  if (idx < 0 or (size_t) idx >= str->num_bytes) return la_out_of_bounds (this);

  if (this->curToken isnot LA_TOKEN_ASSIGN)
    return this->syntax_error (this, "syntax error while setting string, awaiting =");

  la_next_token (this);
  err = la_parse_expr (this, &v);

  if (v.type isnot INTEGER_TYPE)
    return this->syntax_error (this, "awaiting an integer expression, when setting string index");

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

  if (value.refcount < 0) return result;

  if (value.refcount) goto theend;

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
  if (la_next_char (this) is LA_TOKEN_INDEX_CLOS)
    return this->syntax_error (this, "empty array");

  int err;
  int pc;
  int c = 0;
  int instr = 0;
  int inmap = 0;
  int indtokopen = 1;
  int indtokclos = 0;
  int inar = 0;
  int is_array_of_array = 0;
  int num_elem = 1;

  uint n = 0;
  if (la_peek_char_nows (this, &n) is LA_TOKEN_INDEX_OPEN)
    is_array_of_array = 1;

  n = 0;

  while (1) {
    pc = c;
    c = la_peek_char (this, n++);

    if (c is LA_TOKEN_EOF)
      return this->syntax_error (this, "unended array");

    if (c is LA_TOKEN_INDEX_CLOS and 0 is instr) {
      if (inmap)
        continue;

      indtokclos++;

      ifnot (indtokopen - indtokclos)
        break;

      if (1 is indtokopen - indtokclos)
        inar = 0;

      continue;
    }

    if (c is LA_TOKEN_INDEX_OPEN and 0 is instr) {
      if (inmap)
        continue;

      indtokopen++;

      if (indtokopen - indtokclos is 2)
        inar = 1;

     continue;
    }

    if (c is LA_TOKEN_DQUOTE and pc isnot LA_TOKEN_ESCAPE_CHR) {
      if (instr) instr = 0; else instr = 1;
      continue;
    }

    if (c is LA_TOKEN_MAP_OPEN and 0 is instr) {
      inmap++;
      continue;
    }

    if (c is LA_TOKEN_MAP_CLOS and 0 is instr) {
      inmap--;
      continue;
    }

    if (c is LA_TOKEN_COMMA and 0 is instr and 0 is inmap and 0 is inar)
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
  this->curState |= STRING_LITERAL_ARG_STATE;
  la_next_token (this);
  err = la_parse_primary (this, &v);
  this->curState &= ~STRING_LITERAL_ARG_STATE;

  if (err isnot LA_OK)
    return err;

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
      return this->syntax_error (this, "unsupported array type");
  }

  *vp = ary;

  VALUE fidx = INT(1);
  VALUE lidx = INT(num_elem - 1);

  ifnot (1 is num_elem)
    err = la_array_assign (this, &ary, fidx, lidx, 1);
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
      return la_out_of_bounds (this);

    la_next_token (this);

    err = la_parse_expr (this, &val);

    if (err isnot LA_OK) return err;

    if (val.type isnot ARRAY_TYPE and val.type isnot NULL_TYPE)
      return this->syntax_error (this, "error while setting Array of an array, awaiting an array or null");

    if (a_ar[idx] isnot NULL)
      array_release (val);

    if (ar.type isnot ARRAY_TYPE)
      a_ar[idx] = NULL;
    else
      a_ar[idx] = (ArrayType *) AS_ARRAY(val);

    idx++;
  } while (this->curToken is LA_TOKEN_COMMA);

  if (idx - 1 isnot last_idx)
    return la_out_of_bounds (this);

  return LA_OK;
}

static int la_array_set_as_map (la_t *this, VALUE ar, integer len, integer idx, integer last_idx) {
  int err;
  VALUE val;
  Vmap_t **m_ar = (Vmap_t **) AS_ARRAY(ar);

  do {
    if (idx < 0 or idx >= len or idx > last_idx)
      return la_out_of_bounds (this);

    la_next_token (this);

    err = la_parse_expr (this, &val);

    if (err isnot LA_OK) return err;

    if (val.type isnot MAP_TYPE and val.type isnot NULL_TYPE)
      return this->syntax_error (this, "error while setting Map array, awaiting a map or null");

    m_ar[idx] = (val.type is MAP_TYPE) ? AS_MAP(val) : NULL;
    idx++;
  } while (this->curToken is LA_TOKEN_COMMA);

  if (idx - 1 isnot last_idx)
    return la_out_of_bounds (this);

  return LA_OK;
}

static int la_array_set_as_string (la_t *this, VALUE ar, integer len, integer idx, integer last_idx) {
  int err;
  VALUE val;
  string **s_ar = (string **) AS_ARRAY(ar);

  do {
    if (idx < 0 or idx >= len or idx > last_idx)
      return la_out_of_bounds (this);

    this->curState |= STRING_LITERAL_ARG_STATE;
    la_next_token (this);

    err = la_parse_expr (this, &val);

    this->curState &= ~STRING_LITERAL_ARG_STATE;

    if (err isnot LA_OK) return err;

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
        return this->syntax_error (this, "error while setting string array, awaiting a string or null");
    }

    idx++;
  } while (this->curToken is LA_TOKEN_COMMA);

  if (idx - 1 isnot last_idx)
    return la_out_of_bounds (this);

  return LA_OK;
}

static int la_array_set_as_number (la_t *this, VALUE ar, integer len, integer idx, integer last_idx) {
  int err;
  VALUE val;
  number *n_ar = (number *) AS_ARRAY(ar);
  do {
    if (idx < 0 or idx >= len or idx > last_idx)
      return la_out_of_bounds (this);

    la_next_token (this);

    err = la_parse_expr (this, &val);
    if (err isnot LA_OK) return err;
    n_ar[idx] = AS_NUMBER(val);

    idx++;
  } while (this->curToken is LA_TOKEN_COMMA);

  if (idx - 1 isnot last_idx)
    return la_out_of_bounds (this);

  return LA_OK;
}

static int la_array_set_as_int (la_t *this, VALUE ar, integer len, integer idx, integer last_idx) {
  int err;
  VALUE val;
  integer *s_ar = (integer *) AS_ARRAY(ar);

  do {
    if (idx < 0 or idx >= len or idx > last_idx)
      return la_out_of_bounds (this);

    la_next_token (this);

    err = la_parse_expr (this, &val);
    if (err isnot LA_OK) return err;
    s_ar[idx] = AS_INT(val);

    idx++;
  } while (this->curToken is LA_TOKEN_COMMA);

  if (idx - 1 isnot last_idx)
    return la_out_of_bounds (this);

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
    return la_out_of_bounds (this);

  VALUE ary = array->value;

  ifnot (is_single)
    if (la_next_token (this) isnot LA_TOKEN_INDEX_OPEN)
      return this->syntax_error (this, "array assignment: awaiting [");

  if (array->type is INTEGER_TYPE) {
    err = la_array_set_as_int (this, ary, len, idx, last_idx);
  } else if (array->type is STRING_TYPE) {
    err = la_array_set_as_string (this, ary, len, idx, last_idx);
  } else if (array->type is MAP_TYPE) {
    err = la_array_set_as_map (this, ary, len, idx, last_idx);
  } else if (array->type is ARRAY_TYPE) {
    err = la_array_set_as_array (this, ary, len, idx, last_idx);
  } else
    err = la_array_set_as_number (this, ary, len, idx, last_idx);

  if (err isnot LA_OK)
    return err;

  ifnot (is_single) {
    if (this->curToken isnot LA_TOKEN_INDEX_CLOS)
      return this->syntax_error (this, "array assignment: awaiting ]");

    la_next_token (this);
  }

  return LA_OK;
}

static int la_parse_array_def (la_t *this) {
  la_string name;
  int c;
  int err;
  VALUE len;
  VALUE ar;

  c = la_next_raw_token (this);

  if (c isnot LA_TOKEN_SYMBOL)
    return this->syntax_error (this, "syntax error, awaiting a name");

  const char *sp = la_StringGetPtr (this->curStrToken);
  int isname = 0;
  int type = INTEGER_TYPE;

  if (Cstring.eq_n ("integer ", sp, 8))
    type = INTEGER_TYPE;
  else if (Cstring.eq_n ("number ", sp, 7))
    type = NUMBER_TYPE;
  else if (Cstring.eq_n ("pointer ", sp, 8))
    type = POINTER_TYPE;
  else if (Cstring.eq_n ("string ", sp, 7))
    type = STRING_TYPE;
  else if (Cstring.eq_n ("map ", sp, 4))
    type = MAP_TYPE;
  else if (Cstring.eq_n ("array ", sp, 6))
    type = ARRAY_TYPE;
  else
    isname = 1;

  if (isname)
    name = this->curStrToken;
  else {
    c = la_next_raw_token (this);

    if (c isnot LA_TOKEN_SYMBOL)
      return this->syntax_error (this, "syntax error, awaiting a name");

    name = this->curStrToken;
  }

  c = la_next_token (this);

  if (c isnot LA_TOKEN_INDEX_OPEN)
    return this->syntax_error (this, "syntax error, awaiting [");

  this->curState |= INDEX_STATE;
  err = la_parse_primary (this, &len);
  this->curState &= ~INDEX_STATE;

  if (err isnot LA_OK)
    return err;

  if (len.type isnot INTEGER_TYPE)
    return this->syntax_error (this, "awaiting an integer expression, when getting array length");

  integer nlen = AS_INT(len);

  ar = ARRAY(ARRAY_NEW(type, nlen));

  funT *scope = (this->scopeState & PUBLIC_SCOPE ? this->function : this->curScope);
  this->tokenSymbol = la_define_symbol (this, scope, sym_key (this, name), ARRAY_TYPE,
      ar, 0);
  this->scopeState &= ~PUBLIC_SCOPE;

  if (this->curToken is LA_TOKEN_ASSIGN) {
    VALUE at_idx = INT(0);
    VALUE last_idx = INT(-1);
    return la_array_assign (this, &ar, at_idx, last_idx, 0);
  }

  return LA_OK;
}

static int la_parse_array_set (la_t *this) {
  int c = la_next_char (this);

  if (c is '=') {
    VALUE v = this->tokenValue;
    sym_t *sym = v.sym;
    if (sym isnot NULL) {
      sym->type = NULL_TYPE;
      sym->value = NULL_VALUE;
    }

    array_release (v);

    this->objectState |= OBJECT_MMT_REASSIGN;
    return LA_OK;
  }

  VALUE ix = INT(0);

  VALUE ary = this->tokenValue;

  int err;
  c = la_next_token (this);

  VALUE last_ix = INT(-1);

  int is_index = c is LA_TOKEN_INDEX_OPEN;

  if (is_index) {
    uint n = 0;
    c = la_peek_char_nows (this, &n);
    if (c is LA_TOKEN_STAR) {
      for (uint i = 0; i <= n; i++)
        la_ignore_next_char (this);

      c = la_next_token (this);
      if (c isnot LA_TOKEN_INDEX_CLOS)
        return this->syntax_error (this, "array set: awaiting ]");

      c = la_next_token (this);

      if (this->curToken isnot LA_TOKEN_ASSIGN)
        return this->syntax_error (this, "syntax error while setting array, awaiting =");

      VALUE ar_val;
      la_next_token (this);
      err = la_parse_expr (this, &ar_val);
      if (err isnot LA_OK)
        return err;

      ArrayType *array = (ArrayType *) AS_ARRAY(ary);
      if (array->type isnot ar_val.type)
        return la_type_mismatch (this);

      switch (array->type) {
        case STRING_TYPE: {
          string **s_ar = (string **) AS_ARRAY(array->value);
          string *s_val = AS_STRING(ar_val);
          for (size_t i = 0; i < array->len; i++) {
            string *item = s_ar[i];
            String.replace_with_len (item, s_val->bytes, s_val->num_bytes);
          }
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
            la_free (this, ar_val);

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
          return la_unknown_type (this);
      }
    }

    this->curState |= INDEX_STATE;
    err = la_parse_primary (this, &ix);
    this->curState &= ~INDEX_STATE;
    if (err isnot LA_OK)
      return err;

    if (this->curToken is LA_TOKEN_COLON) {
      c = la_next_token (this);
      if (c and c isnot LA_TOKEN_INDEX_CLOS) {
        err = la_parse_expr (this, &last_ix);
        if (err isnot LA_OK)
          return err;
      }

      la_next_token (this);
      is_index = 0;
    }
  }

  c = this->curToken;

  if (c is LA_TOKEN_INDEX_OPEN) {
    ArrayType *array = (ArrayType *) AS_ARRAY(ary);
    VALUE ar = array->value;

    switch (array->type) {
      case STRING_TYPE: {
        string **s_ar = (string **) AS_ARRAY(ar);
        VALUE v = STRING(s_ar[AS_INT(ix)]);
        la_unget_char (this);
        err = la_string_set_char (this, v, 0);
        return err;
      }

      default:
        return this->syntax_error (this, "Arrays of arrays haven't been implemented");
    }
  }

  if (c is LA_TOKEN_DOT) {
    ArrayType *array = (ArrayType *) AS_ARRAY(ary);
    VALUE ar = array->value;

    switch (array->type) {
      case MAP_TYPE: {
        Vmap_t **m_ar = (Vmap_t **) AS_ARRAY(ar);
        VALUE v = MAP(m_ar[AS_INT(ix)]);
        this->tokenValue = v;
        la_unget_char (this);
        err = la_parse_map_set (this);
        return err;
      }

      default:
        return this->syntax_error (this, "not a map");
    }
  }

  if (this->curToken isnot LA_TOKEN_ASSIGN)
    return this->syntax_error (this, "syntax error while setting array, awaiting =");

  ifnot (is_index)
    return la_array_assign (this, &ary, ix, last_ix, is_index);

  last_ix = INT(AS_INT(ix));
  return la_array_assign (this, &ary, ix, last_ix, is_index);
}

static int la_parse_array_get (la_t *this, VALUE *vp) {
  VALUE ar = this->tokenValue;
  ArrayType *array = (ArrayType *) AS_ARRAY(ar);
  integer len = array->len;

  int c = la_next_token (this);

  if (c isnot LA_TOKEN_INDEX_OPEN) {
    *vp = ar;
    return LA_OK;
  }

  VALUE ix;
  this->curState |= INDEX_STATE;
  int err = la_parse_primary (this, &ix);
  this->curState &= ~INDEX_STATE;
  if (err isnot LA_OK)
    return err;

  integer idx = AS_INT(ix);
  if (0 > idx)
    idx += len;

  if (idx <= -1 or idx >= len)
    return la_out_of_bounds (this);

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
      if (ary[idx] isnot NULL)
        *vp = MAP(ary[idx]);
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
      return la_unknown_type (this);

  }

  c = this->curToken;

  if (c is LA_TOKEN_DOT) {
    VALUE v = *vp;
    if (v.type isnot MAP_TYPE)
      return this->syntax_error (this, "not a map");

    this->tokenValue = v;
    la_unget_char (this);
    return la_parse_map_get (this, vp);
  }

  if (c is LA_TOKEN_INDEX_OPEN) {
    VALUE v = *vp;
    switch (v.type) {
      case STRING_TYPE:
        this->tokenValue = v;
        this->objectState |= ARRAY_MEMBER;
        la_unget_char (this);
        err = la_string_get (this, vp);
        if (this->curToken is LA_TOKEN_INDEX_OPEN)
          return this->syntax_error (this, "unsupported indexing");

        return err;

      case ARRAY_TYPE:
        this->tokenValue = v;
        la_unget_char (this);
        return la_parse_array_get (this, vp);

      default:
        return this->syntax_error (this, "Arrays of arrays haven't been implemented");
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
  la_free (NULL, *val);
  free (val);
}

static int map_set_rout (la_t *this, Vmap_t *map, char *key, funT *scope) {
  int err;
  VALUE v;

  int c = la_next_token (this);

  if (c is LA_TOKEN_FUNCDEF) {
    Cstring.cp_fmt (this->curFunName, MAXLEN_SYMBOL + 1, NS_ANON "_%zd", this->anon_id++);

    err = la_parse_func_def (this);
    this->curFunName[0] = '\0';
    if (err isnot LA_OK)
      return err;

    v = PTR(this->curFunDef);
    v.type = FUNCPTR_TYPE;
    goto assign;
  }

  err = la_parse_expr (this, &v);
  if (err isnot LA_OK)
    return err;

  assign: {}
  VALUE *val = Alloc (sizeof (VALUE));
  val->refcount = v.refcount;
  val->type = v.type;

  sym_t *sym = Alloc (sizeof (sym_t));
  sym->scope = scope;
  sym->value = MAP(map);

  val->sym = sym;

  switch (val->type) {
    case STRING_TYPE:
      val->asString  = v.asString;
      this->curState &= ~LITERAL_STRING_STATE;
      break;
    case NUMBER_TYPE: val->asNumber  = v.asNumber;  break;
    case NULL_TYPE  : val->asNull    = v.asNull;    break;
    default:          val->asInteger = v.asInteger; break;
  }

  if (NOTOK is Vmap.set (map, key, val, la_release_map_val, 0)) {
    this->print_bytes (this->err_fp, "Map.set() internal error\n");
    return LA_NOTOK;
  }

  return LA_OK;
}

static void *la_copy_map_cb (void *value, void *mapval) {
  Vmap_t *map = (Vmap_t *) mapval;
  VALUE *v = (VALUE *) value;

  VALUE *val = Alloc (sizeof (VALUE));
  val->refcount = v->refcount;
  val->type = v->type;

  sym_t *sym = Alloc (sizeof (sym_t));
  sym->scope = v->sym->scope;
  sym->value = MAP(map);
  val->sym = sym;

  switch (val->type) {
    case MAP_TYPE   :
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

static int la_parse_map (la_t *this, VALUE *vp) {
  Vmap_t *map = Vmap.new (32);

  la_string saved_ptr = this->parsePtr;
  this->parsePtr = this->curStrToken;

  VALUE v;
  int c;
  int err = LA_OK;

  funT *scope;

  this->curState |= MAP_STATE;
  this->scopeState = PUBLIC_SCOPE;

  for (;;) {
    this->curState |= STRING_LITERAL_ARG_STATE;
    c = err = la_next_token (this);
    this->curState &= ~STRING_LITERAL_ARG_STATE;

    if (c is LA_TOKEN_EOF) break;

    if (err < LA_OK)
      return err;

    if (c is LA_TOKEN_COMMA or c is LA_TOKEN_NL) continue;

    switch (c) {
      case LA_TOKEN_PRIVATE:
        this->scopeState = PRIVATE_SCOPE;
        continue;

      case LA_TOKEN_PUBLIC:
        this->scopeState = PUBLIC_SCOPE;
        continue;

      default:
        this->scopeState = PUBLIC_SCOPE;
      }

    v = this->tokenValue;

    if (v.type isnot STRING_TYPE)
      return this->syntax_error (this, "awaiting a string as a key");

    char *key = AS_STRING_BYTES(v);

    c = la_next_token (this);

    if (c isnot LA_TOKEN_COLON)
      return this->syntax_error (this, "error while setting map field, awaiting :");

    scope = ((this->scopeState & PUBLIC_SCOPE) ? this->function : NULL);
    this->scopeState &= ~(PUBLIC_SCOPE|PRIVATE_SCOPE);

    err = map_set_rout (this, map, key, scope);
    if (err isnot LA_OK)
      return err;

    if (this->curToken is LA_TOKEN_EOF) break;
  }

  this->curState &= ~MAP_STATE;
  this->scopeState = PRIVATE_SCOPE;
  this->parsePtr = saved_ptr;

  v = MAP(map);
  *vp = v;
  la_next_token (this);
  return LA_OK;
}

static int la_parse_map_get (la_t *this, VALUE *vp) {
  int is_this = (la_StringGetLen (this->curStrToken) is 4 and
      Cstring.eq_n (la_StringGetPtr (this->curStrToken), "this", 4));

  int c = la_next_token (this);
  if (c isnot LA_TOKEN_DOT) {
    *vp = this->tokenValue;
    return LA_OK;
  }

  int err;

  redo: {}
  this->objectState |= IDENT_LEAD_CHAR_CAN_BE_DIGIT;
  c = err = la_next_raw_token (this);
  this->objectState &= ~IDENT_LEAD_CHAR_CAN_BE_DIGIT;
  if (err < LA_OK)
    return err;

  if (c isnot LA_TOKEN_SYMBOL)
    return this->syntax_error (this, "not a symbol");

  char *key = map_key (this, this->curStrToken);

  VALUE mapv = this->tokenValue;
  Vmap_t *map = AS_MAP(mapv);

  VALUE *v = Vmap.get (map, key);
  if (v is NULL)
    return la_syntax_error_fmt (this, "%s, not a key", key);

  if (v->sym->scope is NULL)
    ifnot (is_this)
      return la_syntax_error_fmt (this, "%s, symbol has private scope", key);

  *vp = *v;

  if (this->objectState & ASSIGNMENT_STATE) {
    switch (v->type) {
      case STRING_TYPE:
      case ARRAY_TYPE:
      this->objectState |= MMT_OBJECT;
    }
  }

  c = la_next_token (this);

  if (c is LA_TOKEN_DOT) {
    if (v->type isnot MAP_TYPE)
      return la_syntax_error_fmt (this, "%s, not a map", key);

    this->tokenValue = *v;
    goto redo;
  }

  if (c is LA_TOKEN_INDEX_OPEN) {
    if (v->type isnot ARRAY_TYPE)
      return this->syntax_error (this, "not an array");

    this->tokenValue = *v;
    la_unget_char (this);
    return la_parse_array_get (this, vp);
  }

  if (v->type is MAP_TYPE)
    *vp = la_copy_map (*v);

  return LA_OK;
}

static int la_parse_map_set (la_t *this) {
  Vmap_t *map = AS_MAP(this->tokenValue);

  int c = la_next_char (this);

  if (c is '=') {
    VALUE v = this->tokenValue;
    sym_t *sym = v.sym;
    if (sym isnot NULL) {
      sym->type = NULL_TYPE;
      sym->value = NULL_VALUE;
    }

    Vmap.release (map);

    this->objectState |= OBJECT_MMT_REASSIGN;
    return LA_OK;
  }

  c = la_next_token (this);

  if (c isnot LA_TOKEN_DOT)
    return this->syntax_error (this, "awaiting .");

  redo: {}

  int err;
  c = err = la_next_raw_token (this);
  if (err < LA_OK)
    return err;

  if (c isnot LA_TOKEN_SYMBOL)
    return err;

  char key[MAXLEN_SYMBOL + 1];
  Cstring.cp (key, MAXLEN_SYMBOL + 1,
      la_StringGetPtr (this->curStrToken), la_StringGetLen (this->curStrToken));

  c = la_next_token (this);

  if (c is LA_TOKEN_PAREN_OPEN) {
    VALUE *v = Vmap.get (map, key);
    if (NULL is v)
      return la_syntax_error_fmt (this, "%s, method doesn't exists", key);

    int type;
    if (v->type & FUNCPTR_TYPE)
      type = FUNCPTR_TYPE;
    else if ((v->type & 0x77) is LA_TOKEN_BUILTIN)
      type = CFUNCTION_TYPE;
    else
      return la_syntax_error_fmt (this, "%s, not a method", key);

    la_unget_char (this);

    if (type is FUNCPTR_TYPE) {
      funT *uf = AS_FUNC_PTR((*v));
      VALUE th = v->sym->value;
      la_define_symbol (this, uf, "this", MAP_TYPE, th, 0);
      this->funcState |= MAP_METHOD_STATE;
      err = la_parse_func_call (this, v, NULL, uf, *v);
      la_next_token (this);
    } else {
      CFunc op = (CFunc) AS_PTR((*v));
      this->tokenArgs = ((*v).type >> 8) & 0xff;
      err = la_parse_func_call (this, v, op, NULL, *v);
    }

    return err;
  }

  if (c isnot LA_TOKEN_ASSIGN) {
    ifnot (Vmap.key_exists (map, key))
      return this->syntax_error (this, "syntax error while setting map, awaiting =");

    if (c is LA_TOKEN_DOT) {
      VALUE *v = Vmap.get (map, key);
      if (v->type isnot MAP_TYPE)
        return la_syntax_error_fmt (this, "%s, not a map", key);

      map = AS_MAP((*v));
      goto redo;
    }

    if (c is LA_TOKEN_INDEX_OPEN) {
      VALUE *v = Vmap.get (map, key);
      if (v->type isnot ARRAY_TYPE)
        return la_syntax_error_fmt (this, "%s, not an array", key);

      this->tokenValue = *v;
      la_unget_char (this);
      return la_parse_array_set (this);
    }
  }

  if (Vmap.key_exists (map, key))
    la_release_map_val (Vmap.pop (map, key));

  return map_set_rout (this, map, key, this->function);
}

static int la_parse_expr_list (la_t *this) {
  int err, c;
  int count = 0;
  VALUE v;

  do {
    this->curState |= STRING_LITERAL_ARG_STATE;
    this->funcState |= EXPR_LIST_STATE;
    err = la_parse_expr (this, &v);
    this->funcState &= ~EXPR_LIST_STATE;
    if (err isnot LA_OK) return err;

    if (this->curState & FUNC_CALL_RESULT_IS_MMT)
      if (v.sym is NULL)
        v.refcount--;

    stack_push (this, v);

    count++;

    c = this->curToken;

    if (c is LA_TOKEN_COMMA) {
      this->curState |= STRING_LITERAL_ARG_STATE;
      la_next_token (this);
    }
  } while (c is LA_TOKEN_COMMA);


  return count;

}

static int la_parse_char (la_t *this, VALUE *vp, la_string token) {
  VALUE v = INT(0);
  const char *ptr = la_StringGetPtr (token);

  if (ptr[0] is LA_TOKEN_SQUOTE) return this->syntax_error (this, "error while getting a char token ");

  if (ptr[0] is LA_TOKEN_ESCAPE_CHR) {
    if (ptr[1] is 'n')  { v = INT('\n'); goto theend; }
    if (ptr[1] is 't')  { v = INT('\t'); goto theend; }
    if (ptr[1] is 'r')  { v = INT('\r'); goto theend; }
    if (ptr[1] is '\\') { v = INT('\\'); goto theend; }
    if (ptr[1] is LA_TOKEN_SQUOTE) { v = INT(LA_TOKEN_SQUOTE); goto theend; }
    return this->syntax_error (this, "unknown escape sequence");
  }

  if (ptr[0] >= ' ' and ptr[0] <= '~') {
    if (ptr[1] is LA_TOKEN_SQUOTE) {
      v = INT (ptr[0]);
      goto theend;
    } else {
      return this->syntax_error (this, "error while taking character literal");
    }
  }

  int len = 0;
  utf8 c = Ustring.get.code_at ((char *) ptr, 4, 0, &len);

  if (LA_TOKEN_SQUOTE isnot ptr[len])
    return this->syntax_error (this, "error while taking character literal");

  v = INT(c);

theend:
  *vp = v;
  return LA_OK;
}

static int la_parse_string (la_t *this, la_string str) {
  int c,  r;
  la_string savepc = this->parsePtr;

  this->parsePtr = str;

  for (;;) {
    c = la_next_token (this);

    while (c is LA_TOKEN_NL or c is LA_TOKEN_SEMICOLON) {
      if (c is LA_TOKEN_NL) this->lineNum++;
      c = la_next_token (this);
    }

    if (c < 0) break;

    if (c is UFUNC_TYPE) {
      c = la_next_char (this);
      if (c isnot LA_TOKEN_PAREN_OPEN)
        this->curToken = LA_TOKEN_VAR;
    }

    r = la_parse_stmt (this);

    if (r isnot LA_OK) return r;

    c = this->curToken;

    if (c is LA_TOKEN_NL or c is LA_TOKEN_SEMICOLON or c < 0) {
      if (c is LA_TOKEN_NL) this->lineNum++;
      continue;
    } else
      return this->syntax_error (this, STR_FMT("%s(), unknown token |%c| |%d|", __func__, c, c));
  }

  this->parsePtr = savepc;

  return LA_OK;
}

static void la_fun_refcount_incr (int *count) {
  (*count)++;
}

static void la_fun_refcount_decr (int *count) {
  (*count)--;
}

static void la_fun_release_symbols (funT *uf, int clear, int is_method) {
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

static int la_parse_func_call (la_t *this, VALUE *vp, CFunc op, funT *uf, VALUE value) {
  int expectargs;

  if (uf)
    expectargs = uf->nargs;
  else
    expectargs = this->tokenArgs;

  int c = la_next_token (this);

  if (c isnot LA_TOKEN_PAREN_OPEN) {
    la_unget_char (this);
    VALUE v;
    if (uf isnot NULL) {
      v = PTR(uf);
      v.type |= FUNCPTR_TYPE;
    } else {
      v = PTR(op);
      v.type = value.sym->type;
    }
    v.sym = value.sym;
    *vp = v;
    return LA_OK;
  }

  int paramCount = 0;

  this->curState |= STRING_LITERAL_ARG_STATE;
  c = la_next_token (this);

  if (c isnot LA_TOKEN_PAREN_CLOS) {
    paramCount = la_parse_expr_list (this);
    c = this->curToken;
    if (paramCount < 0)
      return paramCount;
  }

  this->curState &= ~(STRING_LITERAL_ARG_STATE);

  if (c isnot LA_TOKEN_PAREN_CLOS)
    return this->syntax_error (this, "expected closed parentheses");

  if (expectargs isnot paramCount)
    return la_arg_mismatch (this);

  while (paramCount > 0) {
    --paramCount;
    this->funArgs[paramCount] = stack_pop (this);
  }

  int err = LA_OK;
  int is_method = this->funcState & MAP_METHOD_STATE;
  this->funcState &= ~MAP_METHOD_STATE;

  if (uf) {
    int refcount = Imap.set_by_callback (this->funRefcount, uf->funname, la_fun_refcount_incr);
    if (refcount > 1) {
      la_symbol_stack_push (this, this->curScope->symbols);
      la_fun_release_symbols (uf, 1, is_method);
    }

    sym_t *uf_argsymbols[expectargs];

    for (int i = 0; i < expectargs; i++) {
      VALUE v = this->funArgs[i];
      if (v.type & FUNCPTR_TYPE) {
        funT *f = AS_FUNC_PTR(v);
        uf_argsymbols[i] = la_define_symbol (this, uf, uf->argName[i], (UFUNC_TYPE | (f->nargs << 8)), v, 0);
      } else {
        v.refcount += (refcount < 2);
        uf_argsymbols[i] = la_define_symbol (this, uf, uf->argName[i], v.type, v, 0);
      }
    }

    this->didReturn = 0;
    uf->result = NULL_VALUE;

    la_fun_stack_push (this, this->curScope);

    this->curScope = uf;

    string *func = AS_STRING(this->func->value);
    String.replace_with (func, uf->funname);

    err = la_parse_string (this, uf->body);

    this->curScope = la_fun_stack_pop (this);

    String.replace_with (func, this->curScope->funname);

    this->didReturn = 0;

    this->curState &= ~FUNC_CALL_RESULT_IS_MMT;
    if (uf->result.type >= FUNCPTR_TYPE) {
      sym_t *sym = uf->result.sym;
      this->curState |= FUNC_CALL_RESULT_IS_MMT;
      ifnot (NULL is sym) {
        VALUE none = NULL_VALUE;
        sym->value = none;
        uf->result.sym = NULL;
      }
    }

    for (int i = 0; i < expectargs; i++) {
      VALUE v = this->funArgs[i];
      if (v.type >= FUNCPTR_TYPE) {
        sym_t *uf_sym = uf_argsymbols[i];
        VALUE uf_val = uf_sym->value;
        sym_t *sym = uf_val.sym;

        if (sym isnot NULL and uf_sym->scope isnot sym->scope) {
          sym->value = uf_val;
          VALUE non = NULL_VALUE;
          uf_sym->value = non;
        } else {
          if (v.type is STRING_TYPE and
              ns_is_malloced_string (this->curScope, AS_STRING(v))) {
            VALUE non = NULL_VALUE;
            uf_sym->value = non;
          }
          if (v.type is MAP_TYPE and v.sym isnot NULL) {
            VALUE non = NULL_VALUE;
            uf_sym->value = non;
          }
        }
      }
    }

    refcount = Imap.set_by_callback (this->funRefcount, uf->funname, la_fun_refcount_decr);

    ifnot (refcount)
      la_fun_release_symbols (uf, 1, is_method);
    else {
      la_fun_release_symbols (uf, 0, is_method);
      this->curScope->symbols = la_symbol_stack_pop (this);
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
    VALUE v = this->funArgs[0];
    if (v.type is STRING_TYPE)
      if (ns_is_malloced_string (this->curScope, AS_STRING(v)))
        continue;

    if (v.sym is NULL)
      la_free (this, v);
  }

  this->curState &= ~FUNC_CALL_RESULT_IS_MMT;
  if (vp->type >= FUNCPTR_TYPE)
    this->curState |= FUNC_CALL_RESULT_IS_MMT;

  err = this->CFuncError;

  la_next_token (this);

  return err;
}

static int la_parse_primary (la_t *this, VALUE *vp) {
  int c, err;

  c = this->curToken;

  switch (c) {
    case LA_TOKEN_INDEX_OPEN:
      ifnot (this->curState & INDEX_STATE) {
        err = la_get_anon_array (this, vp);
        if (err is LA_OK) {
          c = this->curToken;
          if (c isnot LA_TOKEN_INDEX_CLOS)
            return this->syntax_error (this, "array expression, awaiting ]");

          c = la_next_char (this);

          if (c is LA_TOKEN_INDEX_OPEN) {
            VALUE saved_val = *vp;
            this->curToken = LA_TOKEN_ARRAY;
            this->tokenValue = *vp;

            err = la_parse_primary (this, vp);

            if (vp->type is STRING_TYPE) {
              string *str = AS_STRING((*vp));
              *vp = STRING_NEW_WITH_LEN(str->bytes, str->num_bytes);
            }

            la_free (this, saved_val);
            return err;
          }

          c = la_next_token (this);

          return LA_OK;
        }

        return err;
      }
      /* fall through */

    case LA_TOKEN_PAREN_OPEN: {
      int close_token = (c is LA_TOKEN_PAREN_OPEN
         ? LA_TOKEN_PAREN_CLOS : LA_TOKEN_INDEX_CLOS);

      la_next_token (this);

      err = la_parse_expr (this, vp);

      if (err is LA_OK) {
        c = this->curToken;

        if (c is close_token) {
          la_next_token (this);
          return LA_OK;
        }
      }

      return err;
    }

    case LA_TOKEN_NUMBER: {
      VALUE val = la_string_to_num (this->curStrToken);
      *vp = val;
      la_next_token (this);
      return LA_OK;
    }

    case LA_TOKEN_DOUBLE: {
      char *endptr; char str[32];
      Cstring.cp (str, 32, la_StringGetPtr (this->curStrToken), la_StringGetLen (this->curStrToken));
      double val = strtod (str, &endptr);
      *vp = NUMBER(val);
      la_next_token (this);
      return LA_OK;
    }

    case LA_TOKEN_HEX_NUMBER:
      *vp = la_HexStringToNum (this->curStrToken);
      la_next_token (this);
      return LA_OK;

    case LA_TOKEN_CHAR:
      err = la_parse_char (this, vp, this->curStrToken);
      la_next_token (this);
      return err;

    case LA_TOKEN_VAR:
      if (this->tokenValue.type is STRING_TYPE)
        return la_string_get (this, vp);

      *vp = this->tokenValue;
      la_next_token (this);
      return LA_OK;

    case LA_TOKEN_ARRAY:
      err = la_parse_array_get (this, vp);
      if (err isnot LA_OK)
        return err;

      if (this->curToken isnot LA_TOKEN_DOT)
        return err;

      if (vp->type isnot MAP_TYPE)
        return this->syntax_error (this, "not a map");

      la_unget_char (this);
      this->tokenValue = *vp;
      /* fall through */

    case LA_TOKEN_MAP: {
      err = la_parse_map_get (this, vp);
      if (err isnot LA_OK)
        return err;

      int type;
      if (vp->type & FUNCPTR_TYPE)
        type = FUNCPTR_TYPE;
      else if ((vp->type & 0xff) is LA_TOKEN_BUILTIN)
        type = CFUNCTION_TYPE;
      else
        break;

      ifnot (this->curToken is LA_TOKEN_PAREN_OPEN)
        break;

      la_unget_char (this);

      if (type is FUNCPTR_TYPE) {
        funT *uf = AS_FUNC_PTR((*vp));
        VALUE th = vp->sym->value;
        la_define_symbol (this, uf, "this", MAP_TYPE, th, 0);
        this->funcState |= MAP_METHOD_STATE;
        err = la_parse_func_call (this, vp, NULL, uf, *vp);

        la_next_token (this);
        return err;
      } else {
        CFunc op = (CFunc) AS_PTR((*vp));
        this->tokenArgs = ((*vp).type >> 8) & 0xff;
        return la_parse_func_call (this, vp, op, NULL, *vp);
      }
    }

    case LA_TOKEN_USRFUNC: {
      sym_t *symbol = this->tokenSymbol;
      ifnot (symbol)
        return this->syntax_error (this, "user defined function, not declared");

      funT *uf = AS_FUNC_PTR(symbol->value);

      err = la_parse_func_call (this, vp, NULL, uf, symbol->value);

      la_next_token (this);
      return err;
    }

    case LA_TOKEN_BUILTIN: {
      CFunc op = (CFunc) AS_PTR(this->tokenValue);
      return la_parse_func_call (this, vp, op, NULL, this->tokenSymbol->value);
    }

    case LA_TOKEN_FUNCDEF:
      err = la_lambda (this);
      if (err isnot LA_OK)
        return err;
      *vp = this->tokenValue;

      return err;

    case LA_TOKEN_STRING:
      return la_string_get (this, vp);

    case LA_TOKEN_BLOCK:
      err = la_parse_map (this, vp);
      if (err isnot LA_OK)
        return err;

      while (this->curToken is LA_TOKEN_DOT) {
        la_unget_char (this);

        this->tokenValue = *vp;
        VALUE mapval = this->tokenValue;

        this->curToken = LA_TOKEN_MAP;
        err = la_parse_primary (this, vp);
        if (err isnot LA_OK)
          return err;

        if (mapval.sym is NULL) {
          VALUE v = *vp;

          if (v.type isnot MAP_TYPE) {
            *vp = la_copy_value (v);
            this->objectState &= ~MMT_OBJECT;
          }

          la_free (this, mapval);
        }
      }

      return err;

    default:
      if ((c & 0xff) is LA_TOKEN_BINOP) {
        OpFunc op = (OpFunc) AS_PTR(this->tokenValue);
        VALUE v;
        la_next_token (this);
        err = la_parse_expr (this, &v);
        if (err is LA_OK)
          *vp = op (this, INT(0), v);

        return err;
      }

      if (c is LA_TOKEN_NL) {
        la_next_token (this);
        return la_parse_primary (this, vp);
      }

      if (c isnot LA_TOKEN_EOF)
        return this->syntax_error (this, STR_FMT(
            "%s(), syntax error, unknown token |%c| |%d|", __func__, c, c));
  }

  return LA_OK;
}

static int la_parse_stmt (la_t *this) {
  int c;
  la_string name;
  funT *scope = this->curScope;
  VALUE val;
  int err = LA_OK;

  if (this->didReturn) {
    do {
      c = la_get_char (this);

    } while (c >= 0 and c isnot LA_TOKEN_NL and c isnot LA_TOKEN_SEMICOLON and
       c isnot LA_TOKEN_BLOCK_CLOS);

    la_unget_char (this);
    c = la_next_token (this);

    return LA_OK;
  }

do_token:
  c = this->curToken;

  switch (c) {
    case LA_TOKEN_BREAK:
      if (this->curState & LOOP_STATE) {
        this->curState |= BREAK_STATE;
        return LA_ERR_BREAK;
      }

      return this->syntax_error (this, "break is not in a loop");

    case LA_TOKEN_CONTINUE:
      if (this->curState & LOOP_STATE) {
        this->curState |= CONTINUE_STATE;
        return LA_ERR_CONTINUE;
      }

      return this->syntax_error (this, "continue is not in a loop");

    case LA_TOKEN_VARDEF:
    case LA_TOKEN_CONSTDEF: {
      int is_const = c is LA_TOKEN_CONSTDEF;

      c = la_next_raw_token (this);

      if (c isnot LA_TOKEN_SYMBOL)
        return this->syntax_error (this, "expected a symbol");

      name = this->curStrToken;

      ifnot (la_StringGetLen (name))
        return this->syntax_error (this, "unknown symbol");

      char *key = sym_key (this, name);

      sym_t *sym = ns_lookup_symbol (this->std, key);
      ifnot (NULL is sym)
        return this->syntax_error (this, "can not redefine a standard symbol");

      scope = (this->scopeState & PUBLIC_SCOPE ? this->function : this->curScope);

      sym = ns_lookup_symbol (scope, key);

      VALUE v = INT(0);
      int type = INTEGER_TYPE;

      ifnot (NULL is sym)
        return this->syntax_error (this, "can not redeclare a symbol in this scope");
      else if (is_const) {
        type = NULL_TYPE;
        v = NULL_VALUE;
      }

      this->tokenSymbol = la_define_symbol (this, scope, key, type, v, is_const);
      this->scopeState &= ~PUBLIC_SCOPE;

      if (NULL is this->tokenSymbol)
        return this->syntax_error (this, "unknown error on declaration");

      c = LA_TOKEN_VAR;
      /* fall through */
    }

    case LA_TOKEN_VAR: {
      name = this->curStrToken;
      sym_t *symbol = this->tokenSymbol;

      if (symbol->value.type is STRING_TYPE)
        if (la_peek_char (this, 0) is LA_TOKEN_INDEX_OPEN)
          return la_string_set_char (this, symbol->value, symbol->is_const);

      la_next_token (this);

      int token = this->curToken;
      if (token < LA_TOKEN_ASSIGN and
          token > LA_TOKEN_ASSIGN_LAST_VAL)
        return this->syntax_error (this, "expected assignment operator");

      ifnot (symbol) {
        la_print_lastring (this, this->err_fp, name);
        return la_unknown_symbol (this);
      }

      if (symbol->is_const)
        if (symbol->value.type isnot NULL_TYPE)
          return this->syntax_error (this, "can not reassign to a constant");

      const char *ptr = la_StringGetPtr (this->parsePtr);
      while (*ptr is ' ') ptr++;

      c = la_next_token (this);

      if (c is LA_TOKEN_FUNCDEF) {
        char *key = sym_key (this, name);
        la_release_sym (Vmap.pop (scope->symbols, key));
        Cstring.cp (this->curFunName, MAXLEN_SYMBOL + 1, key, la_StringGetLen(name));
        err = la_parse_func_def (this);
        this->curFunName[0] = '\0';
        return err;
      }

      int is_un = c is LA_TOKEN_UNARY;
      if (is_un)
        la_next_token (this);

      this->objectState |= ASSIGNMENT_STATE;
      err = la_parse_expr (this, &val);
      this->objectState &= ~ASSIGNMENT_STATE;

      if (this->objectState & MMT_OBJECT or this->objectState & ARRAY_MEMBER) {
        switch (val.type) {
          case ARRAY_TYPE:
          case STRING_TYPE: {
            VALUE v = val;
            val = la_copy_value (v);
          }
        }
        this->objectState &= ~MMT_OBJECT;
      }

      if (err isnot LA_OK) return err;

      if (is_un)
        AS_INT(val) = ~AS_INT(val);

      if (val.type & FUNCPTR_TYPE) {
        funT *f = AS_FUNC_PTR(val);
        symbol->type = (UFUNC_TYPE | (f->nargs << 8));
        Cstring.cp (f->funname, MAXLEN_SYMBOL, la_StringGetPtr(name),
            la_StringGetLen(name));
        f->prev = scope->prev;
      } else
        symbol->type = val.type;

      if (token is LA_TOKEN_ASSIGN) {
        switch (symbol->value.type) {
          case STRING_TYPE:
            if (Cstring.eq (symbol->scope->funname, scope->funname))
              la_free (this, symbol->value);
            break;

          default:
            if (symbol->type & FUNCPTR_TYPE) {
              funT *f = AS_FUNC_PTR(symbol->value);
              fun_release (&f);
              break;
            }

            la_free (this, symbol->value);
        }
      }


      VALUE result;

      switch (token) {
        case LA_TOKEN_ASSIGN:
          val.sym = symbol;
          result = val;
          goto assign_and_return;

        case LA_TOKEN_ASSIGN_APP:
          this->objectState |= OBJECT_APPEND;
          result = la_add (this, symbol->value, val);
          this->objectState &= ~OBJECT_APPEND;
          break;

        case LA_TOKEN_ASSIGN_SUB: result = la_sub  (this, symbol->value, val); break;
        case LA_TOKEN_ASSIGN_DIV: result = la_div  (this, symbol->value, val); break;
        case LA_TOKEN_ASSIGN_MUL: result = la_mul  (this, symbol->value, val); break;
        case LA_TOKEN_ASSIGN_MOD: result = la_mod  (this, symbol->value, val); break;
        case LA_TOKEN_ASSIGN_BAR: result = la_bset (this, symbol->value, val); break;
        case LA_TOKEN_ASSIGN_AND: result = la_bnot (this, symbol->value, val); break;
        default: return this->syntax_error (this, "unknown operator");
      }

      if (result.type is NULL_TYPE)
        return this->syntax_error (this, "unxpected operation");

      assign_and_return:
      symbol->value = result;

      this->curState &= ~LITERAL_STRING_STATE;
      return LA_OK;
    }

    case LA_TOKEN_ARRAY:
      this->objectState &= ~OBJECT_MMT_REASSIGN;
      err = la_parse_array_set (this);
      if (err isnot LA_OK)
        return err;

      if (this->objectState & OBJECT_MMT_REASSIGN) {
        this->curToken = LA_TOKEN_VAR;
        goto do_token;
      }

      return err;

    case LA_TOKEN_MAP:
      this->objectState &= ~OBJECT_MMT_REASSIGN;
      err = la_parse_map_set (this);
      if (err isnot LA_OK)
        return err;

      if (this->objectState & OBJECT_MMT_REASSIGN) {
        this->curToken = LA_TOKEN_VAR;
        goto do_token;
      }

      return err;

    case LA_TOKEN_BUILTIN:
    case UFUNC_TYPE:
      return la_parse_primary (this, &val);

    case LA_TOKEN_COMMA:
      la_next_token (this);
      return la_parse_stmt (this);

    default:
      if (this->tokenSymbol and AS_INT(this->tokenValue)) {
        int (*func) (la_t *) = AS_VOID_PTR(this->tokenValue);
        err = (*func) (this);
        if (err is LA_TOKEN_PUBLIC)
          goto do_token;
        return err;
      }

      return this->syntax_error (this, STR_FMT("%s(), unknown token |%c| |%d|", __func__, c, c));
  }

  return err;
}

// parse a level n expression
// level 0 is the lowest level (highest precedence)
static int la_parse_expr_level (la_t *this, int max_level, VALUE *vp) {
  int c = this->curToken;
  ifnot ((c & 0xff) is LA_TOKEN_BINOP)
    return LA_OK;

  int err = LA_OK;
  VALUE lhs = *vp;
  VALUE rhs;

  string *x = NULL;
  if (lhs.type is STRING_TYPE and lhs.sym is NULL)
    if (this->curState & LITERAL_STRING_STATE)
      ifnot (ns_is_malloced_string (this->curScope, AS_STRING(lhs)))
        x = AS_STRING(lhs);

  do {
    int level = (c >> 8) & 0xff;
    if (level > max_level) break;

    OpFunc op = (OpFunc) AS_PTR(this->tokenValue);

    this->curState |= STRING_LITERAL_ARG_STATE;
    la_next_token (this);

    err = la_parse_primary (this, &rhs);

    if (err isnot LA_OK) return err;

    c = this->curToken;

    while ((c & 0xff) is LA_TOKEN_BINOP) {
      int nextlevel = (c >> 8) & 0xff;
      if (level <= nextlevel) break;

      err = la_parse_expr_level (this, nextlevel, &rhs);

      if (err isnot LA_OK) return err;

      c = this->curToken;
    }

    this->CFuncError = LA_OK;
    this->curMsg[0] = '\0';
    VALUE sv_lhs = lhs;
    lhs = op (this, lhs, rhs);
    if (this->CFuncError isnot LA_OK) {
      if (this->curMsg[0] isnot '\0')
        this-> print_fmt_bytes (this->err_fp, "binary operation: %s\n", this->curMsg);
      return this->CFuncError;
    }

    if (sv_lhs.type isnot STRING_TYPE) {
      if (sv_lhs.sym is NULL)
        la_free (this, sv_lhs);
    }

    if (rhs.type isnot STRING_TYPE) {
      if (rhs.sym is NULL)
        la_free (this, rhs);
    }

    this->curState &= ~LITERAL_STRING_STATE;
  } while ((c & 0xff) is LA_TOKEN_BINOP);

  this->curState &= ~(STRING_LITERAL_ARG_STATE|LITERAL_STRING_STATE);
  *vp = lhs;

  String.release (x);

  return err;
}

static int la_parse_expr (la_t *this, VALUE *vp) {
  int err = la_parse_primary (this, vp);

  if (err is LA_OK)
    err = la_parse_expr_level (this, MAX_EXPR_LEVEL, vp);

  return err;
}

static int la_consume_ifelse (la_t *this) {
  while (1) {
    int c = la_next_token (this);
    if (c is LA_TOKEN_EOF)
      return this->syntax_error (this, "unended conditional");

    while ((c = la_get_char (this))) {
      if (c is LA_TOKEN_EOF)
        return this->syntax_error (this, "unended conditional");

      if (c is LA_TOKEN_BLOCK_OPEN) {
        int err = la_get_opened_block (this, "unended conditional");
        if (err isnot LA_OK)
          return err;

        c = la_next_token (this);
        if (c isnot LA_TOKEN_ELSE)
          return LA_OK;
      }
    }
  }

  return LA_OK;
}

static int la_parse_if (la_t *this) {
  int token = this->curToken;

  this->curState |= STRING_LITERAL_ARG_STATE;
  int c = la_next_token (this);

  VALUE cond;

  int err = la_parse_expr (this, &cond);
  this->curState &= ~STRING_LITERAL_ARG_STATE;

  if (err isnot LA_OK) return err;

  c = this->curToken;

  if (c isnot LA_TOKEN_BLOCK) return this->syntax_error (this, "parsing if, not a block string");

  la_string elsepart;
  la_string ifpart = this->curStrToken;

  c = la_next_token (this);

  int haveelse = 0;
  int haveelif = 0;

  if (c is LA_TOKEN_ELSE) {
    c = la_next_token (this);

    if (c isnot LA_TOKEN_IF and c isnot LA_TOKEN_IFNOT) {
      if (c isnot LA_TOKEN_BLOCK)
        return this->syntax_error (this, "parsing else, not a block string");

      elsepart = this->curStrToken;
      haveelse = 1;

      la_next_token (this);
    } else {
      haveelif = c;
      la_ignore_last_token (this);
    }
  }

  funT *save_scope = this->curScope;

  funT *fun = Fun_new (this, funNew (
    .name = NS_BLOCK, .namelen = NS_BLOCK_LEN, .parent = this->curScope
  ));

  this->curScope = fun;

  int condition = AS_INT(cond);
  int is_true = (condition
     ? token is LA_TOKEN_IFNOT ? 0 : 1
     : token is LA_TOKEN_IFNOT ? 1 : 0);

  if (is_true) {
    err = la_parse_string (this, ifpart);
    if (haveelif) err = la_consume_ifelse (this);

  } else if (haveelse) {
    err = la_parse_string (this, elsepart);

  } else if (haveelif) {
    this->curToken = haveelif;
    err = la_parse_if (this);
  }

  if (this->didReturn)
    la_StringSetLen (&this->parsePtr, 0);

  this->curScope = save_scope;
  fun_release (&fun);
  return err;
}

static int la_parse_while (la_t *this) {
  int err;

  int c = la_next_token (this);
  if (c isnot LA_TOKEN_PAREN_OPEN)
    return this->syntax_error (this, "error while parsing while loop, awaiting (");

  funT *save_scope = this->curScope;

  funT *fun = Fun_new (this, funNew (
    .name = NS_BLOCK, .namelen = NS_BLOCK_LEN, .parent = this->curScope
  ));

  this->curScope = fun;

  const char *tmp_ptr = la_StringGetPtr (this->parsePtr);
  const char *ptr = tmp_ptr;

  int parenopen = 1;
  while (*ptr) {
    if (*ptr is LA_TOKEN_NL) {
      this->lineNum++;
      ptr++;
      continue;
    }

    if (*ptr is LA_TOKEN_PAREN_OPEN) {
      parenopen++;
      ptr++;
      continue;
    }

    if (*ptr is LA_TOKEN_PAREN_CLOS) {
      parenopen--;
      ifnot (parenopen)
        break;
    }

    ptr++;
  }

  ifnot (*ptr)
    return this->syntax_error (this, "unended while loop");

  this->curToken = LA_TOKEN_PAREN_OPEN;

  integer orig_len = la_StringGetLen (this->parsePtr);
  integer cond_len = ptr - tmp_ptr;

  char cond[cond_len + 3];
  cond[0] = LA_TOKEN_PAREN_OPEN;
  for (integer i = 0; i < cond_len; i++)
    cond[i + 1] = tmp_ptr[i];
  cond[cond_len + 1] = LA_TOKEN_PAREN_CLOS;
  cond[cond_len + 2] = '\0';

  la_string cond_str = la_StringNewLen (cond, cond_len + 2);

  la_StringSetPtr (&this->parsePtr, ptr + 1);
  la_StringSetLen (&this->parsePtr, orig_len - cond_len);

  c = la_next_token (this);

  if (c isnot LA_TOKEN_BLOCK)
    return this->syntax_error (this, "parsing while, not a block string");

  la_get_char (this);

  tmp_ptr = la_StringGetPtr (this->curStrToken);

  integer bodylen = la_StringGetLen (this->curStrToken) - 1;
  char body[bodylen + 1];
  for (integer i = 0; i < bodylen; i++)
    body[i] = tmp_ptr[i];
  body[bodylen] = '\0';

  la_string body_str = la_StringNewLen (body, bodylen);

  la_string savepc = this->parsePtr;

  VALUE v;
  for (;;) {
    this->parsePtr = cond_str;

    this->curToken = LA_TOKEN_PAREN_OPEN;
    err = la_parse_expr (this, &v);
    if (err isnot LA_OK) return err;

    ifnot (AS_INT(v)) goto theend;

    this->curState |= LOOP_STATE;
    this->curState &= ~(BREAK_STATE|CONTINUE_STATE);
    err= la_parse_string (this, body_str);
    this->curState &= ~LOOP_STATE;

    if (err is LA_ERR_BREAK or this->curState & BREAK_STATE) {
      la_next_token (this);
      goto theend;
    }

    if (this->didReturn) {
      this->curState &= ~(BREAK_STATE|CONTINUE_STATE|LOOP_STATE);
      la_StringSetLen (&this->parsePtr, 0);
      fun_release (&fun);
      return LA_OK;
    }

    if (err is LA_ERR_CONTINUE or this->curState & CONTINUE_STATE)
      continue;

    if (err isnot LA_OK) return err;
  }

theend:
  this->curScope = save_scope;
  fun_release (&fun);
  this->curState &= ~(BREAK_STATE|CONTINUE_STATE|LOOP_STATE);
  this->parsePtr = savepc;
  return LA_OK;
}

static int la_parse_do (la_t *this) {
  int err;

  int c = la_next_token (this);

  if (c isnot LA_TOKEN_BLOCK)
    return this->syntax_error (this, "parsing do/while loop, not a block string");

  integer bodylen = la_StringGetLen (this->curStrToken) - 1;
  const char *tmp_ptr = la_StringGetPtr (this->curStrToken);

  char body[bodylen + 1];
  for (integer i = 0; i < bodylen; i++)
    body[i] = tmp_ptr[i];
  body[bodylen] = '\0';

  la_string body_str = la_StringNewLen (body, bodylen);

  c = la_next_token (this);

  if (c isnot LA_TOKEN_WHILE)
    return this->syntax_error (this, "error while parsing do/while loop, awaiting while");

  c = la_next_token (this);

  if (c isnot LA_TOKEN_PAREN_OPEN)
    return this->syntax_error (this, "error while parsing do/while loop, awaiting (");

  tmp_ptr = la_StringGetPtr (this->parsePtr);
  const char *ptr = tmp_ptr;

  int parenopen = 1;
  while (*ptr) {
    if (*ptr is LA_TOKEN_NL) {
      this->lineNum++;
      ptr++;
      continue;
    }

    if (*ptr is LA_TOKEN_PAREN_OPEN) {
      parenopen++;
      ptr++;
      continue;
    }

    if (*ptr is LA_TOKEN_PAREN_CLOS) {
      parenopen--;
      ifnot (parenopen)
        break;
    }

    ptr++;
  }

  ifnot (*ptr)
    return this->syntax_error (this, "unended do/while loop");

  this->curToken = LA_TOKEN_PAREN_OPEN;

  integer orig_len = la_StringGetLen (this->parsePtr);
  integer cond_len = ptr - tmp_ptr;

  char cond[cond_len + 3];
  cond[0] = LA_TOKEN_PAREN_OPEN;
  for (integer i = 0; i < cond_len; i++)
    cond[i + 1] = tmp_ptr[i];
  cond[cond_len + 1] = LA_TOKEN_PAREN_CLOS;
  cond[cond_len + 2] = '\0';

  la_string cond_str = la_StringNewLen (cond, cond_len + 2);

  funT *save_scope = this->curScope;

  funT *fun = Fun_new (this, funNew (
    .name = NS_BLOCK, .namelen = NS_BLOCK_LEN, .parent = this->curScope
  ));

  this->curScope = fun;

  la_StringSetPtr (&this->parsePtr, ptr + 1);
  la_StringSetLen (&this->parsePtr, orig_len - cond_len);

  c = la_next_token (this);

  la_string savepc = this->parsePtr;

  VALUE v;
  for (;;) {
    this->curState |= LOOP_STATE;
    this->curState &= ~(BREAK_STATE|CONTINUE_STATE);
    err= la_parse_string (this, body_str);
    this->curState &= ~LOOP_STATE;

    if (err is LA_ERR_BREAK or this->curState & BREAK_STATE) {
      la_next_token (this);
      goto theend;
    }

    if (this->didReturn) {
      this->curState &= ~(BREAK_STATE|CONTINUE_STATE|LOOP_STATE);
      la_StringSetLen (&this->parsePtr, 0);
      fun_release (&fun);
      return LA_OK;
    }

    if (err is LA_ERR_CONTINUE or this->curState & CONTINUE_STATE)
      continue;

    if (err isnot LA_OK) return err;

    this->parsePtr = cond_str;

    this->curToken = LA_TOKEN_PAREN_OPEN;
    err = la_parse_expr (this, &v);
    if (err isnot LA_OK) return err;

    ifnot (AS_INT(v)) goto theend;
  }

theend:
  this->curScope = save_scope;
  fun_release (&fun);
  this->curState &= ~(BREAK_STATE|CONTINUE_STATE|LOOP_STATE);
  this->parsePtr = savepc;
  return LA_OK;
}

static int la_parse_for (la_t *this) {
  int err;

  int c = la_next_token (this);
  if (c isnot LA_TOKEN_PAREN_OPEN)
    return this->syntax_error (this, "error while parsing for loop, awaiting (");

  funT *save_scope = this->curScope;

  funT *fun = Fun_new (this, funNew (
    .name = NS_BLOCK, .namelen = NS_BLOCK_LEN, .parent = this->curScope
  ));

  this->curScope = fun;

  la_next_token (this);

  do {
    err = la_parse_stmt (this);
    if (err isnot LA_OK) return err;
  } while (this->curToken is LA_TOKEN_COMMA);

  const char *tmp_ptr = la_StringGetPtr (this->parsePtr);
  char *ptr = Cstring.byte.in_str (tmp_ptr, LA_TOKEN_SEMICOLON);
  if (NULL is ptr)
    return this->syntax_error (this, "error while parsing for loop, awaiting ;");

  integer orig_len = la_StringGetLen (this->parsePtr);
  integer cond_len = ptr - tmp_ptr;
  integer advanced_len = cond_len;

  char cond[cond_len + 3];
  cond[0] = LA_TOKEN_PAREN_OPEN;
  for (integer i = 0; i < cond_len; i++)
    cond[i + 1] = tmp_ptr[i];
  cond[cond_len + 1] = LA_TOKEN_PAREN_CLOS;
  cond[cond_len + 2] = '\0';

  la_string cond_str = la_StringNewLen (cond, cond_len + 2);

  advanced_len++;

  tmp_ptr = ptr + 1;
  ptr = Cstring.byte.in_str (tmp_ptr, ')');
  if (NULL is ptr)
    return this->syntax_error (this, "error while parsing for loop, awaiting )");

  integer stmt_len = ptr - tmp_ptr;
  advanced_len += stmt_len;

  char stmt[stmt_len + 2];
  for (integer i = 0; i < stmt_len; i++)
    stmt[i] = tmp_ptr[i];
  stmt[stmt_len] = LA_TOKEN_SEMICOLON;
  stmt[stmt_len + 1] = '\0';

  la_string stmt_str = la_StringNewLen (stmt, stmt_len + 1);

  advanced_len++;

  la_StringSetPtr (&this->parsePtr, ptr + 1);
  la_StringSetLen (&this->parsePtr, orig_len - advanced_len);

  c = la_next_token (this);

  if (c isnot LA_TOKEN_BLOCK)
    return this->syntax_error (this, "parsing for, not a block string");

  la_get_char (this);

  tmp_ptr = la_StringGetPtr (this->curStrToken);

  integer bodylen = la_StringGetLen (this->curStrToken) - 1;
  char body[bodylen + 1];
  for (integer i = 0; i < bodylen; i++)
    body[i] = tmp_ptr[i];
  body[bodylen] = '\0';

  la_string body_str = la_StringNewLen (body, bodylen);

  la_string savepc = this->parsePtr;

  VALUE v;
  for (;;) {
    this->parsePtr = cond_str;

    this->curToken = LA_TOKEN_PAREN_OPEN;
    err = la_parse_expr (this, &v);
    if (err isnot LA_OK) return err;

    ifnot (AS_INT(v)) goto theend;

    this->curState |= LOOP_STATE;
    this->curState &= ~(BREAK_STATE|CONTINUE_STATE);
    err= la_parse_string (this, body_str);
    this->curState &= ~LOOP_STATE;

    if (err is LA_ERR_BREAK or this->curState & BREAK_STATE) {
      la_next_token (this);
      goto theend;
    }

    if (this->didReturn) {
      this->curState &= ~(BREAK_STATE|CONTINUE_STATE|LOOP_STATE);
      this->curToken = LA_TOKEN_SEMICOLON;
      la_StringSetLen (&this->parsePtr, 0);
      fun_release (&fun);
      return LA_OK;
    }

    if (err is LA_ERR_CONTINUE or this->curState & CONTINUE_STATE)
      goto ctl_stmt;

    if (err isnot LA_OK) return err;

    ctl_stmt:
      this->parsePtr = stmt_str;
      do {
        la_next_token (this);
        err = la_parse_stmt (this);
        if (err isnot LA_OK) return err;
      } while (this->curToken is LA_TOKEN_COMMA);
  }

theend:
  this->curScope = save_scope;
  fun_release (&fun);
  this->curState &= ~(BREAK_STATE|CONTINUE_STATE|LOOP_STATE);
  this->parsePtr = savepc;
  return LA_OK;
}

static int la_parse_loop (la_t *this) {
  int err;
  int c = la_next_token (this);
  if (c isnot LA_TOKEN_PAREN_OPEN)
    return this->syntax_error (this, "error while parsing loop, awaiting (");

  funT *save_scope = this->curScope;

  funT *fun = Fun_new (this, funNew (
    .name = NS_BLOCK, .namelen = NS_BLOCK_LEN, .parent = this->curScope
  ));

  this->curScope = fun;

  const char *ptr = la_StringGetPtr (this->parsePtr);
  int parenopen = 1;
  int stmt_found = 0;

  while (*ptr) {
    if (*ptr is LA_TOKEN_PAREN_OPEN) {
      parenopen++;
      ptr++;
      continue;
    }

    if (*ptr is LA_TOKEN_PAREN_CLOS) {
      parenopen--;
      if (parenopen) {
        ptr++;
        continue;
      } else
        break;
    }

    if (*ptr is LA_TOKEN_SEMICOLON) {
      stmt_found = 1;
      break;
    }

    if (*ptr is LA_TOKEN_NL) {
      this->lineNum++;
      ptr++;
    }

    ptr++;
  }

  if (stmt_found) {
    la_next_token (this);
    do {
      err = la_parse_stmt (this);
      if (err isnot LA_OK) return err;
    } while (this->curToken is LA_TOKEN_COMMA);

    if (this->curToken isnot LA_TOKEN_SEMICOLON)
      return this->syntax_error (this, "awaiting ;");

    this->curToken = LA_TOKEN_PAREN_OPEN;
  }

  VALUE v;
  err = la_parse_expr (this, &v);
  if (err isnot LA_OK) return err;

  if (v.type isnot INTEGER_TYPE)
    return this->syntax_error (this, "error while parsing loop, awaiting an integer expression");

  integer num = AS_INT(v);

  c = this->curToken;

  if (c isnot LA_TOKEN_BLOCK)
    return this->syntax_error (this, "parsing loop, not a block string");

  la_get_char (this);

  const char *tmp_ptr = la_StringGetPtr (this->curStrToken);

  integer bodylen = la_StringGetLen (this->curStrToken) - 1;
  char body[bodylen + 1];
  for (integer i = 0; i < bodylen; i++)
    body[i] = tmp_ptr[i];
  body[bodylen] = '\0';

  la_string body_str = la_StringNewLen (body, bodylen);

  la_string savepc = this->parsePtr;

  for (integer i = 0; i < num; i++) {
    this->curState |= LOOP_STATE;
    this->curState &= ~(BREAK_STATE|CONTINUE_STATE);
    err= la_parse_string (this, body_str);
    this->curState &= ~LOOP_STATE;

    if (err is LA_ERR_BREAK or this->curState & BREAK_STATE) {
      la_next_token (this);
      goto theend;
    }

    if (this->didReturn) {
      this->curState &= ~(BREAK_STATE|CONTINUE_STATE|LOOP_STATE);
      la_StringSetLen (&this->parsePtr, 0);
      fun_release (&fun);
      return LA_OK;
    }

    if (err is LA_ERR_CONTINUE or this->curState & CONTINUE_STATE)
      continue;

    if (err isnot LA_OK) return err;
  }

theend:
  this->curScope = save_scope;
  fun_release (&fun);
  this->curState &= ~(BREAK_STATE|CONTINUE_STATE|LOOP_STATE);
  this->parsePtr = savepc;
  return LA_OK;
}

static int la_parse_forever (la_t *this) {
  int err;

  funT *save_scope = this->curScope;

  funT *fun = Fun_new (this, funNew (
    .name = NS_BLOCK, .namelen = NS_BLOCK_LEN, .parent = this->curScope
  ));

  this->curScope = fun;

  int c = la_next_token (this);

  if (c is LA_TOKEN_PAREN_OPEN) {
    la_next_token (this);
    do {
      err = la_parse_stmt (this);
      if (err isnot LA_OK) return err;
    } while (this->curToken is LA_TOKEN_COMMA);

    if (this->curToken isnot LA_TOKEN_PAREN_CLOS)
      return this->syntax_error (this, "awaiting (");
    c = la_next_token (this);
  }

  if (c isnot LA_TOKEN_BLOCK)
    return this->syntax_error (this, "parsing forever, not a block string");

  la_get_char (this);

  const char *tmp_ptr = la_StringGetPtr (this->curStrToken);

  integer bodylen = la_StringGetLen (this->curStrToken) - 1;
  char body[bodylen + 1];
  for (integer i = 0; i < bodylen; i++)
    body[i] = tmp_ptr[i];
  body[bodylen] = '\0';

  la_string body_str = la_StringNewLen (body, bodylen);

  la_string savepc = this->parsePtr;

  for (;;) {
    this->curState |= LOOP_STATE;
    this->curState &= ~(BREAK_STATE|CONTINUE_STATE);
    err= la_parse_string (this, body_str);
    this->curState &= ~LOOP_STATE;

    if (err is LA_ERR_BREAK or this->curState & BREAK_STATE) {
      la_next_token (this);
      goto theend;
    }

    if (this->didReturn) {
      this->curState &= ~(BREAK_STATE|CONTINUE_STATE|LOOP_STATE);
      la_StringSetLen (&this->parsePtr, 0);
      fun_release (&fun);
      return LA_OK;
    }

    if (err is LA_ERR_CONTINUE or this->curState & CONTINUE_STATE)
      continue;

    if (err isnot LA_OK) return err;
  }

theend:
  this->curScope = save_scope;
  fun_release (&fun);
  this->curState &= ~(BREAK_STATE|CONTINUE_STATE|LOOP_STATE);
  this->parsePtr = savepc;
  return LA_OK;
}

static int la_parse_visibility (la_t *this) {
  this->scopeState &= ~(PUBLIC_SCOPE|PRIVATE_SCOPE);
  int token = Cstring.eq_n (la_StringGetPtr (this->curStrToken), "public", 6)
      ? LA_TOKEN_PUBLIC : LA_TOKEN_PRIVATE;
  int scope = (token is LA_TOKEN_PUBLIC ? PUBLIC_SCOPE : PRIVATE_SCOPE);

  int c = la_next_token (this);

  switch (c) {
    case LA_TOKEN_ARYDEF:
    case LA_TOKEN_VARDEF:
    case LA_TOKEN_CONSTDEF:
    case LA_TOKEN_FUNCDEF:
      this->scopeState |= scope;
      break;

    default:
      if (this->curState & MAP_STATE) {
        this->scopeState |= scope;
        break;
      }

      if (c is LA_TOKEN_EOF)
        return this->syntax_error (this, "unended statement");
      return this->syntax_error (this, "unsupported visibility/scope attribute");
  }

  return token;
}

static int la_parse_arg_list (la_t *this, funT *uf) {
  int c;
  int nargs = 0;

  c = la_next_raw_token (this);

  for (;;) {
    if (c is LA_TOKEN_SYMBOL) {
      la_string name = this->curStrToken;

      if (nargs >= MAX_BUILTIN_PARAMS)
        return la_too_many_args (this);

      size_t len = la_StringGetLen (name);
      if (len >= MAXLEN_SYMBOL)
        return this->syntax_error (this, "argument name exceeded maximum length (64)");

      const char *ptr = la_StringGetPtr (name);
      Cstring.cp (uf->argName[nargs], MAXLEN_SYMBOL, ptr, len);

      nargs++;

      c = la_next_token (this);

      if (c is LA_TOKEN_PAREN_CLOS) break;

      if (c is LA_TOKEN_COMMA)
        c = la_next_raw_token (this);

    } else if (c is LA_TOKEN_PAREN_CLOS)
      break;
    else
      return this->syntax_error (this, "var definition, unexpected token");
  }

  uf->nargs = nargs;
  return nargs;
}

static int la_parse_func_def (la_t *this) {
  la_string name;
  int c;
  int err;
  int nargs = 0;
  size_t len = 0;

  char *fn = NULL;

  ifnot (this->curFunName[0]) {
    c = err = la_next_raw_token (this);
    if (c isnot LA_TOKEN_SYMBOL) {
      if (err is LA_ERR_SYNTAX)
        return err;
      else
        return this->syntax_error (this, "function definition, not a symbol");
    }

    name = this->curStrToken;
    len = la_StringGetLen (name);
    fn = sym_key (this, name);
    sym_t *sym = ns_lookup_symbol (this->std, fn);
    ifnot (NULL is sym)
      return this->syntax_error (this, "can not redefine a standard function");

  } else {
    name = la_StringNew (this->curFunName);
    len = bytelen (this->curFunName);
    if (len >= MAXLEN_SYMBOL)
      return this->syntax_error (this, "function name exceeded maximum length (64)");
    fn = this->curFunName;
  }

  funT *uf = Fun_new (this, funNew (
    .name = fn, .namelen = len, .parent = this->curScope
  ));

  c = la_next_token (this);

  if (c is LA_TOKEN_PAREN_OPEN) {
    la_fun_stack_push (this, this->curScope);

    this->curScope = uf;

    nargs = la_parse_arg_list (this, uf);

    this->curScope = la_fun_stack_pop (this);

    if (nargs < 0) return nargs;

    c = la_next_token (this);
  }

  if (c isnot LA_TOKEN_BLOCK) return this->syntax_error (this, "function definition, not a string");

  uf->body = this->curStrToken;

  VALUE v = PTR(uf);

  funT *scope = (this->scopeState & PUBLIC_SCOPE ? this->function : this->curScope);
  this->curSym = la_define_symbol (this, scope, fn, (UFUNC_TYPE | (nargs << 8)), v, 0);
  this->scopeState &= ~PUBLIC_SCOPE;

  this->curFunDef = uf;

  la_next_token (this);
  return LA_OK;
}

static int la_parse_fmt (la_t *this, string *str, int break_at_eof) {
  int c;
  int err = LA_NOTOK;
  int prev = 0;
  VALUE value;
  char directive = 'd';

  this->fmtRefcount++;

  for (;;) {
    c = la_get_char (this);
    if (c is LA_TOKEN_EOF and break_at_eof)
      break;

    if (c is LA_TOKEN_DQUOTE) {
      if (prev isnot LA_TOKEN_ESCAPE_CHR)
        break;

      String.append_byte (str, '\\');
      String.append_byte (str, LA_TOKEN_DQUOTE);
      prev = LA_TOKEN_DQUOTE;
      c = la_get_char (this);
    }

    if (c is LA_TOKEN_ESCAPE_CHR) {
      if (prev is LA_TOKEN_ESCAPE_CHR or la_peek_char (this, 0) isnot '$') {
        prev = str->bytes[str->num_bytes - 1];
        String.append_byte (str, LA_TOKEN_ESCAPE_CHR);
      } else
        prev = LA_TOKEN_ESCAPE_CHR;

      c = la_get_char (this);
    }

    if (c is '$') {
      if (prev is LA_TOKEN_ESCAPE_CHR) {
        String.append_byte (str, '$');
        prev = '$';
        continue;
      }

      c = la_get_char (this);
      if (c isnot LA_TOKEN_BLOCK_OPEN) {
        this->print_bytes (this->err_fp, "string fmt error, awaiting {\n");
        la_err_ptr (this, LA_NOTOK);
        err = LA_ERR_SYNTAX;
        goto theend;
      }

      prev = c;
      c = la_next_token (this);

      if (*(la_StringGetPtr (this->curStrToken)) is '%' and
            la_StringGetLen (this->curStrToken) is 1) {
        c = la_get_char (this);
        if (c isnot 's' and c isnot 'p' and c isnot 'd' and
            c isnot 'o' and c isnot 'x' and c isnot 'f') {
          this->print_fmt_bytes (this->err_fp, "string fmt error, unsupported directive [%c]\n", c);
          la_err_ptr (this, LA_NOTOK);
          err = LA_ERR_SYNTAX;
          goto theend;
        } else
          directive = c;

        c = la_next_token (this);

        if (c isnot LA_TOKEN_COMMA) {
          this->print_fmt_bytes (this->err_fp, "string fmt error, awaiting a comma\n");
          la_err_ptr (this, LA_NOTOK);
          err = LA_ERR_SYNTAX;
          goto theend;
        }

        prev = LA_TOKEN_COMMA;
        c = la_next_token (this);
      }

      this->funcState |= EXPR_LIST_STATE;
      err = la_parse_expr (this, &value);
      this->funcState &= ~EXPR_LIST_STATE;

      if (err isnot LA_OK) {
        this->print_bytes (this->err_fp, "string fmt error, while evaluating expression\n");
        la_err_ptr (this, LA_NOTOK);
        err = LA_ERR_SYNTAX;
        goto theend;
      }

      switch (directive) {
        case 's':
          switch (value.type) {
            case STRING_TYPE:
              String.append_with_fmt (str, "%s", AS_STRING_BYTES(value));
              if ((this->fmtState & FMT_LITERAL) or (value.sym is NULL and
                  0 is ns_is_malloced_string (this->curScope, AS_STRING(value)) and
                  0 is (this->objectState & ARRAY_MEMBER))) {
                la_free (this, value);
              }

              this->fmtState &= ~FMT_LITERAL;
              this->objectState &= ~ARRAY_MEMBER;
              break;

            case NULL_TYPE:
              String.append_with_len (str, "(null)", 6);
              break;

            case ARRAY_TYPE: {
              VALUE v;
              this->tokenValue = value;
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
               0 is ns_is_malloced_string (this->curScope, AS_STRING(value)) and
               0 is (this->objectState & ARRAY_MEMBER))) {
                la_free (this, value);
            }
            this->fmtState &= ~FMT_LITERAL;
            this->objectState &= ~ARRAY_MEMBER;
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
              this->tokenValue = value;
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
              this->tokenValue = value;
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
              this->tokenValue = value;
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

      directive = 'd';

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

  string *str = String.new (32);

  int c = la_ignore_ws (this);

  if (c isnot LA_TOKEN_PAREN_OPEN) {
    this->print_bytes (this->err_fp, "string fmt error, awaiting (\n");
    la_err_ptr (this, LA_NOTOK);
    goto theend;
  }

  c = la_ignore_ws (this);

  FILE *fp = this->out_fp;

  if (c isnot LA_TOKEN_DQUOTE) {
    la_unget_char (this);
    c = la_next_token (this);
    VALUE v;
    err = la_parse_expr (this, &v);
    if (err isnot LA_OK) {
      this->print_bytes (this->err_fp, "string fmt error, awaiting a file ptr value\n");
      la_err_ptr (this, LA_NOTOK);
      goto theend;
    }

    if (v.type is STRING_TYPE) {
      string *vs = AS_STRING(v);
      String.append_with_len (str, vs->bytes, vs->num_bytes);
      c = this->curToken;
      goto print_str;
    }

    if (v.type isnot OBJECT_TYPE) {
      this->print_bytes (this->err_fp, "string fmt error, awaiting a file ptr value\n");
      la_err_ptr (this, LA_NOTOK);
      this->print_fp = fp;
      goto theend;
    }

    fp = AS_FILEPTR(v);
    la_ignore_next_char (this);
    c = la_ignore_ws (this);

    if (c isnot LA_TOKEN_DQUOTE) {
      la_unget_char (this);
      c = la_next_token (this);
      err = la_parse_expr (this, &v);
      if (err isnot LA_OK or v.type isnot STRING_TYPE) {
        this->print_bytes (this->err_fp, "string fmt error, awaiting a string value\n");
        la_err_ptr (this, LA_NOTOK);
        goto theend;
      }

      string *vs = AS_STRING(v);
      String.append_with_len (str, vs->bytes, vs->num_bytes);
      c = this->curToken;
      goto print_str;
    }
  }

  this->print_fp = fp;

  if (c isnot LA_TOKEN_DQUOTE) {
    this->print_bytes (this->err_fp, "string fmt error, awaiting double quote\n");
    la_err_ptr (this, LA_NOTOK);
    goto theend;
  }

  err = la_parse_fmt (this, str, 0);
  if (err isnot LA_OK) goto theend;

  c = la_get_char (this);

print_str:
  if (c isnot LA_TOKEN_PAREN_CLOS) {
    this->print_bytes (this->err_fp, "string fmt error, awaiting )\n");
    la_err_ptr (this, LA_NOTOK);
    err = LA_ERR_SYNTAX;
    goto theend;
  }

  if (LA_NOTOK is this->print_bytes (fp, str->bytes)) {
    this->print_bytes (this->err_fp, "error while printing string\n");
    fprintf (this->err_fp, "%s\n", str->bytes);
    goto theend;
  }

  la_next_token (this);

  err = LA_OK;

theend:
  String.release (str);
  return err;
}

static int la_parse_println (la_t *this) {
  int err = la_parse_print (this);
  if (err is LA_OK)
    fprintf (this->print_fp, "\n");
  return err;
}

static int la_parse_exit (la_t *this) {
  la_next_token (this);

  VALUE v;
  la_parse_expr (this, &v);
  this->exitValue = AS_INT(v);

  la_StringSetLen (&this->parsePtr, 0);
  this->didReturn = 1;

  return LA_ERR_EXIT;
}

static int la_parse_return (la_t *this) {
  int err;
  la_next_token (this);

  funT *scope = this->curScope;
  while (scope and Cstring.eq (scope->funname, NS_BLOCK))
    scope = scope->prev;

  if (NULL is scope)
    return this->syntax_error (this, "error while parsing return, unknown scope");

  err = la_parse_expr (this, &scope->result);

  la_StringSetLen (&this->parsePtr, 0);
  this->didReturn = 1;

  return err;
}

static int la_import_file (la_t *this, const char *module, const char *err_msg) {
  int err = LA_NOTOK;

  void *handle = dlopen (module, RTLD_NOW|RTLD_GLOBAL);
  if (handle is NULL) {
    err_msg = dlerror ();
    ifnot (this->curState & LOADFILE_SILENT)
      this->print_fmt_bytes (this->err_fp, "import error, %s\n", err_msg);

    return LA_ERR_IMPORT;
  }

  char *mname = Path.basename_sans_extname ((char *) module);
  size_t len = bytelen (mname) - 7;
  char tmp[len+1];
  Cstring.substr (tmp, len, mname, len + 7, 0);
  string *init_fun = String.new_with_fmt ("__init_%s_module__", tmp);
  string *deinit_fun = String.new_with_fmt ("__deinit_%s_module__", tmp);

  dlerror ();
  ModuleInit init_sym = (ModuleInit) dlsym (handle, init_fun->bytes);
  err_msg = dlerror ();
  if (err_msg isnot NULL) {
    this->print_fmt_bytes (this->err_fp, "import error, %s\n", err_msg);
    err = LA_ERR_DYNLINK;
    goto theend;
  }

  dlerror ();
  ModuleDeinit deinit_sym = (ModuleDeinit) dlsym (handle, deinit_fun->bytes);
  err_msg = dlerror ();
  if (err_msg isnot NULL) {
    this->print_fmt_bytes (this->err_fp, "import error, %s\n", err_msg);
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
  modl->handle = handle;
  modl->init = init_sym;
  modl->deinit = deinit_sym;
  ListStackPush(this->function->modules, modl);
  err = LA_OK;

theend:
  free (mname);
  String.release (init_fun);
  String.release (deinit_fun);
  return err;
}

static int la_parse_import (la_t *this) {
#ifdef STATIC
  return this->syntax_error (this, "import is disabled in static objects");
#endif

  char *err_msg = "";
  int err;
  int c = la_next_token (this);
  ifnot (c is LA_TOKEN_PAREN_OPEN)
    return this->syntax_error (this, "error while parsing import(), awaiting (");

  VALUE v;
  err = la_parse_expr (this, &v);

  if (err isnot LA_OK)
    return this->syntax_error (this, "error while parsing import()");

  ifnot (v.type is STRING_TYPE)
    return this->syntax_error (this, "error while parsing import(), awaiting a string");

  string *fname = AS_STRING(v);
  string *fn = NULL;

  if (this->curState & LITERAL_STRING_STATE)
    this->curState &= ~LITERAL_STRING_STATE;
  else
    fname = String.dup (fname);

  String.append_with (fname, "-module.so");

  string *ns = NULL;
  funT *load_ns = this->curScope;
  funT *prev_ns = load_ns;

  if (this->curToken is LA_TOKEN_COMMA) {
    la_next_token (this);
    err = la_parse_expr (this, &v);
    ifnot (v.type is STRING_TYPE)
      return this->syntax_error (this, "error while parsing import() ns, awaiting a string");

    ns = AS_STRING(v);

    if (this->curState & LITERAL_STRING_STATE)
      this->curState &= ~LITERAL_STRING_STATE;
    else
      ns = String.dup (ns);

    ifnot (ns->num_bytes)
      goto theload;

    la_string x = la_StringNew (ns->bytes);
    sym_t *symbol = la_lookup_symbol (this, x);
    if (symbol isnot NULL) {
      v = symbol->value;
      if (v.type & FUNCPTR_TYPE) {
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

    sym_t *symbol = la_lookup_symbol (this, la_StringNew ("__importpath"));
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
  this->curScope = prev_ns;
  String.release (fname);
  String.release (fn);
  String.release (ns);

  if (err isnot LA_OK) {
    if (err isnot LA_ERR_DYNLINK)
      err = LA_ERR_IMPORT;

    this->print_fmt_bytes (this->err_fp, "import error: %s\n", err_msg);
  }

  return err;
}

static int la_parse_loadfile (la_t *this) {
  int err;
  int c = la_next_token (this);
  ifnot (c is LA_TOKEN_PAREN_OPEN)
    return this->syntax_error (this, "error while parsing loadfile(), awaiting (");

  VALUE v;
  err = la_parse_expr (this, &v);

  if (err isnot LA_OK)
    return this->syntax_error (this, "error while parsing loadfile()");

  ifnot (v.type is STRING_TYPE)
    return this->syntax_error (this, "error while parsing loadfile(), awaiting a string");

  string *fname = AS_STRING(v);
  string *fn = NULL;

  if (this->curState & LITERAL_STRING_STATE)
    this->curState &= ~LITERAL_STRING_STATE;
  else
    fname = String.dup (fname);

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

  string *ns = NULL;
  funT *load_ns = this->curScope;
  funT *prev_ns = load_ns;

  if (this->curToken is LA_TOKEN_COMMA) {
    la_next_token (this);
    err = la_parse_expr (this, &v);
    ifnot (v.type is STRING_TYPE)
      return this->syntax_error (this, "error while parsing loadfile() ns, awaiting a string");

    ns = AS_STRING(v);

    if (this->curState & LITERAL_STRING_STATE)
      this->curState &= ~LITERAL_STRING_STATE;
    else
      ns = String.dup (ns);

    ifnot (ns->num_bytes)
      goto theload;

    la_string x = la_StringNew (ns->bytes);
    sym_t *symbol = la_lookup_symbol (this, x);
    if (symbol isnot NULL) {
      v = symbol->value;
      if (v.type & FUNCPTR_TYPE) {
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
      String.prepend_with_fmt (fn, "%s/scripts/", this->la_dir->bytes);
      this->curState |= LOADFILE_SILENT;
      err = la_eval_file (this, fn->bytes);
      this->curState &= ~LOADFILE_SILENT;
      if (err isnot LA_ERR_LOAD)
        goto theend;
    }

    String.release (fn);
    fn = String.dup (fname);
    String.prepend_with (fn, "/data/la/scripts/");
    err = la_eval_file (this, fn->bytes);
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
  (void) this;
  VALUE result = FALSE_VALUE;

  switch (x.type) {
    case NUMBER_TYPE:
      switch (y.type) {
        case NUMBER_TYPE:
          result = INT(AS_NUMBER(x) == AS_NUMBER(y)); goto theend;
        case INTEGER_TYPE:
          result = INT(AS_NUMBER(x) == AS_INT(y)); goto theend;
        case NULL_TYPE: goto theend;
        default:
          this->CFuncError = LA_ERR_TYPE_MISMATCH;
          Cstring.cp_fmt (this->curMsg, MAXLEN_MSG + 1,
              "NumberType == %s is not possible",
              AS_STRING_BYTES(la_typeAsString (this, y)));
          goto theend;
      }
      goto theend;

    case INTEGER_TYPE:
      switch (y.type) {
        case NUMBER_TYPE:
          result = INT(AS_INT(x) == AS_NUMBER(y)); goto theend;
        case INTEGER_TYPE:
          result = INT(AS_INT(x) == AS_INT(y)); goto theend;
        case NULL_TYPE: goto theend;
        default:
          this->CFuncError = LA_ERR_TYPE_MISMATCH;
          Cstring.cp_fmt (this->curMsg, MAXLEN_MSG + 1,
              "IntegerType == %s is not possible",
              AS_STRING_BYTES(la_typeAsString (this, y)));
          goto theend;
      }
      goto theend;

    case STRING_TYPE:
      switch (y.type) {
        case STRING_TYPE:
          result = INT(Cstring.eq (AS_STRING_BYTES(x), AS_STRING_BYTES(y)));
          goto theend;
        case NULL_TYPE: goto theend;
        default:
          this->CFuncError = LA_ERR_TYPE_MISMATCH;
          Cstring.cp_fmt (this->curMsg, MAXLEN_MSG + 1,
              "StringType == %s is not possible",
              AS_STRING_BYTES(la_typeAsString (this, y)));
          goto theend;
      }
      goto theend;

    case ARRAY_TYPE:
      switch (y.type) {
        case ARRAY_TYPE:
          result = INT(la_array_eq (x, y));
          goto theend;
        case NULL_TYPE: goto theend;
        default:
          this->CFuncError = LA_ERR_TYPE_MISMATCH;
          Cstring.cp_fmt (this->curMsg, MAXLEN_MSG + 1,
              "ArrayType == %s is not possible",
              AS_STRING_BYTES(la_typeAsString (this, y)));
          goto theend;
      }
      goto theend;

    case NULL_TYPE:
      switch (y.type) {
        case NULL_TYPE: result = TRUE_VALUE; goto theend;
        default: goto theend;
      }

    default:
      this->CFuncError = LA_ERR_TYPE_MISMATCH;
      Cstring.cp_fmt (this->curMsg, MAXLEN_MSG + 1,
         "%s == %s is not possible",
         AS_STRING_BYTES(la_typeAsString (this, x)),
         AS_STRING_BYTES(la_typeAsString (this, y)));
      goto theend;
  }

theend:
  return result;
}

static VALUE la_ne (la_t *this, VALUE x, VALUE y) {
  (void) this;
  VALUE result = la_equals (this, x, y);
  result = INT(0 == AS_INT(result));
  return result;
}

static VALUE la_lt (la_t *this, VALUE x, VALUE y) {
  (void) this;
  VALUE result = FALSE_VALUE;

  switch (x.type) {
    case NUMBER_TYPE:
      switch (y.type) {
        case NUMBER_TYPE:
          result = INT(AS_NUMBER(x) < AS_NUMBER(y)); goto theend;
        case INTEGER_TYPE:
          result = INT(AS_NUMBER(x) < AS_INT(y)); goto theend;
      }
      goto theend;

    case INTEGER_TYPE:
      switch (y.type) {
        case NUMBER_TYPE:
          result = INT(AS_INT(x) < AS_NUMBER(y)); goto theend;
        case INTEGER_TYPE:
          result = INT(AS_INT(x) < AS_INT(y)); goto theend;
      }
  }

theend:
  return result;
}

static VALUE la_le (la_t *this, VALUE x, VALUE y) {
  (void) this;
  VALUE result = FALSE_VALUE;

  switch (x.type) {
    case NUMBER_TYPE:
      switch (y.type) {
        case NUMBER_TYPE:
          result = INT(AS_NUMBER(x) <= AS_NUMBER(y)); goto theend;
        case INTEGER_TYPE:
          result = INT(AS_NUMBER(x) <= AS_INT(y)); goto theend;
      }
      goto theend;

    case INTEGER_TYPE:
      switch (y.type) {
        case NUMBER_TYPE:
          result = INT(AS_INT(x) <= AS_NUMBER(y)); goto theend;
        case INTEGER_TYPE:
          result = INT(AS_INT(x) <= AS_INT(y)); goto theend;
      }
  }

theend:
  return result;
}

static VALUE la_gt (la_t *this, VALUE x, VALUE y) {
  (void) this;
  VALUE result = FALSE_VALUE;

  switch (x.type) {
    case NUMBER_TYPE:
      switch (y.type) {
        case NUMBER_TYPE:
          result = INT(AS_NUMBER(x) > AS_NUMBER(y)); goto theend;
        case INTEGER_TYPE:
          result = INT(AS_NUMBER(x) > AS_INT(y)); goto theend;
      }
      goto theend;

    case INTEGER_TYPE:
      switch (y.type) {
        case NUMBER_TYPE:
          result = INT(AS_INT(x) > AS_NUMBER(y)); goto theend;
        case INTEGER_TYPE:
          result = INT(AS_INT(x) > AS_INT(y)); goto theend;
      }
  }

theend:
  return result;
}

static VALUE la_ge (la_t *this, VALUE x, VALUE y) {
  (void) this;
  VALUE result = FALSE_VALUE;

  switch (x.type) {
    case NUMBER_TYPE:
      switch (y.type) {
        case NUMBER_TYPE:
          result = INT(AS_NUMBER(x) >= AS_NUMBER(y)); goto theend;
        case INTEGER_TYPE:
          result = INT(AS_NUMBER(x) >= AS_INT(y)); goto theend;
      }
      goto theend;

    case INTEGER_TYPE:
      switch (y.type) {
        case NUMBER_TYPE:
          result = INT(AS_INT(x) >= AS_NUMBER(y)); goto theend;
        case INTEGER_TYPE:
          result = INT(AS_INT(x) >= AS_INT(y)); goto theend;
      }
  }

theend:
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
          result = NUMBER(AS_NUMBER(x) + AS_NUMBER(y)); goto theend;
        case INTEGER_TYPE:
          result = NUMBER(AS_NUMBER(x) + AS_INT(y)); goto theend;
      }
      goto theend;

    case INTEGER_TYPE:
      switch (y.type) {
        case NUMBER_TYPE:
          result = NUMBER(AS_INT(x) + AS_NUMBER(y)); goto theend;
        case INTEGER_TYPE:
          result = INT(AS_INT(x) + AS_INT(y)); goto theend;
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

      goto theend;

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
          } else {
            string *new = String.new_with_len (x_str->bytes, x_str->num_bytes);
            String.append_with_len (new, y_str->bytes, y_str->num_bytes);
            result = STRING(new);

            if (x.sym is NULL and 0 is ns_is_malloced_string (this->curScope, x_str))
              String.release (x_str);
          }

          if (y.sym is NULL and 0 is ns_is_malloced_string (this->curScope, y_str))
            String.release (y_str);

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
            if (x.sym is NULL and 0 is ns_is_malloced_string (this->curScope, x_str)
                and 0 is (this->curState & LITERAL_STRING_STATE))
              String.release (x_str);
          }

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
          goto theend;
        }
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

static VALUE la_not (la_t *this, VALUE value) {
  (void) this;
  VALUE result = INT(!AS_INT(value));
  return result;
}

static VALUE la_bool (la_t *this, VALUE value) {
  (void) this;
  VALUE result = INT(!!AS_INT(value));
  return result;
}

static struct def {
  const char *name;
  int toktype;
  VALUE val;
} la_defs[] = {
  { "var",     LA_TOKEN_VARDEF,   NULL_VALUE },
  { "const",   LA_TOKEN_CONSTDEF, NULL_VALUE },
  { "else",    LA_TOKEN_ELSE,     NULL_VALUE },
  { "elseif",  LA_TOKEN_ELSEIF,   NULL_VALUE },
  { "break",   LA_TOKEN_BREAK,    NULL_VALUE },
  { "continue",LA_TOKEN_CONTINUE, NULL_VALUE },
  { "if",      LA_TOKEN_IF,       PTR(la_parse_if) },
  { "ifnot",   LA_TOKEN_IFNOT,    PTR(la_parse_if) },
  { "while",   LA_TOKEN_WHILE,    PTR(la_parse_while) },
  { "do",      LA_TOKEN_DO,       PTR(la_parse_do) },
  { "for",     LA_TOKEN_FOR,      PTR(la_parse_for) },
  { "forever", LA_TOKEN_FOREVER,  PTR(la_parse_forever) },
  { "loop",    LA_TOKEN_LOOP,     PTR(la_parse_loop) },
  { "print",   LA_TOKEN_PRINT,    PTR(la_parse_print) },
  { "println", LA_TOKEN_PRINTLN,  PTR(la_parse_println) },
  { "func",    LA_TOKEN_FUNCDEF,  PTR(la_parse_func_def) },
  { "return",  LA_TOKEN_RETURN,   PTR(la_parse_return) },
  { "exit",    LA_TOKEN_EXIT,     PTR(la_parse_exit) },
  { "loadfile",LA_TOKEN_LOADFILE, PTR(la_parse_loadfile) },
  { "import",  LA_TOKEN_IMPORT,   PTR(la_parse_import) },
  { "array",   LA_TOKEN_ARYDEF,   PTR(la_parse_array_def) },
  { "public",  LA_TOKEN_PUBLIC,   PTR(la_parse_visibility) },
  { "private", LA_TOKEN_PRIVATE,  PTR(la_parse_visibility) },
  { "*",       BINOP(1),          PTR(la_mul) },
  { "/",       BINOP(1),          PTR(la_div) },
  { "%",       BINOP(1),          PTR(la_mod) },
  { "+",       BINOP(2),          PTR(la_add) },
  { "-",       BINOP(2),          PTR(la_sub) },
  { "&",       BINOP(3),          PTR(la_bitand) },
  { "|",       BINOP(3),          PTR(la_bitor) },
  { "^",       BINOP(3),          PTR(la_bitxor) },
  { ">>",      BINOP(3),          PTR(la_shr) },
  { "<<",      BINOP(3),          PTR(la_shl) },
  { "==",      BINOP(4),          PTR(la_equals) },
  { "is",      BINOP(4),          PTR(la_equals) },
  { "!=",      BINOP(4),          PTR(la_ne) },
  { "isnot",   BINOP(4),          PTR(la_ne) },
  { "<",       BINOP(4),          PTR(la_lt) },
  { "<=",      BINOP(4),          PTR(la_le) },
  { ">",       BINOP(4),          PTR(la_gt) },
  { ">=",      BINOP(4),          PTR(la_ge) },
  { "&&",      BINOP(5),          PTR(la_logical_and) },
  { "and",     BINOP(5),          PTR(la_logical_and) },
  { "||",      BINOP(5),          PTR(la_logical_or) },
  { "or",      BINOP(5),          PTR(la_logical_or) },
  { "=",       LA_TOKEN_ASSIGN,     NULL_VALUE },
  { "+=",      LA_TOKEN_ASSIGN_APP, NULL_VALUE },
  { "-=",      LA_TOKEN_ASSIGN_SUB, NULL_VALUE },
  { "/=",      LA_TOKEN_ASSIGN_DIV, NULL_VALUE },
  { "*=",      LA_TOKEN_ASSIGN_MUL, NULL_VALUE },
  { "&=",      LA_TOKEN_ASSIGN_AND, NULL_VALUE },
  { "|=",      LA_TOKEN_ASSIGN_BAR, NULL_VALUE },
  { "NullType",    INTEGER_TYPE,  INT(NULL_TYPE) },
  { "NumberType",  INTEGER_TYPE,  INT(NUMBER_TYPE) },
  { "IntegerType", INTEGER_TYPE,  INT(INTEGER_TYPE) },
  { "FunctionType",INTEGER_TYPE,  INT(FUNCPTR_TYPE) },
  { "CFunctionType",INTEGER_TYPE,  INT(CFUNCTION_TYPE) },
  { "StringType",  INTEGER_TYPE,  INT(STRING_TYPE) },
  { "ArrayType",   INTEGER_TYPE,  INT(ARRAY_TYPE) },
  { "ObjectType",  INTEGER_TYPE,  INT(OBJECT_TYPE) },
  { "MapType",     INTEGER_TYPE,  INT(MAP_TYPE) } ,
  { "ok",          INTEGER_TYPE,  OK_VALUE },
  { "notok",       INTEGER_TYPE,  NOTOK_VALUE },
  { "true",        INTEGER_TYPE,  TRUE_VALUE },
  { "false",       INTEGER_TYPE,  FALSE_VALUE },
  { "null",        NULL_TYPE,     NULL_VALUE },
  { NULL,          NULL_TYPE,     NULL_VALUE }
};

LaDefCFun la_funs[] = {
  { "not",              PTR(la_not), 1},
  { "len",              PTR(la_len), 1},
  { "bool",             PTR(la_bool), 1},
  { "free",             PTR(la_free), 1},
  { "malloc",           PTR(la_malloc), 1},
  { "realloc",          PTR(la_realloc), 2},
  { "format",           PTR(la_format), 1},
  { "fopen",            PTR(la_fopen), 2},
  { "fflush",           PTR(la_fflush), 1},
  { "getkey",           PTR(la_getkey), 1},
  { "fileno",           PTR(la_fileno), 1},
  { "getcwd",           PTR(la_getcwd), 0},
  { "typeof",           PTR(la_typeof), 1},
  { "typeAsString",     PTR(la_typeAsString), 1},
  { "typeofArray",      PTR(la_typeofArray), 1},
  { "typeArrayAsString",PTR(la_typeArrayAsString), 1},
  /* testing or undecided */
  { "term_new",         PTR(la_term_new), 0},
  { "term_raw_mode",    PTR(la_term_raw_mode), 1},
  { "term_sane_mode",   PTR(la_term_sane_mode), 1},
  { NULL,               NULL_VALUE, NULL_TYPE},
};

static int la_std_def (la_t *this, la_opts opts) {
  VALUE v = OBJECT(opts.out_fp);
  object *o = la_object_new (NULL, NULL, v);
  int err = la_define (this, "stdout", OBJECT_TYPE, OBJECT(o));
  if (err) return LA_NOTOK;

  v = OBJECT(opts.err_fp);
  o = la_object_new (NULL, NULL, v);
  err = la_define (this, "stderr", OBJECT_TYPE, OBJECT(o));
  if (err) return LA_NOTOK;

  v= OBJECT(stdin);
  o = la_object_new (NULL, NULL, v);
  err = la_define (this, "stdin", OBJECT_TYPE, OBJECT(o));
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

#ifdef STATIC
  v = INT(1);
#else
  v = INT(0);
#endif

  err = la_define (this, "__static", INTEGER_TYPE, v);
  if (err) return LA_NOTOK;

  int len = 1;
  if (this->la_dir->num_bytes) len++;
  #ifdef ZLIBDIR
    len += 2;
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
    String.replace_with_fmt (arimp[ind++], "%s/la-modules", ZLIBDIR);
  #endif
  #ifdef LIBDIR
    String.replace_with_fmt (arimp[ind++], "%s/la-modules", LIBDIR);
  #endif
  String.replace_with (arimp[ind], "/lib/la-modules");

  v = ARRAY(imp_path);
  err = la_define (this, "__importpath", ARRAY_TYPE, v);
  if (err) return LA_NOTOK;

  string *loadpath = String.dup (this->la_dir);
  v = STRING(loadpath);
  err = la_define (this, "__loadpath", STRING_TYPE, v);

  return err;
}

/* ABSTRACTION CODE */

static int la_print_bytes (FILE *fp, const char *bytes) {
  if (NULL is bytes) return 0;
  string *parsed = IO.parse_escapes ((char *)bytes);
  if (NULL is parsed) return LA_NOTOK;
  int nbytes = fprintf (fp, "%s", parsed->bytes);
  String.release (parsed);
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

static VALUE I_print_bytes (la_t *this, char *bytes) {
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

  la_string x = la_StringNew (buf);

  string *file = AS_STRING(this->file->value);
  integer len =  file->num_bytes;
  char prev_file[len + 1];
  Cstring.cp (prev_file, len + 1, file->bytes, len);
  String.replace_with (file, LA_STRING_NS);

  int prev_linenum = this->lineNum;
  this->lineNum = 0;

  int retval = la_parse_string (this, x);

  this->lineNum = prev_linenum;

  String.replace_with_len (file, prev_file, len);

  this->script_buffer = prev_buffer;

  if (retval is LA_ERR_EXIT)
    return this->exitValue;

  return retval;
}

static int la_eval_expr (la_t *this, const char *buf, VALUE *v) {
  if (*buf isnot LA_TOKEN_PAREN_OPEN)
    return this->syntax_error (this, "awaiting (");

  this->script_buffer = buf;
  la_string x = la_StringNew (buf);

  this->parsePtr = x;
  this->curToken = LA_TOKEN_PAREN_OPEN;

  int retval = la_parse_expr (this, v);

  return retval;
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

  if (Vmap.key_exists (this->units, fn))
    ifnot (this->curState & FORCE_LOADFILE)
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

  string *script_buf = String.new_with_len (script, r);
  Vmap.set (this->units, fn, script_buf, la_release_unit, 0);

  const char *prev_buffer = this->script_buffer;
  this->script_buffer = script_buf->bytes;

  string *file = AS_STRING(this->file->value);
  integer len =  file->num_bytes;
  char prev_file[len + 1];
  Cstring.cp (prev_file, len + 1, file->bytes, len);
  String.replace_with (file, fn);

  int prev_linenum = this->lineNum;
  this->lineNum = 0;

  la_string x = la_StringNew (script_buf->bytes);

  int retval = la_parse_string (this, x);

  this->lineNum = prev_linenum;

  String.replace_with_len (file, prev_file, len);

  this->script_buffer = prev_buffer;

  if (retval is LA_ERR_EXIT or retval >= LA_NOTOK)
    return retval;

  char *err_msg[] = {
      "NO MEMORY", "SYNTAX ERROR", "UNKNOWN SYMBOL",
      "UNKNOWN TYPE", "BAD ARGUMENTS", "TOO MANY ARGUMENTS",
      "OUT OF BOUNDS", "TYPE MISMATCH", "LOAD ERROR", "IMPORT ERROR",
      "DYNAMIC LINKING ERROR"
  };
  this->print_fmt_bytes (this->err_fp, "%s\n", err_msg[-retval - 2]);
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

static void la_release (la_t **thisp) {
  if (NULL is *thisp) return;
  la_t *this = *thisp;

  String.release (this->la_dir);
  String.release (this->message);
  Imap.release   (this->funRefcount);
  Vmap.release   (this->units);
  fun_release (&this->function);
  fun_release (&this->std);

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

static char *la_name_gen (char *name, int *name_gen, char *prefix, size_t prelen) {
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

static void la_set_la_dir (la_t *this, char *fn) {
  if (NULL is fn) return;
  size_t len = bytelen (fn);
  String.replace_with_len (this->la_dir, fn, len);
  sym_t *sym = ns_lookup_symbol (this->std, "__loadpath");
  string *loadpath = AS_STRING(sym->value);
  String.replace_with_len (loadpath, fn, len);
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

public la_T *la_get_root (la_t *this) {
  return this->root;
}

static char *la_get_eval_str (la_t *this) {
  return (char *) la_StringGetPtr (this->parsePtr);
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

  this->didReturn = 0;
  this->exitValue = LA_OK;
  this->curState = 0;
  this->funcState = 0;
  this->objectState = 0;
  this->scopeState = 0;
  this->stackValIdx = -1;
  this->anon_id = 0;

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

  Fun_new (this,
      funNew (.name = NS_GLOBAL, .namelen = NS_GLOBAL_LEN, .num_symbols = 256));

  for (i = 0; la_defs[i].name; i++) {
    err = la_define (this, la_defs[i].name, la_defs[i].toktype, la_defs[i].val);

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

  la_append_instance (interp, this);

  return LA_OK;
}

static la_t *la_init_instance (la_T *__la__, la_opts opts) {
  la_t *this = la_new (__la__);

  la_init (__la__, this, opts);

  this->root = __la__;

  return this;
}

static int la_loadfile (la_t *this, char *fn) {
  int err = la_eval_file (this, fn);
  if (err is LA_ERR_EXIT)
    return this->exitValue;
  return err;
}

static int la_load_file (la_T *__la__, la_t *this, char *fn) {
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
  __INIT__ (term);
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
      .get = (la_get_self) {
        .root = la_get_root,
        .message = la_get_message,
        .current = la_get_current,
        .eval_str = la_get_eval_str,
        .user_data = la_get_user_data,
        .current_idx = la_get_current_idx,
      },
      .set = (la_set_self) {
        .la_dir = la_set_la_dir,
        .current = la_set_current,
        .user_data = la_set_user_data,
        .define_funs_cb = la_set_define_funs_cb
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
