#ifndef LEX_H
#define LEX_H

#define MAXLEN_LEX_ERR_MSG  255

typedef struct lex_type {
  int token_id;
  const char *token_name;
  size_t token_len;

  struct lex_type *prev;
  struct lex_type *next;
} lex_type;

typedef struct {
   const char *keyword;
  int token_id;
} keyword_type;

typedef struct Lex_Unit Lex_Unit;

typedef struct {
  lex_type *lexs_beg;
  lex_type *lexs_end;

  int num_tokens;

  const char *src;
  const char *src_beg;
  const char *beg;
  const char *end;

  int lineNum;

  Map_Type *std_keywords;

  Lex_Unit *unit;
} Lex;

struct Lex_Unit {
  int retval;
  char err_msg[MAXLEN_LEX_ERR_MSG + 1];

  lex_type *beg;
  lex_type *end;
  int num_atoms;

  const char *src;
  const char *unit_name;

  keyword_type *keywords;
  int keyword_len;

  const char *operators;

  Lex *lex;
};

int   lex_validate_output (Lex_Unit *, char **);
void  lex_print_lexemes (Lex_Unit *, int, int);
char *lex_unit_to_string (Lex_Unit *);
char *lex_token_to_string (char *, int);
Lex_Unit *lex_parse (Lex_Unit *);

#endif /* LEX_H */
