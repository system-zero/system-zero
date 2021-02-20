#ifndef RLINE_HDR
#define RLINE_HDR

typedef struct rline_t rline_t;

typedef struct rlineCompletions rlineCompletions;
typedef void (*RlineCompletion_cb) (const char *, rlineCompletions *, void *);

typedef char *(*RlineHints_cb) (const char *, int *, int *, void *);
typedef void  (*RlineFreeHints_cb) (void *, void *);

typedef struct rline_set_self {
  void
     (*prompt) (rline_t *, char *),
     (*hints_cb) (rline_t *, RlineHints_cb, void *),
     (*completion_cb) (rline_t *, RlineCompletion_cb, void *),
     (*release_hints_cb) (rline_t *, RlineFreeHints_cb);
} rline_set_self;

typedef struct rline_history_set_self {
  void (*file) (rline_t *, char *);
  int  (*length) (rline_t *, int);
} rline_history_set_self;

typedef struct rline_history_get_self {
  char **(*lines) (rline_t *);
  int (*length) (rline_t *);
} rline_history_get_self;

typedef struct rline_history_self {
  rline_history_set_self set;
  rline_history_get_self get;

  void (*release) (rline_t *);

  int
    (*add) (rline_t *, char *),
    (*save) (rline_t *),
    (*load) (rline_t *);
} rline_history_self;

typedef struct rline_self {
  rline_set_self set;
  rline_history_self history;

  void
    (*release) (rline_t *),
    (*add_completion) (rline_t *, rlineCompletions *, char *);

  char *(*edit) (rline_t *);

  rline_t *(*new) (void);
} rline_self;

typedef struct rline_T {
  rline_self self;
} rline_T;

public rline_T __init_rline__ (void);
#endif /* RLINE_HDR */
