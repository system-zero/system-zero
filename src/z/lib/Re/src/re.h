#ifndef RE_HDR
#define RE_HDR

#define RE_MAXLEN_ERR_MSG MAXLEN_ERR_MSG

#define RE_IGNORE_CASE               (1 << 0)
#define RE_ENCLOSE_PAT_IN_PAREN      (1 << 1)
#define RE_PATTERN_IS_STRING_LITERAL (1 << 2)

#define RE_MAX_NUM_CAPTURES 9

/* These correspond to SLRE failure codes */
#define RE_NO_MATCH                          -1
#define RE_UNEXPECTED_QUANTIFIER_ERROR       -2
#define RE_UNBALANCED_BRACKETS_ERROR         -3
#define RE_INTERNAL_ERROR                    -4
#define RE_INVALID_CHARACTER_SET_ERROR       -5
#define RE_INVALID_METACHARACTER_ERROR       -6
#define RE_CAPS_ARRAY_TOO_SMALL_ERROR        -7
#define RE_TOO_MANY_BRANCHES_ERROR           -8
#define RE_TOO_MANY_BRACKETS_ERROR           -9
#define RE_SUBSTITUTION_STRING_PARSING_ERROR -10

typedef struct capture_t {
  const char *ptr;
  int len;
} capture_t;


typedef struct re_t {
  string_t *pat;
  capture_t **cap;
  char *buf;
  size_t buflen;
  int retval;
  int flags;
  int num_caps;
  int total_caps;
  int match_idx;
  int match_len;
  char *match_ptr;
  string_t *match;
  char errmsg[RE_MAXLEN_ERR_MSG];
} re_t;

typedef int  (*ReCompile_cb) (re_t *);

typedef struct re_self {
  re_t *(*new) (const char *, int, int, int (*) (re_t *));
  void
    (*release) (re_t *),
    (*release_captures) (re_t *),
    (*reset_captures) (re_t *),
    (*allocate_captures) (re_t *, int),
    (*release_pat) (re_t *);

  int
    (*exec) (re_t *, char *, size_t),
    (*compile) (re_t *);

  string_t
    *(*parse_substitute) (re_t *, const char *, const char *),
    *(*get_match) (re_t *, int);
} re_self;

typedef struct re_T {
  re_self self;
} re_T;

public re_T __init_re__ (void);

#endif /* RE_HDR */
