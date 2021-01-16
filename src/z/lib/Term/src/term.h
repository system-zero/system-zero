#ifndef TERM_HDR
#define TERM_HDR

typedef struct term_t {
  int is_initialized;

  char
     mode,
    *name;

  int
    state,
    in_fd,
    out_fd,
    orig_curs_row_pos,
    orig_curs_col_pos,
    num_rows,
    num_cols;

  struct termios
    orig_mode,
    raw_mode;

} term_t;


typedef struct term_screen_self {
  void
    (*save)    (term_t *),
    (*clear)   (term_t *),
    (*restore) (term_t *);
} term_screen_self;

typedef struct term_self {
  term_screen_self screen;

  void
    (*release)   (term_t **),
    (*init_size) (term_t *, int *, int *);

  int
    (*raw_mode)  (term_t *),
    (*orig_mode) (term_t *),
    (*sane_mode) (term_t *);

} term_self;

typedef struct term_T {
  term_self self;
} term_T;

public term_T __init_term__ (void);

#endif /* TERM_HDR */
