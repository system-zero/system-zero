#ifndef RLINE_HDR
#define RLINE_HDR

#define RLINE_ACCEPT_ONE_ITEM  (1 << 0)

typedef struct rline_t rline_t;
typedef struct currentLine currentLine;

typedef struct rlineCompletions rlineCompletions;
                /* incompatible API */
typedef int (*RlineCompletion_cb) (const char *, int, rlineCompletions *, void *);
/* this is to support tab completions even if the pointer is not at the end
 * of the line */

typedef char *(*RlineHints_cb) (const char *, int *, int *, void *);
typedef void  (*RlineFreeHints_cb) (void *, void *);

/* ADDITION: if not NULL this is called first before any parsing */
typedef int   (*OnInput_cb) (const char *, string *, int *, int, rlineCompletions *, void *);
/* ADDITION: if not NULL this is called when a carriage return is received */
typedef void  (*OnCarriageReturn_cb) (const char *, void *);
/* ADDITION: if not NULL this is called on tab completion and there is just one match */ 
typedef int   (*AcceptOneItem_cb) (const char *, rlineCompletions *, void *);

/* this is our API, a little bit more than merely a wrapper around linenoise */
typedef struct rline_set_self {
  void
     (*flags) (rline_t *, rlineCompletions *, int),
     (*prompt) (rline_t *, const char *),
     (*curpos) (rline_t *, rlineCompletions *, int),
     (*current) (rline_t *, rlineCompletions *, const char *),
     (*hints_cb) (rline_t *, RlineHints_cb, void *),
     (*on_input_cb) (rline_t *, OnInput_cb),
     (*completion_cb) (rline_t *, RlineCompletion_cb, void *),
     (*release_hints_cb) (rline_t *, RlineFreeHints_cb),
     (*accept_one_item_cb) (rline_t *, AcceptOneItem_cb),
     (*on_carriage_return_cb) (rline_t *, OnCarriageReturn_cb);
} rline_set_self;

typedef struct rline_unset_self {
  void (*flags) (rline_t *, rlineCompletions *, int);
} rline_unset_self;

typedef struct rline_history_set_self {
  void
    (*file) (rline_t *, char *);

  int (*length) (rline_t *, int);
} rline_history_set_self;

typedef struct rline_get_self {
  char ** (*array) (rline_t *, rlineCompletions *);
  size_t (*arraylen) (rline_t *, rlineCompletions *);
} rline_get_self;

typedef struct rline_history_get_self {
  char **(*lines) (rline_t *);
  int (*length) (rline_t *);
  void *(*userdata) (rline_t *);
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
  rline_get_self get;

  rline_unset_self unset;
  rline_history_self history;

  void
    (*release) (rline_t *),
    (*refresh_line) (rline_t *, rlineCompletions *),
    (*add_completion) (rline_t *, rlineCompletions *, char *, int);

  rlineCompletions *
    (*release_completions) (rline_t *, rlineCompletions *);

  char *(*edit) (rline_t *);

  rline_t *(*new) (void);
  int (*fd_read) (rline_t *, int);
  int (*check_special) (rline_t *, int);
} rline_self;

typedef struct rline_T {
  rline_self self;
} rline_T;

public rline_T __init_rline__ (void);
#endif /* RLINE_HDR */
