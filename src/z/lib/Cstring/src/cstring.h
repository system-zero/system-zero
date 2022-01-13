#ifndef CSTRING_HDR
#define CSTRING_HDR

#define CSTRING_TOK_OK    OK
#define CSTRING_TOK_NOTOK NOTOK
#define CSTRING_TOK_RETURN (OK + 1)

typedef struct cstring_tok {
  int num_tokens;
  char **tokens;
  int *length;
} cstring_tok;

typedef int (*CstringTok_cb) (cstring_tok *, char *, size_t, void *);

typedef struct cstring_trim_self {
  char *(*end) (char *, char);
} cstring_trim_self;

typedef struct cstring_byte_self {
  size_t
    (*cp) (char *, const char *, size_t),
    (*cp_all) (char *, const char *, size_t),
    (*mv) (char *, size_t, size_t, size_t, size_t);

  char
    *(*in_str) (const char *, int),
    *(*in_str_r) (const char *, int),
    *(*null_in_str) (const char *);
} cstring_byte_self;

typedef struct cstring_self {
  cstring_trim_self trim;
  cstring_byte_self byte;

  void (*tok_release) (cstring_tok *);

  char
    *(*dup) (const char *, size_t),
    *(*itoa) (int, char *, int),
    *(*substr) (char *, size_t, char *, size_t, size_t),
    *(*bytes_in_str) (const char *, const char *),
    *(*extract_word_at) (const char *, size_t, char *, size_t, const char *, size_t, int, int *, int *);

  int
    (*eq) (const char *, const char *),
    (*eq_n) (const char *, const char *, size_t),
    (*cmp) (const char *, const char *),
    (*cmp_n) (const char *, const char *, size_t);

  size_t
    (*cp) (char *, size_t, const char *, size_t),
    (*cat) (char *, size_t, const char *),
    (*cp_fmt) (char *, size_t, const char *, ...);

  cstring_tok *(*tokenize) (cstring_tok *, const char *, const char *, CstringTok_cb, void *);
} cstring_self;

typedef struct cstring_T {
  cstring_self self;
} cstring_T;

public cstring_T __init_cstring__ (void);

#endif /* CSTRING_HDR */
