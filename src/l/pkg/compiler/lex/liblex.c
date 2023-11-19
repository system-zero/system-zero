#define REQUIRE_VSNPRINTF
#define REQUIRE_STRING
#define REQUIRE_ALLOC
#define REQUIRE_STR_COPY
#define REQUIRE_STR_NEW
#define REQUIRE_STR_EQ
#define REQUIRE_ISDIGIT
#define REQUIRE_ISSPACE
#define REQUIRE_MAP

#ifndef NO_DEBUG
#define REQUIRE_STDIO
#endif

#include <libc.h>
#include "tokens.h"
#include <z/lex.h>

#define LEX_ERROR LA_ERROR
#define MAXLEN_SYM_NAME 255

static const keyword_type std_keywords[] = {
  { "for",    TOKEN_FOR },
  { "func",   TOKEN_FUNC },
  { "if",     TOKEN_IF },
  { "ifnot",  TOKEN_IFNOT },
  { "import", TOKEN_IMPORT },
  { "in",     TOKEN_IN },
  { "is",     TOKEN_IS },
  { "isnot",  TOKEN_ISNOT },
  { "null",   TOKEN_NULL },
  { "var",    TOKEN_VAR },
  { "const",  TOKEN_CONST },
  { "then",   TOKEN_THEN },
  { "!",      TOKEN_BANG },
  { "%",      TOKEN_PERC },
  { "$",      TOKEN_DOLLAR },
  { "&",      TOKEN_AMP },
  { "&=",     TOKEN_AMP_EQ },
  { "*",      TOKEN_STAR },
  { "+",      TOKEN_PLUS },
  { "+=",     TOKEN_PLUS_EQ },
  { "++",     TOKEN_PLUS_PLUS },
  { "-",      TOKEN_MINUS },
  { "--",     TOKEN_MINUS_MINUS },
  { ".",      TOKEN_DOT },
  { "/",      TOKEN_SLASH },
  { ":",      TOKEN_COLON },
  { "<",      TOKEN_LESS },
  { "<<",     TOKEN_LESS_LESS },
  { "=",      TOKEN_ASSIGN },
  { ">",      TOKEN_GT },
  { ">>",     TOKEN_GT_GT },
  { "^",      TOKEN_CIRCUM },
  { "|",      TOKEN_BAR },
  { "|=",     TOKEN_BAR_EQ }
};

static const char *std_operators = "=+-!/*%<>&|^:.$";

static int throw_error (Lex *lex, int err, const char *fmt, ...) {
#ifndef NO_DEBUG
  int max = 256;
  int n = 0;
  char *sp = (char *)lex->src;
  while (true) {
    if (n == max + 1)
      break;
     if (sp == lex->src_beg)
       break;
     sp--;
     n++;
  }

  sys_fprintf (sys_stderr, "LEX ERROR:\n");
  for (int i = 0; i <= n; i++)
    sys_fprintf (sys_stderr, "%c", *sp++);
  sys_fprintf (sys_stderr, "\n");
#endif

  va_list ap;
  va_start(ap, fmt);
  sys_vsnprintf (lex->unit->err_msg, MAXLEN_LEX_ERR_MSG + 1, fmt, ap);
  va_end(ap);
  lex->unit->retval = err;

  return err;
}

char *lex_token_to_string (char *buf, int d) {
  buf[0] = '\0';
  if (d >= ' ') {
    if (d < '~') {
      buf[0] = (uchar) d;
      buf[1] = '\0';
    } else {
      if (d - '~' == ' ') {
        buf[0] = ' ';
      } else {
        buf[2] = '\0';
        if (d - '~' == '-') {
          buf[0] = '-'; buf[1] = '-';
        } else if (d - '~' == '+') {
          buf[0] = '+'; buf[1] = '+';
        } else if (d - '~' == '<') {
          buf[0] = '<';buf[1] = '<';
        } else if (d - '~' == '>') {
          buf[0] = '>';buf[1] = '>';
        } else if (d - '~' - '+'  == '=') {
          buf[0] = '+'; buf[1] = '=';
        } else if (d - '~'  == '=') {
          buf[0] = '|'; buf[1] = '=';
        } else if (d - '=' - '~'  == '|') {
          buf[0] = '|'; buf[1] = '=';
        } else if (d - '=' - '~'  == '&') {
          buf[0] = '&'; buf[1] = '=';
        }
      }
    }
  }

  return buf;
}

void lex_print_lexemes (Lex_Unit *unit, int as_json, int to_lang) {
  lex_type *atom = unit->beg;
  char idstr[4];

  if (to_lang) {
    sys_fprintf(sys_stdout, "var map[%d] lex_array = %s", unit->lex->num_tokens);
    as_json = 1;
  }

  sys_fprintf(sys_stdout, "[\n");
  if (as_json) {
    int is_null;
    int is_comment;
    while (atom) {
      is_null = atom->token_name == NULL;
      is_comment = atom->token_id == TOKEN_SL_COMMENT;
      sys_fprintf(sys_stdout, "%s", is_comment ? "# COMMM\n" : "");
      sys_fprintf(sys_stdout,
        "{ token_id : %d, token_name : %s%s%s, token_len : %d }%s",
         atom->token_id, is_null ? "" : "\"",
            atom->token_name == NULL ? "null" :
              (is_comment ? "" :  atom->token_name),
                         is_null ? "" : "\"", atom->token_len, (atom->next ? ",\n" : ""));
      atom = atom->next;
    }
  } else {
    while (atom) {
      sys_fprintf(sys_stdout,
        "{ token_id : %-3d, token_id_string : %-2s, token_name : `%s`, token_len : %d }%s",
         atom->token_id, lex_token_to_string (idstr, atom->token_id), atom->token_name, atom->token_len, (atom->next ? ",\n" : ""));
      atom = atom->next;
    }
  }
  sys_fprintf(sys_stdout, "\n]\n");
}

char *lex_unit_to_string (Lex_Unit *unit) {
  Lex *lex = unit->lex;
  size_t len = lex->src - lex->src_beg;
  char *buf = Alloc (len + 1);
  char *sp = buf;

  lex_type *atom = unit->beg;
  while (atom) {
    if (atom->token_name != NULL) {
      for (size_t i = 0; i < atom->token_len; i++)
        *sp++ = atom->token_name[i];
    } else if (atom->token_id == TOKEN_NL) {
      *sp++ = '\n';
    } else if (atom->token_id == TOKEN_WS || atom->token_id == TOKEN_LDWS) {
      for (size_t i = 0; i < atom->token_len; i++)
        *sp++ = ' ';
    } else {
      char idstr[4];
      lex_token_to_string (idstr, atom->token_id);
      int c; int i = 0;
      while ((c = idstr[i++]))
        *sp++ = c;
    }

    atom = atom->next;
  }

  buf[len] = '\0';
  return buf;
}

int lex_validate_output (Lex_Unit *unit, char **buf) {
  Lex *lex = unit->lex;
  *buf = lex_unit_to_string (unit);
  if (str_eq (*buf, lex->src_beg))
    return 1;
  return 0;
}

static lex_type *lex_add_atom (Lex *lex, const char *token_name, size_t token_len, int token_id) {
  lex->num_tokens++;
  lex_type *atom  = Alloc (sizeof (lex_type));
  atom->token_name = token_name;
  atom->token_len  = token_len;
  atom->token_id   = token_id;

  if (NULL == lex->lexs_beg) {
    atom->prev = atom->next = NULL;
    lex->lexs_beg = atom;
    lex->lexs_end = atom;
    return atom;
  }

  atom->next = NULL;
  atom->prev = lex->lexs_end;
  lex->lexs_end->next = atom;
  lex->lexs_end = atom;
  return atom;
}

static inline void lex_unget_byte (Lex *lex) {
  lex->src--;
}

static inline int lex_get_byte (Lex *lex) {
  return *lex->src++;
}

static int lex_get_ws (Lex *lex) {
  int n = 1;

  int c;

  for (;;) {
    c = lex_get_byte (lex);

    if (' ' != (c)) {
      lex_unget_byte (lex);
      break;
    }

    n++;
  }

  if (lex->lexs_end->token_id == TOKEN_NL)
    lex_add_atom (lex, NULL, n, TOKEN_LDWS);
  else
    lex_add_atom (lex, NULL, n, TOKEN_WS);

  return TOKEN_WS;
}

static inline int is_identifier (int c) {
  return (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') ||
           c == '_' || isdigit (c));
}

static inline int is_operator (int c) {
  char *sp = (char *) std_operators;
  while (*sp)
    if (*sp++ == c)
      return 1;

  return 0;
}

static int lex_get_identifier (Lex *lex) {
   lex->beg = lex->src - 1;
   lex->end = lex->src;

  int c;

  for (;;) {
    c = lex_get_byte (lex);

    ifnot (c)
      return throw_error (lex, LEX_ERROR, "awaiting identifier, found EOF\n");

    ifnot (is_identifier (c)) {
      lex_unget_byte (lex);
      break;
    }

    lex->end++;
  }

  size_t len = lex->end - lex->beg;

  if (len > MAXLEN_SYM_NAME) {
    char err[len + 1];
    str_copy (err, len + 1, lex->beg, len);
    return throw_error (lex, LEX_ERROR, "%s: token name exceeded maximum %zd bytes\n", err, MAXLEN_SYM_NAME);
  }

  char *name = str_new_with_len (lex->beg, len);

  keyword_type *k = map_get_value (lex->std_keywords, name);

  ifnot (NULL == k) {
    lex_add_atom (lex, name, len, k->token_id);
    return k->token_id;
  }

  lex_add_atom (lex, name, len, TOKEN_SYMBOL);

  return TOKEN_SYMBOL;
}

static int lex_get_operator (Lex *lex) {
  lex->beg = lex->src - 1;
  lex->end = lex->src;

  int c;

  for (;;) {
    c = lex_get_byte (lex);

    if (is_operator (c)) {
      lex->end++;
      continue;
    }

    lex_unget_byte (lex);
    break;
  }

  size_t len = lex->end - lex->beg;

  char *op = str_new_with_len (lex->beg, len);

  keyword_type *k = map_get_value (lex->std_keywords, op);

  if (NULL == k)
    return throw_error (lex, LEX_ERROR, "|%s|: unknown operator\n", op);

  lex_add_atom (lex, op, len, k->token_id);
  return k->token_id;
}

static int lex_get_binary (Lex *lex) {
  (void) lex;
  return 0;
}

static int lex_get_octal (Lex *lex) {
  lex->beg = lex->src - 2;
  lex->end = lex->src;

  int c;
  while ((c = lex_get_byte (lex))) {
    if (c >= 0 && c <= 7)
      continue;

    if (c == 8 || c == 9)
      return throw_error (lex, LEX_ERROR, "error while getting an octal number, got %c", c);

    lex_unget_byte (lex);
    break;
  }

  size_t len = lex->end - lex->beg;
  char *number = str_new_with_len (lex->beg, len);

  lex_add_atom (lex, number, len, TOKEN_OCTAL);
  return TOKEN_OCTAL;
}

static int lex_get_hex (Lex *lex) {
  (void) lex;
  return 0;
}

static int lex_get_number (Lex *lex) {
  lex->beg = lex->src - 1;
  lex->end = lex->src;

  size_t len;
  char *number;
  int token_id = TOKEN_INT;
  int dot_num = 0;
  int plus_num = 0;
  int minus_num = 0;

  int prev_c = *lex->beg;
  int c = lex_get_byte (lex);

  switch (prev_c) {
    case '0':
      switch (c) {
        case 'b':
          return lex_get_binary (lex);

        case '0'...'7':
          return lex_get_octal (lex);

        case 'x':
        case 'X':
          return lex_get_hex (lex);

        case ' ':
        case '\t':
        case TOKEN_RPAREN:
        case TOKEN_RBRACKET:
          lex_unget_byte (lex);
          goto theend;

        case TOKEN_DOT:
          dot_num = 1;
          break;

        default:
          lex_unget_byte (lex);
          goto theend;
      }

      break;

    case TOKEN_TILDE:
      lex_add_atom (lex, NULL, 0, TOKEN_UNARY);
      break;

    case TOKEN_MINUS:
      switch (c) {
        case TOKEN_ASSIGN:
        case ' ':
        case '\t':
          lex_unget_byte (lex);
          return TOKEN_MINUS;

        case '-':
          return TOKEN_MINUS_MINUS;

        case '1'...'9':
          break;

        default:
          return throw_error (lex, LEX_ERROR, "awaiting a number after -\n");
      }
  }

  ifnot (isdigit (c)) {
    lex_unget_byte (lex);
    goto theend;
  }

  for (;;) {
    c = lex_get_byte (lex);

    if (c == TOKEN_MINUS || c == TOKEN_PLUS || c == TOKEN_DOT) {
      check:
      if (c == TOKEN_MINUS) {
        if (minus_num++)
          return throw_error (lex, LEX_ERROR, "error while parsing a number, found two -\n");
      } else if (c == TOKEN_PLUS) {
        if (plus_num++)
          return throw_error (lex, LEX_ERROR, "error while parsing a number, found two +\n");
      } else {
        if (dot_num++)
          return throw_error (lex, LEX_ERROR, "error while parsing a number, found two .\n");
        token_id = TOKEN_NUMBER;
      }

      c = lex_get_byte (lex);
      ifnot (isdigit (c))
        return throw_error (lex, LEX_ERROR, "awaiting a digit after %c\n", c);
    }

    if (c == 'e' || c == 'E') {
      c = lex_get_byte (lex);
      ifnot (isdigit (c)) {
        if (c == TOKEN_MINUS ||
            c == TOKEN_PLUS  ||
            c == TOKEN_DOT)
          goto check;

        return throw_error (lex, LEX_ERROR, "awaiting a +,-,. or digit after %c\n\n", c);
      }
      token_id = TOKEN_NUMBER;
      continue;
    }

    ifnot (isdigit (c)) {
      lex_unget_byte (lex);
      break;
    }
  }

theend:
  lex->end = lex->src;
  len = lex->end - lex->beg;
  number = str_new_with_len (lex->beg, len);

  lex_add_atom (lex, number, len, token_id);
  return token_id;
}

static int lex_get_char (Lex *lex) {
  lex->beg = lex->src;

  int c = lex_get_byte (lex);

  if (c == '\\') {
    c = lex_get_byte (lex);
    switch (c) {
      case 'n':
      case 'r':
      case 't':
      case 'b':
      case 'a':
      case 'f':
      case 'v':
      case 'e':
        break;

      default:
        return throw_error (lex, LEX_ERROR, "awaiting one of [nrtbafve] after a backslash, got %c", c);
    }
  } else {
    if (c < ' ' || c > '~')
      return throw_error (lex, LEX_ERROR, "awaiting one character into the ascii range, got %c", c);
  }

  c = lex_get_byte (lex);
  if (c != TOKEN_SQUOTE)
    return throw_error (lex, LEX_ERROR, "awaiting a single quote, got: %c\n", c);

  lex->end = lex->src;
  size_t len = lex->end - lex->beg - 1;

  char *chr = str_new_with_len (lex->beg, len);
  lex_add_atom (lex, NULL, 0, TOKEN_SQUOTE);
  lex_add_atom (lex, chr, len, TOKEN_CHAR);
  lex_add_atom (lex, NULL, 0, TOKEN_SQUOTE);
  return TOKEN_CHAR;
}

static int lex_get_qouted_string (Lex *lex, char qtype) {
  lex->beg = lex->src;

  int prev_c;
  int c = 0;

  for (;;) {
    prev_c = c;
    c = lex_get_byte (lex);

    ifnot (c)
      return throw_error (lex, LEX_ERROR, "unterminated %c qouted string\n", qtype);

    if (c == TOKEN_NL) {
      lex->lineNum++;
      continue;
    }

    if (c == qtype) {
      if (prev_c == TOKEN_BSLASH)
        continue;
      lex->end = lex->src - 1;
      break;
    }
  }

  size_t len = lex->end - lex->beg;
  char *str = str_new_with_len (lex->beg, len);
  lex_add_atom (lex, NULL, 0, qtype);
  if (qtype == '"')
    lex_add_atom (lex, str, len, TOKEN_DQ_STRING);
  else
    lex_add_atom (lex, str, len, TOKEN_BQ_STRING);

  lex_add_atom (lex, NULL, 0, qtype);
  return qtype;
}

static int lex_get_comment (Lex *lex) {
  lex->beg = lex->src - 1;
  int c;

  while ((c = lex_get_byte (lex))) {
    if (c == TOKEN_NL) {
      lex_unget_byte (lex);
      break;
    }
  }

  lex->end = lex->src;
  size_t len = lex->end - lex->beg;
  char *str = str_new_with_len (lex->beg, len);
  lex_add_atom (lex, str, len, TOKEN_SL_COMMENT);
  return TOKEN_SL_COMMENT;
}

static int __lex_parse (Lex *lex) {
  int c;
  int r;
  lex->src_beg = lex->src;

  for (;;) {
    c = lex_get_byte (lex);

    switch (c) {
      case 0:
        lex_add_atom (lex, NULL, 0, TOKEN_EOF);
        return TOKEN_EOF;

      case ' ':
        r = lex_get_ws (lex);
        continue;

      case '\t':
        return throw_error (lex, LEX_ERROR, "tabs are forbidden as a space character");

      case TOKEN_SL_COMMENT:
        lex_get_comment (lex);
        continue;

      case 'a'...'z':
      case 'A'...'Z':
      case '_':
        r = lex_get_identifier (lex);
        if (r == LEX_ERROR) return LEX_ERROR;
        continue;

      case TOKEN_NL:
        lex_add_atom (lex, NULL, 0, TOKEN_NL);
        lex->lineNum++;
        continue;

      case TOKEN_LPAREN:
      case TOKEN_RPAREN:
      case TOKEN_LBRACE:
      case TOKEN_RBRACE:
      case TOKEN_LBRACKET:
      case TOKEN_RBRACKET:
      case TOKEN_DOT:
      case TOKEN_COLON:
      case TOKEN_SEMICOLON:
      case TOKEN_COMMA:
        lex_add_atom (lex, NULL, 0, c);
        continue;

      case TOKEN_SQUOTE:
        r = lex_get_char (lex);
        if (r == LEX_ERROR) return LEX_ERROR;
        continue;

      case TOKEN_DQUOTE:
        r = lex_get_qouted_string (lex, TOKEN_DQUOTE);
        if (r == LEX_ERROR) return LEX_ERROR;
        continue;

      case TOKEN_BQUOTE:
        r = lex_get_qouted_string (lex, TOKEN_BQUOTE);
        if (r == LEX_ERROR) return LEX_ERROR;
        continue;

      case '0'...'9':
      case '~':
      case '-':
        r = lex_get_number (lex);
        if (r == LEX_ERROR) return LEX_ERROR;
        if (r == TOKEN_MINUS)
          lex_add_atom (lex, NULL, 0, TOKEN_MINUS);
        else if (r == TOKEN_MINUS_MINUS)
          lex_add_atom (lex, NULL, 0, TOKEN_MINUS_MINUS);

        continue;

        // fallthrough
      default:
        ifnot (is_operator (c))
          return throw_error (lex, LEX_ERROR, "awaiting an operator got: %c\n", c);

        if (lex_get_operator (lex) == LEX_ERROR)
          return throw_error (lex, LEX_ERROR, "awaiting an operator got: %c\n", c);
    }
  }

#ifndef NO_DEBUG
#endif
  return TOKEN_EOF;
}

static void lex_add_std_keywords (Lex *lex) {
  lex->std_keywords = map_new (128);

  size_t len = (sizeof (std_keywords) / sizeof (std_keywords[0]));

  keyword_type *k;

  for (size_t i = 0; k = (void *) &std_keywords[i], i < len; i++)
    map_set (lex->std_keywords, (char *) k->keyword, k, NULL);
}

static Lex *lex_init (Lex *lex) {
  lex->lineNum = 0;
  lex->num_tokens = 0;
  lex->lexs_beg = lex->lexs_end = NULL;
  lex_add_atom (lex, NULL, 0, TOKEN_LDWS);
  lex_add_std_keywords (lex);
  return lex;
}

Lex_Unit *lex_parse (Lex_Unit *unit) {
  Lex *lex = unit->lex;
  lex_init (lex);
  lex->src = unit->src;
  lex->unit = unit;

  unit->retval = __lex_parse (lex);

  unit->beg = lex->lexs_beg;
  unit->end = lex->lexs_end;
  return unit;
}
