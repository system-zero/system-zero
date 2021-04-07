/* Derived from the Tinyscript project at:
 * https://github.com/totalspectrum/ (see LICENSE included in this directory)
 *
 * See data/docs/la.md for details about syntax and semantics.
 */

#define LIBRARY "la"

#define REQUIRE_STDIO
#define REQUIRE_STDARG

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

#ifdef DEBUG

static  int  CURIDX = 0;
static  char PREVFUNC[MAXLEN_SYMBOL_LEN + 1];

#define $CUR_IDX      CURIDX
#define $PREV_FUNC    PREVFUNC
#define $CUR_FUNC     __func__
#define $CUR_SCOPE    this->curScope->funname
#define $CUR_TOKEN    this->curToken
#define $CUR_VALUE    this->tokenValue.type
//#define $CUR_VALUE    (VALUE) this->tokenValue
#define $CODE_PATH   fprintf (this->err_fp,                                     \
  "CurIdx   : %d,  PrevFunc : %s,\n"                                            \
  "CurFunc  : %s,  CurScope : %s,\n"                                            \
  "CurToken : ['%c', %d], CurValueType : %d\n",                                    \
  $CUR_IDX++, $PREV_FUNC,                                                       \
  $CUR_FUNC, $CUR_SCOPE, $CUR_TOKEN, $CUR_TOKEN, $CUR_VALUE);                   \
  Cstring.cp ($PREV_FUNC, MAXLEN_SYMBOL_LEN + 1, $CUR_FUNC, MAXLEN_SYMBOL_LEN); \
  fprintf (this->err_fp, "CurStringToken : ['");                                \
  la_print_lastring (this, this->err_fp, this->curStrToken);                      \
  fprintf (this->err_fp, "']\n\n");
#endif

#define STRING_LITERAL_ARG_STATE      (1 << 0)
#define LOOP_STATE                    (1 << 1)
#define BREAK_STATE                   (1 << 2)
#define CONTINUE_STATE                (1 << 3)

#define BINOP(x) (((x) << 8) + BINOP_TYPE)
#define CFUNC(x) (((x) << 8) + CFUNC_TYPE)

#define CFUNC_TYPE          'B'  // builtin: number of operands in high 8 bits
#define UFUNC_TYPE          'f'
#define BINOP_TYPE          'o'

#define LA_TOKEN_SYMBOL     'A'
#define LA_TOKEN_BUILTIN    'B'
#define LA_TOKEN_CHAR       'C'
#define LA_TOKEN_FUNCDEF    'F'
#define LA_TOKEN_IFNOT      'I'
#define LA_TOKEN_LOOP       'L'
#define LA_TOKEN_CONTINUE   'O'
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
#define LA_TOKEN_IF         'i'
#define LA_TOKEN_FOR        'l'
#define LA_TOKEN_FOREVER    'm'
#define LA_TOKEN_NUMBER     'n'
#define LA_TOKEN_BINOP      'o'
#define LA_TOKEN_VAR        'v'
#define LA_TOKEN_WHILE      'w'
#define LA_TOKEN_HEX_NUMBER 'x'
#define LA_TOKEN_ARY        'y'
#define LA_TOKEN_BLOCK      'z'
#define LA_TOKEN_INDEX_OPEN '['
#define LA_TOKEN_INDEX_CLOS ']'
#define LA_TOKEN_PAREN_OPEN '('
#define LA_TOKEN_PAREN_CLOS ')'
#define LA_TOKEN_BLOCK_OPEN '{'
#define LA_TOKEN_BLOCK_CLOS '}'
#define LA_TOKEN_SEMICOLON  ';'
#define LA_TOKEN_COMMA      ','
#define LA_TOKEN_DQUOTE     '"'
#define LA_TOKEN_SQUOTE     '\''
#define LA_TOKEN_NL         '\n'
#define LA_TOKEN_SLASH      '\\'
#define LA_TOKEN_LINE_CONT  LA_TOKEN_SLASH
#define LA_TOKEN_ESCAPE_CHR LA_TOKEN_SLASH
#define LA_TOKEN_NONE       '0'

typedef struct la_string {
  uint len_;
  const char *ptr_;
} la_string;

typedef struct malloced_string malloced_string;

struct malloced_string {
  string_t *data;
  malloced_string *next;
};

typedef struct sym_t {
  int type;
  int is_const;
  VALUE value;
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

struct funType {
  char funname[MAXLEN_SYMBOL_LEN + 1];

  la_string body;

  int nargs;
  char argName[MAX_BUILTIN_PARAMS][MAXLEN_SYMBOL_LEN + 1];

  Vmap_t *symbols;

  malloced_string *head;

  funT
    *root,
    *prev,
    *next;
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
  funT *curScope;

  fun_stack funstack[1];
  symbol_stack symbolstack[1];
  Imap_t *refcount;

  char name[32];

  const char *script_buffer;

  char symKey[MAXLEN_SYMBOL_LEN + 1];
  char curFunName[MAXLEN_SYMBOL_LEN + 1];
  funT *curFunDef;

  sym_t *curSym;

  int
    curState,
    exitValue,
    lineNum,
    curToken,    // kind of current token
    tokenArgs,   // number of arguments for this token
    didReturn;

  string_t
    *la_dir,
    *message;

  la_string
    curStrToken, // the actual string representing the token
    parsePtr;    // acts as instruction pointer

  VALUE
     funResult,  // function returned value
     tokenValue, // for symbolic tokens, the symbol's value
     lastExpr,
     funArgs[MAX_BUILTIN_PARAMS];

  integer stackValIdx;
  VALUE stackVal[MAX_BUILTIN_PARAMS];

  sym_t
    *tokenSymbol;

  FILE
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
};

typedef struct ArrayType {
  VALUE value;
  int type;
  size_t len;
} ArrayType;

#define MAX_EXPR_LEVEL 5

static int la_parse_stmt (la_t *);
static int la_parse_expr (la_t *, VALUE *);
static int la_parse_primary (la_t *, VALUE *);
static int la_parse_func_def (la_t *);
static int la_next_token (la_t *);
static VALUE la_prod (VALUE, VALUE);
static VALUE la_sum  (VALUE, VALUE);
static VALUE la_diff (VALUE, VALUE);
static VALUE la_quot (VALUE, VALUE);
static VALUE la_mod  (VALUE, VALUE);
static VALUE la_bset (VALUE, VALUE);
static VALUE la_bnot (VALUE, VALUE);
static VALUE array_release (VALUE);
static ArrayType *array_copy (ArrayType *);

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
  if (la_StringGetLen (this->parsePtr) <= n) return -1;
  return *(la_StringGetPtr (this->parsePtr) + n);
}

static int la_get_char (la_t *this) {
  uint len = la_StringGetLen (this->parsePtr);

  ifnot (len) return -1;

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

static inline int is_idpunct (int c) {
  return NULL isnot Cstring.byte.in_str (".:_", c);
}

static inline int is_identifier (int c) {
  return is_alpha (c) or is_idpunct (c) or is_digit (c);
}

static inline int is_operator (int c) {
  return NULL isnot Cstring.byte.in_str ("+-!/*%=<>&|^", c);
}

static inline int is_operator_span (int c) {
  return NULL isnot Cstring.byte.in_str ("=<>&|^", c);
}

static inline int is_number (la_t *this, int c, int *token_type) {
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

  if (c isnot -1) la_unget_char (this);

  return LA_OK;
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

static void la_get_span (la_t *this, int (*testfn) (int)) {
  int c;
  do
    c = la_get_char (this);
  while (testfn (c));

  if (c isnot -1) la_unget_char (this);
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

static VALUE la_typeof (la_t *this, VALUE val) {
  malloced_string *mbuf = new_malloced_string (16);
  string *buf = mbuf->data;

  switch (val.type) {
    case INTEGER_TYPE: String.append_with_len (buf, "IntegerType", 11); break;
    case NUMBER_TYPE:  String.append_with_len (buf, "NumberType",  10); break;
    case STRING_TYPE:  String.append_with_len (buf, "StringType",  10); break;
    case ARRAY_TYPE:   String.append_with_len (buf, "ArrayType",    9); break;
    default:           String.append_with_len (buf, "UnknownType", 10); break;
  }

  ListStackPush (this->curScope, mbuf);
  VALUE v = STRING (buf);
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
  }

  return result;
}

static VALUE la_free (la_t *this, VALUE value) {
  (void) this;
  VALUE result = INT(LA_NOTOK);
  if (value.type <= INTEGER_TYPE) goto theend;

  void *object = NULL;
  switch (value.type) {
    case POINTER_TYPE: object = AS_VOID_PTR(value); break;
    case   ARRAY_TYPE: result = array_release (value); break;
    case  STRING_TYPE: String.release (AS_STRING(value)); break;
    default: goto theend;
  }

  ifnot (NULL is object) {
    free (object);
    value = NONE;
    result = INT(LA_OK);
  }

  object = NULL;

theend:
  return result;
}

static void fun_release (funT **thisp) {
  if (*thisp is NULL) return;
  funT *this = *thisp;
  Vmap.release (this->symbols);
  ns_release_malloced_strings (this);
  free (this);
  *thisp = NULL;
}

static void la_release_sym (void *sym) {
  if (sym is NULL) return;

  sym_t *this = (sym_t *) sym;

  if ((this->type & 0xff) is UFUNC_TYPE) {
    VALUE v = this->value;
    ifnot (v.type & FUNCPTR_TYPE) {
      funT *f = AS_FUNC_PTR(v);
      fun_release (&f);
    }
  } else
    la_free (NULL, this->value);

  free (this);
  this = NULL;
}

static funT *fun_new (funNewArgs options) {
  funT *uf = Alloc (sizeof (funT));
  Cstring.cp (uf->funname, MAXLEN_SYMBOL_LEN, options.name, options.namelen);
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
    return f;
  }

  f->prev = parent;
  f->root = this->function;
  return f;
}

static inline char *sym_key (la_t *this, la_string x) {
  Cstring.cp (this->symKey, MAXLEN_SYMBOL_LEN + 1,
      la_StringGetPtr (x), la_StringGetLen (x));
  return this->symKey;
}

static sym_t *la_define_symbol (la_t *this, funT *f, char *key, int typ, VALUE value, int is_const) {
#ifdef DEBUG
  if ($CUR_IDX < 65) {
    $CUR_IDX++;
    goto body;
  }
  $CODE_PATH
body:
#endif
  (void) this;
  ifnot (key) return NULL;

  sym_t *sym = Alloc (sizeof (sym_t));
  sym->type = typ;
  sym->value = value;
  sym->is_const = is_const;

  if (NOTOK is Vmap.set (f->symbols, key, sym, la_release_sym, is_const)) {
    free (sym);
    return NULL;
  }

  return sym;
}

static inline sym_t *ns_lookup_symbol (funT *scope, char *key) {
  return Vmap.get (scope->symbols, key);
}

static sym_t *la_lookup_symbol (la_t *this, la_string name) {
  char *key = sym_key (this, name);

#ifdef DEBUG
  fprintf (this->err_fp, "Queried Symbol: %s\n", key);
  $CODE_PATH
#endif

  sym_t *sym = NULL;

  funT *f = this->curScope;
  while (NULL isnot f) {
    sym = ns_lookup_symbol (f, key);

    ifnot (NULL is sym) {
      return sym;
   }
    f = f->prev;
  }

  return NULL;
}

static int la_lambda (la_t *this) {
  Cstring.cp (this->curFunName, MAXLEN_SYMBOL_LEN + 1, "anonymous", 9);

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
  return (this->tokenSymbol->type & 0xff);
}

static int la_do_next_token (la_t *this, int israw) {
#ifdef DEBUG
  $CODE_PATH
#endif
  int r = LA_NOTOK;

  sym_t *symbol = NULL;
  this->tokenSymbol = NULL;

  la_reset_token (this);

  int c = la_ignore_ws (this);

  if (c is LA_TOKEN_LINE_CONT and la_peek_char (this, 0) is LA_TOKEN_NL) {
    this->lineNum++;
    la_ignore_next_char (this);
    la_reset_token (this);
    c = la_ignore_ws (this);
  }

  if (c is '#') {
    do
      c = la_get_char (this);
    while (c >= 0 and c isnot LA_TOKEN_NL);
    this->lineNum++;

    r = c;

  } else if (is_digit (c) or (c is '-' and is_digit (la_peek_char (this, 0)))) {
    if (c is '0' and NULL isnot Cstring.byte.in_str ("xX", la_peek_char (this, 0))
        and is_hexchar (la_peek_char(this, 1))) {
      la_get_char (this);
      la_ignore_first_token (this);
      la_ignore_first_token (this);
      la_get_span (this, is_hexchar);
      r = LA_TOKEN_HEX_NUMBER;
    } else {
      if (LA_NOTOK is is_number (this, c, &r))
        return this->syntax_error (this, "error while tokenizing a number");
    }
  } else if (c is LA_TOKEN_SQUOTE) {
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
  } else if (is_alpha (c)) {
    la_get_span (this, is_identifier);

    r = LA_TOKEN_SYMBOL;

    ifnot (israw) {
      if (Cstring.eq_n ("lambda", la_StringGetPtr (this->curStrToken), 6)) {
        r = la_lambda (this);
        if (r < LA_OK)
          return this->syntax_error (this, "lambda error");
      } else {
        this->tokenSymbol = symbol = la_lookup_symbol (this, this->curStrToken);

        if (symbol) {
          r = symbol->type & 0xff;

          this->tokenArgs = (symbol->type >> 8) & 0xff;

          if (r is ARRAY_TYPE) {
            r = LA_TOKEN_ARY;
            symbol->value.type = ARRAY_TYPE;
          } else
            if (r < '@')
              r = LA_TOKEN_VAR;

          this->tokenValue = symbol->value;
        }
      }
    }

  } else if (is_operator (c)) {
    la_get_span (this, is_operator_span);

    this->tokenSymbol = symbol = la_lookup_symbol (this, this->curStrToken);

    if (symbol) {
      r = symbol->type;
      this->tokenValue = symbol->value;
    } else
      r = LA_TOKEN_SYNTAX_ERR;

  } else if (c is LA_TOKEN_BLOCK_OPEN) {
    int bracket = 1;
    la_reset_token (this);
    while (bracket > 0) {
      c = la_get_char (this);

      if (c is LA_NOTOK) return LA_TOKEN_SYNTAX_ERR;

      if (c is LA_TOKEN_BLOCK_CLOS)
        --bracket;
      else if (c is LA_TOKEN_BLOCK_OPEN)
        ++bracket;
    }

    la_ignore_last_token (this);
    r = LA_TOKEN_BLOCK;

  } else if (c is LA_TOKEN_DQUOTE) {
    size_t len = 0;
    int pc = 0;
    int cc = 0;

    while (pc = cc, (cc = la_peek_char (this, len)) isnot -1) {
      if (LA_TOKEN_DQUOTE is cc and pc isnot LA_TOKEN_ESCAPE_CHR) break;
      len++;
    }

    if (cc is -1)
      return this->syntax_error (this, "unended string, a '\"' is missing");

    ifnot (this->curState & STRING_LITERAL_ARG_STATE) {
      string_t *str = String.new (len + 1);
      for (size_t i = 0; i < len; i++) {
        c = la_get_char (this);
        String.append_byte (str, c);
      }

      this->tokenValue = STRING(str);

    } else {
      malloced_string *mbuf = new_malloced_string (len + 1);
      for (size_t i = 0; i < len; i++) {
        c = la_get_char (this);
        String.append_byte (mbuf->data, c);
      }

      ListStackPush (this->curScope, mbuf);
      this->tokenValue = STRING (mbuf->data);
    }

    c = la_get_char (this);
    la_reset_token (this);

    r = LA_TOKEN_STRING;

  } else
    r = c;

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

static void *la_clone_sym_item (void *item) {
  sym_t *sym = (void *) item;

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
    string_t *old_str = AS_STRING(sym->value);
    new->value = STRING_NEW_WITH_LEN(old_str->bytes, old_str->num_bytes);
    return new;
  }

  new->value = sym->value;
  return new;
}

static void la_symbol_stack_push (la_t *this, Vmap_t *symbols) {
  symbolstack_t *item = Alloc (sizeof (symbolstack_t));
  item->symbols = Vmap.clone (symbols, la_clone_sym_item);
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

static VALUE array_release (VALUE value) {
  VALUE result = INT(LA_NOTOK);

  if (value.type is NONE_TYPE or value.refcount < 0)
    return result;

  if (value.refcount) {
    result = INT(LA_OK);
    goto theend;
  }

  ArrayType *array = (ArrayType *) AS_ARRAY(value);
  if (NULL is array) return result;

  VALUE ary = array->value;

  if (array->type is STRING_TYPE) {
    string **s_ar = (string **) AS_ARRAY(ary);
    if (s_ar is NULL) return result;
    for (size_t i = 0; i < array->len; i++)
      String.release (s_ar[i]);
    free (s_ar);
    s_ar = NULL;

  } else {
    char *ar = (char *) AS_ARRAY(ary);
    if (ar is NULL) return result;
    free (ar);
  }

  free ((void *) array);
  array = NULL;

  value = NONE;
  result = INT(LA_OK);

theend:
  value.refcount = -1;
  return result;
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

static int la_array_set_as_string (la_t *this, VALUE ar, integer len, integer idx) {
  int err;
  VALUE val;
  string **s_ar = (string **) AS_ARRAY(ar);

  do {
    if (idx < 0 or idx >= len)
      return la_out_of_bounds (this);

    this->curState |= STRING_LITERAL_ARG_STATE;
    la_next_token (this);

    err = la_parse_expr (this, &val);

    this->curState &= ~STRING_LITERAL_ARG_STATE;

    if (err isnot LA_OK) return err;

    string *item = s_ar[idx];
    string *s_val = AS_STRING(val);
    String.replace_with_len (item, s_val->bytes, s_val->num_bytes);
    idx++;
  } while (this->curToken is LA_TOKEN_COMMA);

  return LA_OK;
}

static int la_array_set_as_number (la_t *this, VALUE ar, integer len, integer idx) {
  int err;
  VALUE val;
  number *n_ar = (number *) AS_ARRAY(ar);
  do {
    if (idx < 0 or idx >= len)
      return la_out_of_bounds (this);

    la_next_token (this);

    err = la_parse_expr (this, &val);
    if (err isnot LA_OK) return err;
    n_ar[idx] = AS_NUMBER(val);

    idx++;
  } while (this->curToken is LA_TOKEN_COMMA);

  return LA_OK;
}

static int la_array_set_as_int (la_t *this, VALUE ar, integer len, integer idx) {
  int err;
  VALUE val;
  integer *s_ar = (integer *) AS_ARRAY(ar);
  do {
    if (idx < 0 or idx >= len)
      return la_out_of_bounds (this);

    la_next_token (this);

    err = la_parse_expr (this, &val);
    if (err isnot LA_OK) return err;
    s_ar[idx] = AS_INT(val);

    idx++;
  } while (this->curToken is LA_TOKEN_COMMA);

  return LA_OK;
}

/* Initial Array Interface by MickeyDelp <mickey at delptronics dot com> */
static int la_array_assign (la_t *this, VALUE *ar, VALUE ix) {
  int err;

  integer idx = AS_INT(ix);

  ArrayType *array = (ArrayType *) AS_ARRAY((*ar));
  integer len = array->len;

  VALUE ary = array->value;

  if (array->type is INTEGER_TYPE) {
    err = la_array_set_as_int (this, ary, len, idx);
  } else if (array->type is STRING_TYPE) {
    err = la_array_set_as_string (this, ary, len, idx);
  } else
    err = la_array_set_as_number (this, ary, len, idx);

  if (err isnot LA_OK)
    return err;

   return LA_OK;
}

static int la_parse_array_def (la_t *this) {
  la_string name;
  int c;
  int err;
  VALUE len;
  VALUE ar;
  VALUE ary;
  ArrayType *array;

  c = la_next_raw_token (this);

  if (c isnot LA_TOKEN_SYMBOL) {
    return this->syntax_error (this, "syntax error, awaiting a name");
  }

  const char *sp = la_StringGetPtr (this->curStrToken);
  uint splen = la_StringGetLen (this->curStrToken);
  int isname = 0;
  int type = INTEGER_TYPE;

  if (Cstring.eq_n ("integer", sp, splen))
    type = INTEGER_TYPE;
  else if (Cstring.eq_n ("number", sp, splen))
    type = NUMBER_TYPE;
  else if (Cstring.eq_n ("pointer", sp, splen))
    type = POINTER_TYPE;
  else if (Cstring.eq_n ("string", sp, splen))
    type = STRING_TYPE;
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

  err = la_parse_primary (this, &len);
  if (err isnot LA_OK)
    return err;

  integer nlen = AS_INT(len);

  array = Alloc (sizeof (ArrayType));
  array->type = type;
  array->len = nlen;

  if (array->type is INTEGER_TYPE) {
    integer *i_ar = Alloc (nlen * sizeof (integer));
    for (integer i = 0; i < nlen; i++)
      i_ar[i] = 0;
    ary = ARRAY(i_ar);

  }  else if (array->type is NUMBER_TYPE) {
    number *n_ar = Alloc (nlen * sizeof (number));
    for (integer i = 0; i < nlen; i++)
      n_ar[i] = 0.0;
    ary = ARRAY(n_ar);

  } else if (array->type is STRING_TYPE) {
    string **s_ar = Alloc (nlen * sizeof (string));
    for (integer i = 0; i < nlen; i++) {
      s_ar[i] = String.new_with ("");
    }
    ary = ARRAY(s_ar);
  }

  array->value = ary;
  ar = ARRAY(array);

  this->tokenSymbol = la_define_symbol (this, this->curScope, sym_key (this, name), ARRAY_TYPE,
    ar, 0);

  VALUE at_idx = INT(0);
  if (la_StringGetPtr (this->curStrToken)[0] is '=' and la_StringGetLen (this->curStrToken) is 1)
    return la_array_assign (this, &ar, at_idx);

  return LA_OK;
}

static int la_parse_array_set (la_t *this) {
  int err;
  VALUE ix = INT(0);

  VALUE ary = this->tokenValue;

  int c = la_next_token (this);

  if (c is LA_TOKEN_INDEX_OPEN) {
    err = la_parse_primary (this, &ix);
    if (err isnot LA_OK)
      return err;
  }

  if (la_StringGetPtr (this->curStrToken)[0] isnot '=' or la_StringGetLen (this->curStrToken) isnot 1)
    return this->syntax_error (this, "syntax error");

  return la_array_assign (this, &ary, ix);
}

static int la_parse_array_get (la_t *this, VALUE *vp) {
  VALUE ar = this->tokenValue;
  ArrayType *array = (ArrayType *) AS_ARRAY(ar);
  integer len = array->len;

  int c = la_next_token (this);

  if (c is LA_TOKEN_INDEX_OPEN) {
    VALUE ix;
    int err = la_parse_primary (this, &ix);
    if (err isnot LA_OK)
      return err;

    integer idx = AS_INT(ix);

    if (idx is -1) {
      *vp = INT(len);
      return LA_OK;
    }

    if (idx < -1 or idx >= len)
      return la_out_of_bounds (this);

    if (array->type is INTEGER_TYPE) {
      integer *ary = (integer *) AS_ARRAY(array->value);
      *vp = INT(ary[idx]);
    } else if (array->type is NUMBER_TYPE) {
      number *ary = (number *) AS_ARRAY(array->value);
      *vp = NUMBER(ary[idx]);
    } else if (array->type is STRING_TYPE) {
      string **ary = (string **) AS_ARRAY(array->value);
      *vp = STRING(ary[idx]);
    } else
      return la_unknown_type (this);
  } else {
    // if no parens, then return the pointer to the array
    // needed for passing to C functions
    *vp = ar;
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

static int la_parse_expr_list (la_t *this) {
#ifdef DEBUG
  $CODE_PATH
#endif
  int err, c;
  int count = 0;
  VALUE v;

  do {
    err = la_parse_expr (this, &v);
    if (err isnot LA_OK) return err;

    stack_push (this, v);

    count++;

    c = this->curToken;
    if (c is LA_TOKEN_COMMA) la_next_token (this);
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

  /* multibyte support */
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
#ifdef DEBUG
  $CODE_PATH
#endif

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

    r = la_parse_stmt (this);

    if (r isnot LA_OK) return r;

    c = this->curToken;

    if (c is LA_TOKEN_NL or c is LA_TOKEN_SEMICOLON or c < 0) {
      if (c is LA_TOKEN_NL) this->lineNum++;
      continue;
    } else
      return this->syntax_error (this, "evaluated string failed, unknown token");

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

static int la_parse_func_call (la_t *this, Cfunc op, VALUE *vp, funT *uf) {
#ifdef DEBUG
  $CODE_PATH
#endif
  int paramCount = 0;
  int expectargs;
  int c;

  if (uf)
    expectargs = uf->nargs;
  else
    expectargs = this->tokenArgs;

  c = la_next_token (this);

  if (c isnot LA_TOKEN_PAREN_OPEN) {
    la_unget_char (this);
    VALUE v = PTR(uf);
    v.type |= FUNCPTR_TYPE;
    *vp = v;
    return LA_OK;
  }

  this->curState |= STRING_LITERAL_ARG_STATE;

  c = la_next_token (this);

  if (c isnot LA_TOKEN_PAREN_CLOS) {
    paramCount = la_parse_expr_list (this);
    c = this->curToken;
    if (paramCount < 0) {
      this->curState &= ~(STRING_LITERAL_ARG_STATE);
      return paramCount;
    }
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

  if (uf) {
    int i;
    int err;

    int refcount = 0;

    int is_anonymous = Cstring.eq (uf->funname, "anonymous");
    ifnot (is_anonymous) {
      refcount = Imap.set_by_callback (this->refcount, uf->funname, la_fun_refcount_incr);
      if (refcount > 1) {
        la_symbol_stack_push (this, this->curScope->symbols);
        Vmap.clear (uf->symbols);
      }
    }

    for (i = 0; i < expectargs; i++) {
      VALUE v = this->funArgs[i];
      if (v.type & FUNCPTR_TYPE) {
        funT *f = AS_FUNC_PTR(v);
        la_define_symbol (this, uf, uf->argName[i], (UFUNC_TYPE | (f->nargs << 8)), v, 0);
      } else {
        v.refcount += (refcount < 2);
        la_define_symbol (this, uf, uf->argName[i], v.type, v, 0);
      }
    }

    this->didReturn = 0;

    la_fun_stack_push (this, this->curScope);

    this->curScope = uf;

    err = la_parse_string (this, uf->body);

    this->curScope = la_fun_stack_pop (this);

    this->didReturn = 0;

    ifnot (is_anonymous) {
      refcount = Imap.set_by_callback (this->refcount, uf->funname, la_fun_refcount_decr);

      ifnot (refcount)
        Vmap.clear (uf->symbols);
      else {
        Vmap.release (uf->symbols);
        this->curScope->symbols = la_symbol_stack_pop (this);
      }
    }

    *vp = this->funResult;
    return err;
  } else {
    *vp = op (this, this->funArgs[0], this->funArgs[1], this->funArgs[2],
                    this->funArgs[3], this->funArgs[4], this->funArgs[5],
                    this->funArgs[6], this->funArgs[7], this->funArgs[8]);
  }

  la_next_token (this);

  return LA_OK;
}

static int la_parse_primary (la_t *this, VALUE *vp) {
#ifdef DEBUG
  $CODE_PATH
#endif
  int c, err;

  c = this->curToken;

  if (c is LA_TOKEN_PAREN_OPEN or c is LA_TOKEN_INDEX_OPEN) {
    int close_token = (c is LA_TOKEN_PAREN_OPEN
       ? LA_TOKEN_PAREN_CLOS : LA_TOKEN_INDEX_CLOS);

    la_next_token (this);

    err = la_parse_expr (this, vp);

    if (err is LA_OK) {
      c = this->curToken;

      if (c is close_token) {
        la_next_token (this);

        this->lastExpr = this->tokenValue;
        return LA_OK;
      }
    }

    return err;

  } else if (c is LA_TOKEN_NUMBER) {
    VALUE val = la_string_to_num (this->curStrToken);
    *vp = val;
    la_next_token (this);
    return LA_OK;

  } else if (c is LA_TOKEN_DOUBLE) {
    char *endptr; char str[32];
    Cstring.cp (str, 32, la_StringGetPtr (this->curStrToken), la_StringGetLen (this->curStrToken));
    double val = strtod (str, &endptr);
    *vp = NUMBER(val);
    la_next_token (this);
    return LA_OK;

  } else if (c is LA_TOKEN_HEX_NUMBER) {
    *vp = la_HexStringToNum (this->curStrToken);
    la_next_token (this);
    return LA_OK;

  } else if (c is LA_TOKEN_CHAR) {
      err = la_parse_char (this, vp, this->curStrToken);

      la_next_token (this);
      return err;

  } else if (c is LA_TOKEN_VAR) {
    *vp = this->tokenValue;
    la_next_token (this);
    return LA_OK;

  } else if (c is LA_TOKEN_ARY) {
     return la_parse_array_get (this, vp);

  } else if (c is LA_TOKEN_BUILTIN) {
    Cfunc op = (Cfunc) AS_PTR(this->tokenValue);
    return la_parse_func_call (this, op, vp, NULL);

  } else if (c is LA_TOKEN_USRFUNC) {
    sym_t *symbol = this->tokenSymbol;
    ifnot (symbol)
      return this->syntax_error (this, "user defined function, not declared");

    funT *uf = AS_FUNC_PTR(symbol->value);

    err = la_parse_func_call (this, NULL, vp, uf);
    la_next_token (this);
    return err;

  } else if ((c & 0xff) is LA_TOKEN_BINOP) {
    // binary operator
    Opfunc op = (Opfunc) AS_PTR(this->tokenValue);
    VALUE v;

    la_next_token (this);
    err = la_parse_expr (this, &v);
    if (err is LA_OK)
      *vp = op (INT(0), v);

    return err;

  } else if (c is LA_TOKEN_STRING) {
    *vp = this->tokenValue;

    la_next_token (this);
    return LA_OK;

  } else {
    if (c isnot -1) {
      fprintf (this->err_fp, "unknown token |%c| |%d|", c, c);
      return this->syntax_error (this, "syntax error");
    }
  }
}

static int la_parse_stmt (la_t *this) {
#ifdef DEBUG
  $CODE_PATH
#endif
  int c;
  la_string name;
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

  c = this->curToken;

  if (c is LA_TOKEN_BREAK) {
    if (this->curState & LOOP_STATE) {
      this->curState |= BREAK_STATE;
      return LA_ERR_BREAK;
    }

    return this->syntax_error (this, "break is not in a loop");
  }

  if (c is LA_TOKEN_CONTINUE) {
    if (this->curState & LOOP_STATE) {
      this->curState |= CONTINUE_STATE;
      return LA_ERR_CONTINUE;
    }

    return this->syntax_error (this, "continue is not in a loop");
  }

  if (c is LA_TOKEN_VARDEF or c is LA_TOKEN_CONSTDEF) {
    int is_const = c is LA_TOKEN_CONSTDEF;

    c = la_next_raw_token (this); // we want to get VAR_SYMBOL directly

    if (c isnot LA_TOKEN_SYMBOL)
      return this->syntax_error (this, "expected symbol");

    name = this->curStrToken;

    ifnot (la_StringGetLen (name))
      return this->syntax_error (this, "unknown symbol");

    char *key = sym_key (this, name);
    sym_t *sym = ns_lookup_symbol (this->curScope, key);

    ifnot (NULL is sym)
      return this->syntax_error (this, "can not redeclare a symbol in this scope");

    VALUE ival = INT(0);
    this->tokenSymbol = la_define_symbol (this, this->curScope, key, INTEGER_TYPE, ival, is_const);

    if (NULL is this->tokenSymbol)
      return this->syntax_error (this, "unknown error while declaring");

    c = LA_TOKEN_VAR;
    /* fall through */
  }

  if (c is LA_TOKEN_VAR) {
    name = this->curStrToken;
    sym_t *symbol = this->tokenSymbol;

    c = la_next_token (this);

    const char *ptr = la_StringGetPtr (this->curStrToken);
    int len = la_StringGetLen (this->curStrToken);
    if (len > 2 or len is 0)
      return this->syntax_error (this, "expected [+/*-]=");

    int operator = *ptr;
    if (operator isnot '=') {
      if (*(ptr + 1) isnot '=')
        return this->syntax_error (this, "expected =");
    }

    ifnot (symbol) {
      la_print_lastring (this, this->err_fp, name);
      return la_unknown_symbol (this);
    }

    if (symbol->is_const and AS_INT(symbol->value) isnot 0)
      return this->syntax_error (this, "can not reassign to a constant");

    ptr += len;
    while (*ptr is ' ') ptr++;
    int is_un = *ptr is '~';

    if (Cstring.eq_n (ptr, "func", 4)) {
      la_release_sym (Vmap.pop (this->curScope->symbols, sym_key (this, name)));

      la_next_token (this);

      Cstring.cp (this->curFunName, MAXLEN_SYMBOL_LEN + 1, la_StringGetPtr(name), la_StringGetLen(name));
      err = la_parse_func_def (this);
      this->curFunName[0] = '\0';
      return err;
    }

    la_next_token (this);

    if (is_un)
      la_next_token (this);

    err = la_parse_expr (this, &val);

    if (err isnot LA_OK) return err;

    if (is_un)
      AS_INT(val) = ~AS_INT(val);

    if (val.type & FUNCPTR_TYPE) {
      funT *f = AS_FUNC_PTR(val);
      symbol->type = (UFUNC_TYPE | (f->nargs << 8));
    } else
      symbol->type = val.type;

    if ((symbol->type & 0x77) is STRING_TYPE)
      la_free (this, symbol->value);

    switch (operator) {
      case '=': symbol->value = val; break;
      case '+': symbol->value = la_sum (symbol->value, val); break;
      case '-': symbol->value = la_diff (symbol->value, val); break;
      case '/': symbol->value = la_quot (symbol->value, val); break;
      case '*': symbol->value = la_prod (symbol->value, val); break;
      case '%': symbol->value = la_mod  (symbol->value, val); break;
      case '|': symbol->value = la_bset (symbol->value, val); break;
      case '&': symbol->value = la_bnot (symbol->value, val); break;
      default: return this->syntax_error (this, "unknown operator");
    }

  } else if (c is LA_TOKEN_ARY) {
    err = la_parse_array_set (this);

  } else if (c is LA_TOKEN_BUILTIN or c is UFUNC_TYPE) {
    err = la_parse_primary (this, &val);
    return err;

  } else if (this->tokenSymbol and AS_INT(this->tokenValue)) {
    int (*func) (la_t *) = AS_VOID_PTR(this->tokenValue);
    err = (*func) (this);

  } else if (c is LA_TOKEN_COMMA) {
    la_next_token (this);
    err = la_parse_stmt (this);

  } else
    return this->syntax_error (this, STR_FMT("unknown token |%c| |%d|", c, c));

  if (err is LA_ERR_OK_ELSE)
    err = LA_OK;

  return err;
}

// parse a level n expression
// level 0 is the lowest level (highest precedence)
static int la_parse_expr_level (la_t *this, int max_level, VALUE *vp) {
#ifdef DEBUG
  $CODE_PATH
#endif
  int err = LA_OK;
  int c;
  VALUE lhs;
  VALUE rhs;

  lhs = *vp;
  c = this->curToken;
  while ((c & 0xff) is LA_TOKEN_BINOP) {
    int level = (c >> 8) & 0xff;
    if (level > max_level) break;

    Opfunc op = (Opfunc) AS_PTR(this->tokenValue);

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

    lhs = op (lhs, rhs);
  }

  *vp = lhs;
  return err;
}

static int la_parse_expr (la_t *this, VALUE *vp) {
#ifdef DEBUG
  $CODE_PATH
#endif
  int err = la_parse_primary (this, vp);

  if (err is LA_OK)
    err = la_parse_expr_level (this, MAX_EXPR_LEVEL, vp);

  return err;
}

static int la_parse_if_rout (la_t *this, VALUE *cond, int *haveelse, la_string *ifpart, la_string *elsepart) {
#ifdef DEBUG
  $CODE_PATH
#endif
  int c;
  int err;

  *haveelse = 0;

  this->curState |= STRING_LITERAL_ARG_STATE;
  c = la_next_token (this);

  err = la_parse_expr (this, cond);
  if (err isnot LA_OK) return err;

  this->curState &= ~STRING_LITERAL_ARG_STATE;

  c = this->curToken;

  if (c isnot LA_TOKEN_BLOCK) return this->syntax_error (this, "parsing if, not a block string");

  *ifpart = this->curStrToken;

  c = la_next_token (this);

  if (c is LA_TOKEN_ELSE) {
    c = la_next_token (this);

    if (c isnot LA_TOKEN_BLOCK) return this->syntax_error (this, "parsing else, not a block string");

    *elsepart = this->curStrToken;
    *haveelse = 1;

    la_next_token (this);
  }

  return LA_OK;
}

static int la_parse_if (la_t *this) {
  la_string ifpart, elsepart;
  int haveelse = 0;
  VALUE cond;

  int err = la_parse_if_rout (this, &cond, &haveelse, &ifpart, &elsepart);
  if (err isnot LA_OK)
    return err;

  if (AS_INT(cond))
    err = la_parse_string (this, ifpart);
  else if (haveelse)
    err = la_parse_string (this, elsepart);

  if (this->didReturn)
    la_StringSetLen (&this->parsePtr, 0);

  if (err is LA_OK and 0 is AS_INT(cond)) err = LA_ERR_OK_ELSE;
  return err;
}

static int la_parse_ifnot (la_t *this) {
  la_string ifpart, elsepart;
  int haveelse = 0;
  VALUE cond;
  int err = la_parse_if_rout (this, &cond, &haveelse, &ifpart, &elsepart);
  if (err isnot LA_OK)
    return err;

  if (0 is AS_INT(cond))
    err = la_parse_string (this, ifpart);
  else if (haveelse)
    err = la_parse_string (this, elsepart);

  if (this->didReturn)
    la_StringSetLen (&this->parsePtr, 0);

  if (err is LA_OK and 0 isnot AS_INT(cond)) err = LA_ERR_OK_ELSE;
  return err;
}

static int la_parse_while (la_t *this) {
  int err;
  la_string savepc = this->parsePtr;
  int len = la_StringGetLen (this->parsePtr);
  this->curState |= LOOP_STATE;

again:
  this->curState &= ~(BREAK_STATE|CONTINUE_STATE);

  err = la_parse_if (this);

  if (this->curState & BREAK_STATE) {
    const char *savepcptr = la_StringGetPtr (savepc);
    const char *parsePtr = la_StringGetPtr (this->parsePtr);
    while (savepcptr isnot parsePtr) {savepcptr++; len--;}
    la_StringSetLen (&this->parsePtr, len);

    int c;
    int brackets = 2;

    do {
      c = la_get_char (this);
      if (c is LA_TOKEN_BLOCK_CLOS)
        --brackets;
      else if (c is LA_TOKEN_BLOCK_OPEN)
        ++brackets;
      ifnot (brackets) break;
    } while (c >= 0);
    if (brackets)
      return this->syntax_error (this, "a closed bracket '}' is missing");

    la_next_token (this);
    err = LA_ERR_OK_ELSE;
  }

  if (err is LA_ERR_OK_ELSE or this->didReturn) {
    this->curState &= ~LOOP_STATE;
    return LA_OK;

  } else if (err is LA_OK or this->curState & CONTINUE_STATE) {
    la_StringSetPtr (&this->parsePtr, la_StringGetPtr (savepc));
    la_StringSetLen (&this->parsePtr, len);
    goto again;
  }

  this->curState &= ~LOOP_STATE;
  return err;
}

static int la_parse_for (la_t *this) {
  int err;

  int c = la_next_token (this);
  if (c isnot LA_TOKEN_PAREN_OPEN)
    return this->syntax_error (this, "error while parsing for loop, awaiting (");

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
    return this->syntax_error (this, "error while parsing for loop, awaiting a)");

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

    ifnot (AS_INT(v)) {
      this->curToken = LA_TOKEN_SEMICOLON;
      goto theend;
    }

    this->curState |= LOOP_STATE;
    this->curState &= ~(BREAK_STATE|CONTINUE_STATE);
    err= la_parse_string (this, body_str);
    this->curState &= ~LOOP_STATE;

    if (err is LA_ERR_BREAK or this->curState & BREAK_STATE) {
      la_next_token (this);
      this->curToken = LA_TOKEN_SEMICOLON;
      goto theend;
    }

    if (this->didReturn) {
      this->curState &= ~(BREAK_STATE|CONTINUE_STATE|LOOP_STATE);
      this->curToken = LA_TOKEN_SEMICOLON;
      la_StringSetLen (&this->parsePtr, 0);
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
  this->curState &= ~(BREAK_STATE|CONTINUE_STATE|LOOP_STATE);
  this->parsePtr = savepc;
  return LA_OK;
}

static int la_parse_loop (la_t *this) {
  int err;
  int c = la_next_token (this);
  if (c isnot LA_TOKEN_PAREN_OPEN)
    return this->syntax_error (this, "error while parsing loop, awaiting (");

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

    if (*ptr is LA_TOKEN_LINE_CONT and *(ptr + 1) is LA_TOKEN_NL) {
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

  for (integer i; i < num; i++) {
    this->curState |= LOOP_STATE;
    this->curState &= ~(BREAK_STATE|CONTINUE_STATE);
    err= la_parse_string (this, body_str);
    this->curState &= ~LOOP_STATE;

    if (err is LA_ERR_BREAK or this->curState & BREAK_STATE) {
      la_next_token (this);
   //   this->curToken = ';';
      goto theend;
    }

    if (this->didReturn) {
      this->curState &= ~(BREAK_STATE|CONTINUE_STATE|LOOP_STATE);
   //   this->curToken = ';';
      la_StringSetLen (&this->parsePtr, 0);
      return LA_OK;
    }

    if (err is LA_ERR_CONTINUE or this->curState & CONTINUE_STATE)
      continue;

    if (err isnot LA_OK) return err;
  }

theend:
  this->curState &= ~(BREAK_STATE|CONTINUE_STATE|LOOP_STATE);
  this->parsePtr = savepc;
  return LA_OK;
}

static int la_parse_forever (la_t *this) {
  int err;
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
      return LA_OK;
    }

    if (err is LA_ERR_CONTINUE or this->curState & CONTINUE_STATE)
      continue;

    if (err isnot LA_OK) return err;
  }

theend:
  this->curState &= ~(BREAK_STATE|CONTINUE_STATE|LOOP_STATE);
  this->parsePtr = savepc;
  return LA_OK;
}

static int la_parse_var_list (la_t *this, funT *uf) {
#ifdef DEBUG
  $CODE_PATH
#endif
  int c;
  int nargs = 0;

  c = la_next_raw_token (this);

  for (;;) {
    if (c is LA_TOKEN_SYMBOL) {
      la_string name = this->curStrToken;

      if (nargs >= MAX_BUILTIN_PARAMS)
        return la_too_many_args (this);

      size_t len = la_StringGetLen (name);
      if (len >= MAXLEN_SYMBOL_LEN)
        return this->syntax_error (this, "argument name exceeded maximum length (64)");

      const char *ptr = la_StringGetPtr (name);
      Cstring.cp (uf->argName[nargs], MAXLEN_SYMBOL_LEN, ptr, len);

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
#ifdef DEBUG
  $CODE_PATH
#endif
  la_string name;
  int c;
  int nargs = 0;
  size_t len = 0;

  ifnot (this->curFunName[0]) {
    c = la_next_raw_token (this); // do not interpret the symbol
    if (c isnot LA_TOKEN_SYMBOL) return this->syntax_error (this, "function definition, not a symbol");

    name = this->curStrToken;
    len = la_StringGetLen (name);
    if (len >= MAXLEN_SYMBOL_LEN)
      return this->syntax_error (this, "function name exceeded maximum length (64)");
  } else {
    name = la_StringNew (this->curFunName);
    len = bytelen (this->curFunName);
  }

  funT *uf = Fun_new (this, funNew (
   .name = la_StringGetPtr (name), .namelen = len, .parent = this->curScope
    ));

  c = la_next_token (this);

  if (c is LA_TOKEN_PAREN_OPEN) {
    la_fun_stack_push (this, this->curScope);

    this->curScope = uf;

    nargs = la_parse_var_list (this, uf);

    this->curScope = la_fun_stack_pop (this);

    if (nargs < 0) return nargs;

    c = la_next_token (this);
  }

  if (c isnot LA_TOKEN_BLOCK) return this->syntax_error (this, "function definition, not a string");

  uf->body = this->curStrToken;

  VALUE v = PTR(uf);

  this->curSym = la_define_symbol (this, this->curScope, sym_key (this, name),
      (UFUNC_TYPE | (nargs << 8)), v, 0);

  this->curFunDef = uf;

  la_next_token (this);

  return LA_OK;
}

static int la_parse_print (la_t *this) {
  int err = LA_NOTOK;
  VALUE value;

  string_t *str = String.new (32);

  int c = la_ignore_ws (this);

  if (c isnot LA_TOKEN_PAREN_OPEN) {
    this->print_bytes (this->err_fp, "string fmt error, awaiting (\n");
    la_err_ptr (this, LA_NOTOK);
    goto theend;
  }

  c = la_ignore_ws (this);

  FILE *fp = this->out_fp;

  /* for now */
  if (c is 's' and Cstring.eq_n (la_StringGetPtr (this->parsePtr), "tderr,", 6)) {
    fp = stderr;
    for (int i = 0; i < 6; i++)
      la_ignore_next_char (this);
    c = la_ignore_ws (this);
  }

  if (c isnot LA_TOKEN_DQUOTE) {
    this->print_bytes (this->err_fp, "string fmt error, awaiting double quote\n");
    la_err_ptr (this, LA_NOTOK);
    goto theend;
  }

  int prev = c;
  char directive = 'd';

  for (;;) {
    c = la_get_char (this);
    if (c is LA_TOKEN_DQUOTE) {
      if (prev isnot '\\')
        break;

      String.append_byte (str, LA_TOKEN_DQUOTE);
      prev = LA_TOKEN_DQUOTE;
      c = la_get_char (this);
    }

    if (c is '\\') {
      if (prev is '\\' or la_peek_char (this, 1) isnot '$') {
        prev = str->bytes[str->num_bytes - 1];
        String.append_byte (str, '\\');
      } else  prev = '\\';

      c = la_get_char (this);
    }

    if (c is '$') {
      if (prev is '\\') {
        String.append_byte (str, '$');
        prev = '$';
        continue;
      }

      prev = c;
      c = la_get_char (this);
      if (c isnot '{') {
        this->print_bytes (this->err_fp, "string fmt error, awaiting {\n");
        la_err_ptr (this, LA_NOTOK);
        goto theend;
      }

      char sym[MAXLEN_SYMBOL_LEN];
      int tmp = prev;

      c = la_get_char (this);
      prev = c;

      if (c is '%') {
        c = la_get_char (this);
        if (c isnot 's' and c isnot 'p' and c isnot 'd' and
            c isnot 'o' and c isnot 'x' and c isnot 'f') {
          this->print_fmt_bytes (this->err_fp, "string fmt error, unsupported directive [%c]\n", c);
          la_err_ptr (this, LA_NOTOK);
          goto theend;
        } else
          directive = c;

        if (la_peek_char (this, 0) isnot LA_TOKEN_COMMA and la_peek_char (this, 1) isnot ' ') {
          this->print_fmt_bytes (this->err_fp, "string fmt error, awaiting a comma and a space after directive\n");
          la_err_ptr (this, LA_NOTOK);
          goto theend;
        }

        tmp = la_get_char (this);
        prev = la_get_char (this);
        if (la_peek_char (this, 0) is LA_TOKEN_PAREN_OPEN)
          prev = la_get_char (this);

      } else
        la_unget_char (this);

      c = prev;
      prev = tmp;

      if (c is LA_TOKEN_PAREN_OPEN) {
        const char *saved_ptr = la_StringGetPtr (this->parsePtr);

        la_next_token (this);

        err = la_parse_expr (this, &value);
        if (err isnot LA_OK) {
          this->print_bytes (this->err_fp, "string fmt error, while evaluating expression\n");
          la_err_ptr (this, LA_NOTOK);
          goto theend;
        }

        const char *ptr = la_StringGetPtr (this->parsePtr);
        if (*ptr is '}') la_get_char (this);
        while (ptr isnot saved_ptr) {
          if (*ptr is '}')
            goto append_value;
          ptr--;
        }

        this->print_bytes (this->err_fp, "string fmt error, awaiting }\n");
        la_err_ptr (this, LA_NOTOK);
        goto theend;
      }

      int len = 0;
      prev = c;
      while ((c = la_get_char (this))) {
        if (c is -1) {
          this->print_bytes (this->err_fp, "string fmt error, unended string\n");
          la_err_ptr (this, LA_NOTOK);
          goto theend;
        }

        if (c is '}') break;
        if (c is LA_TOKEN_INDEX_OPEN) {
          la_unget_char (this);
          break;
        }

        sym[len++] = c;
        prev = c;
      }

      ifnot (len) {
        this->print_bytes (this->err_fp, "string fmt error, awaiting symbol\n");
        la_err_ptr (this, LA_NOTOK);
        goto theend;
      }

      sym[len] = '\0';
      la_string x = la_StringNew (sym);
      sym_t *symbol = la_lookup_symbol (this, x);

      if (NULL is symbol) {
        this->print_fmt_bytes (this->err_fp, "string fmt error, unknown symbol %s\n", sym);
        la_err_ptr (this, LA_NOTOK);
        goto theend;
      }

      value = symbol->value;

      append_value:

      switch (directive) {
        case 's':
          switch (value.type) {
            case STRING_TYPE:
              String.append_with_fmt (str, "%s", AS_STRING_BYTES(value));
              break;

            case ARRAY_TYPE: {
              VALUE v;
              this->tokenValue = value;
              err = la_parse_array_get (this, &v);
              if (err isnot LA_OK)
                this->print_bytes (this->err_fp, "string fmt error, awaiting array\n");

              switch (v.type) {
                case STRING_TYPE:
                  String.append_with_fmt (str, "%s", AS_STRING_BYTES(v));
                  break;

                default:
                  this->print_bytes (this->err_fp, "string fmt error, awaiting string type\n");

              }
              break;
            }

          }
          break;

        case 'p':
          String.append_with_fmt (str, "%p", AS_PTR(value));
          break;

        case 'o':
          String.append_with_fmt (str, "0%o", AS_INT(value));
          break;

        case 'x':
          String.append_with_fmt (str, "0x%x", AS_INT(value));
          break;

        case 'f':
          String.append_with_fmt (str, "%.15f", AS_NUMBER(value));
          break;

        case 'd':
        default:
          String.append_with_fmt (str, "%d", AS_INT(value));
      }

      directive = 'd';

      continue;
    }

    String.append_byte (str, c);
  }

  c = la_get_char (this);

  if (c isnot ')') {
    this->print_bytes (this->err_fp, "string fmt error, awaiting )\n");
    la_err_ptr (this, LA_NOTOK);
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

static int la_parse_exit (la_t *this) {
#ifdef DEBUG
  $CODE_PATH
#endif
  la_next_token (this);

  la_parse_expr (this, &this->funResult);
  this->exitValue = AS_INT(this->funResult);

  la_StringSetLen (&this->parsePtr, 0);
  this->didReturn = 1;

  return LA_ERR_EXIT;
}

static int la_parse_return (la_t *this) {
#ifdef DEBUG
  $CODE_PATH
#endif
  int err;
  la_next_token (this);

  err = la_parse_expr (this, &this->funResult);

  la_StringSetLen (&this->parsePtr, 0);
  this->didReturn = 1;

  return err;
}

static int la_define (la_t *this, const char *key, int typ, VALUE val) {
  la_define_symbol (this, this->function, (char *) key, typ, val, 0);
  return LA_OK;
}

static int la_eval_string (la_t *this, const char *buf) {
#ifdef DEBUG
  Cstring.cp ($PREV_FUNC, MAXLEN_SYMBOL_LEN + 1, " ", 1);
  $CODE_PATH
#endif
  this->script_buffer = buf;
  la_string x = la_StringNew (buf);

  int retval = la_parse_string (this, x);

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

static VALUE la_equals (VALUE x, VALUE y) {
  VALUE result = INT(0);

  switch (x.type) {
    case NUMBER_TYPE:
      switch (y.type) {
        case NUMBER_TYPE:
          result = INT(AS_NUMBER(x) == AS_NUMBER(y)); goto theend;
        case INTEGER_TYPE:
          result = INT(AS_NUMBER(x) == AS_INT(y)); goto theend;
      }
      goto theend;

    case INTEGER_TYPE:
      switch (y.type) {
        case NUMBER_TYPE:
          result = INT(AS_INT(x) == AS_NUMBER(y)); goto theend;
        case INTEGER_TYPE:
          result = INT(AS_INT(x) == AS_INT(y)); goto theend;
      }
      goto theend;

    case STRING_TYPE:
      switch (y.type) {
        case STRING_TYPE:
          result = INT(Cstring.eq (AS_STRING_BYTES(x), AS_STRING_BYTES(y)));
          goto theend;
      }
      goto theend;

    case ARRAY_TYPE:
      switch (y.type) {
        case ARRAY_TYPE:
          result = INT(la_array_eq (x, y));
          goto theend;
      }
      goto theend;
  }

theend:
  return result;
}


static VALUE la_ne (VALUE x, VALUE y) {
  VALUE result = la_equals (x, y);
  result = INT(0 == AS_INT(result));
  return result;
}

static VALUE la_lt (VALUE x, VALUE y) {
  VALUE result = INT(0);

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

static VALUE la_le (VALUE x, VALUE y) {
  VALUE result = INT(0);

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

static VALUE la_gt (VALUE x, VALUE y) {
  VALUE result = INT(0);

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

static VALUE la_ge (VALUE x, VALUE y) {
  VALUE result = INT(0);

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

static VALUE la_mod (VALUE x, VALUE y) {
  VALUE result = INT(0);

  switch (x.type) {
    case NUMBER_TYPE: result = INT(0); goto theend; // error

    case INTEGER_TYPE:
      switch (y.type) {
        case NUMBER_TYPE: result = INT(0); goto theend; // error
        case INTEGER_TYPE:
          result = INT(AS_INT(x) % AS_INT(y)); goto theend;
      }
  }

theend:
  return result;
}

static VALUE la_sum  (VALUE x, VALUE y) {
  VALUE result = INT(0);

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
      }
  }

theend:
  return result;
}

static VALUE la_prod (VALUE x, VALUE y) {
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

static VALUE la_quot (VALUE x, VALUE y) {
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

static VALUE la_diff (VALUE x, VALUE y) {
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

static VALUE la_bset (VALUE x, VALUE y) {
  int xx = AS_INT(x); xx |= AS_INT(y); x = INT(xx);
  return x;
}

static VALUE la_bnot (VALUE x, VALUE y) {
  int xx = AS_INT(x); xx &= AS_INT(y); x = INT(xx);
  return x;
}

static VALUE la_shl  (VALUE x, VALUE y) {
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

static VALUE la_shr  (VALUE x, VALUE y) {
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

static VALUE la_bitor (VALUE x, VALUE y) {
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

static VALUE la_bitand (VALUE x, VALUE y) {
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

static VALUE la_bitxor (VALUE x, VALUE y) {
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

static VALUE la_logical_and (VALUE x, VALUE y) {
  VALUE result = INT(0);

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

static VALUE la_logical_or (VALUE x, VALUE y) {
  VALUE result = INT(0);

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

#define NONE_VALUE INT(0)
static struct def {
  const char *name;
  int toktype;
  VALUE val;
} la_defs[] = {
  { "var",     LA_TOKEN_VARDEF,   NONE_VALUE },
  { "const",   LA_TOKEN_CONSTDEF, NONE_VALUE },
  { "else",    LA_TOKEN_ELSE,     NONE_VALUE },
  { "break",   LA_TOKEN_BREAK,    NONE_VALUE },
  { "continue",LA_TOKEN_CONTINUE, NONE_VALUE },
  { "=",       LA_TOKEN_NONE,     NONE_VALUE },
  { "+=",      LA_TOKEN_NONE,     NONE_VALUE },
  { "-=",      LA_TOKEN_NONE,     NONE_VALUE },
  { "/=",      LA_TOKEN_NONE,     NONE_VALUE },
  { "*=",      LA_TOKEN_NONE,     NONE_VALUE },
  { "&=",      LA_TOKEN_NONE,     NONE_VALUE },
  { "|=",      LA_TOKEN_NONE,     NONE_VALUE },
  { "if",      LA_TOKEN_IF,       PTR(la_parse_if) },
  { "ifnot",   LA_TOKEN_IFNOT,    PTR(la_parse_ifnot) },
  { "while",   LA_TOKEN_WHILE,    PTR(la_parse_while) },
  { "for",     LA_TOKEN_FOR,      PTR(la_parse_for) },
  { "forever", LA_TOKEN_FOREVER,  PTR(la_parse_forever) },
  { "loop",    LA_TOKEN_LOOP,     PTR(la_parse_loop) },
  { "print",   LA_TOKEN_PRINT,    PTR(la_parse_print) },
  { "func",    LA_TOKEN_FUNCDEF,  PTR(la_parse_func_def) },
  { "return",  LA_TOKEN_RETURN,   PTR(la_parse_return) },
  { "exit",    LA_TOKEN_EXIT,     PTR(la_parse_exit) },
  { "array",   LA_TOKEN_ARYDEF,   PTR(la_parse_array_def) },
  { "*",       BINOP(1),          PTR(la_prod) },
  { "/",       BINOP(1),          PTR(la_quot) },
  { "%",       BINOP(1),          PTR(la_mod) },
  { "+",       BINOP(2),          PTR(la_sum) },
  { "-",       BINOP(2),          PTR(la_diff) },
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
  { "OK",      INTEGER_TYPE,      INT(0) },
  { "NOTOK",   INTEGER_TYPE,      INT(-1) },
  { NULL, 0, NONE_VALUE }
};

struct la_def_fun_t {
  const char *name;
  VALUE val;
  int nargs;
} la_funs[] = {
  { "not",     PTR(la_not), 1},
  { "bool",    PTR(la_bool), 1},
  { "len",     PTR(la_len), 1},
  { "free",    PTR(la_free), 1},
  { "malloc",  PTR(la_malloc), 1},
  { "realloc", PTR(la_realloc), 2},
  { "typeof",  PTR(la_typeof), 1},
  { NULL, NONE_VALUE, 0},
};

/* ABSTRACTION CODE */

static int la_print_bytes (FILE *fp, const char *bytes) {
  if (NULL is bytes) return 0;
  string_t *parsed = IO.parse_escapes ((char *)bytes);
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

static int la_eval_file (la_t *this, const char *filename) {
  ifnot (File.exists (filename)) {
    this->print_fmt_bytes (this->err_fp, "%s: doesn't exists\n", filename);
    return NOTOK;
  }

  FILE *fp = fopen (filename, "r");
  if (NULL is fp) {
    this->print_fmt_bytes (this->err_fp, "%s\n", Error.errno_string (errno));
    return NOTOK;
  }

  if (-1 is fseek (fp, 0, SEEK_END)) {
    fclose (fp);
    this->print_fmt_bytes (this->err_fp, "%s\n", Error.errno_string (errno));
    return NOTOK;
  }

  long n = ftell (fp);

  if (-1 is n) {
    fclose (fp);
    this->print_fmt_bytes (this->err_fp, "%s\n", Error.errno_string (errno));
    return NOTOK;
  }

  if (-1 is fseek (fp, 0, SEEK_SET)) {
    fclose (fp);
    this->print_fmt_bytes (this->err_fp, "%s\n", Error.errno_string (errno));
    return NOTOK;
  }

  char script[n + 1];
  int r = fread (script, 1, n, fp);
  fclose (fp);

  if (r <= 0) {
    this->print_fmt_bytes (this->err_fp, "Couldn't read script\n");
    return NOTOK;
  }

  if (r > n) {
    this->print_fmt_bytes (this->err_fp, "race condition, aborts now\n");
    return NOTOK;
  }

  script[r] = '\0';

  r = la_eval_string (this, script);

  if (r isnot LA_OK) {
    char *err_msg[] = {"NO MEMORY", "SYNTAX ERROR", "UNKNOWN SYMBOL",
        "BAD ARGUMENTS", "TOO MANY ARGUMENTS"};
    this->print_fmt_bytes (this->err_fp, "%s\n", err_msg[-r - 2]);
  }

  return r;
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
  Imap.release   (this->refcount);
  fun_release (&this->function);

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
}

static void la_set_define_funs_cb (la_t *this, LaDefineFuns_cb cb) {
  this->define_funs_cb = cb;
}

static void la_set_user_data (la_t *this, void *user_data) {
  this->user_data = user_data;
}

static VALUE la_get_last_expr_value (la_t *this) {
  return this->lastExpr;
}

static void *la_get_user_data (la_t *this) {
  return this->user_data;
}

static char *la_get_message (la_t *this) {
  return this->message->bytes;
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
    la_name_gen (this->name, &$my(name_gen), "i:", 2);
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
  this->stackValIdx = -1;

  if (NULL is opts.la_dir)
    this->la_dir = String.new (32);
  else
    this->la_dir = String.new_with (opts.la_dir);

  this->message = String.new (32);

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
    err = la_define (this, la_funs[i].name, CFUNC (la_funs[i].nargs), la_funs[i].val);

    if (err isnot LA_OK) {
      la_release (&this);
      return err;
    }
  }

  if (LA_OK isnot opts.define_funs_cb (this)) {
    la_release (&this);
    return err;
  }

  this->refcount = Imap.new (256);

  la_append_instance (interp, this);

  return LA_OK;
}

static la_t *la_init_instance (la_T *__la__, la_opts opts) {
  la_t *this = la_new (__la__);

  la_init (__la__, this, opts);

  return this;
}

static int la_load_file (la_T *__la__, la_t *this, char *fn) {
  if (this is NULL)
    this = la_init_instance (__la__, LaOpts());

  ifnot (Path.is_absolute (fn)) {
    if (File.exists (fn) and File.is_reg (fn))
      return la_eval_file (this, fn);

    size_t fnlen = bytelen (fn);
    char fname[fnlen+3];
    Cstring.cp (fname, fnlen + 1, fn, fnlen);

    char *extname = Path.extname (fname);
    size_t extlen = bytelen (extname);

    if (0 is extlen or 0 is Cstring.eq (".i", extname)) {
      fname[fnlen] = '.'; fname[fnlen+1] = 'i'; fname[fnlen+2] = '\0';
      if (File.exists (fname))
        return la_eval_file (this, fname);

      fname[fnlen] = '\0';
    }

    ifnot (this->la_dir->num_bytes) {
      la_set_message_fmt (this, 0, "%s: couldn't locate script", fn);
      return NOTOK;
    }

    string_t *ddir = this->la_dir;
    size_t len = ddir->num_bytes + bytelen (fname) + 2 + 7;
    char tmp[len + 3];
    Cstring.cp_fmt (tmp, len + 1, "%s/scripts/%s", ddir->bytes, fname);

    if (0 is File.exists (tmp) or 0 is File.is_reg (tmp)) {
      tmp[len] = '.'; tmp[len+1] = 'i'; tmp[len+2] = '\0';
    }

    ifnot (File.exists (tmp)) {
      la_set_message_fmt (this, 0, "%s: couldn't locate script", fn);
      return NOTOK;
    }

    return la_eval_file (this, tmp);
  }

  ifnot (File.exists (fn)) {
    la_set_message_fmt (this, 0, "%s: couldn't locate script", fn);
    return NOTOK;
  }

  return la_eval_file (this, fn);
}

public la_T *__init_la__ (void) {
  __INIT__ (io);
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
        .message = la_get_message,
        .current = la_get_current,
        .eval_str = la_get_eval_str,
        .user_data = la_get_user_data,
        .current_idx = la_get_current_idx,
        .last_expr_value = la_get_last_expr_value
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
