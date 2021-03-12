/* Derived from Tinyscript project at:
 * https://github.com/totalspectrum/
 * 
 * - added dynamic allocation
 * - ability to create more than one instance
 * - an instance is being passed as the first function argument
 * - \ as the last character in the line is a continuation character
 * - syntax_error() got a char * argument, that describes the error
 * - added ignore_next_char() to ignore next char in parsePtr
 * - print* function references got a FILE * argument
 * - remove abort() and disable exit()
 * - added is, isnot, true, false, ifnot, OK and NOTOK keywords
 * - added println (that emit a newline character, while print does not)
 * - added the ability to pass literal strings when calling C defined functions
 *   (those they ought to be freed automatically after ending the script)
 * - added the ability to assign literal strings
 *   (those they should be freed by the user)
 *   (when assigning such a variable to another, the behavior is undefined)
 * - added constant type objects (those can not be reassigned)
 * - added C-strings support (note that same danger rules with C apply to this interface)
 *   (that means out of bound operations and that those should be freed by the user) 
 * - define symbols by using a hash/map type
 * - removed memory arena and operations based on its existance
 *   (that means that there is no case of out of memory operations)
 * - added multibyte support for subscripted chars e.g., 'c', so 'α' will return 945
 * - print functions can print multibyte characters
 * - parse escape sequences when printing
 * - functions can be defined in arbitrary nested depth
 * - and quite of many changes that integrates the original code to this environment
 * Note, that tinyscript scripts are not guaranteed to run without modifications.
 * Generally speaking, this implementation except its usefulness (as it is provides
 * a scripting environment to applications), is being used mostly as a prototype to
 * set semantics.
 */

#define LIBRARY "i"

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
#define REQUIRE_I_TYPE       DONOT_DECLARE

#include <z/cenv.h>

#define $myprop      this->prop
#define $my(__v__)   $myprop->__v__

#define MAX_BUILTIN_PARAMS 9
#define MAXLEN_SYMBOL_LEN  64
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
#define $CUR_VALUE    (ival_t) this->tokenValue
#define $CODE_PATH   fprintf (this->err_fp,                                     \
  "CurIdx   : %d,  PrevFunc : %s,\n"                                            \
  "CurFunc  : %s,  CurScope : %s,\n"                                            \
  "CurToken : ['%c', %d], CurValue : %ld\n",                                    \
  $CUR_IDX++, $PREV_FUNC,                                                       \
  $CUR_FUNC, $CUR_SCOPE, $CUR_TOKEN, $CUR_TOKEN, $CUR_VALUE);                   \
  Cstring.cp ($PREV_FUNC, MAXLEN_SYMBOL_LEN + 1, $CUR_FUNC, MAXLEN_SYMBOL_LEN); \
  fprintf (this->err_fp, "CurStringToken : ['");                                \
  i_print_istring (this, this->err_fp, this->curStrToken);                      \
  fprintf (this->err_fp, "']\n\n");

#endif

#define FUNCTION_ARGUMENT_SCOPE        (1 << 0)
#define FUNC_CALL_BUILTIN              (1 << 1)
#define AT_LOOP                        (1 << 2)
#define BREAK                          (1 << 3)
#define CONTINUE                       (1 << 4)

#define BINOP(x) (((x) << 8) + TOK_BINOP)
#define CFUNC(x) (((x) << 8) + BUILTIN)

#define INT      0x0  // integer
//#define STRING   0x1  // string
//#define OPERATOR 0x2  // operator; precedence in high 8 bits
//#define ARG      0x3  // argument; value is offset on stack
#define ARRAY    0x4  // integer array
#define BUILTIN  'B'  // builtin: number of operands in high 8 bits
#define USRFUNC  'f'  // user defined a procedure; number of operands in high 8 bits
#define TOK_BINOP 'o'

#define I_TOK_SYMBOL     'A'
#define I_TOK_BUILTIN    'B'
#define I_TOK_CHAR       'C'
#define I_TOK_FUNCDEF    'F'
#define I_TOK_IFNOT      'I'
#define I_TOK_CONTINUE   'O'
#define I_TOK_PRINT      'P'
#define I_TOK_RETURN     'R'
#define I_TOK_STRING     'S'
#define I_TOK_VARDEF     'V'
#define I_TOK_EXIT       'X'
#define I_TOK_ARYDEF     'Y'
#define I_TOK_SYNTAX_ERR 'Z'
#define I_TOK_BREAK      'b'
#define I_TOK_CONSTDEF   'c'
#define I_TOK_ELSE       'e'
#define I_TOK_USRFUNC    'f'
#define I_TOK_IF         'i'
#define I_TOK_NUMBER     'n'
#define I_TOK_BINOP      'o'
#define I_TOK_VAR        'v'
#define I_TOK_WHILE      'w'
#define I_TOK_HEX_NUMBER 'x'
#define I_TOK_ARY        'y'

typedef struct istring_t {
  unsigned len_;
  const char *ptr_;
} istring_t;

typedef struct malloced_string malloced_string;

struct malloced_string {
  char *data;
  malloced_string *next;
  int is_const;
};

typedef struct sym_t {
  int type;
  int is_const;
  ival_t value;
} sym_t;

typedef struct i_stackval_t i_stackval_t;

struct i_stackval_t {
  ival_t data;
  i_stackval_t *next;
};

typedef struct i_stack {
  i_stackval_t *head;
} i_stack;

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

  istring_t body;

  int nargs;
  char argName[MAX_BUILTIN_PARAMS][MAXLEN_SYMBOL_LEN];

  Vmap_t *symbols;

  funT
    *root,
    *prev,
    *next;
};

typedef struct funNewArgs {
  const char *name;
  size_t namelen;
  istring_t body;
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

struct i_prop {
  int name_gen;

  i_t *head;
  int num_instances;
  int current_idx;
};

struct i_t {
  funT *function;
  funT *curScope;

  fun_stack funstack[1];
  symbol_stack symbolstack[1];
  Imap_t *refcount;

  char name[32];

  const char *script_buffer;

  int
    curState,
    exitValue,
    lineNum,
    curToken,    // kind of current token
    tokenArgs,   // number of arguments for this token
    didReturn;

  string_t
    *idir,
    *message;

  istring_t
    curStrToken, // the actual string representing the token
    parsePtr;    // acts as instruction pointer

  malloced_string *head;

  ival_t
     funResult,  // function returned value
     tokenValue, // for symbolic tokens, the symbol's value
     funArgs[MAX_BUILTIN_PARAMS];

  i_stack stack[1];

  sym_t
    *tokenSymbol;

  FILE
    *err_fp,
    *out_fp;

  i_prop *prop;

  void *user_data;

  IPrintByte_cb print_byte;
  IPrintBytes_cb print_bytes;
  IPrintFmtBytes_cb print_fmt_bytes;
  ISyntaxError_cb syntax_error;
  IDefineFuns_cb define_funs_cb;

  i_t *next;
};

#define MAX_EXPR_LEVEL 5

static int i_parse_stmt (i_t *);
static int i_parse_expr (i_t *, ival_t *);
static int i_parse_primary (i_t *, ival_t *);

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

static void i_set_message (i_t *this, int append, char *msg) {
  if (NULL is msg) return;
  if (append)
    String.append_with (this->message, msg);
  else
    String.replace_with (this->message, msg);
}

static void i_set_message_fmt (i_t *this, int append, char *fmt, ...) {
  size_t len = VA_ARGS_FMT_SIZE(fmt);
  char bytes[len + 1];
  VA_ARGS_GET_FMT_STR(bytes, len, fmt);
  i_set_message (this, append, bytes);
}

static inline unsigned i_StringGetLen (istring_t s) {
  return (unsigned)s.len_;
}

static inline const char *i_StringGetPtr (istring_t s) {
  return (const char *)(ival_t)s.ptr_;
}

static inline void i_StringSetLen (istring_t *s, unsigned len) {
  s->len_ = len;
}

static inline void i_StringSetPtr (istring_t *s, const char *ptr) {
  s->ptr_ = ptr;
}

static inline istring_t i_StringNew (const char *str) {
  istring_t x;
  i_StringSetLen (&x, bytelen (str));
  i_StringSetPtr (&x, str);
  return x;
}

static void i_print_istring (i_t *this, FILE *fp, istring_t s) {
  unsigned len = i_StringGetLen (s);
  const char *ptr = (const char *) i_StringGetPtr (s);
  while (len > 0) {
    this->print_byte (fp, *ptr);
    ptr++;
    --len;
  }
}

static int i_err_ptr (i_t *this, int err) {
  const char *keep = i_StringGetPtr (this->parsePtr);
  size_t len = i_StringGetLen (this->parsePtr);

  char *sp = (char *) keep;
  while (sp > this->script_buffer and 0 is Cstring.byte.in_str (";\n", *(sp - 1)))
    sp--;

  i_StringSetPtr (&this->parsePtr, sp);
  i_StringSetLen (&this->parsePtr, len + (keep - sp));

  i_print_istring (this, this->err_fp, this->parsePtr);

  i_StringSetPtr (&this->parsePtr, keep);
  i_StringSetLen (&this->parsePtr, len);

  this->print_bytes (this->err_fp, "\n");

  return err;
}

static int i_syntax_error (i_t *this, const char *msg) {
  this->print_fmt_bytes (this->err_fp, "\nSYNTAX ERROR: %s\n", msg);
  return i_err_ptr (this, I_ERR_SYNTAX);
}

static int i_arg_mismatch (i_t *this) {
  this->print_fmt_bytes (this->err_fp, "\nargument mismatch:");
  return i_err_ptr (this, I_ERR_BADARGS);
}

static int i_too_many_args (i_t *this) {
  this->print_fmt_bytes (this->err_fp, "\ntoo many arguments:");
  return i_err_ptr (this, I_ERR_TOOMANYARGS);
}

static int i_unknown_symbol (i_t *this) {
  this->print_fmt_bytes (this->err_fp, "\nunknown symbol:");
  return i_err_ptr (this, I_ERR_UNKNOWN_SYM);
}

static int i_out_of_bounds (i_t *this) {
  this->print_fmt_bytes (this->err_fp, "\nout of bounds:");
  return i_err_ptr (this, I_ERR_OUTOFBOUNDS);
}

static void i_reset_token (i_t *this) {
  i_StringSetLen (&this->curStrToken, 0);
  i_StringSetPtr (&this->curStrToken, i_StringGetPtr (this->parsePtr));
}

static void i_ignore_last_token (i_t *this) {
  i_StringSetLen (&this->curStrToken, i_StringGetLen (this->curStrToken) - 1);
}

static void i_ignore_first_token (i_t *this) {
  i_StringSetPtr (&this->curStrToken, i_StringGetPtr (this->curStrToken) + 1);
  i_StringSetLen (&this->curStrToken, i_StringGetLen (this->curStrToken) - 1);
}

static void i_ignore_next_char (i_t *this) {
  i_StringSetPtr (&this->parsePtr, i_StringGetPtr (this->parsePtr) + 1);
  i_StringSetLen (&this->parsePtr, i_StringGetLen (this->parsePtr) - 1);
}

static int i_peek_char (i_t *this, unsigned int n) {
  if (i_StringGetLen (this->parsePtr) <= n) return -1;
  return *(i_StringGetPtr (this->parsePtr) + n);
}

static int i_get_char (i_t *this) {
  unsigned int len = i_StringGetLen (this->parsePtr);

  ifnot (len) return -1;

  const char *ptr = i_StringGetPtr (this->parsePtr);
  int c = *ptr++;

  --len;

  i_StringSetPtr (&this->parsePtr, ptr);
  i_StringSetLen (&this->parsePtr, len);
  i_StringSetLen (&this->curStrToken, i_StringGetLen (this->curStrToken) + 1);
  return c;
}

static void i_unget_char (i_t *this) {
  i_StringSetLen (&this->parsePtr, i_StringGetLen (this->parsePtr) + 1);
  i_StringSetPtr (&this->parsePtr, i_StringGetPtr (this->parsePtr) - 1);
  i_ignore_last_token (this);
}

static int i_ignore_ws (i_t *this) {
  int c;
  for (;;) {
    c = i_get_char (this);

    if (is_space (c)) {
      i_reset_token (this);
    } else
      break;
  }

  return c;
}

static void i_get_span (i_t *this, int (*testfn) (int)) {
  int c;
  do
    c = i_get_char (this);
  while (testfn (c));

  if (c isnot -1) i_unget_char (this);
}

static void fun_release (funT **thisp) {
  if (*thisp is NULL) return;
  funT *this = *thisp;
  Vmap.release (this->symbols);
  free (this);
  *thisp = NULL;
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

static funT *Fun_new (i_t *this, funNewArgs options) {
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

static void i_release_malloced_strings (i_t *this, int release_const) {
#ifdef DEBUG
  $CODE_PATH
#endif
  malloced_string *item = this->head;
  while (item isnot NULL) {
    malloced_string *tmp = item->next;
    if (item->is_const is 0 or (item->is_const and release_const)) {
      free (item->data);
      free (item);
    }
    item = tmp;
  }
  this->head = NULL;
}

static void i_release_sym (void *sym) {
  if (sym is NULL) return;

  sym_t *this = (sym_t *) sym;

  if ((this->type & 0xff) is USRFUNC) {
    funT *f = (funT *) this->value;
    fun_release (&f);
  }

  if ((this->type & 0xff) is ARRAY)
    free ((char *) this->value);

  free (this);
  this = NULL;
}

static sym_t *i_define_symbol (i_t *this, funT *f, istring_t name, int typ, ival_t value, int is_const) {
#ifdef DEBUG
  if ($CUR_IDX < 65) {
    $CUR_IDX++;
    goto body;
  }
  $CODE_PATH
#endif
body:
  (void) this;
  if (i_StringGetPtr (name) is NULL) return NULL;

  size_t len = i_StringGetLen (name);
  char key[len + 1];
  Cstring.cp (key, len + 1, i_StringGetPtr (name), len);

  sym_t *sym = Alloc (sizeof (sym_t));
  sym->type = typ;
  sym->value = value;
  sym->is_const = is_const;

  if (NOTOK is Vmap.set (f->symbols, key, sym, i_release_sym, is_const)) {
    free (sym);
    return NULL;
  }

  return sym;
}

static sym_t *i_lookup_symbol (i_t *this, istring_t name) {
#ifdef DEBUG
  $CODE_PATH
#endif
  size_t len = i_StringGetLen (name);
  char key[len + 1];
  Cstring.cp (key, len + 1, i_StringGetPtr (name), len);

  sym_t *sym = NULL;

  funT *f = this->curScope;
  while (NULL isnot f) {
    sym = Vmap.get (f->symbols, key);

    ifnot (NULL is sym) return sym;

    f = f->prev;
  }

  return NULL;
}

static sym_t *i_define_var_symbol (i_t *this, funT *f, istring_t name, int is_const) {
  return i_define_symbol (this, f, name, INT, 0, is_const);
}

static int i_do_next_token (i_t *this, int israw) {
#ifdef DEBUG
  $CODE_PATH
#endif
  int r = I_NOTOK;

  sym_t *symbol = NULL;
  this->tokenSymbol = NULL;

  i_reset_token (this);

  int c = i_ignore_ws (this);

  if (c is '\\' and i_peek_char (this, 0) is '\n') {
    this->lineNum++;
    i_ignore_next_char (this);
    i_reset_token (this);
    c = i_ignore_ws (this);
  }

  if (c is '#') {
    do
      c = i_get_char (this);
    while (c >= 0 and c isnot '\n');
    this->lineNum++;

    r = c;

  } else if (IS_DIGIT (c)) {
    if (c is '0' and NULL isnot Cstring.byte.in_str ("xX", i_peek_char (this, 0))
        and is_hexchar (i_peek_char(this, 1))) {
      i_get_char (this);
      i_ignore_first_token (this);
      i_ignore_first_token (this);
      i_get_span (this, is_hexchar);
      r = I_TOK_HEX_NUMBER;
    } else {
      i_get_span (this, is_digit);
      r = I_TOK_NUMBER;
    }

  } else if (c is '\'') {
      c = i_get_char (this); // get first
      if (c is '\\') i_get_char (this);
      int max = 4;
      r = I_TOK_SYNTAX_ERR;

      /* multibyte support */
      do {
        c = i_get_char (this);
        if (c is '\'') {
          i_ignore_first_token (this);
          i_ignore_last_token (this);
          r = I_TOK_CHAR;
          break;
        }
      } while (--max isnot 0);

  } else if (is_alpha (c)) {
    i_get_span (this, is_identifier);

    r = I_TOK_SYMBOL;

    // check for special tokens
    ifnot (israw) {
      this->tokenSymbol = symbol = i_lookup_symbol (this, this->curStrToken);

      if (symbol) {
        r = symbol->type & 0xff;

        this->tokenArgs = (symbol->type >> 8) & 0xff;
        if (r is ARRAY)
          r = I_TOK_ARY;
        else
          if (r < '@')
             r = I_TOK_VAR;

        this->tokenValue = symbol->value;
      }
    }

  } else if (is_operator (c)) {
    i_get_span (this, is_operator_span);

    this->tokenSymbol = symbol = i_lookup_symbol (this, this->curStrToken);

    if (symbol) {
      r = symbol->type;
      this->tokenValue = symbol->value;
    } else
      r = I_TOK_SYNTAX_ERR;

  } else if (c is '{') {
    int bracket = 1;
    i_reset_token (this);
    while (bracket > 0) {
      c = i_get_char (this);
      if (c < 0) return I_TOK_SYNTAX_ERR;

      if (c is '}')
        --bracket;
      else if (c is '{')
        ++bracket;
    }

    i_ignore_last_token (this);
    r = I_TOK_STRING;

  } else if (c is '"') {
    size_t len = 0;
    int pc = 0;
    int cc = 0;

    while (pc = cc, (cc = i_peek_char (this, len)) isnot -1) {
      if ('"' is cc and pc isnot '\\') break;
      len++;
    }

    if (cc is -1)
      return this->syntax_error (this, "unended string, a '\"' is missing");

    ifnot (this->curState & FUNCTION_ARGUMENT_SCOPE) {
      char *str = Alloc (len + 1);
      for (size_t i = 0; i < len; i++) {
        c = i_get_char (this);
        str[i] = c;
      }
      str[len] = '\0';
      this->tokenValue = (ival_t) str;

    } else {
      this->curState &= ~(FUNC_CALL_BUILTIN);
      malloced_string *mbuf = Alloc (sizeof (malloced_string));
      mbuf->data = Alloc (len + 1);
      for (size_t i = 0; i < len; i++) {
        c = i_get_char (this);
        mbuf->data[i] = c;
      }

      mbuf->data[len] = '\0';

      ListStackPush (this, mbuf);
      mbuf->is_const = 0;
      this->tokenValue = (ival_t) mbuf->data;
    }

    c = i_get_char (this);
    i_reset_token (this);

    r = I_TOK_STRING;

  } else
    r = c;

  this->curToken = r;
  return r;
}

static int i_next_token (i_t *this) {
  return i_do_next_token (this, 0);
}

static int i_next_raw_token (i_t *this) {
  return i_do_next_token (this, 1);
}

static void i_stack_push (i_t *this, ival_t x) {
  i_stackval_t *item = Alloc (sizeof (i_stackval_t));
  item->data = x;
  ListStackPush (this->stack, item);
}

static ival_t i_stack_pop (i_t *this) {
  i_stackval_t *item = ListStackPop (this->stack, i_stackval_t);
  ival_t data = item->data;
  free (item);
  return data;
}

static void i_fun_stack_push (i_t *this, funT *f) {
  funstack_t *item = Alloc (sizeof (funstack_t));
  item->f = f;
  ListStackPush (this->funstack, item);
}

static funT *i_fun_stack_pop (i_t *this) {
  funstack_t *item = ListStackPop (this->funstack, funstack_t);
  funT *f = item->f;
  free (item);
  return f;
}

static void *i_clone_sym_item (void *item) {
  sym_t *sym = (void *) item;

  sym_t *new = Alloc (sizeof (sym_t));

  new->type = sym->type;
  new->is_const = sym->is_const;

  if ((new->type & 0xff) is ARRAY) {
    char *a = (char *) sym->value;
    size_t len = a[0];
    char *ary =  Alloc (sizeof (ival_t) * len);
    for (size_t i = 0; i < len + 1; i++)
      ary[i] = a[i];
    new->value = (ival_t) ary;
    return new;
  }

  new->value = sym->value;
  return new;
}

static void i_symbol_stack_push (i_t *this, Vmap_t *symbols) {
  symbolstack_t *item = Alloc (sizeof (symbolstack_t));
  item->symbols = Vmap.clone (symbols, i_clone_sym_item);
  ListStackPush (this->symbolstack, item);
}

static Vmap_t *i_symbol_stack_pop (i_t *this) {
  symbolstack_t *item = ListStackPop (this->symbolstack, symbolstack_t);
  Vmap_t *symbols = item->symbols;
  free (item);
  return symbols;
}

static ival_t i_string_to_num (istring_t s) {
  ival_t r = 0;
  int c;
  const char *ptr = i_StringGetPtr (s);
  int len = i_StringGetLen (s);
  while (len-- > 0) {
    c = *ptr++;
    ifnot (is_digit (c)) break;
    r = 10 * r + (c - '0');
  }
  return r;
}

static ival_t i_HexStringToNum (istring_t s) {
  ival_t r = 0;
  int c;
  const char *ptr = i_StringGetPtr (s);
  int len = i_StringGetLen (s);
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
  return r;
}

/* Initial Array Interface by MickeyDelp <mickey at delptronics dot com> */

// assign a value or list of values to an array
static int i_array_assign (i_t *this, ival_t *ary, ival_t ix) {
  int err;
  ival_t val;
  do {
    if (ix < 0 or ix >= ary[0]) {
      return i_out_of_bounds (this);
    }

    i_next_token (this);

    err = i_parse_expr (this, &val);
    if (err isnot I_OK) return err;

    ary[ix + 1] = val;
    ix++;
  } while (this->curToken is ',');

   return I_OK;
}

// handle defining an array
static int i_parse_array_def (i_t *this) {
  istring_t name;
  int c;
  int err;
  ival_t len;

  c = i_next_raw_token (this);

  if (c isnot I_TOK_SYMBOL) {
    return this->syntax_error (this, "syntax error");
  }

  name = this->curStrToken;

  c = i_next_token (this);

  if (c isnot '(')
    return this->syntax_error (this, "syntax error");

  err = i_parse_primary (this, &len);
  if (err isnot I_OK)
    return err;

  len++;

  char *ary = Alloc (sizeof (ival_t) * len);

  memset (ary, 0, len * sizeof (ival_t));

  ((ival_t *) ary)[0] = len - 1;

  this->tokenSymbol = i_define_symbol (this, this->curScope, name, ARRAY, (ival_t) ary, 0);

  if (i_StringGetPtr (this->curStrToken)[0] is '=' and i_StringGetLen (this->curStrToken) is 1)
    return i_array_assign (this, (ival_t *)ary, 0);

  return I_OK;
}

// handle setting an array value
static int i_parse_array_set (i_t *this) {
  int err;
  ival_t ix = 0;

  ival_t *ary = (ival_t *) this->tokenValue;

  int c = i_next_token (this);

  if (c is '(') {
    err = i_parse_primary (this, &ix);
    if (err isnot I_OK)
      return err;
  }

  if (i_StringGetPtr (this->curStrToken)[0] isnot '=' or i_StringGetLen (this->curStrToken) isnot 1)
    return this->syntax_error (this, "syntax error");

  return i_array_assign (this, ary, ix);
}

// handle getting an array value
static int i_parse_array_get (i_t *this, ival_t *vp) {
  ival_t* ary = (ival_t *) this->tokenValue;

  int c = i_next_token (this);

  if (c is '(') {
    ival_t ix;
    int err = i_parse_primary (this, &ix);
    if (err isnot I_OK)
      return err;

    if (ix < -1 or ix >= ary[0])
      return i_out_of_bounds (this);

    *vp = ary[ix + 1];
  } else {
    // if no parens, then return the pointer to the array
    // needed for passing to C functions
    *vp = (ival_t ) ary;
  }

  return I_OK;
}

static int i_parse_expr_list (i_t *this) {
#ifdef DEBUG
  $CODE_PATH
#endif
  int err, c;
  int count = 0;
  ival_t v;

  do {
    err = i_parse_expr (this, &v);
    if (err isnot I_OK) return err;

    i_stack_push (this, v);

    count++;

    c = this->curToken;
    if (c is ',') i_next_token (this);
  } while (c is ',');

  return count;
}

static int i_parse_char (i_t *this, ival_t *vp, istring_t token) {
  const char *ptr = i_StringGetPtr (token);

  if (ptr[0] is '\'') return this->syntax_error (this, "error while getting a char token ");
  if (ptr[0] is '\\') {
    /* if (i_StringGetLen(token) != 2) return SyntaxError(); */
    if (ptr[1] is 'n') { *vp = '\n'; return I_OK; }
    if (ptr[1] is 't') { *vp = '\t'; return I_OK; }
    if (ptr[1] is 'r') { *vp = '\r'; return I_OK; }
    if (ptr[1] is '\\') { *vp = '\\'; return I_OK; }
    if (ptr[1] is '\'') { *vp = '\''; return I_OK; }
    return this->syntax_error (this, "unknown escape sequence");
  }

  if (ptr[0] >= ' ' and ptr[0] <= '~') {
    if (ptr[1] is '\'') {
      *vp = ptr[0];
      return I_OK;
    } else {
      return this->syntax_error (this, "error while taking character literal");
    }
  }

  /* multibyte support */
  int len = 0;
  utf8 c = Ustring.get.code_at ((char *) ptr, 4, 0, &len);

  if ('\'' isnot ptr[len])
    return this->syntax_error (this, "error while taking character literal");

  *vp = c;
  return I_OK;
}

static int i_parse_string (i_t *this, istring_t str) {
#ifdef DEBUG
  $CODE_PATH
#endif

  int c,  r;
  istring_t savepc = this->parsePtr;

  this->parsePtr = str;

  for (;;) {
    c = i_next_token (this);

    while (c is '\n' or c is ';') {
      if (c is '\n') this->lineNum++;
      c = i_next_token (this);
     }

    if (c < 0) break;

    r = i_parse_stmt (this);

    if (r isnot I_OK) return r;

    c = this->curToken;
    if (c is '\n' or c is ';' or c < 0) {
      if (c is '\n') this->lineNum++;
      continue;
    }
    else
      return this->syntax_error (this, "evaluated string failed, unknown token");
  }

  this->parsePtr = savepc;

  return I_OK;
}

static void i_fun_refcount_incr (int *count) {
  (*count)++;
}

static void i_fun_refcount_decr (int *count) {
  (*count)--;
}

static int i_parse_func_call (i_t *this, Cfunc op, ival_t *vp, funT *uf) {
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

  c = i_next_token (this);

  if (c isnot '(') return this->syntax_error (this, "expected open parentheses");

  this->curState |= FUNCTION_ARGUMENT_SCOPE;

  c = i_next_token (this);

  if (c isnot ')') {
    paramCount = i_parse_expr_list (this);
    c = this->curToken;
    if (paramCount < 0) {
      this->curState &= ~(FUNCTION_ARGUMENT_SCOPE);
      return paramCount;
    }
  }

  this->curState &= ~(FUNCTION_ARGUMENT_SCOPE);

  if (c isnot ')')
    return this->syntax_error (this, "expected closed parentheses");

  if (expectargs isnot paramCount)
    return i_arg_mismatch (this);

  // we now have "paramCount" items pushed on to the stack
  // pop em off
  while (paramCount > 0) {
    --paramCount;
    this->funArgs[paramCount] = i_stack_pop (this);
  }

  if (uf) {
    // need to invoke the script here
    // set up an environment for the script
    int i;
    int err;

    int refcount = Imap.set_by_callback (this->refcount, uf->funname, i_fun_refcount_incr);
    if (refcount > 1) {
      i_symbol_stack_push (this, this->curScope->symbols);
      Vmap.clear (uf->symbols);
    }

    for (i = 0; i < expectargs; i++)
      i_define_symbol (this, uf, i_StringNew (uf->argName[i]), INT, this->funArgs[i], 0);

    this->didReturn = 0;

    i_fun_stack_push (this, this->curScope);

    this->curScope = uf;

    err = i_parse_string (this, uf->body);

    this->curScope = i_fun_stack_pop (this);

    this->didReturn = 0;

    refcount = Imap.set_by_callback (this->refcount, uf->funname, i_fun_refcount_decr);

    ifnot (refcount)
      Vmap.clear (uf->symbols);
    else {
      Vmap.release (uf->symbols);
      this->curScope->symbols = i_symbol_stack_pop (this);
    }

    *vp = this->funResult;
    return err;
  } else {
    *vp = op (this, this->funArgs[0], this->funArgs[1], this->funArgs[2],
                    this->funArgs[3], this->funArgs[4], this->funArgs[5],
                    this->funArgs[6], this->funArgs[7], this->funArgs[8]);

    this->curState &= ~(FUNC_CALL_BUILTIN);
  }

  i_next_token (this);

  return I_OK;
}

// parse a primary value; for now, just a number or variable
// returns 0 if valid, non-zero if syntax error
// puts result into *vp
static int i_parse_primary (i_t *this, ival_t *vp) {
#ifdef DEBUG
  $CODE_PATH
#endif
  int c, err;

  c = this->curToken;

  if (c is '(') {
    this->curState |= FUNCTION_ARGUMENT_SCOPE;

    i_next_token (this);
    err = i_parse_expr (this, vp);

    if (err is I_OK) {
      c = this->curToken;

      if (c is ')') {
        i_next_token (this);

        this->curState &= ~(FUNCTION_ARGUMENT_SCOPE);
        return I_OK;
      }
    }

    return err;

  } else if (c is I_TOK_NUMBER) {
    *vp = i_string_to_num (this->curStrToken);
    i_next_token (this);
    return I_OK;

  } else if (c is I_TOK_HEX_NUMBER) {
    *vp = i_HexStringToNum (this->curStrToken);
    i_next_token (this);
    return I_OK;

  } else if (c is I_TOK_CHAR) {
      err = i_parse_char (this, vp, this->curStrToken);

      i_next_token (this);
      return err;

  } else if (c is I_TOK_VAR) {
    *vp = this->tokenValue;
    i_next_token (this);
    return I_OK;

  } else if (c is I_TOK_ARY) {
     return i_parse_array_get (this, vp);

  } else if (c is I_TOK_BUILTIN) {
    Cfunc op = (Cfunc) this->tokenValue;
    this->curState |= FUNC_CALL_BUILTIN;
    return i_parse_func_call (this, op, vp, NULL);

  } else if (c is I_TOK_USRFUNC) {
    sym_t *symbol = this->tokenSymbol;
    ifnot (symbol)
      return this->syntax_error (this, "user defined function, not declared");

    funT *uf = (funT *) symbol->value;

    err = i_parse_func_call (this, NULL, vp, uf);
    i_next_token (this);
    return err;

  } else if ((c & 0xff) is I_TOK_BINOP) {
    // binary operator
    Opfunc op = (Opfunc) this->tokenValue;
    ival_t v;

    i_next_token (this);
    err = i_parse_expr (this, &v);
    if (err is I_OK)
      *vp = op (0, v);

    return err;

  } else if (c is I_TOK_STRING) {
    *vp = this->tokenValue;

    i_next_token (this);
    return I_OK;

  } else
    return this->syntax_error (this, "syntax error");
}

static int i_parse_stmt (i_t *this) {
#ifdef DEBUG
  $CODE_PATH
#endif
  int c;
  istring_t name;
  ival_t val;
  int err = I_OK;

  if (this->didReturn) {
    do {
      c = i_get_char (this);
    } while (c >= 0 and c isnot '\n' and c isnot ';' and c isnot '}');

    i_unget_char (this);
    i_next_token (this);

    return I_OK;
  }

  c = this->curToken;

  if (c is I_TOK_BREAK) {

  if (this->curState & AT_LOOP) {
      this->curState |= BREAK;
      return I_ERR_BREAK;
    }

    return this->syntax_error (this, "break is not in a loop");
  }

  if (c is I_TOK_CONTINUE) {
    if (this->curState & AT_LOOP) {
      this->curState |= CONTINUE;
      return I_ERR_CONTINUE;
    }

    return this->syntax_error (this, "continue is not in a loop");
  }

  if (c is I_TOK_VARDEF or c is I_TOK_CONSTDEF) {
    int is_const = c is I_TOK_CONSTDEF;

    // a definition var a=x
    c = i_next_raw_token (this); // we want to get VAR_SYMBOL directly

    if (c isnot I_TOK_SYMBOL)
      return this->syntax_error (this, "expected symbol");

    name = this->curStrToken;

    ifnot (i_StringGetLen (name))
      return this->syntax_error (this, "unknown symbol");

    this->tokenSymbol = i_define_var_symbol (this, this->curScope, name, is_const);

    if (NULL is this->tokenSymbol) {
      if (is_const)
        return this->syntax_error (this, "can not reassign to a constant");
      return this->syntax_error (this, "unknown error");
    }

    c = I_TOK_VAR;
    /* fall through */
  }

  if (c is I_TOK_VAR) {
    // is this a=expr?

    name = this->curStrToken;
    sym_t *symbol = this->tokenSymbol;

    c = i_next_token (this);

    // we expect the "=" operator, so verify that it is "="
    if (i_StringGetPtr (this->curStrToken)[0] isnot '=' or
        i_StringGetLen(this->curStrToken) isnot 1)
      return this->syntax_error (this, "expected =");

    ifnot (symbol) {
      i_print_istring (this, this->err_fp, name);
      return i_unknown_symbol (this);
    }

   if (symbol->is_const and symbol->value isnot 0)
     return this->syntax_error (this, "can not reassign to a constant");

    i_next_token (this);

    err = i_parse_expr (this, &val);

    if (err isnot I_OK) return err;

    symbol->value = val;

  } else if (c is I_TOK_ARY) {
    err = i_parse_array_set (this);

  } else if (c is I_TOK_BUILTIN or c is USRFUNC) {
    err = i_parse_primary (this, &val);
    return err;

  } else if (this->tokenSymbol and this->tokenValue) {
    int (*func) (i_t *) = (void *) this->tokenValue;
    err = (*func) (this);

  } else return this->syntax_error (this, "unknown token");

  if (err is I_ERR_OK_ELSE)
    err = I_OK;

  return err;
}

// parse a level n expression
// level 0 is the lowest level (highest precedence)
// result goes in *vp
static int i_parse_expr_level (i_t *this, int max_level, ival_t *vp) {
#ifdef DEBUG
  $CODE_PATH
#endif
  int err = I_OK;
  int c;
  ival_t lhs;
  ival_t rhs;

  lhs = *vp;
  c = this->curToken;

  while ((c & 0xff) is I_TOK_BINOP) {
    int level = (c >> 8) & 0xff;
    if (level > max_level) break;

    Opfunc op = (Opfunc) this->tokenValue;

    i_next_token (this);

    err = i_parse_primary (this, &rhs);
    if (err isnot I_OK) return err;

    c = this->curToken;

    while ((c & 0xff) is I_TOK_BINOP) {
      int nextlevel = (c >> 8) & 0xff;
      if (level <= nextlevel) break;

      err = i_parse_expr_level (this, nextlevel, &rhs);
      if (err isnot I_OK) return err;

      c = this->curToken;
    }

    lhs = op (lhs, rhs);
  }

  *vp = lhs;
  return err;
}

static int i_parse_expr (i_t *this, ival_t *vp) {
#ifdef DEBUG
  $CODE_PATH
#endif
  int err = i_parse_primary (this, vp);

  if (err is I_OK)
    err = i_parse_expr_level (this, MAX_EXPR_LEVEL, vp);

  return err;
}

static int i_parse_if_rout (i_t *this, ival_t *cond, int *haveelse, istring_t *ifpart, istring_t *elsepart) {
#ifdef DEBUG
  $CODE_PATH
#endif
  int c;
  int err;

  *haveelse = 0;

  c = i_next_token (this);

  err = i_parse_expr (this, cond);
  if (err isnot I_OK) return err;

  c = this->curToken;

  if (c isnot I_TOK_STRING) return this->syntax_error (this, "parsing if, not a string");

  *ifpart = this->curStrToken;

  c = i_next_token (this);

  if (c is I_TOK_ELSE) {
    c = i_next_token (this);

    if (c isnot I_TOK_STRING) return this->syntax_error (this, "parsing else, not a string");

    *elsepart = this->curStrToken;
    *haveelse = 1;

    i_next_token (this);
  }

  return I_OK;
}

static int i_parse_if (i_t *this) {
  istring_t ifpart, elsepart;
  int haveelse = 0;
  ival_t cond;
  int err = i_parse_if_rout (this, &cond, &haveelse, &ifpart, &elsepart);
  if (err isnot I_OK)
    return err;

  if (cond)
    err = i_parse_string (this, ifpart);
  else if (haveelse)
    err = i_parse_string (this, elsepart);

  if (err is I_OK and 0 is cond) err = I_ERR_OK_ELSE;
  return err;
}

static int i_parse_ifnot (i_t *this) {
  istring_t ifpart, elsepart;
  int haveelse = 0;
  ival_t cond;
  int err = i_parse_if_rout (this, &cond, &haveelse, &ifpart, &elsepart);
  if (err isnot I_OK)
    return err;

  if (!cond)
    err = i_parse_string (this, ifpart);
  else if (haveelse)
    err = i_parse_string (this, elsepart);

  if (err is I_OK and 0 isnot cond) err = I_ERR_OK_ELSE;
  return err;
}

static int i_parse_var_list (i_t *this, funT *uf) {
#ifdef DEBUG
  $CODE_PATH
#endif
  int c;
  int nargs = 0;

  //c = i_next_token (this);
  c = i_next_raw_token (this);

  for (;;) {
    if (c is I_TOK_SYMBOL) {
      istring_t name = this->curStrToken;

      if (nargs >= MAX_BUILTIN_PARAMS)
        return i_too_many_args (this);

      size_t len = i_StringGetLen (name);
      if (len >= MAXLEN_SYMBOL_LEN)
        return this->syntax_error (this, "argument name exceeded maximum length (64)");

      const char *ptr = i_StringGetPtr (name);
      Cstring.cp (uf->argName[nargs], MAXLEN_SYMBOL_LEN, ptr, len);

      nargs++;

      //c = i_next_raw_token (this);
      c = i_next_token (this);

      if (c is ')') break;

      if (c is ',')
        //c = i_next_token (this);
        c = i_next_raw_token (this);

    } else if (c is ')')
      break;
    else
      return this->syntax_error (this, "var definition, unexpected token");
  }

  uf->nargs = nargs;
  return nargs;
}

static int i_parse_func_def (i_t *this) {
  istring_t name;
  int c;
  int nargs = 0;

  c = i_next_raw_token (this); // do not interpret the symbol
  if (c isnot I_TOK_SYMBOL) return this->syntax_error (this, "fun definition, not a symbol");

  name = this->curStrToken;
  size_t len = i_StringGetLen (name);
  if (len >= MAXLEN_SYMBOL_LEN)
    return this->syntax_error (this, "function name exceeded maximum length (64)");

  funT *uf = Fun_new (this, funNew (
    .name = i_StringGetPtr (name), .namelen = len, .parent = this->curScope
    ));

  c = i_next_token (this);

  if (c is '(') {
    i_fun_stack_push (this, this->curScope);

    this->curScope = uf;

    nargs = i_parse_var_list (this, uf);

    this->curScope = i_fun_stack_pop (this);

    if (nargs < 0) return nargs;

    c = i_next_token (this);
  }

  if (c isnot I_TOK_STRING) return this->syntax_error (this, "function definition, not a string");

  uf->body = this->curStrToken;

  i_define_symbol (this, this->curScope, name, USRFUNC | (nargs << 8), (ival_t) uf, 0);

  i_next_token (this);

  return I_OK;
}

static int i_parse_print (i_t *this) {
  int err = I_NOTOK;

  string_t *str = String.new (32);

  int c = i_ignore_ws (this);

  if (c isnot '(') {
    this->print_bytes (this->err_fp, "string fmt error, awaiting (\n");
    i_err_ptr (this, I_NOTOK);
    goto theend;
  }

  c = i_ignore_ws (this);

  FILE *fp = this->out_fp;

  /* for now */
  if (c is 's' and Cstring.eq_n (i_StringGetPtr (this->parsePtr), "tderr,", 6)) {
    fp = stderr;
    for (int i = 0; i < 6; i++)
      i_ignore_next_char (this);
    c = i_ignore_ws (this);
  }

  if (c isnot '"') {
    this->print_bytes (this->err_fp, "string fmt error, awaiting double quote\n");
    i_err_ptr (this, I_NOTOK);
    goto theend;
  }

  int prev = c;
  char directive = 'd';

  for (;;) {
    c = i_get_char (this);
    if (c is '"') {
      if (prev isnot '\\')
        break;

      String.append_byte (str, '"');
      prev = '"';
      c = i_get_char (this);
    }

    if (c is '\\') {
      if (prev is '\\' or i_peek_char (this, 1) isnot '$') {
        prev = str->bytes[str->num_bytes - 1];
        String.append_byte (str, '\\');
      } else  prev = '\\';

      c = i_get_char (this);
    }

    if (c is '$') {
      if (prev is '\\') {
        String.append_byte (str, '$');
        prev = '$';
        continue;
      }

      prev = c;
      c = i_get_char (this);
      if (c isnot '{') {
        this->print_bytes (this->err_fp, "string fmt error, awaiting {\n");
        i_err_ptr (this, I_NOTOK);
        goto theend;
      }

      char sym[MAXLEN_SYMBOL_LEN];
      int tmp = prev;

      c = i_get_char (this);
      prev = c;

      if (c is '%') {
        c = i_get_char (this);
        if (c isnot 's' and c isnot 'p' and c isnot 'd' and
            c isnot 'o' and c isnot 'x') {
          this->print_fmt_bytes (this->err_fp, "string fmt error, unsupported directive [%c]\n", c);
          i_err_ptr (this, I_NOTOK);
          goto theend;
        } else
          directive = c;

        if (i_peek_char (this, 0) isnot ',' and i_peek_char (this, 1) isnot ' ') {
          this->print_fmt_bytes (this->err_fp, "string fmt error, awaiting a comma and a space after directive\n");
          i_err_ptr (this, I_NOTOK);
          goto theend;
        }

        i_get_char (this); i_get_char (this);

      } else
        i_unget_char (this);

      c = prev;
      prev = tmp;

      int len = 0;
      prev = c;
      while ((c = i_get_char (this))) {
        if (c is -1) {
          this->print_bytes (this->err_fp, "string fmt error, unended string\n");
          i_err_ptr (this, I_NOTOK);
          goto theend;
        }

        if (c is '}') break;

        sym[len++] = c;
        prev = c;
      }

      ifnot (len) {
        this->print_bytes (this->err_fp, "string fmt error, awaiting symbol\n");
        i_err_ptr (this, I_NOTOK);
        goto theend;
      }

      sym[len] = '\0';
      istring_t x = i_StringNew (sym);
      sym_t *symbol = i_lookup_symbol (this, x);

      if (NULL is symbol) {
        this->print_fmt_bytes (this->err_fp, "string fmt error, unknown symbol %s\n", sym);
        i_err_ptr (this, I_NOTOK);
        goto theend;
      }

      switch (directive) {
        case 's':
          String.append_with_fmt (str, "%s", symbol->value);
          break;

        case 'p':
          String.append_with_fmt (str, "%p", symbol->value);
          break;

        case 'o':
          String.append_with_fmt (str, "0%o", symbol->value);
          break;

        case 'x':
          String.append_with_fmt (str, "0x%x", symbol->value);
          break;

        case 'd':
        default:
          String.append_with_fmt (str, "%d", symbol->value);
      }

      directive = 'd';

      continue;
    }

    String.append_byte (str, c);
  }

  c = i_get_char (this);

  if (c isnot ')') {
    this->print_bytes (this->err_fp, "string fmt error, awaiting )\n");
    i_err_ptr (this, I_NOTOK);
    goto theend;
  }

  if (I_NOTOK is this->print_bytes (fp, str->bytes)) {
    this->print_bytes (this->err_fp, "error while printing string\n");
    fprintf (this->err_fp, "%s\n", str->bytes);
    goto theend;
  }

  i_next_token (this);

  err = I_OK;

theend:
  String.release (str);
  return err;
}

static int i_parse_exit (i_t *this) {
#ifdef DEBUG
  $CODE_PATH
#endif
  i_next_token (this);

  i_parse_expr (this, &this->funResult);
  this->exitValue = this->funResult;

  i_StringSetLen (&this->parsePtr, 0);
  this->didReturn = 1;

  return I_ERR_EXIT;
}

static int i_parse_return (i_t *this) {
#ifdef DEBUG
  $CODE_PATH
#endif
  int err;
  i_next_token (this);

  err = i_parse_expr (this, &this->funResult);

  // terminate scope
  i_StringSetLen (&this->parsePtr, 0);
  this->didReturn = 1;

  return err;
}

static int i_parse_while (i_t *this) {
  int err;
  istring_t savepc = this->parsePtr;
  int len = i_StringGetLen (this->parsePtr);
  this->curState |= AT_LOOP;

again:
  this->curState &= ~BREAK;
  this->curState &= ~CONTINUE;

  err = i_parse_if (this);

  if (this->curState & BREAK) {
    const char *savepcptr = i_StringGetPtr (savepc);
    const char *parsePtr = i_StringGetPtr (this->parsePtr);
    while (savepcptr isnot parsePtr) {savepcptr++; len--;}
    i_StringSetLen (&this->parsePtr, len);

    int c;
    int brackets = 2;

    do {
      c = i_get_char (this);
      if (c is '}')
        --brackets;
      else if (c is '{')
        ++brackets;
      ifnot (brackets) break;
    } while (c >= 0);
    if (brackets)
      return this->syntax_error (this, "a closed bracket is missing");

    i_next_token (this);
    err = I_ERR_OK_ELSE;
  }

  if (err is I_ERR_OK_ELSE) {
    this->curState &= ~AT_LOOP;
    return I_OK;
  } else if (err is I_OK or this->curState & CONTINUE) {
    i_StringSetPtr (&this->parsePtr, i_StringGetPtr (savepc));
    i_StringSetLen (&this->parsePtr, len);
    goto again;
  }

  this->curState &= ~AT_LOOP;
  return err;
}

static int i_define (i_t *this, const char *name, int typ, ival_t val) {
  i_define_symbol (this, this->function, i_StringNew (name), typ, val, 0);
  return I_OK;
}

static int i_eval_string (i_t *this, const char *buf) {
#ifdef DEBUG
  Cstring.cp ($PREV_FUNC, MAXLEN_SYMBOL_LEN + 1, " ", 1);
  $CODE_PATH
#endif
  this->script_buffer = buf;
  istring_t x = i_StringNew (buf);
  int retval = i_parse_string (this, x);
  i_release_malloced_strings (this, 0);

  if (retval is I_ERR_EXIT)
    return this->exitValue;

  return retval;
}

static int i_eval_file (i_t *this, const char *filename) {
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

  r = i_eval_string (this, script);

  if (r isnot I_OK) {
    char *err_msg[] = {"NO MEMORY", "SYNTAX ERROR", "UNKNOWN SYMBOL",
        "BAD ARGUMENTS", "TOO MANY ARGUMENTS"};
    this->print_fmt_bytes (this->err_fp, "%s\n", err_msg[-r - 2]);
  }

  return r;
}

static void i_release (i_t **thisp) {
  if (NULL is *thisp) return;
  i_t *this = *thisp;

  String.release (this->idir);
  String.release (this->message);
  Imap.release (this->refcount);
  i_release_malloced_strings (this, 1);
  fun_release (&this->function);

  free (this);
  *thisp = NULL;
}

static void i_release_instance (i_t **thisp) {
  i_release (thisp);
}

static i_t *i_new (i_T *__i__) {
  i_t *this = Alloc (sizeof (i_t));
  this->prop = __i__->prop;
  return this;
}

static char *i_name_gen (char *name, int *name_gen, char *prefix, size_t prelen) {
  size_t num = (*name_gen / 26) + prelen;
  size_t i = 0;
  for (; i < prelen; i++) name[i] = prefix[i];
  for (; i < num; i++) name[i] = 'a' + ((*name_gen)++ % 26);
  name[num] = '\0';
  return name;
}

static void i_remove_instance (i_T *this, i_t *instance) {
  i_t *it = $my(head);
  i_t *prev = NULL;

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
  i_release_instance (&it);
}

static i_t *i_append_instance (i_T *this, i_t *instance) {
  instance->next = NULL;
  $my(current_idx) = $my(num_instances);
  $my(num_instances)++;

  if (NULL is $my(head))
    $my(head) = instance;
  else {
    i_t *it = $my(head);
    while (it) {
      if (it->next is NULL) break;
      it = it->next;
    }

    it->next = instance;
  }

  return instance;
}

static i_t *i_set_current (i_T *this, int idx) {
  if (idx >= $my(num_instances)) return NULL;
  i_t *it = $my(head);
  int i = 0;
  while (i++ < idx) it = it->next;

  return it;
}

static void i_set_idir (i_t *this, char *fn) {
  if (NULL is fn) return;
  size_t len = bytelen (fn);
  String.replace_with_len (this->idir, fn, len);
}

static void i_set_define_funs_cb (i_t *this, IDefineFuns_cb cb) {
  this->define_funs_cb = cb;
}

static void i_set_user_data (i_t *this, void *user_data) {
  this->user_data = user_data;
}

static void *i_get_user_data (i_t *this) {
  return this->user_data;
}

static char *i_get_message (i_t *this) {
  return this->message->bytes;
}

static char *i_get_eval_str (i_t *this) {
  return (char *) i_StringGetPtr (this->parsePtr);
}

static i_t *i_get_current (i_T *this) {
  i_t *it = $my(head);
  int i = 0;
  while (i++ < $my(current_idx)) it = it->next;

  return it;
}

static int i_get_current_idx (i_T *this) {
  return $my(current_idx);
}

static int i_print_bytes (FILE *fp, const char *bytes) {
  string_t *parsed = IO.parse_escapes ((char *)bytes);
  if (NULL is parsed) return I_NOTOK;
  int nbytes = fprintf (fp, "%s", parsed->bytes);
  String.release (parsed);
  fflush (fp);
  return nbytes;
}

static int i_print_fmt_bytes (FILE *fp, const char *fmt, ...) {
  size_t len = VA_ARGS_FMT_SIZE (fmt);
  char bytes[len + 1];
  VA_ARGS_GET_FMT_STR(bytes, len, fmt);
  return i_print_bytes (fp, bytes);
}

static int i_print_byte (FILE *fp, int c) {
  return i_print_fmt_bytes (fp, "%c", c);
}

/*
ival_t i_print_str (i_t *this, char *str) {
  return i_print_bytes (this->out_fp, str);
}
*/

static ival_t I_print_bytes (i_t *this, char *bytes) {
  return this->print_bytes (this->out_fp, bytes);
}

static ival_t I_print_byte (i_t *this, char byte) {
  return this->print_byte (this->out_fp, byte);
}

static ival_t i_not (i_t *this, ival_t value) {
  (void) this;
  return !value;
}

static ival_t i_bool (i_t *this, ival_t value) {
  (void) this;
  return !!value;
}

static ival_t i_free (i_t *this, void *value) {
  (void) this;
  ifnot (NULL is value)
    free (value);
  return I_OK;
}

static void *i_alloc (i_t *this, ival_t size) {
  (void) this;
  return Alloc ((size_t) size);
}

static void *i_realloc (i_t *this, ival_t obj, ival_t size) {
  (void) this;
  return Realloc ((void *)obj, (size_t) size);
}

static ival_t i_cstring_bytelen (i_t *this, char *str) {
  (void) this;
  return bytelen (str);
}

static ival_t i_cstring_cp (i_t *this, char *dest, ival_t size, char *src, ival_t len) {
  (void) this;
  return Cstring.cp (dest, size, src, len);
}

static ival_t i_cstring_trim_end (i_t *this, char *str, ival_t ch) {
  (void) this;
  return (ival_t) Cstring.trim.end (str, ch);
}

static ival_t i_cstring_byte_cp (i_t *this, char *dest, const char *src, ival_t size) {
  (void) this;
  return Cstring.byte.cp (dest, src, size);
}

static ival_t i_cstring_byte_cp_all (i_t *this, char *dest, const char *src, ival_t size) {
  (void) this;
  return Cstring.byte.cp_all (dest, src, size);
}

static ival_t i_cstring_byte_mv (i_t *this, char *dest, ival_t len, ival_t to_idx, ival_t from_idx, ival_t nelem) {
  (void) this;
  return Cstring.byte.mv (dest, len, to_idx, from_idx, nelem);
}

static ival_t i_cstring_byte_in_str (i_t *this, char *str, ival_t ch) {
  (void) this;
  return (ival_t) Cstring.byte.in_str (str, ch);
}

static ival_t i_cstring_byte_in_str_r (i_t *this, char *str, ival_t ch) {
  (void) this;
  return (ival_t) Cstring.byte.in_str_r (str, ch);
}

static ival_t i_cstring_byte_null_in_str (i_t *this, char *str) {
  (void) this;
  return (ival_t) Cstring.byte.null_in_str (str);
}

static ival_t i_cstring_dup (i_t *this, char *str, ival_t size) {
  (void) this;
  return (ival_t) Cstring.dup (str, size);
}

static ival_t i_cstring_new (i_t *this, ival_t len) {
  (void) this;
  char *new = Alloc ((size_t) len + 1);
  return (ival_t) new;
}

static ival_t i_cstring_new_with (i_t *this, char *str) {
  (void) this;
  size_t len = bytelen (str);
  char *new = Alloc (len + 1);
  Cstring.cp (new, len + 1, str, len);
  return (ival_t) new;
}

static ival_t i_cstring_substr (i_t *this, char *dest, size_t len, char *src, size_t src_len, size_t idx) {
  (void) this;
  return (ival_t) Cstring.substr (dest, len, src, src_len, idx);
}

static ival_t i_cstring_bytes_in_str (i_t *this, char *str, char *substr) {
  (void) this;
  return (ival_t) Cstring.bytes_in_str (str, substr);
}

static ival_t i_cstring_eq (i_t *this, char *sa, char *sb) {
  (void) this;
  return Cstring.eq (sa, sb);
}

static ival_t i_cstring_eq_n (i_t *this, char *sa, char *sb, ival_t n) {
  (void) this;
  return Cstring.eq_n (sa, sb, n);
}

static ival_t i_cstring_cmp_n (i_t *this, char *sa, char *sb, ival_t n) {
  (void) this;
  return Cstring.cmp_n (sa, sb, n);
}

static ival_t i_cstring_cat (i_t *this, char *dest, ival_t size, char *src) {
  (void) this;
  return Cstring.cat (dest, size, src);
}

static ival_t i_ne (ival_t x, ival_t y) { return x != y; }
static ival_t i_lt (ival_t x, ival_t y) { return x < y; }
static ival_t i_le (ival_t x, ival_t y) { return x <= y; }
static ival_t i_gt (ival_t x, ival_t y) { return x > y; }
static ival_t i_ge (ival_t x, ival_t y) { return x >= y; }
static ival_t i_mod  (ival_t x, ival_t y) { return x % y; }
static ival_t i_sum  (ival_t x, ival_t y) { return x + y; }
static ival_t i_shl  (ival_t x, ival_t y) { return x << y; }
static ival_t i_shr  (ival_t x, ival_t y) { return x >> y; }
static ival_t i_prod (ival_t x, ival_t y) { return x * y; }
static ival_t i_quot (ival_t x, ival_t y) { return x / y; }
static ival_t i_diff (ival_t x, ival_t y) { return x - y; }
static ival_t i_bitor (ival_t x, ival_t y) { return x | y; }
static ival_t i_bitand (ival_t x, ival_t y) { return x & y; }
static ival_t i_bitxor (ival_t x, ival_t y) { return x ^ y; }
static ival_t i_equals (ival_t x, ival_t y) { return x == y; }
static ival_t i_logical_and (ival_t x, ival_t y) { return x && y; }
static ival_t i_logical_or (ival_t x, ival_t y) { return x || y; }

static struct def {
  const char *name;
  int toktype;
  ival_t val;
} idefs[] = {
  { "var",     I_TOK_VARDEF,   (ival_t) 0 },
  { "const",   I_TOK_CONSTDEF, (ival_t) 0 },
  { "else",    I_TOK_ELSE,     (ival_t) 0 },
  { "break",   I_TOK_BREAK,    (ival_t) 0 },
  { "continue",I_TOK_CONTINUE, (ival_t) 0 },
  { "if",      I_TOK_IF,       (ival_t) i_parse_if },
  { "ifnot",   I_TOK_IFNOT,    (ival_t) i_parse_ifnot },
  { "while",   I_TOK_WHILE,    (ival_t) i_parse_while },
  { "print",   I_TOK_PRINT,    (ival_t) i_parse_print },
  { "func",    I_TOK_FUNCDEF,  (ival_t) i_parse_func_def },
  { "return",  I_TOK_RETURN,   (ival_t) i_parse_return },
  { "exit",    I_TOK_EXIT,     (ival_t) i_parse_exit },
  { "true",    INT, (ival_t) 1},
  { "false",   INT, (ival_t) 0},
  { "OK",      INT, (ival_t) 0},
  { "NOTOK",   INT, (ival_t) -1},
  { "array",   I_TOK_ARYDEF, (ival_t) i_parse_array_def },
  { "*",       BINOP(1), (ival_t) i_prod },
  { "/",       BINOP(1), (ival_t) i_quot },
  { "%",       BINOP(1), (ival_t) i_mod },
  { "+",       BINOP(2), (ival_t) i_sum },
  { "-",       BINOP(2), (ival_t) i_diff },
  { "&",       BINOP(3), (ival_t) i_bitand },
  { "|",       BINOP(3), (ival_t) i_bitor },
  { "^",       BINOP(3), (ival_t) i_bitxor },
  { ">>",      BINOP(3), (ival_t) i_shr },
  { "<<",      BINOP(3), (ival_t) i_shl },
  { "==",      BINOP(4), (ival_t) i_equals },
  { "is",      BINOP(4), (ival_t) i_equals },
  { "!=",      BINOP(4), (ival_t) i_ne },
  { "isnot",   BINOP(4), (ival_t) i_ne },
  { "<",       BINOP(4), (ival_t) i_lt },
  { "lt",      BINOP(4), (ival_t) i_lt },
  { "<=",      BINOP(4), (ival_t) i_le },
  { "le",      BINOP(4), (ival_t) i_le },
  { ">",       BINOP(4), (ival_t) i_gt },
  { "gt",      BINOP(4), (ival_t) i_gt },
  { ">=",      BINOP(4), (ival_t) i_ge },
  { "ge",      BINOP(4), (ival_t) i_ge },
  { "&&",      BINOP(5), (ival_t) i_logical_and },
  { "and",     BINOP(5), (ival_t) i_logical_and },
  { "||",      BINOP(5), (ival_t) i_logical_or },
  { "or",      BINOP(5), (ival_t) i_logical_or },
  { NULL, 0, 0 }
};

struct i_def_fun_t {
  const char *name;
  ival_t val;
  int nargs;
} i_def_funs[] = {
  { "not",             (ival_t) i_not, 1},
  { "bool",            (ival_t) i_bool, 1},
  { "free",            (ival_t) i_free, 1},
  { "alloc",           (ival_t) i_alloc, 1},
  { "realloc",         (ival_t) i_realloc, 2},
  { "cstring_cp",      (ival_t) i_cstring_cp, 4},
  { "cstring_eq",      (ival_t) i_cstring_eq, 2},
  { "cstring_new",     (ival_t) i_cstring_new, 1},
  { "cstring_dup",     (ival_t) i_cstring_dup, 2},
  { "cstring_cat",     (ival_t) i_cstring_cat, 3},
  { "cstring_eq_n",    (ival_t) i_cstring_eq_n, 3},
  { "cstring_cmp_n",   (ival_t) i_cstring_cmp_n, 3},
  { "cstring_substr",  (ival_t) i_cstring_substr, 5},
  { "cstring_bytelen", (ival_t) i_cstring_bytelen, 1},
  { "cstring_byte_mv", (ival_t) i_cstring_byte_mv, 5},
  { "cstring_byte_cp", (ival_t) i_cstring_byte_cp, 3},
  { "cstring_new_with",(ival_t) i_cstring_new_with, 1},
  { "cstring_trim_end",(ival_t) i_cstring_trim_end, 2},
  { "cstring_byte_cp_all", (ival_t) i_cstring_byte_cp_all, 3},
  { "cstring_byte_in_str", (ival_t) i_cstring_byte_in_str, 2},
  { "cstring_bytes_in_str", (ival_t) i_cstring_bytes_in_str, 2},
  { "cstring_byte_in_str_r", (ival_t) i_cstring_byte_in_str_r, 2},
  { "cstring_byte_null_in_str", (ival_t) i_cstring_byte_null_in_str, 1},
  { NULL, 0, 0}
};

static int i_define_funs_default_cb (i_t *this) {
  (void) this;
  return I_OK;
}

static i_opts i_default_options (i_t *this, i_opts opts) {
  (void) this;
  if (NULL is opts.print_bytes)
    opts.print_bytes = i_print_bytes;

  if (NULL is opts.print_byte)
    opts.print_byte = i_print_byte;

  if (NULL is opts.print_fmt_bytes)
    opts.print_fmt_bytes = i_print_fmt_bytes;

  if (NULL is opts.syntax_error)
    opts.syntax_error = i_syntax_error;

  if (NULL is opts.err_fp)
    opts.err_fp = stderr;

  if (NULL is opts.define_funs_cb)
    opts.define_funs_cb = i_define_funs_default_cb;

  return opts;
}

static int i_init (i_T *interp, i_t *this, i_opts opts) {
  int i;
  int err = 0;

  if (NULL is opts.name)
    i_name_gen (this->name, &$my(name_gen), "i:", 2);
  else
    Cstring.cp (this->name, 32, opts.name, 31);

  opts = i_default_options (this, opts);

  this->print_byte = opts.print_byte;
  this->print_fmt_bytes = opts.print_fmt_bytes;
  this->print_bytes = opts.print_bytes;
  this->syntax_error = opts.syntax_error;
  this->err_fp = opts.err_fp;
  this->out_fp = opts.out_fp;
  this->user_data = opts.user_data;
  this->define_funs_cb = opts.define_funs_cb;

  this->didReturn = 0;
  this->exitValue = I_OK;
  this->curState = 0;

  if (NULL is opts.idir)
    this->idir = String.new (32);
  else
    this->idir = String.new_with (opts.idir);

  this->message = String.new (32);

  Fun_new (this,
      funNew (.name = NS_GLOBAL, .namelen = NS_GLOBAL_LEN, .num_symbols = 256));

  for (i = 0; idefs[i].name; i++) {
    err = i_define (this, idefs[i].name, idefs[i].toktype, idefs[i].val);

    if (err isnot I_OK) {
      i_release (&this);
      return err;
    }
  }

  for (i = 0; i_def_funs[i].name; i++) {
    err = i_define (this, i_def_funs[i].name, CFUNC (i_def_funs[i].nargs), i_def_funs[i].val);

    if (err isnot I_OK) {
      i_release (&this);
      return err;
    }
  }

  if (I_OK isnot opts.define_funs_cb (this)) {
    i_release (&this);
    return err;
  }

  this->refcount = Imap.new (256);

  i_append_instance (interp, this);

  return I_OK;
}

static i_t *i_init_instance (i_T *__i__, i_opts opts) {
  i_t *this = i_new (__i__);

  i_init (__i__, this, opts);

  return this;
}

static int i_load_file (i_T *__i__, i_t *this, char *fn) {
  if (this is NULL)
    this = i_init_instance (__i__, IOpts());

  ifnot (Path.is_absolute (fn)) {
    if (File.exists (fn) and File.is_reg (fn))
      return i_eval_file (this, fn);

    size_t fnlen = bytelen (fn);
    char fname[fnlen+3];
    Cstring.cp (fname, fnlen + 1, fn, fnlen);

    char *extname = Path.extname (fname);
    size_t extlen = bytelen (extname);

    if (0 is extlen or 0 is Cstring.eq (".i", extname)) {
      fname[fnlen] = '.'; fname[fnlen+1] = 'i'; fname[fnlen+2] = '\0';
      if (File.exists (fname))
        return i_eval_file (this, fname);

      fname[fnlen] = '\0';
    }

    ifnot (this->idir->num_bytes) {
      i_set_message_fmt (this, 0, "%s: couldn't locate script", fn);
      return NOTOK;
    }

    string_t *ddir = this->idir;
    size_t len = ddir->num_bytes + bytelen (fname) + 2 + 7;
    char tmp[len + 3];
    Cstring.cp_fmt (tmp, len + 1, "%s/scripts/%s", ddir->bytes, fname);

    if (0 is File.exists (tmp) or 0 is File.is_reg (tmp)) {
      tmp[len] = '.'; tmp[len+1] = 'i'; tmp[len+2] = '\0';
    }

    ifnot (File.exists (tmp)) {
      i_set_message_fmt (this, 0, "%s: couldn't locate script", fn);
      return NOTOK;
    }

    return i_eval_file (this, tmp);
  }

  ifnot (File.exists (fn)) {
    i_set_message_fmt (this, 0, "%s: couldn't locate script", fn);
    return NOTOK;
  }

  return i_eval_file (this, fn);
}

public i_T *__init_i__ (void) {
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

  i_T *this =  Alloc (sizeof (i_T));
  $myprop = Alloc (sizeof (i_prop));

  *this = (i_T) {
    .self = (i_self) {
      .new = i_new,
      .init = i_init,
      .def =  i_define,
      .release = i_release,
      .eval_file = i_eval_file,
      .load_file = i_load_file,
      .print_byte = I_print_byte,
      .print_bytes = I_print_bytes,
      .eval_string =  i_eval_string,
      .init_instance = i_init_instance,
      .remove_instance = i_remove_instance,
      .append_instance = i_append_instance,
      .get = (i_get_self) {
        .message = i_get_message,
        .current = i_get_current,
        .eval_str = i_get_eval_str,
        .user_data = i_get_user_data,
        .current_idx = i_get_current_idx
      },
      .set = (i_set_self) {
        .idir = i_set_idir,
        .current = i_set_current,
        .user_data = i_set_user_data,
        .define_funs_cb = i_set_define_funs_cb
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

public void __deinit_i__ (i_T **thisp) {
  if (NULL is *thisp) return;
  i_T *this = *thisp;

  i_t *it = $my(head);
  while (it) {
    i_t *tmp = it->next;
    i_release_instance (&it);
    it = tmp;
  }

  free ($myprop);
  free (this);
  *thisp = NULL;
}
