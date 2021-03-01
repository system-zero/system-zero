/* Derived from Tinyscript project at:
 * https://github.com/totalspectrum/
 * 
 * - added dynamic allocation
 * - ability to create more than one instance
 * - an instance can be passed as a first argument
 * - \ as the last character in the line is a continuation character
 * - syntax_error() got a char * argument, that describes the error
 * - added ignore_next_char() to ignore next char in parseptr
 * - print* function references got a FILE * argument
 * - remove abort() and disable exit()
 * - added is, isnot, true, false, ifnot, OK and NOTOK keywords
 * - added println (that emit a newline character, while print does not)
 * - add the ability to pass literal strings when calling C defined functions
 *   (note that they should be free'd automatically after ending the script)
 * - print functions can print multibyte characters
 * - quite of many changes that integrates Tinyscript to this environment
 */

/* Tinyscript interpreter
 *
 * Copyright 2016 Total Spectrum Software Inc.
 *
 * +--------------------------------------------------------------------
 * ¦  TERMS OF USE: MIT License
 * +--------------------------------------------------------------------
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * +--------------------------------------------------------------------
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
#define REQUIRE_ERROR_TYPE   DECLARE
#define REQUIRE_LIST_MACROS
#define REQUIRE_I_TYPE       DONOT_DECLARE

#include <z/cenv.h>

#define $myprop    this->prop
#define $my(__v__) $myprop->__v__

#define INT      0x0  // integer
//#define STRING   0x1  // string
//#define OPERATOR 0x2  // operator; precedence in high 8 bits
//#define ARG      0x3  // argument; value is offset on stack
#define BUILTIN  'B'  // builtin: number of operands in high 8 bits
#define USRFUNC  'f'  // user defined a procedure; number of operands in high 8 bits
#define TOK_BINOP 'o'

#define OUT_OF_FUNCTION_ARGUMENT_SCOPE 1 << 0
//#define FUNCTION_SCOPE                 1 << 1
#define FUNCTION_ARGUMENT_SCOPE        1 << 2
#define FUNC_CALL_BUILTIN              1 << 3
#define FUNC_CALL_USRFUNC              1 << 4

#define BINOP(x) (((x) << 8) + TOK_BINOP)
#define CFUNC(x) (((x) << 8) + BUILTIN)

#define I_TOK_SYMBOL     'A'
#define I_TOK_NUMBER     'N'
#define I_TOK_HEX_NUMBER 'X'
#define I_TOK_STRING     'S'
#define I_TOK_IF         'i'
#define I_TOK_IFNOT      'I'
#define I_TOK_ELSE       'e'
#define I_TOK_WHILE      'w'
#define I_TOK_PRINT      'p'
#define I_TOK_PRINTLN    'P'
#define I_TOK_VAR        'v'
#define I_TOK_VARDEF     'V'
#define I_TOK_BUILTIN    'B'
#define I_TOK_BINOP      'o'
#define I_TOK_FUNCDEF    'F'
#define I_TOK_USRFUNC    'f'
#define I_TOK_SYNTAX_ERR 'Z'
#define I_TOK_RETURN     'r'
#define I_TOK_CHAR       'C'

#define MAX_BUILTIN_PARAMS 9

typedef struct istring_t {
  unsigned len_;
  const char *ptr_;
} istring_t;

typedef struct symbol {
  istring_t name;
  int type;      // symbol type
  ival_t value;  // symbol value, or string ptr
} Sym;

typedef struct ufunc {
  istring_t body;
  int nargs;
  istring_t argName[MAX_BUILTIN_PARAMS];
} UserFunc;

typedef struct i_prop {
  int name_gen;
  size_t default_size;

  i_t *head;
  int num_instances;
  int current_idx;
} i_prop;

typedef struct malloced_string malloced_string;

struct malloced_string {
  char *data;
  malloced_string *next;
};

typedef struct i_t {
  char name[32];
  string_t
    *idir,
    *message;

  i_prop *prop;

  size_t
    mem_size,
    max_script_size;

  const char *script_buffer;

  char *arena;
  int
    didReturn,
    linenum,
    state;

  FILE
    *err_fp,
    *out_fp;

  Sym *symptr;
  ival_t *valptr;

  istring_t parseptr;  // acts as instruction pointer

  char ns[MAXLEN_NAME];
  ival_t fArgs[MAX_BUILTIN_PARAMS];
  ival_t fResult;

  // variables for parsing
  int curToken;  // what kind of token is current
  int tokenArgs; // number of arguments for this token
  istring_t token;  // the actual string representing the token
  ival_t tokenVal;  // for symbolic tokens, the symbol's value
  Sym *tokenSym;

  IPrintByte_cb print_byte;
  IPrintBytes_cb print_bytes;
  IPrintFmtBytes_cb print_fmt_bytes;
  ISyntaxError_cb syntax_error;

  IDefineFuns_cb define_funs_cb;

  void *user_data;

  malloced_string *head;

  i_t *next;
} i_t;


#define ERROR_COLOR "\033[31m"
#define TERM_RESET  "\033[m"
#define MAX_EXPR_LEVEL 5

static int i_parse_stmt (i_t *, int);
static int i_parse_expr (i_t *, ival_t *);

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
  return is_alpha (c) or is_idpunct (c);
}

static inline int is_notquote (int chr) {
  uchar c = (unsigned char) chr;
  return NULL is Cstring.byte.in_str ("\"\n", c);
}

static inline int is_operator (int c) {
  return NULL isnot Cstring.byte.in_str ("+-/*%=<>&|^", c);
}

static void i_release_malloced_strings (i_t *this) {
  malloced_string *item = this->head;
  while (item isnot NULL) {
    malloced_string *tmp = item->next;
    free (item->data);
    free (item);
    item = tmp;
  }
  this->head = NULL;
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

static int i_StringEq (istring_t ai, istring_t bi) {
  const char *a, *b;
  size_t i, len;

  len = i_StringGetLen (ai);
  if (len isnot i_StringGetLen (bi))
    return 0;

  a = i_StringGetPtr (ai);
  b = i_StringGetPtr (bi);

  for (i = 0; i < len; i++) {
    if (*a isnot *b) return 0;
    a++; b++;
  }

  return 1;
}

static void i_print_string (i_t *this, FILE *fp, istring_t s) {
  unsigned len = i_StringGetLen (s);
  const char *ptr = (const char *) i_StringGetPtr (s);
  while (len > 0) {
    this->print_byte (fp, *ptr);
    ptr++;
    --len;
  }
}

static void i_print_number (i_t *this, ival_t v) {
  unsigned long x;
  unsigned base = 10;
  int prec = 1;
  int digits = 0;
  int c;
  char buf[32];

  if (v < 0) {
    this->print_byte (this->out_fp, '-');
    x = -v;
  } else
    x = v;

  while (x > 0 or digits < prec) {
    c = x % base;
    x = x / base;
    if (c < 10) c += '0';
    else c = (c - 10) + 'a';
    buf[digits++] = c;
  }

  while (digits > 0) {
    --digits;
    this->print_byte (this->out_fp, buf[digits]);
  }
}

static char *i_stack_alloc (i_t *this, int len) {
  int mask = sizeof (ival_t) -1;
  ival_t base;

  len = (len + mask) & ~mask;
  base = ((ival_t) this->valptr) - len;
  if (base < (ival_t) this->symptr)
    return NULL;

  this->valptr = (ival_t *) base;
  return (char *) base;
}

static istring_t i_dup_string (i_t *this, istring_t orig) {
  istring_t x;
  char *ptr;
  unsigned len = i_StringGetLen (orig);
  ptr = i_stack_alloc (this, len);
  if (ptr)
    Cstring.byte.cp (ptr, i_StringGetPtr (orig), len);

  i_StringSetLen (&x, len);
  i_StringSetPtr (&x, ptr);
  return x;
}

static int i_err_ptr (i_t *this, int err) {
  const char *keep = i_StringGetPtr (this->parseptr);
  char *sp = (char *) keep;
  while (sp > this->script_buffer and 0 is Cstring.byte.in_str (";\n", *(sp - 1)))
    sp--;

  i_StringSetPtr (&this->parseptr, sp);
  i_print_string (this, this->err_fp, this->parseptr);
  i_StringSetPtr (&this->parseptr, keep);

  this->print_bytes (this->err_fp, TERM_RESET "\n");
  return err;
}

static int i_syntax_error (i_t *this, const char *msg) {
  this->print_fmt_bytes (this->err_fp, "\n" ERROR_COLOR "SYNTAX ERROR: %s\n", msg);
  return i_err_ptr (this, I_ERR_SYNTAX);
}

static int i_arg_mismatch (i_t *this) {
  this->print_fmt_bytes (this->err_fp, "\n" ERROR_COLOR "argument mismatch:");
  return i_err_ptr (this, I_ERR_BADARGS);
}

static int i_too_many_args (i_t *this) {
  this->print_fmt_bytes (this->err_fp, "\n" ERROR_COLOR "too many arguments:");
  return i_err_ptr (this, I_ERR_TOOMANYARGS);
}

static int i_unknown_symbol (i_t *this) {
  this->print_fmt_bytes (this->err_fp, "\n" ERROR_COLOR "unknown symbol:");
  return i_err_ptr (this, I_ERR_UNKNOWN_SYM);
}

static int i_out_of_mem (i_t *this) {
  this->print_fmt_bytes (this->err_fp, ERROR_COLOR "out of memory" TERM_RESET "\n");
  return I_ERR_NOMEM;
}

static void i_reset_token (i_t *this) {
  i_StringSetLen (&this->token, 0);
  i_StringSetPtr (&this->token, i_StringGetPtr (this->parseptr));
}

static void i_ignore_last_token (i_t *this) {
  i_StringSetLen (&this->token, i_StringGetLen (this->token) - 1);
}

static void i_ignore_first_token (i_t *this) {
  i_StringSetPtr (&this->token, i_StringGetPtr (this->token) + 1);
  i_StringSetLen (&this->token, i_StringGetLen (this->token) - 1);
}

static void i_ignore_next_char (i_t *this) {
  i_StringSetPtr (&this->parseptr, i_StringGetPtr (this->parseptr) + 1);
  i_StringSetLen (&this->parseptr, i_StringGetLen (this->parseptr) - 1);
}

static int i_peek_char (i_t *this, unsigned int n) {
  if (i_StringGetLen (this->parseptr) <= n) return -1;
  return *(i_StringGetPtr (this->parseptr) + n);
}

static int i_get_char (i_t *this) {
  unsigned int len = i_StringGetLen (this->parseptr);

  ifnot (len) return -1;

  const char *ptr = i_StringGetPtr (this->parseptr);
  int c = *ptr++;

  --len;

  i_StringSetPtr (&this->parseptr, ptr);
  i_StringSetLen (&this->parseptr, len);
  i_StringSetLen (&this->token, i_StringGetLen (this->token) + 1);
  return c;
}

static void i_unget_char (i_t *this) {
  i_StringSetLen (&this->parseptr, i_StringGetLen (this->parseptr) + 1);
  i_StringSetPtr (&this->parseptr, i_StringGetPtr (this->parseptr) - 1);
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

static Sym *i_lookup_sym (i_t *this, istring_t name) {
  Sym *s = this->symptr;

  while ((ival_t) s > (ival_t) this->arena) {
    --s;
    if (i_StringEq (s->name, name))
      return s;
  }

  return NULL;
}

static int i_do_next_token (i_t *this, int israw) {
  int c;
  int r = -1;

  Sym *sym = NULL;

  this->tokenSym = NULL;
  i_reset_token (this);

  c = i_ignore_ws (this);

  if (c is '\\' and i_peek_char (this, 0) is '\n') {
    this->linenum++;
    i_ignore_next_char (this);
    i_reset_token (this);
    c = i_ignore_ws (this);
  }

  if (c is '#') {
    do
      c = i_get_char (this);
    while (c >= 0 and c isnot '\n');
    this->linenum++;

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
      this->tokenSym = sym = i_lookup_sym (this, this->token);
      if (sym) {
        r = sym->type & 0xff;
        this->tokenArgs = (sym->type >> 8) & 0xff;
        if (r < '@')
          r = I_TOK_VAR;
        this->tokenVal = sym->value;
      }
    }

  } else if (is_operator (c)) {
    i_get_span (this, is_operator);
    this->tokenSym = sym = i_lookup_sym (this, this->token);
    if (sym) {
      r = sym->type;
      this->tokenVal = sym->value;
    } else
      r = I_TOK_SYNTAX_ERR;

  } else if (c == '{') {
    int bracket = 1;
    i_reset_token (this);
    while (bracket > 0) {
      c = i_get_char (this);
      if (c < 0) return I_TOK_SYNTAX_ERR;

      if (c is '}')
        --bracket;
      else if (c == '{')
        ++bracket;
    }

    i_ignore_last_token (this);
    r = I_TOK_STRING;

  } else if (c is '"') {
    if (0 is (this->state & FUNCTION_ARGUMENT_SCOPE) or
        (this->state & FUNC_CALL_USRFUNC)) {
      this->state &= ~(FUNC_CALL_USRFUNC);
      i_reset_token (this);
      i_get_span (this, is_notquote);
      c = i_get_char (this);
      if (c < 0) return I_TOK_SYNTAX_ERR;
      i_ignore_last_token (this);
    } else {
      this->state &= ~(FUNC_CALL_BUILTIN);
      size_t len = 0;
      int pc = 0;
      int cc = 0;

      while (pc = cc, (cc = i_peek_char (this, len)) isnot -1) {
        if ('"' is cc and pc isnot '\\') break;
        len++;
      }

      if (cc is -1) return I_TOK_SYNTAX_ERR;

      malloced_string *mbuf = Alloc (sizeof (malloced_string));
      mbuf->data = Alloc (len + 1);
      for (size_t i = 0; i < len; i++) {
        c = i_get_char (this);
        mbuf->data[i] = c;
      }
      mbuf->data[len] = '\0';

      ListStackPush (this, mbuf);
      c = i_get_char (this);
      this->tokenVal = (ival_t) mbuf->data;
      i_reset_token (this);
    }

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

static int i_push (i_t *this, ival_t x) {
  --this->valptr;

  if ((ival_t) this->valptr < (ival_t) this->symptr)
    return i_out_of_mem (this);

  *this->valptr = x;
  return I_OK;
}

static ival_t i_pop (i_t *this) {
  ival_t r = 0;
  if ((ival_t) this->valptr < (ival_t) (this->arena + this->mem_size))
    r = *(this)->valptr++;

  return r;
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

static Sym *i_define_sym (i_t *this, istring_t name, int typ, ival_t value) {
  Sym *s = this->symptr;
  if (i_StringGetPtr (name) is NULL) return NULL;

  this->symptr++;
  if ((ival_t) this->symptr >= (ival_t) this->valptr)
    return NULL;

  s->name = name;
  s->value = value;
  s->type = typ;
  return s;
}

static Sym *i_define_var (i_t *this, istring_t name) {
  return i_define_sym (this, name, INT, 0);
}

static istring_t i_istring_new (const char *str) {
  istring_t x;
  i_StringSetLen (&x, bytelen (str));
  i_StringSetPtr (&x, str);
  return x;
}

static int i_parse_expr_list (i_t *this) {
  int err, c;
  int count = 0;
  ival_t v;

  do {
    err = i_parse_expr (this, &v);
    if (err isnot I_OK) return err;

      err = i_push (this, v);

    if (err isnot I_OK) return err;

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

static int i_parse_string (i_t *this, istring_t str, int saveStrings, int topLevel) {
  int c,  r;
  istring_t savepc = this->parseptr;
  Sym *savesymptr = this->symptr;

  this->parseptr = str;

  for (;;) {
    c = i_next_token (this);

    while (c is '\n' or c is ';') {
      if (c is '\n') this->linenum++;
      c = i_next_token (this);
     }

    if (c < 0) break;

    r = i_parse_stmt (this, saveStrings);
    if (r isnot I_OK) return r;

    c = this->curToken;
    if (c is '\n' or c is ';' or c < 0) {
      if (c is '\n') this->linenum++;
      continue;
    }
    else
      return this->syntax_error (this, "evaluated string failed, unknown token");
  }

  this->parseptr = savepc;

  ifnot (topLevel)
    this->symptr = savesymptr;

  return I_OK;
}

static int i_parse_func_call (i_t *this, Cfunc op, ival_t *vp, UserFunc *uf) {
  int paramCount = 0;
  int expectargs;
  int c;

  if (uf)
    expectargs = uf->nargs;
  else
    expectargs = this->tokenArgs;

  c = i_next_token (this);
  if (c isnot '(') return this->syntax_error (this, "expected open parentheses");

  this->state |= FUNCTION_ARGUMENT_SCOPE;

  c = i_next_token (this);
  if (c isnot ')') {
    paramCount = i_parse_expr_list (this);
    c = this->curToken;
    if (paramCount < 0) {
      this->state &= ~(OUT_OF_FUNCTION_ARGUMENT_SCOPE);
      return paramCount;
    }
  }

  this->state &= ~(OUT_OF_FUNCTION_ARGUMENT_SCOPE);

  if (c isnot ')')
    return this->syntax_error (this, "expected closed parentheses");

  if (expectargs isnot paramCount)
    return i_arg_mismatch (this);

  // we now have "paramCount" items pushed on to the stack
  // pop em off
  while (paramCount > 0) {
    --paramCount;
    this->fArgs[paramCount] = i_pop (this);
  }

  if (uf) {
    // need to invoke the script here
    // set up an environment for the script
    int i;
    int err;
    Sym* savesymptr = this->symptr;

    for (i = 0; i < expectargs; i++)
      i_define_sym (this, uf->argName[i], INT, this->fArgs[i]);

    this->didReturn = 0;
    err = i_parse_string (this, uf->body, 0, 0);
    this->didReturn = 0;

    *vp = this->fResult;
    this->symptr = savesymptr;
    this->state &= ~(FUNC_CALL_USRFUNC);
    return err;
  } else {
    *vp = op (this, this->fArgs[0], this->fArgs[1], this->fArgs[2],
                    this->fArgs[3], this->fArgs[4], this->fArgs[5],
                    this->fArgs[6], this->fArgs[7], this->fArgs[8]);

    this->state &= ~(FUNC_CALL_BUILTIN);
  }

  i_next_token (this);
  return I_OK;
}

// parse a primary value; for now, just a number or variable
// returns 0 if valid, non-zero if syntax error
// puts result into *vp
static int i_parse_primary (i_t *this, ival_t *vp) {
  int c, err;

  c = this->curToken;
  if (c is '(') {
    this->state |= FUNCTION_ARGUMENT_SCOPE;
    i_next_token (this);
    err = i_parse_expr (this, vp);

    if (err is I_OK) {
      c = this->curToken;

      if (c is ')') {
        i_next_token (this);
        this->state &= ~(OUT_OF_FUNCTION_ARGUMENT_SCOPE);
        return I_OK;
      }
    }

    return err;

  } else if (c is I_TOK_NUMBER) {
    *vp = i_string_to_num (this->token);
    i_next_token (this);
    return I_OK;

  } else if (c is I_TOK_HEX_NUMBER) {
    *vp = i_HexStringToNum (this->token);
    i_next_token (this);
    return I_OK;

  } else if (c is I_TOK_CHAR) {
      err = i_parse_char (this, vp, this->token);
      i_next_token (this);
      return err;

  } else if (c is I_TOK_VAR) {
    *vp = this->tokenVal;
    i_next_token (this);
    return I_OK;

  } else if (c is I_TOK_BUILTIN) {
    Cfunc op = (Cfunc) this->tokenVal;
    this->state |= FUNC_CALL_BUILTIN;
    return i_parse_func_call (this, op, vp, NULL);

  } else if (c is I_TOK_USRFUNC) {
    Sym *sym = this->tokenSym;
    ifnot (sym) return this->syntax_error (this, "user defined function, not declared");
    this->state |= FUNC_CALL_USRFUNC;
    err = i_parse_func_call (this, NULL, vp, (UserFunc *)sym->value);
    i_next_token (this);
    return err;

  } else if ((c & 0xff) is I_TOK_BINOP) {
    // binary operator
    Opfunc op = (Opfunc) this->tokenVal;
    ival_t v;
    i_next_token (this);
    err = i_parse_expr (this, &v);
    if (err is I_OK)
      *vp = op (0, v);

    return err;

  } else if (c is I_TOK_STRING) {
    *vp = this->tokenVal;
    i_next_token (this);
    return I_OK;

  } else
    return this->syntax_error (this, "syntax error");
}

/* parse one statement
 * 1 is true if we need to save strings we encounter (we've been passed
 * a temporary string)
 */

static int i_parse_stmt (i_t *this, int saveStrings) {
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

  if (c is I_TOK_VARDEF) {
    // a definition var a=x
    c = i_next_raw_token (this); // we want to get VAR_SYMBOL directly
    if (c isnot I_TOK_SYMBOL)
      return this->syntax_error (this, "expected symbol");

    if (saveStrings)
      name = i_dup_string (this, this->token);
    else
      name = this->token;

    this->tokenSym = i_define_var (this, name);

    ifnot (this->tokenSym)
      return i_out_of_mem (this);

    c = I_TOK_VAR;
    /* fall through */
  }

  if (c is I_TOK_VAR) {
    // is this a=expr?
    Sym *s;
    name = this->token;
    s = this->tokenSym;
    c = i_next_token (this);
    // we expect the "=" operator, so verify that it is "="
    if (i_StringGetPtr (this->token)[0] isnot '=' or
        i_StringGetLen(this->token) isnot 1)
      return this->syntax_error (this, "expected =");

    ifnot (s) {
      i_print_string (this, this->err_fp, name);
      return i_unknown_symbol (this);
    }

    i_next_token (this);
    err = i_parse_expr (this, &val);
    if (err isnot I_OK) return err;

    s->value = val;
  } else if (c is I_TOK_BUILTIN or c is USRFUNC) {
    err = i_parse_primary (this, &val);
    return err;

  } else if (this->tokenSym and this->tokenVal) {
    int (*func) (i_t *, int) = (void *) this->tokenVal;
    err = (*func) (this, saveStrings);

  } else return this->syntax_error (this, "unknown token");

  if (err is I_ERR_OK_ELSE)
    err = I_OK;

  return err;
}

// parse a level n expression
// level 0 is the lowest level (highest precedence)
// result goes in *vp
static int i_parse_expr_level (i_t *this, int max_level, ival_t *vp) {
  int err = I_OK;
  int c;
  ival_t lhs;
  ival_t rhs;

  lhs = *vp;
  c = this->curToken;

  while ((c & 0xff) is I_TOK_BINOP) {
    int level = (c >> 8) & 0xff;
    if (level > max_level) break;

    Opfunc op = (Opfunc) this->tokenVal;
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
  int err = i_parse_primary (this, vp);
  if (err is I_OK)
    err = i_parse_expr_level (this, MAX_EXPR_LEVEL, vp);

  return err;
}

static int i_parse_if_rout (i_t *this, ival_t *cond, int *haveelse, istring_t *ifpart, istring_t *elsepart) {
  int c;
  int err;

  *haveelse = 0;

  c = i_next_token (this);
  err = i_parse_expr (this, cond);
  if (err isnot I_OK) return err;

  c = this->curToken;
  if (c isnot I_TOK_STRING) return this->syntax_error (this, "parsing if, not a string");

  *ifpart = this->token;
  c = i_next_token (this);
  if (c is I_TOK_ELSE) {
    c = i_next_token (this);
    if (c isnot I_TOK_STRING) return this->syntax_error (this, "parsing else, not a string");

    *elsepart = this->token;
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
    err = i_parse_string (this, ifpart, 0, 0);
  else if (haveelse)
    err = i_parse_string(this, elsepart, 0, 0);

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
    err = i_parse_string (this, ifpart, 0, 0);
  else if (haveelse)
    err = i_parse_string(this, elsepart, 0, 0);

  if (err is I_OK and 0 isnot cond) err = I_ERR_OK_ELSE;
  return err;
}

static int i_parse_var_list (i_t *this, UserFunc *uf, int saveStrings) {
  int c;
  int nargs = 0;
  c = i_next_raw_token (this);

  for (;;) {
    if (c is I_TOK_SYMBOL) {
      istring_t name = this->token;
      if (saveStrings)
        name = i_dup_string (this, name);

      if (nargs >= MAX_BUILTIN_PARAMS)
        return i_too_many_args (this);

      uf->argName[nargs] = name;
      nargs++;
      c = i_next_token (this);
      if (c is ')') break;

      if (c is ',')
        c = i_next_token (this);

    } else if (c is ')')
      break;

    else
      return this->syntax_error (this, "var definition, unxpected token");
  }

  uf->nargs = nargs;
  return nargs;
}

static int i_parse_func_def (i_t *this, int saveStrings) {
  Sym *sym;
  istring_t name;
  istring_t body;
  int c;
  int nargs = 0;
  UserFunc *uf;

  c = i_next_raw_token (this); // do not interpret the symbol
  if (c isnot I_TOK_SYMBOL) return this->syntax_error (this, "fun definition, not a symbol");

  name = this->token;
  c = i_next_token (this);

  uf = (UserFunc *) i_stack_alloc (this, sizeof (*uf));
  ifnot (uf) return i_out_of_mem (this);

  uf->nargs = 0;
  if (c is '(') {
    nargs = i_parse_var_list (this, uf, saveStrings);
    if (nargs < 0) return nargs;

    c = i_next_token (this);
  }

  if (c isnot I_TOK_STRING) return this->syntax_error (this, "fun definition, not a string");

  body = this->token;

  if (saveStrings) {
    name = i_dup_string(this, name);
    body = i_dup_string(this, body);
  }

  uf->body = body;

  sym = i_define_sym (this, name, USRFUNC | (nargs << 8), (ival_t) uf);
  ifnot (sym) return i_out_of_mem (this);

  i_next_token (this);
  return I_OK;
}

static int i_parse_print_rout (i_t *this) {
  int c;
  int err = I_OK;

print_more:
  c = i_next_token (this);
  if (c is I_TOK_STRING) {
    i_print_string (this, this->out_fp, this->token);
    i_next_token (this);
  } else {
    ival_t val;
    err = i_parse_expr (this, &val);
    if (err isnot I_OK) return err;

    i_print_number (this, val);
  }

  if (this->curToken is ',') goto print_more;
  return err;
}

static int i_parse_println (i_t *this) {
  int err = i_parse_print_rout (this);
  this->print_byte (this->out_fp, '\n');
  return err;
}

static int i_parse_print (i_t *this) {
  return i_parse_print_rout (this);
}

static int i_parse_return (i_t *this) {
  int err;
  i_next_token (this);
  err = i_parse_expr (this, &this->fResult);
  // terminate the script
  i_StringSetLen (&this->parseptr, 0);
  this->didReturn = 1;
  return err;
}

static int i_parse_while (i_t *this) {
  int err;
  istring_t savepc = this->parseptr;

again:
  err = i_parse_if (this);
  if (err is I_ERR_OK_ELSE) {
    return I_OK;
  } else if (err is I_OK) {
    this->parseptr = savepc;
    goto again;
  }

  return err;
}

// builtin
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

static struct def {
  const char *name;
  int toktype;
  ival_t val;
} idefs[] = {
  { "var",     I_TOK_VARDEF,  (ival_t) 0 },
  { "else",    I_TOK_ELSE,    (ival_t) 0 },
  { "if",      I_TOK_IF,      (ival_t) i_parse_if },
  { "ifnot",   I_TOK_IFNOT,   (ival_t) i_parse_ifnot },
  { "while",   I_TOK_WHILE,   (ival_t) i_parse_while },
  { "println", I_TOK_PRINTLN, (ival_t) i_parse_println },
  { "print",   I_TOK_PRINT,   (ival_t) i_parse_print },
  { "func",    I_TOK_FUNCDEF, (ival_t) i_parse_func_def },
  { "return",  I_TOK_RETURN,  (ival_t) i_parse_return },
  { "true",    INT, (ival_t) 1},
  { "false",   INT, (ival_t) 0},
  { "OK",      INT, (ival_t) 0},
  { "NOTOK",   INT, (ival_t) -1},
  // operators
  { "*",     BINOP(1), (ival_t) i_prod },
  { "/",     BINOP(1), (ival_t) i_quot },
  { "%",     BINOP(1), (ival_t) i_mod },
  { "+",     BINOP(2), (ival_t) i_sum },
  { "-",     BINOP(2), (ival_t) i_diff },
  { "&",     BINOP(3), (ival_t) i_bitand },
  { "|",     BINOP(3), (ival_t) i_bitor },
  { "^",     BINOP(3), (ival_t) i_bitxor },
  { ">>",    BINOP(3), (ival_t) i_shr },
  { "<<",    BINOP(3), (ival_t) i_shl },
  { "=",     BINOP(4), (ival_t) i_equals },
  { "is",    BINOP(4), (ival_t) i_equals },
  { "isnot", BINOP(4), (ival_t) i_ne },
  { "<>",    BINOP(4), (ival_t) i_ne },
  { "<",     BINOP(4), (ival_t) i_lt },
  { "<=",    BINOP(4), (ival_t) i_le },
  { ">",     BINOP(4), (ival_t) i_gt },
  { ">=",    BINOP(4), (ival_t) i_ge },
  { NULL, 0, 0 }
};

static int i_define (i_t *this, const char *name, int typ, ival_t val) {
  Sym *s;
  s = i_define_sym (this, i_istring_new (name), typ, val);
  if (NULL is s) return i_out_of_mem (this);
  return I_OK;
}

static int i_eval_string (i_t *this, const char *buf, int saveStrings, int topLevel) {
  this->script_buffer = buf;
  istring_t x = i_istring_new (buf);
  int retval = i_parse_string (this, x, saveStrings, topLevel);
  i_release_malloced_strings (this);
  return retval;
}

static int i_eval_file (i_t *this, const char *filename) {
  char script[this->max_script_size];

  ifnot (File.exists (filename)) {
    this->print_fmt_bytes (this->err_fp, "%s: doesn't exists\n", filename);
    return NOTOK;
  }

  int r = OK;
  FILE *fp = fopen (filename, "r");
  if (NULL is fp) {
    this->print_fmt_bytes (this->err_fp, "%s\n", Error.errno_string (errno));
    return NOTOK;
  }

  r = fread (script, 1, this->max_script_size, fp);
  fclose (fp);

  if (r <= 0) {
    this->print_fmt_bytes (this->err_fp, "Couldn't read script\n");
    return NOTOK;
  }

  script[r] = 0;
  r = i_eval_string (this, script, 0, 1);
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
  i_release_malloced_strings (this);
  free (this->arena);
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
  return (char *) i_StringGetPtr (this->parseptr);
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
  return fprintf (fp, "%s", bytes);
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

ival_t i_print_str (i_t *this, char *str) {
  char *sp = str;
  int nbytes = 0;
  while (*sp)
    nbytes += this->print_byte (this->out_fp, *sp++);
  return nbytes;
}

ival_t i_println_str (i_t *this, char *str) {
  int nbytes = i_print_str (this, str);
  this->print_byte (this->out_fp, '\n');
  return nbytes + 1;
}

ival_t i_not (i_t *this, ival_t value) {
  (void) this;
  return !value;
}

ival_t i_bool (i_t *this, ival_t value) {
  (void) this;
  return !!value;
}

ival_t i_free (i_t *this, void *value) {
  (void) this;
  ifnot (NULL is value)
    free (value);
  return I_OK;
}

void *i_alloc (i_t *this, ival_t size) {
  (void) this;
  return Alloc ((uint) size);
}

struct i_def_fun_t {
  const char *name;
  ival_t val;
  int nargs;
} i_def_funs[] = {
  { "not",             (ival_t) i_not, 1},
  { "bool",            (ival_t) i_bool, 1},
  { "free",            (ival_t) i_free, 1},
  { "alloc",           (ival_t) i_alloc, 1},
  { "print_str",       (ival_t) i_print_str, 1},
  { "println_str",     (ival_t) i_println_str, 1},
  { NULL, 0, 0}
};

static int i_define_funs_default_cb (i_t *this) {
/*  int err;
  for (int i = 0; ifuns[i].name; i++) {
    if (I_OK isnot (err = i_define (this, ifuns[i].name, CFUNC (ifuns[i].nargs), ifuns[i].val)))
      return err;
  }
*/
  (void) this;
  return I_OK;
}

static i_opts i_default_options (i_t *this, i_opts opts) {
  if (0 >= opts.mem_size)
    opts.mem_size = $my(default_size);

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

  this->arena = (char *) Alloc (opts.mem_size);
  this->didReturn = 0;
  this->mem_size = opts.mem_size;
  this->print_byte = opts.print_byte;
  this->print_fmt_bytes = opts.print_fmt_bytes;
  this->print_bytes = opts.print_bytes;
  this->syntax_error = opts.syntax_error;
  this->err_fp = opts.err_fp;
  this->out_fp = opts.out_fp;
  this->max_script_size = opts.max_script_size;
  this->user_data = opts.user_data;
  this->define_funs_cb = opts.define_funs_cb;
  this->state = 0;

  if (NULL is opts.idir)
    this->idir = String.new (32);
  else
    this->idir = String.new_with (opts.idir);

  this->message = String.new (32);

  this->symptr = (Sym *) this->arena;
  this->valptr = (ival_t *) (this->arena + this->mem_size);

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

static ival_t I_print_bytes (i_t *this, char *bytes) {
  return this->print_bytes (this->out_fp, bytes);
}

static ival_t I_print_byte (i_t *this, char byte) {
  return this->print_byte (this->out_fp, byte);
}

public i_T *__init_i__ (void) {
  __INIT__ (path);
  __INIT__ (file);
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
  $my(default_size) = 1 << 12;

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
